// Includes
#include "PigChase/Agent.hpp"
#include "PigChase/Malmo.hpp"

using namespace LightBulb;

Agent::Agent(FeedForwardNetworkTopologyOptions& options, Malmo& malmo_)
	: AbstractDefaultIndividual(malmo_), AbstractMalmoAgent(malmo_)
{
	buildNeuralNetwork(options);
}

void Agent::reset()
{
	AbstractMalmoAgent::reset();
	resetNN();
}

void Agent::resetNN()
{
	LightBulb::AbstractDefaultIndividual::resetNN();
}

void Agent::getNNInput(std::vector<double>& input)
{
	currentGame->getNNInput(input);
}

void Agent::interpretNNOutput(std::vector<double>& output)
{
	// Determine the first output which is > 0.5 and execute the respective action
	if (output[0] > 0.5 || (output[0] <= 0.5 && output[1] <= 0.5 && output[2] <= 0.5))
	{
		doStep(location);
	}
	else if (output[1] > 0.5)
	{
		turnLeft(location);
	}
	else if (output[2] > 0.5)
	{
		turnRight(location);
	}
}

void Agent::doNNCalculation()
{
	// Set the current location as new previous location
	prevLocation = location;
	LightBulb::AbstractDefaultIndividual::doNNCalculation();
}
