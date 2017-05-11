// Includes
#include "PongEvolution/Agent.hpp"
#include "PongEvolution/Minecraft.hpp"

using namespace LightBulb;

Agent::Agent(FeedForwardNetworkTopologyOptions& options, Minecraft& pong_)
	: AbstractDefaultIndividual(pong_)
{
	currentGame = &pong_;
	buildNeuralNetwork(options);

	do
	{
		startLocation.x = currentGame->getRandomGenerator().randInt(2, 6);
		startLocation.y = currentGame->getRandomGenerator().randInt(1, 5);
	} while (!currentGame->isFieldAllowed(startLocation.x, startLocation.y + 1));

	startLocation.dir = currentGame->getRandomGenerator().randInt(0, 3) * 90;
}


void Agent::getNNInput(std::vector<double>& input)
{
	currentGame->getNNInput(input);
}

void Agent::interpretNNOutput(std::vector<double>& output)
{
	if (false && static_cast<AbstractCoevolutionEnvironment*>(environment)->isParasiteEnvironment())
	{
		if (location.dir == 0 || location.dir == 180)
		{
			location.dir -= 90;
			if (location.dir < 0)
				location.dir += 360;
		}
		else
		{
			int nextX = location.x;
			int nextY = location.y;
			if (location.dir == 0)
				nextY++;
			else if (location.dir == 90)
				nextX--;
			else if (location.dir == 180)
				nextY--;
			else if (location.dir == 270)
				nextX++;
			if (currentGame->isFieldAllowed(nextX, nextY + 1))
			{
				location.x = nextX;
				location.y = nextY;
			}
		}
	}
	else
	{
		if (output[0] > 0.5 || (output[0] <= 0.5 && output[1] <= 0.5 && output[2] <= 0.5))
		{
			int nextX = location.x;
			int nextY = location.y;
			if (location.dir == 0)
				nextY++;
			else if (location.dir == 90)
				nextX--;
			else if (location.dir == 180)
				nextY--;
			else if (location.dir == 270)
				nextX++;
			if (currentGame->isFieldAllowed(nextX, nextY + 1))
			{
				location.x = nextX;
				location.y = nextY;
			}
		}
		else if (output[1] > 0.5)
		{
			location.dir -= 90;
			if (location.dir < 0)
				location.dir += 360;
		}
		else if (output[2] > 0.5)
		{
			location.dir += 90;
			location.dir %= 360;
		}
	}
}


void Agent::setEnv(Minecraft &currentGame_)
{
	currentGame = &currentGame_;
}

Location Agent::getLocation() const
{
	return location;
}

Location Agent::getStartLocation() const
{
	return startLocation;
}

void Agent::setLocation(Location location)
{
	this->location = location;
}

Location Agent::setStartLocation(Location startLocation)
{
	if (currentGame->isFieldAllowed(startLocation.x, startLocation.y + 1))
		this->startLocation = startLocation;
	return this->startLocation;
}

void Agent::copyPropertiesFrom(AbstractIndividual& notUsedIndividual)
{
	AbstractDefaultIndividual::copyPropertiesFrom(notUsedIndividual);
	Agent& agent = dynamic_cast<Agent&>(notUsedIndividual);
	startLocation = agent.startLocation;
}
