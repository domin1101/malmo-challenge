// Includes
#include "PongEvolution/Agent.hpp"
#include "PongEvolution/Minecraft.hpp"

using namespace LightBulb;

Agent::Agent(FeedForwardNetworkTopologyOptions& options, Minecraft& pong_)
	: AbstractDefaultIndividual(pong_)
{
	buildNeuralNetwork(options);
}



void Agent::getNNInput(std::vector<double>& input)
{
	static_cast<Minecraft*>(environment)->getNNInput(input);
}

void Agent::interpretNNOutput(std::vector<double>& output)
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
		if (static_cast<Minecraft*>(environment)->isFieldAllowed(nextX, nextY + 1))
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

const Location& Agent::getLocation() const
{
	return location;
}

void Agent::setLocation(Location location)
{
	this->location = location;
}