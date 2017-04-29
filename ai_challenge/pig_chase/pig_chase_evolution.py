# Copyright (c) 2017 Microsoft Corporation.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
# documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
#  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of
# the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
# THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
#  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# ===================================================================================================================

import numpy as np
import os
import sys
import tensorflow as tf

from argparse import ArgumentParser
from datetime import datetime
from os import path
from threading import Thread, active_count
from time import sleep

from mockenvironment import MockPigChaseEnvironment
from evolution import Evolution
from model import NeuralNetwork, MLPTensor
from malmopy.agent import RandomAgent
try:
    from malmopy.visualization.tensorboard import TensorboardVisualizer
    from malmopy.visualization.tensorboard.cntk import CntkConverter
except ImportError:
    print('Cannot import tensorboard, using ConsoleVisualizer.')

from malmopy.visualization import ConsoleVisualizer
from common import parse_clients_args, visualize_evolution, ENV_AGENT_NAMES, ENV_TARGET_NAMES
from agent import PigChaseChallengeAgent, FocusedAgent, EvolutionAgent
from environment import PigChaseEnvironment, PigChaseSymbolicStateBuilder, PigChaseTopDownStateBuilder

# Enforce path
sys.path.insert(0, os.getcwd())
sys.path.insert(1, os.path.join(os.path.pardir, os.getcwd()))

BASELINES_FOLDER = 'results/baselines/pig_chase/%s/%s'
EPOCH_SIZE = 100


def agent_factory(name, role, baseline_agent, clients, matches, logdir, visualizer, agents, env):

    assert len(clients) >= 2, 'Not enough clients (need at least 2)'
    clients = parse_clients_args(clients)

    agent_index = 0

    env = PigChaseEnvironment(clients, PigChaseTopDownStateBuilder(True), role=role, randomize_positions=True)

    obs = env.reset()
    reward = 0
    agent_done = False
    viz_rewards = []

    agent = agents[agent_index]
    match = 0
    while True:

        # check if env needs reset
        if env.done:

            visualize_evolution(visualizer, role, agent_index, viz_rewards)

            if role == 0:
                agents[agent_index].rewards.append(sum(viz_rewards))
                match = match + 1

            if role != 0 or match >= matches:
                if role == 0:
                    match = 0

                agent_index = agent_index + 1
                if agent_index >= len(agents):
                    if role == 0:
                        break
                    else:
                        match = match + 1
                        if match >= matches:
                            break
                        else:
                            agent_index = 0

                agent = agents[agent_index]

            viz_rewards = []
            obs = env.reset()

        # select an action
        action = agent.act(obs, reward, agent_done, is_training=True)
        # take a step
        obs, reward, agent_done = env.do(action)
        viz_rewards.append(reward)



def agent_factory_mock(population, parasites, env):

    reward = 0
    agent_done = False

    for key1, agent1 in enumerate(population):
        for key2, agent2 in enumerate(parasites):
            agents = [agent1, agent2]

            obs = env.reset()
            viz_rewards = [[], []]

            while not env.done:
                action = agents[env.current_player].act(obs, reward, agent_done, is_training=True)
                obs, reward, agent_done = env.do(action)
                viz_rewards[env.current_player].append(reward)

            agents[0].matches.append({'own_reward': sum(viz_rewards[0]), 'other_reward': sum(viz_rewards[1])})
            agents[1].matches.append({'own_reward': sum(viz_rewards[1]), 'other_reward': sum(viz_rewards[0])})

            visualize_evolution(visualizer, role, key1, viz_rewards[0])
            visualize_evolution(visualizer, role, key2, viz_rewards[1])


def reset_agents(agents):
    for agent in agents:
        agent.matches = []


def run_experiment(threads, fast):
    assert len(threads) == 2, 'Not enough agents (required: 2, got: %d)'\
                % len(threads)

    sess = tf.Session()

    population = []
    parasites = []

    if fast:
        env = MockPigChaseEnvironment(args.clients, PigChaseTopDownStateBuilder(True), role=0, randomize_positions=True)
    else:
        env = PigChaseEnvironment(args.clients, PigChaseTopDownStateBuilder(True), role=0, randomize_positions=True)

    evolution = Evolution(visualizer, env, sess)

    current_pop1 = population
    current_pop2 = parasites

    while True:
        population = evolution.process_generation(population)

        sess.run(tf.global_variables_initializer())

        sample = evolution.combine(parasites, 5)

        reset_agents(population)
        reset_agents(parasites)

        if type(env) != MockPigChaseEnvironment:
            processes = []
            for thread in threads:
                thread['env'] = env

                if thread['role'] == 0:
                    thread['agents'] = current_pop1
                    thread['matches'] = len(current_pop2)/2
                else:
                    thread['agents'] = current_pop2[:len(current_pop2)/2]
                    thread['matches'] = len(current_pop1)

                p = Thread(target=agent_factory, kwargs=thread)
                p.daemon = True
                p.start()

                # Give the server time to start
                if thread['role'] == 0:
                    sleep(1)

                processes.append(p)

            try:
                # wait until only the challenge agent is left
                while processes[0].isAlive() or processes[1].isAlive():
                    sleep(0.1)
            except KeyboardInterrupt:
                print('Caught control-c - shutting down.')
        else:
            agent_factory_mock(population, sample, env)

        print("Process generation")
        population = evolution.evaluate_generation(population)

        population, parasites = parasites, population



if __name__ == '__main__':
    arg_parser = ArgumentParser('Pig Chase baseline experiment')
    arg_parser.add_argument('-t', '--type', type=str, default='astar',
                            choices=['astar', 'random'],
                            help='The type of baseline to run.')
    arg_parser.add_argument('-e', '--epochs', type=int, default=5,
                            help='Number of epochs to run.')
    arg_parser.add_argument('--fast', dest='fast', action='store_true')
    arg_parser.add_argument('clients', nargs='*',
                            default=['127.0.0.1:10000', '127.0.0.1:10001'],
                            help='Minecraft clients endpoints (ip(:port)?)+')
    args = arg_parser.parse_args()

    logdir = BASELINES_FOLDER % (args.type, datetime.utcnow().isoformat())
    if 'malmopy.visualization.tensorboard' in sys.modules and False:
        visualizer = TensorboardVisualizer()
        visualizer.initialize(logdir, None)
    else:
        visualizer = ConsoleVisualizer()


    threads = [{'name': agent, 'role': role, 'baseline_agent': args.type,
               'clients': args.clients,
               'logdir': logdir, 'visualizer': visualizer}
              for role, agent in enumerate(ENV_AGENT_NAMES)]

    run_experiment(threads, args.fast)

