#include "BackpropagationXorExample.hpp"
#include <LightBulb/NetworkTopology/FeedForwardNetworkTopology.hpp>
#include <LightBulb/NeuralNetwork/NeuralNetwork.hpp>
#include <LightBulb/NeuronDescription/DifferentNeuronDescriptionFactory.hpp>
#include <LightBulb/Function/InputFunction/WeightedSumFunction.hpp>
#include <LightBulb/Teaching/TeachingLessonBooleanInput.hpp>
#include <LightBulb/Teaching/Teacher.hpp>
#include <LightBulb/Function/ActivationFunction/FermiFunction.hpp>
#include <LightBulb/NeuronDescription/NeuronDescription.hpp>
#include "LightBulb/Learning/Supervised/GradientDescentLearningRule.hpp"
#include "LightBulbApp/TrainingPlans/Preferences/PredefinedPreferenceGroups/Supervised/GradientDescentLearningRulePreferenceGroup.hpp"
#include "LightBulb/Learning/Supervised/GradientDescentAlgorithms/SimpleGradientDescent.hpp"
#include "LightBulb/Learning/Supervised/GradientDescentAlgorithms/ResilientLearningRate.hpp"
#include "LightBulbApp/TrainingPlans/Preferences/PredefinedPreferenceGroups/FeedForwardNetworkTopologyPreferenceGroup.hpp"
#include <LightBulb/Function/ActivationFunction/BinaryFunction.hpp>
#include <LightBulb/Function/ActivationFunction/RectifierFunction.hpp>

#define FIELD_SIZE 9

using namespace LightBulb;

BackpropagationXorExample::BackpropagationXorExample()
{
	FeedForwardNetworkTopologyOptions networkTopologyOptions;
	networkTopologyOptions.descriptionFactory = new DifferentNeuronDescriptionFactory(new NeuronDescription(new WeightedSumFunction(), new RectifierFunction()), new NeuronDescription(new WeightedSumFunction(), new FermiFunction()));
	networkTopologyOptions.neuronsPerLayerCount = std::vector<unsigned int>(4);
	networkTopologyOptions.neuronsPerLayerCount[0] = 3;
	networkTopologyOptions.neuronsPerLayerCount[1] = 128;
	networkTopologyOptions.neuronsPerLayerCount[2] = 128;
	networkTopologyOptions.neuronsPerLayerCount[3] = 3;
	addPreferenceGroup(new FeedForwardNetworkTopologyPreferenceGroup(networkTopologyOptions));

	GradientDescentLearningRuleOptions options;
	options.maxTotalErrorValue = 100;
	options.maxIterationsPerTry = 1000000;
	options.totalErrorGoal = 0.001f;
	options.maxTries = 1;
	SimpleGradientDescentOptions simpleGradientDescentOptions;
	simpleGradientDescentOptions.learningRate = 0.12345;
	ResilientLearningRateOptions resilientLearningRateOptions;
	addPreferenceGroup(new GradientDescentLearningRulePreferenceGroup(options, simpleGradientDescentOptions, resilientLearningRateOptions));
}

AbstractLearningRule* BackpropagationXorExample::createLearningRate()
{
	teacher.reset(new Teacher());
	for (int x = 2; x <= 6; x++)
	{
		for (int y = 2; y <= 6; y++)
		{
			for (int dir = 0; dir < 4; dir++)
			{
				std::vector<double> teachingPattern(3);
				TeachingInput<bool>* teachingInput = new TeachingInput<bool>(3);

				teachingPattern[0] = (double)x / FIELD_SIZE;
				teachingPattern[1] = (double)y / FIELD_SIZE;
				teachingPattern[2] = (double)dir / 3;

				int action = 0;
				if (y == 4 || x == 3 || x == 5)
				{
					if (x < 4)
					{
						if (dir == 0)
							action = 0;
						else if (dir == 1)
							action = 1;
						else
							action = 2;
					}
					else
					{
						if (dir == 2)
							action = 0;
						else if (dir == 1)
							action = 2;
						else
							action = 1;
					}
				}
				else if (y > 4)
				{
					if (dir == 1)
						action = 0;
					else if (dir == 2)
						action = 2;
					else
						action = 1;
				}
				else if (y < 4)
				{
					if (dir == 3)
						action = 0;
					else if (dir == 0)
						action = 2;
					else
						action = 1;
				}

				(*teachingInput).set(0, (action == 0));
				if (action == 1 || action == 2)
					(*teachingInput).set(1, (action == 1));
				if (action == 2)
					(*teachingInput).set(2, (action == 2));
				teacher->addTeachingLesson(new TeachingLessonBooleanInput(teachingPattern, teachingInput));
			}
		}
	}
	
	GradientDescentLearningRuleOptions options = createOptions<GradientDescentLearningRuleOptions, GradientDescentLearningRulePreferenceGroup>();
	options.teacher = teacher.get();
	fillDefaultLearningRuleOptions(options);

	return new GradientDescentLearningRule(options);
}


AbstractNeuralNetwork* BackpropagationXorExample::createNeuralNetwork()
{
	FeedForwardNetworkTopologyOptions options = createOptions<FeedForwardNetworkTopologyOptions, FeedForwardNetworkTopologyPreferenceGroup>();
	FeedForwardNetworkTopology* networkTopology = new FeedForwardNetworkTopology(options);

	return new NeuralNetwork(networkTopology);
}


std::string BackpropagationXorExample::getOriginalName() const
{
	return "Backpropagation xor example";
}

std::string BackpropagationXorExample::getDescription() const
{
	return "Trains a network to simulate the xor function!";
}


AbstractTrainingPlan* BackpropagationXorExample::createNewFromSameType() const
{
	return new BackpropagationXorExample();
}


std::string BackpropagationXorExample::getLearningRuleName() const
{
	return GradientDescentLearningRule::getName();
}
