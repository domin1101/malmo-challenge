#include "MalmoEvolution.hpp"
#include "MinecraftFactory.hpp"
#include <LightBulb/Learning/Evolution/ConstantMutationCommand.hpp>
#include <LightBulb/Learning/Evolution/EvolutionStrategy/MutationAlgorithm.hpp>
#include <LightBulb/Learning/Evolution/BestSelectionCommand.hpp>
#include <LightBulb/Learning/Evolution/ConstantReuseCommand.hpp>
#include <LightBulb/Learning/Evolution/ConstantRecombinationCommand.hpp>
#include <LightBulb/Learning/Evolution/EvolutionStrategy/RecombinationAlgorithm.hpp>
#include <LightBulb/Learning/Evolution/ConstantCreationCommand.hpp>
#include <LightBulb/Learning/Evolution/EvolutionLearningRule.hpp>
#include <LightBulb/Learning/Evolution/RandomSelector.hpp>
#include <PongEvolution/Minecraft.hpp>
#include <LightBulb/Learning/Evolution/SharedCoevolutionFitnessFunction.hpp>
#include <LightBulb/Learning/Evolution/PerfectIndividualFoundCondition.hpp>
#include <LightBulb/Learning/Evolution/CoevolutionLearningRule.hpp>
#include <fstream>
#include <LightBulb/Learning/Evolution/BestReuseSelector.hpp>
#include <LightBulbApp/TrainingPlans/Preferences/DoublePreference.hpp>
#include <LightBulbApp/TrainingPlans/Preferences/IntegerPreference.hpp>
#include <LightBulbApp/TrainingPlans/Preferences/BooleanPreference.hpp>
#include <LightBulb/NeuronDescription/SameNeuronDescriptionFactory.hpp>
#include <LightBulb/Function/InputFunction/WeightedSumFunction.hpp>
#include <LightBulb/Function/ActivationFunction/BinaryFunction.hpp>
#include <LightBulb/NeuronDescription/NeuronDescription.hpp>
#include <LightBulb/Learning/Evolution/MagnitudeBasedPruningMutationAlgorithm.hpp>
#include <LightBulb/Learning/Evolution/NetworkGrowMutationAlgorithm.hpp>
#include <LightBulb/Learning/Evolution/PhasedTopologyMutationAlgorithm.hpp>
#include <LightBulb/Learning/Evolution/RandomHallOfFameAlgorithm.hpp>

#define PREFERENCE_POPULATION_SIZE "Population size"
#define PREFERENCE_MUTATION_PERCENTAGE "Mutation percentage"
#define PREFERENCE_TOPOLOGY_MUTATION_PERCENTAGE "Topology mutation percentage"
#define PREFERENCE_RECOMBINATION_PERCENTAGE "Recombination percentage"
#define PREFERENCE_COMPETITIONS_SIZE "Competitions per individual"
#define PREFERENCE_HALLOFFAME_COMPETITIONS_SIZE "Hall of fame competitions"
#define PREFERENCE_SHORTCUT_ENABLE "Enable shortcut connections"
#define PREFERENCE_NEURON_COUNT_FIRST_LAYER "Neuron count in 1. layer"
#define PREFERENCE_SECOND_LAYER_ENABLE "Enable 2. layer"
#define PREFERENCE_NEURON_COUNT_SECOND_LAYER "Neuron count in 2. layer"
#define PREFERENCE_MUTATIONSTRENGTH_CHANGESPEED "Mutationstrength changespeed"
#define PREFERENCE_WEIGHTDECAY_FAC "Weight decay fac"
#define PREFERENCE_CREATE_UP_TO "Create up to"

using namespace LightBulb;

AbstractLearningRule* MalmoEvolution::createLearningRate()
{

	EvolutionLearningRuleOptions options;
	
	options.creationCommands.push_back(new ConstantCreationCommand(getIntegerPreference(PREFERENCE_CREATE_UP_TO)));
	options.exitConditions.push_back(new PerfectIndividualFoundCondition(1000));
	options.reuseCommands.push_back(new ConstantReuseCommand(new BestReuseSelector(), 16));
	options.selectionCommands.push_back(new BestSelectionCommand(getIntegerPreference(PREFERENCE_POPULATION_SIZE)));
	options.mutationsCommands.push_back(new ConstantMutationCommand(new MutationAlgorithm(getDoublePreference(PREFERENCE_MUTATIONSTRENGTH_CHANGESPEED)), new RandomSelector(new RankBasedRandomFunction()), getDoublePreference(PREFERENCE_MUTATION_PERCENTAGE)));
	options.recombinationCommands.push_back(new ConstantRecombinationCommand(new RecombinationAlgorithm(), new RandomSelector(new RankBasedRandomFunction()), getDoublePreference(PREFERENCE_RECOMBINATION_PERCENTAGE)));
	//options.mutationsCommands.push_back(new ConstantMutationCommand(new MagnitudeBasedPruningMutationAlgorithm(1, 0, true, true), new RandomSelector(new RankBasedRandomFunction()), getDoublePreference(PREFERENCE_TOPOLOGY_MUTATION_PERCENTAGE)));
	//options.fitnessFunctions.push_back(new NeuronDecayFitnessFunction(getDoublePreference(PREFERENCE_WEIGHTDECAY_FAC)));
	std::vector<unsigned int> maxNeuronsPerLayer(4);
	maxNeuronsPerLayer[0] = 6;
	maxNeuronsPerLayer[1] = 100;
	maxNeuronsPerLayer[2] = 100;
	maxNeuronsPerLayer[3] = 2;
	//options.mutationsCommands.push_back(new ConstantMutationCommand(new PhasedTopologyMutationAlgorithm(new MagnitudeBasedPruningMutationAlgorithm(1, 0, true, true), new NetworkGrowMutationAlgorithm(maxNeuronsPerLayer), 70), new RandomSelector(new RankBasedRandomFunction()), getDoublePreference(PREFERENCE_TOPOLOGY_MUTATION_PERCENTAGE)));

	//options.fitnessFunctions.push_back(new FitnessSharingFitnessFunction(150));

	fillDefaultEvolutionLearningRule1Options(options);

	//options.recombinationCommands.push_back(new ConstantRecombinationCommand(7));

	EvolutionLearningRule* learningRule1 = new EvolutionLearningRule(options);
	fillDefaultEvolutionLearningRule2Options(options);
	EvolutionLearningRule* learningRule2 = new EvolutionLearningRule(options);

	CoevolutionLearningRuleOptions coevolutionLearningRuleOptions;
	coevolutionLearningRuleOptions.learningRule1 = learningRule1;
	coevolutionLearningRuleOptions.learningRule2 = learningRule2;
	fillDefaultLearningRuleOptions(coevolutionLearningRuleOptions);

	return new CoevolutionLearningRule(coevolutionLearningRuleOptions);
}


FeedForwardNetworkTopologyOptions MalmoEvolution::getNetworkOptions()
{
	FeedForwardNetworkTopologyOptions options;
	options.enableShortcuts = getBooleanPreference(PREFERENCE_SHORTCUT_ENABLE);

	options.neuronsPerLayerCount.push_back(18 * 18);
	options.neuronsPerLayerCount.push_back(getIntegerPreference(PREFERENCE_NEURON_COUNT_FIRST_LAYER));
	if (getBooleanPreference(PREFERENCE_SECOND_LAYER_ENABLE))
		options.neuronsPerLayerCount.push_back(getIntegerPreference(PREFERENCE_NEURON_COUNT_SECOND_LAYER));
	options.neuronsPerLayerCount.push_back(3);

	options.descriptionFactory = new SameNeuronDescriptionFactory(new NeuronDescription(new WeightedSumFunction(), new BinaryFunction()));
	return options;
}

AbstractEvolutionEnvironment* MalmoEvolution::createEnvironment()
{
	cs1 = new SharedSamplingCombiningStrategy(getIntegerPreference(PREFERENCE_COMPETITIONS_SIZE));

	FeedForwardNetworkTopologyOptions options = getNetworkOptions();
	Minecraft* pong1 = new Minecraft(options, false, cs1, new SharedCoevolutionFitnessFunction(), &hof1, &hof2);

	cs1->setSecondEnvironment(static_cast<Minecraft&>(*parasiteEnvironment.get()));
	cs2->setSecondEnvironment(*pong1);

	return pong1;

}

AbstractEvolutionEnvironment* MalmoEvolution::createParasiteEnvironment()
{
	cs2 = new SharedSamplingCombiningStrategy(getIntegerPreference(PREFERENCE_COMPETITIONS_SIZE));

	hof1.reset(new RandomHallOfFameAlgorithm(getIntegerPreference(PREFERENCE_HALLOFFAME_COMPETITIONS_SIZE)));
	hof2.reset(new RandomHallOfFameAlgorithm(getIntegerPreference(PREFERENCE_HALLOFFAME_COMPETITIONS_SIZE)));

	FeedForwardNetworkTopologyOptions options = getNetworkOptions();
	return new Minecraft(options, true, cs2, new SharedCoevolutionFitnessFunction(), &hof2, &hof1);
}

MalmoEvolution::MalmoEvolution()
{
	addCustomSubApp(new MinecraftFactory());
	addPreference(new DoublePreference(PREFERENCE_MUTATION_PERCENTAGE, 1.8, 0, 3));
	addPreference(new DoublePreference(PREFERENCE_RECOMBINATION_PERCENTAGE, 0.3, 0, 3));
	addPreference(new DoublePreference(PREFERENCE_TOPOLOGY_MUTATION_PERCENTAGE, 0, 0, 3));
	addPreference(new IntegerPreference(PREFERENCE_POPULATION_SIZE, 15, 1, 1000));
	addPreference(new IntegerPreference(PREFERENCE_CREATE_UP_TO, 25, 1, 1000));
	addPreference(new IntegerPreference(PREFERENCE_COMPETITIONS_SIZE, 10, 1, 1000));
	addPreference(new IntegerPreference(PREFERENCE_HALLOFFAME_COMPETITIONS_SIZE, 5, 1, 1000));
	addPreference(new BooleanPreference(PREFERENCE_SHORTCUT_ENABLE, false));
	addPreference(new IntegerPreference(PREFERENCE_NEURON_COUNT_FIRST_LAYER, 32, 1, 30));
	addPreference(new BooleanPreference(PREFERENCE_SECOND_LAYER_ENABLE, false));
	addPreference(new IntegerPreference(PREFERENCE_NEURON_COUNT_SECOND_LAYER, 1, 1, 30));
	addPreference(new DoublePreference(PREFERENCE_MUTATIONSTRENGTH_CHANGESPEED, 1.6, 0, 2)); // 1.6, simple
	addPreference(new DoublePreference(PREFERENCE_WEIGHTDECAY_FAC, 0, 0.003, 0.3));
}

std::string MalmoEvolution::getOriginalName() const
{
	return "Minecraft evolution example";
}

std::string MalmoEvolution::getDescription() const
{
	return "Evolution of a Minecraft AI with coevolution.";
}

AbstractTrainingPlan * MalmoEvolution::createNewFromSameType() const
{
	return new MalmoEvolution();
}

std::string MalmoEvolution::getLearningRuleName() const
{
	return CoevolutionLearningRule::getName();
}
