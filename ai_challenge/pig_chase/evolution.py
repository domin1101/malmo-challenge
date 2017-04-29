import random

from agent import EvolutionAgent
from model import MLPTensor, NeuralNetwork


class Evolution:
    def __init__(self, visualizer, env, sess, select_count=15, mutation_factor=1.8, create_up_to=25):
        self._select_count = select_count
        self._mutation_factor = mutation_factor
        self._create_up_to = create_up_to
        self._env = env
        self._visualizer = visualizer
        self.sess = sess

    def create(self, name):
        chain = MLPTensor(18 * 18, self._env.available_actions, [32], self.sess)
        model = NeuralNetwork(chain, -1)
        return EvolutionAgent(name, self._env.available_actions, model, self._visualizer)

    def fitness(self, agents):
        win_counter = {}

        for agent in agents:
            for key, match in enumerate(agent.matches):
                if match['own_reward'] > match['other_reward']:
                    if key not in win_counter:
                        win_counter[key] = 1
                    else:
                        win_counter[key] += 1

        for agent in agents:
            agent.fitness = 0
            for key, match in enumerate(agent.matches):
                if match['own_reward'] > match['other_reward']:
                    agent.fitness += 1.0 / win_counter[key]

    def select(self, agents):
        return agents[:self._select_count]

    def mutate(self, agents):
        startSize = len(agents)
        while len(agents) < startSize * self._mutation_factor:
            agents.append(self.mutateAgent(agents[random.randint(0, startSize - 1)]))
        return agents

    def mutateAgent(self, agent):
        newAgent = self.create(agent.name + 1)
        newAgent.copyParametersFrom(agent)
        newAgent.mutate()
        return agent

    def create_new(self, agents, up_to):
        while len(agents) < up_to:
            name = 0
            if len(agents) > 0:
                name = agents[-1].name + 1
            agents.append(self.create(name))
        return agents

    def process_generation(self, agents):
        if len(agents) > 0:
            agents = self.mutate(agents)

        agents = self.create_new(agents, self._create_up_to)

        return agents

    def evaluate_generation(self, agents):
        self.fitness(agents)

        agents.sort(key=lambda x: x.fitness, reverse=True)

        agents = self.select(agents)

        if len(agents) > 2:
            print(agents[0].fitness, agents[1].fitness)

        return agents

    def combine(self, parasites, sample_size):
        sample = []
        beat = {}

        while len(sample) < sample_size:
            best_individual = None
            best_fitness = 0
            for parasite in parasites:
                if parasite not in sample:
                    fitness = 0
                    for key, match in enumerate(parasite.matches):
                        if match['own_reward'] > match['other_reward']:
                            fitness += 1.0 / (1 + beat.get(key, 0))
                    if best_individual is None or fitness > best_fitness:
                        best_individual = parasite
                        best_fitness = fitness
            if best_individual is None:
                break

            sample.append(best_individual)
            for key, match in enumerate(best_individual.matches):
                if match['own_reward'] > match['other_reward']:
                    if key not in beat:
                        beat[key] = 0
                    beat[key] += 1

        return sample

