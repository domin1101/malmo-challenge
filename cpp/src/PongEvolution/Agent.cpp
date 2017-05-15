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
		popStartLocation.x = currentGame->getRandomGenerator().randInt(2, 6);
		popStartLocation.y = currentGame->getRandomGenerator().randInt(1, 5);
	} while (!currentGame->isFieldAllowed(popStartLocation.x, popStartLocation.y + 1));
	popStartLocation.dir = currentGame->getRandomGenerator().randInt(0, 3) * 90;

	do
	{
		parStartLocation.x = currentGame->getRandomGenerator().randInt(2, 6);
		parStartLocation.y = currentGame->getRandomGenerator().randInt(1, 5);
	} while (!currentGame->isFieldAllowed(parStartLocation.x, parStartLocation.y + 1));
	parStartLocation.dir = currentGame->getRandomGenerator().randInt(0, 3) * 90;

	do
	{
		pigStartLocation.x = currentGame->getRandomGenerator().randInt(2, 6);
		pigStartLocation.y = currentGame->getRandomGenerator().randInt(1, 5);
	} while (!currentGame->isFieldAllowed(pigStartLocation.x, pigStartLocation.y + 1));
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

const Location& Agent::getLocation() const
{
	return location;
}

const Location& Agent::getParStartLocation() const
{
	return parStartLocation;
}

const Location& Agent::getPopStartLocation() const
{
	return popStartLocation;
}

void Agent::setLocation(Location location)
{
	this->location = location;
}

Location Agent::getPigStartLocation() const
{
	return pigStartLocation;
}

Location Agent::setPigStartLocation(Location pigStartLocation)
{
	if (currentGame->isFieldAllowed(pigStartLocation.x, pigStartLocation.y + 1))
		this->pigStartLocation = pigStartLocation;
	return this->pigStartLocation;
}

Location Agent::setPopStartLocation(Location popStartLocation)
{
	if (currentGame->isFieldAllowed(popStartLocation.x, popStartLocation.y + 1))
		this->popStartLocation = popStartLocation;
	return this->popStartLocation;
}

Location Agent::setParStartLocation(Location parStartLocation)
{
	if (currentGame->isFieldAllowed(parStartLocation.x, parStartLocation.y + 1))
		this->parStartLocation = parStartLocation;
	return this->parStartLocation;
}

void Agent::copyPropertiesFrom(AbstractIndividual& notUsedIndividual)
{
	AbstractDefaultIndividual::copyPropertiesFrom(notUsedIndividual);
	Agent& agent = dynamic_cast<Agent&>(notUsedIndividual);
	parStartLocation = agent.parStartLocation;
	popStartLocation = agent.popStartLocation;
}
