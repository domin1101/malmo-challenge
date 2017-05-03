
from __future__ import absolute_import

import json
import re
import threading
import collections

import numpy as np
from multiprocessing import Lock, Semaphore

from common import Entity, ENV_ACTIONS, ENV_BOARD, ENV_ENTITIES, \
    ENV_BOARD_SHAPE, ENV_AGENT_NAMES

from MalmoPython import MissionSpec

from environment import PigChaseEnvironment
from malmopy.environment.malmo import MalmoEnvironment, MalmoStateBuilder

class QLock:
    def __init__(self):
        self.lock = threading.Lock()
        self.waiters = collections.deque()
        self.count = 0

    def acquire(self):
        self.lock.acquire()
        if self.count:
            new_lock = threading.Lock()
            new_lock.acquire()
            self.waiters.append(new_lock)
            self.lock.release()
            new_lock.acquire()
            self.lock.acquire()
        self.count += 1
        self.lock.release()

    def release(self):
        with self.lock:
            if not self.count:
                raise ValueError("lock not acquired")
            self.count -= 1
            if self.waiters:
                self.waiters.popleft().release()

    def locked(self):
        return self.count > 0

class MockPigChaseEnvironment(PigChaseEnvironment):
    """
    Represent the Pig chase with two agents and a pig. Agents can try to catch
    the pig (high reward), or give up by leaving the pig pen (low reward).
    """

    VALID_START_POSITIONS = [
        (2, 1), (3, 1), (4, 1), (5, 1), (6, 1),
        (2, 2), (4, 2), (6, 2),
        (2, 3), (3, 3), (4, 3), (5, 3), (6, 3),
        (2, 4), (4, 4), (6, 4),
        (2, 5), (3, 5), (4, 5), (5, 5), (6, 5)
    ]

    LAPIS_BLOCKS = [
        (1, 3), (7, 3)
    ]

    DEFAULT_BOARD = [
        [u'grass', u'grass', u'grass', u'grass', u'grass', u'grass', u'grass', u'grass', u'grass'],
        [u'grass', u'sand', u'sand', u'sand', u'sand', u'sand', u'sand', u'sand', u'grass'],
        [u'grass', u'sand', u'grass', u'grass', u'grass', u'grass', u'grass', u'sand', u'grass'],
        [u'sand', u'sand', u'grass', u'sand', u'grass', u'sand', u'grass', u'sand', u'sand'],
        [u'sand', u'lapis_block', u'grass', u'grass', u'grass', u'grass', u'grass', u'lapis_block', u'sand'],
        [u'sand', u'sand', u'grass', u'sand', u'grass', u'sand', u'grass', u'sand', u'sand'],
        [u'grass', u'sand', u'grass', u'grass', u'grass', u'grass', u'grass', u'sand', u'grass'],
        [u'grass', u'sand', u'sand', u'sand', u'sand', u'sand', u'sand', u'sand', u'grass'],
        [u'grass', u'grass', u'grass', u'grass', u'grass', u'grass', u'grass', u'grass', u'grass']
    ]

    GRAY_PALETTE = {
        'sand': 255,
        'grass': 200,
        'lapis_block': 150,
        'Agent_1': 100,
        'Agent_2': 50,
        'Pig': 0
    }

    def __init__(self, remotes,
                 state_builder,
                 actions=ENV_ACTIONS,
                 role=0, exp_name="",
                 human_speed=False, randomize_positions=False, debug_level=0):

        super(MockPigChaseEnvironment, self).__init__(remotes, state_builder, actions, role, exp_name, human_speed, randomize_positions)
        self.pig = []
        self.agents = []
        self.current_player = 0
        self.start_player = 0
        self.debug_level = debug_level
        self._buffer_cache = None

    @property
    def state(self):
        return self._user_defined_builder.build(self)

    @property
    def done(self):
        """
        Done if we have caught the pig
        """
        if self.start_player == self.current_player:
            if self.agents[0]['pos'] in MockPigChaseEnvironment.LAPIS_BLOCKS or self.agents[1]['pos'] in MockPigChaseEnvironment.LAPIS_BLOCKS:
                if self.debug_level > 0:
                    print("Lapis end")
                return True
            if self.agents[0]['steps'] == 0 or self.agents[1]['steps'] == 0:
                if self.debug_level > 0:
                    print("Step end")
                return True

        return False

    def _construct_mission(self):
        # set agent helmet
        # set agent starting pos
        pos = [MockPigChaseEnvironment.VALID_START_POSITIONS[i]
               for i in np.random.choice(
                range(len(MockPigChaseEnvironment.VALID_START_POSITIONS)),
                3, replace=False)]
        while not (self._get_pos_dist(pos[0], pos[1]) > 1.1 and
                           self._get_pos_dist(pos[1], pos[2]) > 1.1 and
                           self._get_pos_dist(pos[0], pos[2]) > 1.1):
            pos = [MockPigChaseEnvironment.VALID_START_POSITIONS[i]
                   for i in np.random.choice(
                    range(len(MockPigChaseEnvironment.VALID_START_POSITIONS)),
                    3, replace=False)]
        pos[0] = (2, 1)
        pos[1] = (2, 3)
        pos[2] = (2, 3)
        self.pig = {'name': 'Pig', 'pos': pos[0], 'dir': 0}
        self.agents = []
        self.agents.append({'name': 'Agent_1', 'pos': pos[1], 'dir': 90, 'steps': 25})
        self.agents.append({'name': 'Agent_2', 'pos': pos[2], 'dir': 90, 'steps': 25})
        self.entities = [self.agents[0], self.agents[1], self.pig]
        self.current_player = np.random.randint(0, 1)
        self.start_player = self.current_player

        if self.debug_level > 0:
            print("Reset env", self.agents[0]['pos'][0], self.agents[0]['pos'][1], self.agents[0]['dir'], self.agents[1]['pos'][0], self.agents[1]['pos'][1], self.agents[1]['dir'])

    def _get_pos_dist(self, pos1, pos2):
        return np.sqrt((pos1[0] - pos2[0]) ** 2 + (pos1[1] - pos2[1]) ** 2)

    def reset(self, agent_type=None, agent_positions=None):
        """ Overrides reset() to allow changes in agent appearance between
        missions."""

        self._construct_mission()

        return self.get_state()
       # return super(PigChaseEnvironment, self).reset()

    def get_dir_vec(self, dir):
        if dir == 0:
            return [0, 1]
        elif dir == 90:
            return [-1, 0]
        elif dir == 180:
            return [0, -1]
        elif dir == 270:
            return [1, 0]
        else:
            raise ValueError("Dir not allowed")

    def do(self, action):
        """
        Do the action
        """
        if self.debug_level > 1:
            print("Player", self.current_player, "Action", action)

        if action == 0:
            new_pos = (self.agents[self.current_player]['pos'][0] + self.get_dir_vec(self.agents[self.current_player]['dir'])[0],
                       self.agents[self.current_player]['pos'][1] + self.get_dir_vec(self.agents[self.current_player]['dir'])[1])

            if new_pos in self.VALID_START_POSITIONS or new_pos in self.LAPIS_BLOCKS:
                self.agents[self.current_player]['pos'] = new_pos
        elif action == 1:
            self.agents[self.current_player]['dir'] = self.agents[self.current_player]['dir'] - 90
            if self.agents[self.current_player]['dir'] < 0:
                self.agents[self.current_player]['dir'] += 360
        elif action == 2:
            self.agents[self.current_player]['dir'] = (self.agents[self.current_player]['dir'] + 90) % 360

        self.agents[self.current_player]['steps'] -= 1

        reward = self._get_reward(self.agents[self.current_player])

        self.current_player = 1 - self.current_player
        return self.get_state(), -1 + reward, False

    def _get_reward(self, agent):
        if agent['pos'] in MockPigChaseEnvironment.LAPIS_BLOCKS:
            return 5
        else:
            return 0

    def _get_board_state(self):
        if self._buffer_cache is None:
            buffer_shape = (len(self.DEFAULT_BOARD) * 2, len(self.DEFAULT_BOARD[0]) * 2)
            buffer = np.zeros(buffer_shape, dtype=np.float32)

            for x in range(0, len(self.DEFAULT_BOARD)):
                for y in range(0, len(self.DEFAULT_BOARD[0])):
                    mapped_value = self.GRAY_PALETTE[self.DEFAULT_BOARD[x][y]]
                    # draw 4 pixels per block
                    buffer[x * 2:x * 2 + 2, y * 2:y * 2 + 2] = mapped_value

            self._buffer_cache = buffer

        return self._buffer_cache.copy()

    def get_state(self):
        buffer = self._get_board_state()

        for agent in self.entities:
            agent_x = int(agent['pos'][0])
            agent_z = int(agent['pos'][1]) + 1
            agent_pattern = buffer[agent_z * 2:agent_z * 2 + 2, agent_x * 2:agent_x * 2 + 2]

            # convert Minecraft yaw to 0=north, 1=west etc.
            agent_direction = ((((int(agent['dir']) - 45) % 360) // 90) - 1) % 4

            if agent['name'] == 'Pig':
                agent_name = agent['name']
            else:
                if self.agents[self.current_player] == agent:
                    agent_name = 'Agent_2'
                else:
                    agent_name = 'Agent_1'

            if agent_direction == 0:
                # facing north
                agent_pattern[1, 0:2] = self.GRAY_PALETTE[agent_name]
                agent_pattern[0, 0:2] += self.GRAY_PALETTE[agent_name]
                agent_pattern[0, 0:2] /= 2.
            elif agent_direction == 1:
                # west
                agent_pattern[0:2, 1] = self.GRAY_PALETTE[agent_name]
                agent_pattern[0:2, 0] += self.GRAY_PALETTE[agent_name]
                agent_pattern[0:2, 0] /= 2.
            elif agent_direction == 2:
                # south
                agent_pattern[0, 0:2] = self.GRAY_PALETTE[agent_name]
                agent_pattern[1, 0:2] += self.GRAY_PALETTE[agent_name]
                agent_pattern[1, 0:2] /= 2.
            else:
                # east
                agent_pattern[0:2, 0] = self.GRAY_PALETTE[agent_name]
                agent_pattern[0:2, 1] += self.GRAY_PALETTE[agent_name]
                agent_pattern[0:2, 1] /= 2.

            buffer[agent_z * 2:agent_z * 2 + 2,  agent_x * 2:agent_x * 2 + 2] = agent_pattern

        if self.debug_level > 1:
            for y in range(0, len(self.DEFAULT_BOARD)):
                text = ""
                for x in range(0, len(self.DEFAULT_BOARD[0])):

                    skip = False
                    for agent in self.entities:
                        agent_x = int(agent['pos'][0])
                        agent_z = int(agent['pos'][1]) + 1
                        if x == agent_x and y == agent_z:
                            if agent['name'] == "Pig":
                                text += " P "
                            else:
                                if agent['dir'] == 0:
                                    text += " v "
                                elif agent['dir'] == 90:
                                    text += " < "
                                elif agent['dir'] == 180:
                                    text += " ^ "
                                elif agent['dir'] == 270:
                                    text += " > "

                            skip = True
                            break

                    if not skip:
                        if self.DEFAULT_BOARD[y][x] == u'sand':
                            text += " + "
                        elif self.DEFAULT_BOARD[y][x] == u'grass':
                            text += "   "
                        elif self.DEFAULT_BOARD[y][x] == u'lapis_block':
                            text += " O "
                print(text)

        return buffer / 255

