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
        self.unused_individuals = []
        self.next_name = 0

    def _create(self):
        chain = MLPTensor(18 * 18, self._env.available_actions, [32], self.sess)
        model = NeuralNetwork(chain, -1)
        self.next_name = self.next_name + 1
        return EvolutionAgent(self.next_name, self._env.available_actions, model, self._visualizer)

    def has_won(self, match, is_parasite):
        return match['own_reward'] > match['other_reward'] or match['own_reward'] == match['other_reward'] and not is_parasite

    def fitness(self, agents, is_parasite):
        win_counter = {}

        for agent in agents:
            for key, match in enumerate(agent.matches):
                if self.has_won(match, is_parasite):
                    if key not in win_counter:
                        win_counter[key] = 1
                    else:
                        win_counter[key] += 1

        for agent in agents:
            agent.fitness = 0
            for key, match in enumerate(agent.matches):
                if self.has_won(match, is_parasite):
                    agent.fitness += 1.0 / win_counter[key]

    def _get_new_individual(self, force_random=False):
        if len(self.unused_individuals) > 0:
            individual = self.unused_individuals.pop()
            if force_random:
                individual.randomize()
            return individual
        else:
            return self._create()

    def select(self, agents):
        print("Selecting %d individuals" % self._select_count)
        for i in range(self._select_count, len(agents)):
            self.unused_individuals.append(agents[i])

        return agents[:self._select_count]

    def mutate(self, agents):
        startSize = len(agents)
        print("Mutating %d individuals" % (startSize * self._mutation_factor - len(agents)))

        while len(agents) < startSize * self._mutation_factor:
            agents.append(self.mutateAgent(agents[random.randint(0, startSize - 1)]))
        return agents

    def mutateAgent(self, agent):
        newAgent = self._get_new_individual()
        newAgent.mutate_and_assign(agent)
        return newAgent

    def create_new(self, agents, up_to):
        print("Creating %d new individuals" % (up_to - len(agents)))
        while len(agents) < up_to:
            agents.append(self._get_new_individual(True))
        return agents

    def process_generation(self, agents):
        if len(agents) > 0:
            agents = self.mutate(agents)

        agents = self.create_new(agents, self._create_up_to)

        return agents

    def evaluate_generation(self, agents, is_parasite):
        self.fitness(agents, is_parasite)

        agents.sort(key=lambda x: x.fitness, reverse=True)

        agents = self.select(agents)

        if len(agents) > 2:
            print(agents[0].fitness, agents[1].fitness)

        return agents

    def combine(self, parasites, sample_size, is_parasite):
        sample = []
        beat = {}

        while len(sample) < sample_size:
            best_individual = None
            best_fitness = 0
            for parasite in parasites:
                if parasite not in sample:
                    fitness = 0
                    for key, match in enumerate(parasite.matches):
                        if self.has_won(match, is_parasite):
                            fitness += 1.0 / (1 + beat.get(key, 0))
                    if best_individual is None or fitness > best_fitness:
                        best_individual = parasite
                        best_fitness = fitness
            if best_individual is None:
                break

            sample.append(best_individual)
            for key, match in enumerate(best_individual.matches):
                if self.has_won(match, is_parasite):
                    if key not in beat:
                        beat[key] = 0
                    beat[key] += 1

        return sample

