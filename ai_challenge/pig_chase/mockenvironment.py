
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
                 human_speed=False, randomize_positions=False):

        super(MockPigChaseEnvironment, self).__init__(remotes, state_builder, actions, role, exp_name, human_speed, randomize_positions)
        self.pig = []
        self.agents = []
        self.lock = Lock()
        self.acquire_lock = Lock()
        self.is_waiting = Semaphore(0)
        self.first_lock = True
        self.first_round = True
        self.first_end = True

    @property
    def state(self):
        return self._user_defined_builder.build(self)

    @property
    def done(self):
        """
        Done if we have caught the pig
        """
        return self.agents[0]['steps'] == 0 or self.agents[1]['steps'] == 0

    def _construct_mission(self):
        # set agent helmet
        # set agent starting pos
        pos = [PigChaseEnvironment.VALID_START_POSITIONS[i]
               for i in np.random.choice(
                range(len(PigChaseEnvironment.VALID_START_POSITIONS)),
                3, replace=False)]
        while not (self._get_pos_dist(pos[0], pos[1]) > 1.1 and
                           self._get_pos_dist(pos[1], pos[2]) > 1.1 and
                           self._get_pos_dist(pos[0], pos[2]) > 1.1):
            pos = [PigChaseEnvironment.VALID_START_POSITIONS[i]
                   for i in np.random.choice(
                    range(len(PigChaseEnvironment.VALID_START_POSITIONS)),
                    3, replace=False)]
        self.pig = {'name': 'Pig', 'pos': pos[0], 'dir': 0}
        self.agents = []
        self.agents.append({'name': 'Agent_1', 'pos': pos[1], 'dir': np.random.choice(PigChaseEnvironment.VALID_YAW), 'steps': 25})
        self.agents.append({'name': 'Agent_2', 'pos': pos[2], 'dir': np.random.choice(PigChaseEnvironment.VALID_YAW), 'steps': 25})
        self.entities = [self.agents[0], self.agents[1], self.pig]
        self.start_player = 0
        self.first_end = True

        print("Reset env")

    def _get_pos_dist(self, pos1, pos2):
        return np.sqrt((pos1[0] - pos2[0]) ** 2 + (pos1[1] - pos2[1]) ** 2)

    def reset(self, role, agent_type=None, agent_positions=None):
        """ Overrides reset() to allow changes in agent appearance between
        missions."""
        print("in")

        if not self.first_round:
            self.lock.release()
            with self.acquire_lock:
                self.is_waiting.release()
                self.lock.acquire()
            self.is_waiting.acquire()

        if self.first_round and role == 0 or self.done:
            self._construct_mission()

        if not self.first_round:
            self.lock.release()
        with self.acquire_lock:
            if not self.first_lock:
                self.is_waiting.release()
            else:
                self.first_lock = False
            self.lock.acquire()
        self.is_waiting.acquire()

        return self.get_state()
       # return super(PigChaseEnvironment, self).reset()

    def get_dir_vec(self, dir):
        if dir == 0:
            return [1, 0]
        elif dir == 90:
            return [0, 1]
        elif dir == 180:
            return [-1, 0]
        elif dir == 270:
            return [0, -1]
        else:
            raise ValueError("Dir not allowed")

    def do(self, action, role):
        """
        Do the action
        """
        print("Player", role, "Action", action)

        if action == 0:
            new_pos = []
            new_pos.append(self.agents[role]['pos'][0] + self.get_dir_vec(self.agents[role]['dir'])[0])
            new_pos.append(self.agents[role]['pos'][1] + self.get_dir_vec(self.agents[role]['dir'])[1])

            if new_pos in self.VALID_START_POSITIONS or new_pos in self.LAPIS_BLOCKS:
                self.agents[role]['pos'] = new_pos
        elif action == 1:
            self.agents[role]['dir'] = self.agents[role]['dir'] - 90
            if self.agents[role]['dir'] < 0:
                self.agents[role]['dir'] += 360
        elif action == 2:
            self.agents[role]['dir'] = (self.agents[role]['dir'] + 90) % 360

        self.agents[role]['steps'] -= 1

        self.lock.release()
        with self.acquire_lock:
            self.is_waiting.release()
            self.lock.acquire()
        self.is_waiting.acquire()

        self.first_round = False

        return self.get_state(), -1, False

    def end(self):
        if self.first_end:
            self.first_end = False
            self.lock.release()
            self.is_waiting.release()
        else:
            self.lock.release()
        self.first_round = True
        self.first_lock = True

    def get_state(self):
        buffer_shape = (len(self.DEFAULT_BOARD) * 2, len(self.DEFAULT_BOARD[0]) * 2)
        buffer = np.zeros(buffer_shape, dtype=np.float32)

        for x in range(0, len(self.DEFAULT_BOARD)):
            for y in range(0, len(self.DEFAULT_BOARD[0])):
                mapped_value = self.GRAY_PALETTE[self.DEFAULT_BOARD[x][y]]
                # draw 4 pixels per block
                buffer[x * 2:x * 2 + 2,
                        y * 2:y * 2 + 2] = mapped_value


        for agent in self.entities:
            agent_x = int(agent['pos'][0])
            agent_z = int(agent['pos'][1]) + 1
            agent_pattern = buffer[agent_z * 2:agent_z * 2 + 2,
                                   agent_x * 2:agent_x * 2 + 2]

            # convert Minecraft yaw to 0=north, 1=west etc.
            agent_direction = ((((int(agent['dir']) - 45) % 360) // 90) - 1) % 4

            if agent_direction == 0:
                # facing north
                agent_pattern[1, 0:2] = self.GRAY_PALETTE[agent['name']]
                agent_pattern[0, 0:2] += self.GRAY_PALETTE[agent['name']]
                agent_pattern[0, 0:2] /= 2.
            elif agent_direction == 1:
                # west
                agent_pattern[0:2, 1] = self.GRAY_PALETTE[agent['name']]
                agent_pattern[0:2, 0] += self.GRAY_PALETTE[agent['name']]
                agent_pattern[0:2, 0] /= 2.
            elif agent_direction == 2:
                # south
                agent_pattern[0, 0:2] = self.GRAY_PALETTE[agent['name']]
                agent_pattern[1, 0:2] += self.GRAY_PALETTE[agent['name']]
                agent_pattern[1, 0:2] /= 2.
            else:
                # east
                agent_pattern[0:2, 0] = self.GRAY_PALETTE[agent['name']]
                agent_pattern[0:2, 1] += self.GRAY_PALETTE[agent['name']]
                agent_pattern[0:2, 1] /= 2.

            buffer[agent_z * 2:agent_z * 2 + 2,
                   agent_x * 2:agent_x * 2 + 2] = agent_pattern

        return buffer / 255

