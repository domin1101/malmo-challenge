// Includes
#include "PongEvolution/Agent.hpp"
#include "PongEvolution/Minecraft.hpp"

using namespace LightBulb;

Agent::Agent(FeedForwardNetworkTopologyOptions& options, Minecraft& pong_)
	: AbstractDefaultIndividual(pong_)
{
	currentGame = &pong_;
	buildNeuralNetwork(options);
}


void Agent::getNNInput(std::vector<double>& input)
{
	currentGame->getNNInput(input);
}

void Agent::interpretNNOutput(std::vector<double>& output)
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

int Agent::getDir()const
{
	return dir;
}
