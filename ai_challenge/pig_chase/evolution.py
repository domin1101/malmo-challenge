import random

from ai_challenge.pig_chase.agent import EvolutionAgent
from ai_challenge.pig_chase.model import MLPTensor, NeuralNetwork


class Evolution:

    def __init__(self, visualizer, env):
        self._selectCount = 2
        self._mutationFactor = 2
        self._env = env
        self._visualizer = visualizer

    def create(self, name):
        chain = MLPTensor(18 * 18, self._env.available_actions, 128)
        model = NeuralNetwork(chain, -1)
        return EvolutionAgent(name, self._env.available_actions, model, self._visualizer)

    def executeFitnessFunction(self, agents):
        for agent in agents:
            agent.fitness = sum(agent.rewards)

    def select(self, agents):
        return agents[:-2]

    def mutate(self, agents):
        startSize = len(agents)
        while len(agents) < startSize * 2:
            agents.append(self.mutateAgent(agents[random.randint(0, startSize - 1)]))
        return agents

    def mutateAgent(self, agent):
        newAgent = self.create(agent.name + 1)
        newAgent.copyParametersFrom(agent)
        return agent

    def processGeneration(self, agents):
        self.executeFitnessFunction(agents)

        agents.sort(key=lambda x: x.fitness, reverse=True)

        agents = self.select(agents)
        agents = self.mutate(agents)

        print(agents[0].fitness, agents[1].fitness)
