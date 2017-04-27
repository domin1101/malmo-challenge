import random

from agent import EvolutionAgent
from model import MLPTensor, NeuralNetwork


class Evolution:

    def __init__(self, visualizer, env, select_count=150, mutation_factor=1.8, create_up_to=250):
        self._select_count = select_count
        self._mutation_factor = mutation_factor
        self._create_up_to = create_up_to
        self._env = env
        self._visualizer = visualizer

    def create(self, name):
        chain = MLPTensor(18 * 18, self._env.available_actions, [32])
        model = NeuralNetwork(chain, -1)
        return EvolutionAgent(name, self._env.available_actions, model, self._visualizer)

    def executeFitnessFunction(self, agents):
        for agent in agents:
            agent.fitness = sum(agent.rewards)

    def select(self, agents):
        return agents[:-self._select_count]

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
            agents.append(self.create(agents[-1].name + 1))
        return agents

    def processGeneration(self, agents):
        self.executeFitnessFunction(agents)

        agents.sort(key=lambda x: x.fitness, reverse=True)

        agents = self.select(agents)
        agents = self.mutate(agents)
        agents = self.create_new(agents, self._create_up_to)

        print(agents[0].fitness, agents[1].fitness)
