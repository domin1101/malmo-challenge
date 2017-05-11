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
		startX = currentGame->getRandomGenerator().randInt(2, 6);
		startY = currentGame->getRandomGenerator().randInt(1, 5);
	} while (!currentGame->isFieldAllowed(startX, startY + 1));

	dir = currentGame->getRandomGenerator().randInt(0, 3) * 90;
}


void Agent::getNNInput(std::vector<double>& input)
{
	currentGame->getNNInput(input);
}

void Agent::interpretNNOutput(std::vector<double>& output)
{
	if (false && static_cast<AbstractCoevolutionEnvironment*>(environment)->isParasiteEnvironment())
	{
		if (dir == 0 || dir == 180)
		{
			dir -= 90;
			if (dir < 0)
				dir += 360;
		}
		else
		{
			int nextX = x;
			int nextY = y;
			if (dir == 0)
				nextY++;
			else if (dir == 90)
				nextX--;
			else if (dir == 180)
				nextY--;
			else if (dir == 270)
				nextX++;
			if (currentGame->isFieldAllowed(nextX, nextY + 1))
			{
				x = nextX;
				y = nextY;
			}
		}
	}
	else
	{
		if (output[0] > 0.5 || (output[0] <= 0.5 && output[1] <= 0.5 && output[2] <= 0.5))
		{
			int nextX = x;
			int nextY = y;
			if (dir == 0)
				nextY++;
			else if (dir == 90)
				nextX--;
			else if (dir == 180)
				nextY--;
			else if (dir == 270)
				nextX++;
			if (currentGame->isFieldAllowed(nextX, nextY + 1))
			{
				x = nextX;
				y = nextY;
			}
		}
		else if (output[1] > 0.5)
		{
			dir -= 90;
			if (dir < 0)
				dir += 360;
		}
		else if (output[2] > 0.5)
		{
			dir += 90;
			dir %= 360;
		}
	}
}


void Agent::setEnv(Minecraft &currentGame_)
{
	currentGame = &currentGame_;
}

int Agent::getX()const
{
	return x;
}

int Agent::getY()const
{
	return y;
}

void Agent::setPositionAndDir(int x_, int y_, int dir_)
{
	x = x_;
	y = y_;
	dir = dir_;
}

int Agent::getStartX() const
{
	return startX;
}

int Agent::getStartY() const
{
	return startY;
}

int Agent::getStartDir() const
{
	return startDir;
}

void Agent::setStartX(int startX)
{
	if (currentGame->isFieldAllowed(startX, startY + 1))
		this->startX = startX;
}

void Agent::setStartY(int startY)
{
	if (currentGame->isFieldAllowed(startX, startY + 1))
		this->startY = startY;
}

void Agent::setStartDir(int startDir)
{
	this->startDir = startDir;
}

int Agent::getDir()const
{
	return dir;
}

void Agent::copyPropertiesFrom(AbstractIndividual& notUsedIndividual)
{
	AbstractDefaultIndividual::copyPropertiesFrom(notUsedIndividual);
	Agent& agent = dynamic_cast<Agent&>(notUsedIndividual);
	startX = agent.startX;
	startY = agent.startY;
	startDir = agent.startDir;
}
