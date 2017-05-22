# Coevolution - Malmo Collaborative AI Challenge

* Team name: Pathfinder
* Strategy: Coevolution
* Frameworks: LightBulb(Training) and Tensorflow(Evaluation)

## Short description

My approach for solving this challenge was to use coevolution. I know such algorithms are not used that often these days, but I think they can work really good especially in such two agents situations. In the core coevolution acts like evolutionary algorithms, but instead of using just one population of individuals coevolution uses two. One is called "parasites" and the other just "population". For the Malmo challenge, I used the parasites to setup the environment. So their goal is to find starting positions for the agents and the pig which resolve in a very hard challenge. The other population now tries with the help of a neural network to get as much rewards as possible out of those given environments (This is also the population where the final agent comes from). Both populations execute alternating a standard evolutionary algorithm (selection, mutation, recombination etc.). In the optimal case both population process slowly, but steady to achieve more and more rewards and the in the end you hopefully get an agent which has been tested against all kinds of environment constellations.

## Advantages of the algorithm

* Highly parallelizable
* No derivation required => Works with every type of network
* Could also be useful to train agents for collaborate with every kind of opponent (In my implementation the parasite is always the ChallengeAgent, but the parasite could theoretically also be guided by a neural network)
* Less hyperparameters

## Evolutionary algorithm per population

Every population runs the same algorithm which has the following main steps:

### Creation

At first a few new individuals are created with random parameters.

### Mutation & Recombination

Now the best individuals from the last iteration are mutated and combined to create new individuals which base on good existing ones.

### Evaluation

Then all individuals play a few matches against a subset of the best agents from the other population.

### Selection

After that, the best ones are selected and the other ones get deleted. Now the algorithm starts again from the top.

## Fitness values

Fitness values are used to determine how good an agent is and to compare it with other agents. When using coevolution the calculation of fitness values is one of the most important steps. In this scenario you cannot determine how good an agent was by just comparing its reward with the reward of its opponent, as this would not result in cooperation. In this case different fitness functions were necessary for parasites and non-parasites (They are calculated matchwise and then summarized over all matches of an agent):

Non-Parasites: Just the own total reward in the match. The higher its reward, the better is an agent. This is also the goal in the Challenge itself.

Parasites: Here comes the trick: Difference between maximum opponent reward and opponent reward in this match. In this way we reward parasites against which a high reward is possible, but currently not often achieved!

## Stabilization

The algorithm described above would not work in this way as it would be to unstable. To keep up diversity in the populations and to make sure not to remove individuals which do not have the best total score, but which can win against rarely beaten opponents, i used "Competitive Fitness Sharing" and "Shared Sampling". Those methods keep sure to reward individuals which seem to have unique abilities. As the original algorithms do not work with rewards, but just with the information which player won the game, I had to adjust the original algorithms to also make use the information given by different rewards.

## Speedup

Those evolutionary processes require that a few million matches are being played. I had to speedup learning process as my computer does not have the required capabilities to run the algorithm highly parallelized ;). So I created a mocking environment which does not use socket communication and used my little ML framework "LightBulb" for neural networks and the learning process. In contrast to e.q. Tensorflow LightBulb is optimized for fast executing small neural networks on CPUs which also gave me a huge speedup.
Also to keep the network small i used only a few input parameters: position and direction of both agents and the pig in binary form.

## Results

At the end I didn't had enough time to do much hyperparameter optimization and to try bigger neural networks, but at least I got an agent which at first tries to chase the pig in cooperation with the opponent and then if that is not possible, uses the lapis exit.

### Agent details:

* Network structure: 32 - 64 - 3
* Trained for 8000 iterations (> 100 million matches) 
* Tensorflow required

Results (100k and 500k are the same agents):
```
{"500k": {"var": 22.225563068538591, "count": 1383, "mean": -0.0057845263919016629}, "100k": {"var": 21.548280463682879, "count": 1378, "mean": -0.032656023222060959}, "experimentname": "My Exp 1"}
```

How to evaluate:
```
python ai_challenge/pig_chase/pig_chase_eval_evolution.py
```

How to use the agent in general (uses PigChaseSymbolicStateBuilder):
```
from model import MLPTensor, NeuralNetwork
from agent import EvolutionAgent

chain = MLPTensor("final_agent.json")
model = NeuralNetwork(chain)
agent = EvolutionAgent(ENV_AGENT_NAMES[1], 3, model)
```

Video - Here you can see a video of my agent playing against the ChallengeAgent:
https://www.youtube.com/watch?v=0njImwKbvbQ

## Future

I think this solution has more potential than the results may show. Especially when trying to train an agent to collaborate without having something like a challenge agent coevolution would be very interesting.
