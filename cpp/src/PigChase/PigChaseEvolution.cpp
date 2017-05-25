#include "PigChaseEvolution.hpp"
#include "MalmoTopDownWindowFactory.hpp"
#include <LightBulb/Learning/Evolution/ConstantMutationCommand.hpp>
#include <LightBulb/Learning/Evolution/BestSelectionCommand.hpp>
#include <LightBulb/Learning/Evolution/ConstantReuseCommand.hpp>
#include <LightBulb/Learning/Evolution/ConstantRecombinationCommand.hpp>
#include <LightBulb/Learning/Evolution/EvolutionStrategy/RecombinationAlgorithm.hpp>
#include <LightBulb/Learning/Evolution/ConstantCreationCommand.hpp>
#include <LightBulb/Learning/Evolution/EvolutionLearningRule.hpp>
#include <LightBulb/Learning/Evolution/RandomSelector.hpp>
#include <PigChase/Malmo.hpp>
#include <LightBulb/Learning/Evolution/CoevolutionLearningRule.hpp>
#include <LightBulb/Learning/Evolution/BestReuseSelector.hpp>
#include <LightBulbApp/TrainingPlans/Preferences/DoublePreference.hpp>
#include <LightBulbApp/TrainingPlans/Preferences/IntegerPreference.hpp>
#include <LightBulbApp/TrainingPlans/Preferences/BooleanPreference.hpp>
#include <LightBulb/NeuronDescription/DifferentNeuronDescriptionFactory.hpp>
#include <LightBulb/Function/InputFunction/WeightedSumFunction.hpp>
#include <LightBulb/Function/ActivationFunction/BinaryFunction.hpp>
#include <LightBulb/Function/ActivationFunction/RectifierFunction.hpp>
#include <LightBulb/NeuronDescription/NeuronDescription.hpp>
#include <LightBulb/Learning/Evolution/RandomHallOfFameAlgorithm.hpp>
#include "AgentMutationAlgorithm.hpp"
#include "MalmoCombiningStrategy.hpp"
#include "MalmoFitnessFunction.hpp"
#include <LightBulb/Learning/Evolution/SharedCoevolutionFitnessFunction.hpp>

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

AbstractLearningRule* PigChaseEvolution::createLearningRate()
{
	// Setup learning rule options
	EvolutionLearningRuleOptions options;	
	options.creationCommands.push_back(new ConstantCreationCommand(getIntegerPreference(PREFERENCE_CREATE_UP_TO)));
	options.reuseCommands.push_back(new ConstantReuseCommand(new BestReuseSelector(), 1));
	options.selectionCommands.push_back(new BestSelectionCommand(getIntegerPreference(PREFERENCE_POPULATION_SIZE)));
	options.mutationsCommands.push_back(new ConstantMutationCommand(new AgentMutationAlgorithm(getDoublePreference(PREFERENCE_MUTATIONSTRENGTH_CHANGESPEED), 1), new RandomSelector(new RankBasedRandomFunction()), getDoublePreference(PREFERENCE_MUTATION_PERCENTAGE)));
	options.recombinationCommands.push_back(new ConstantRecombinationCommand(new RecombinationAlgorithm(), new RandomSelector(new RankBasedRandomFunction()), getDoublePreference(PREFERENCE_RECOMBINATION_PERCENTAGE)));
	
	// Create the learning rule for the first population
	fillDefaultEvolutionLearningRule1Options(options);
	EvolutionLearningRule* learningRule1 = new EvolutionLearningRule(options);

	// Create the learning rule for the second population 
	fillDefaultEvolutionLearningRule2Options(options);
	EvolutionLearningRule* learningRule2 = new EvolutionLearningRule(options);

	// Combine both learning rules as a new one
	CoevolutionLearningRuleOptions coevolutionLearningRuleOptions;
	coevolutionLearningRuleOptions.learningRule1 = learningRule1;
	coevolutionLearningRuleOptions.learningRule2 = learningRule2;
	coevolutionLearningRuleOptions.maxIterationsPerTry = 8000;
	fillDefaultLearningRuleOptions(coevolutionLearningRuleOptions);
	return new CoevolutionLearningRule(coevolutionLearningRuleOptions);
}


FeedForwardNetworkTopologyOptions PigChaseEvolution::getNetworkOptions(int inputSize)
{
	FeedForwardNetworkTopologyOptions options;
	options.enableShortcuts = getBooleanPreference(PREFERENCE_SHORTCUT_ENABLE);

	// Adds layer sizes
	options.neuronsPerLayerCount.push_back(inputSize);
	options.neuronsPerLayerCount.push_back(getIntegerPreference(PREFERENCE_NEURON_COUNT_FIRST_LAYER));
	if (getBooleanPreference(PREFERENCE_SECOND_LAYER_ENABLE))
		options.neuronsPerLayerCount.push_back(getIntegerPreference(PREFERENCE_NEURON_COUNT_SECOND_LAYER));
	options.neuronsPerLayerCount.push_back(3);

	// Determine which activation functionn we should use
	options.descriptionFactory = new DifferentNeuronDescriptionFactory(new NeuronDescription(new WeightedSumFunction(), new RectifierFunction()), new NeuronDescription(new WeightedSumFunction(), new BinaryFunction()));
	return options;
}

AbstractEvolutionEnvironment* PigChaseEvolution::createEnvironment()
{
	// Create first combining strategy
	cs1 = new SharedSamplingCombiningStrategy(getIntegerPreference(PREFERENCE_COMPETITIONS_SIZE));

	FeedForwardNetworkTopologyOptions options = getNetworkOptions(8);

	// Create first environment
	Malmo* malmo1 = new Malmo(options, false, cs1, new SharedCoevolutionFitnessFunction(), &hof1, &hof2);

	cs1->setSecondEnvironment(static_cast<Malmo&>(*parasiteEnvironment.get()));
	cs2->setSecondEnvironment(*malmo1);

	return malmo1;

}

AbstractEvolutionEnvironment* PigChaseEvolution::createParasiteEnvironment()
{
	// Create second combining strategy
	cs2 = new SharedSamplingCombiningStrategy(getIntegerPreference(PREFERENCE_COMPETITIONS_SIZE));

	// Create hall of fame algorithms (not used)
	hof1.reset(new RandomHallOfFameAlgorithm(getIntegerPreference(PREFERENCE_HALLOFFAME_COMPETITIONS_SIZE)));
	hof2.reset(new RandomHallOfFameAlgorithm(getIntegerPreference(PREFERENCE_HALLOFFAME_COMPETITIONS_SIZE)));

	FeedForwardNetworkTopologyOptions options = getNetworkOptions(8);

	// Create second environment
	return new Malmo(options, true, cs2, new SharedCoevolutionFitnessFunction(), &hof2, &hof1);
}

PigChaseEvolution::PigChaseEvolution()
{
	// Register window
	addCustomSubApp(new MalmoTopDownWindowFactory());
	// Add preferences
	addPreference(new DoublePreference(PREFERENCE_MUTATION_PERCENTAGE, 1.8, 0, 3));
	addPreference(new DoublePreference(PREFERENCE_RECOMBINATION_PERCENTAGE, 0.3, 0, 3));
	addPreference(new DoublePreference(PREFERENCE_TOPOLOGY_MUTATION_PERCENTAGE, 0, 0, 3));
	addPreference(new IntegerPreference(PREFERENCE_POPULATION_SIZE, 150, 1, 1000));
	addPreference(new IntegerPreference(PREFERENCE_CREATE_UP_TO, 250, 1, 1000));
	addPreference(new IntegerPreference(PREFERENCE_COMPETITIONS_SIZE, 25, 1, 1000));
	addPreference(new IntegerPreference(PREFERENCE_HALLOFFAME_COMPETITIONS_SIZE, 0, 1, 1000));
	addPreference(new BooleanPreference(PREFERENCE_SHORTCUT_ENABLE, false));
	addPreference(new IntegerPreference(PREFERENCE_NEURON_COUNT_FIRST_LAYER, 32, 1, 30));
	addPreference(new BooleanPreference(PREFERENCE_SECOND_LAYER_ENABLE, false));
	addPreference(new IntegerPreference(PREFERENCE_NEURON_COUNT_SECOND_LAYER, 32, 1, 30));
	addPreference(new DoublePreference(PREFERENCE_MUTATIONSTRENGTH_CHANGESPEED, 1.6, 0, 2));
	addPreference(new DoublePreference(PREFERENCE_WEIGHTDECAY_FAC, 0, 0.003, 0.3));
}

std::string PigChaseEvolution::getOriginalName() const
{
	return "PigChase Coevolution experiment";
}

std::string PigChaseEvolution::getDescription() const
{
	return "Tries to solve the PigChase challenge with coevolution";
}

AbstractTrainingPlan * PigChaseEvolution::createNewFromSameType() const
{
	return new PigChaseEvolution();
}

std::string PigChaseEvolution::getLearningRuleName() const
{
	return CoevolutionLearningRule::getName();
}

