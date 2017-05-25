// Includes
#include "PigChase/AbstractMalmoAgent.hpp"
#include "PigChase/Malmo.hpp"

using namespace LightBulb;

AbstractMalmoAgent::AbstractMalmoAgent(Malmo& malmo_)
{
	currentGame = &malmo_;
}


bool AbstractMalmoAgent::doStep(Location& location)
{
	// Set next position to current position
	int nextX = location.x;
	int nextY = location.y;

	// Change next position depending on the current direction
	if (location.dir == 0)
		nextY++;
	else if (location.dir == 90)
		nextX--;
	else if (location.dir == 180)
		nextY--;
	else if (location.dir == 270)
		nextX++;

	// If field is allowed set next position as new position 
	if (currentGame->isFieldAllowed(nextX, nextY + 1))
	{
		currentGame->agentMovedTo(nextX, nextY, nextX - location.x, nextY - location.y);
		location.x = nextX;
		location.y = nextY;
		return true;
	}
	else
		return false;
}

void AbstractMalmoAgent::turnLeft(Location& location)
{
	location.dir -= 90;
	if (location.dir < 0)
		location.dir += 360;
}

void AbstractMalmoAgent::turnRight(Location& location)
{
	location.dir += 90;
	location.dir %= 360;
}

const Location& AbstractMalmoAgent::getPrevLocation() const
{
	return prevLocation;
}

void AbstractMalmoAgent::setEnv(Malmo &currentGame_)
{
	currentGame = &currentGame_;
}

const Location& AbstractMalmoAgent::getLocation() const
{
	return location;
}

void AbstractMalmoAgent::setLocation(Location location)
{
	this->location = location;
}

void AbstractMalmoAgent::reset()
{
	// Reset previous location
	prevLocation.x = 0;
	prevLocation.y = 0;
	prevLocation.dir = 0;
}
