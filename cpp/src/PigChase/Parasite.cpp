// Includes
#include "PigChase/Parasite.hpp"
#include "PigChase/Malmo.hpp"

using namespace LightBulb;

Parasite::Parasite(FeedForwardNetworkTopologyOptions& options, Malmo& malmo_)
	: Agent(options, malmo_)
{
	randomizeState();
}

void Parasite::randomizeState()
{
	// Search for a new random but valid start constellation
	do
	{
		setToRandomLocation(popStartLocation);
		setToRandomLocation(parStartLocation);
		setToRandomLocation(pigStartLocation);
	} while (!isValidStartConstellation(popStartLocation, parStartLocation, pigStartLocation));

	// Randomize directions.
	popStartLocation.dir = currentGame->getRandomGenerator().randInt(0, 3) * 90;
	parStartLocation.dir = currentGame->getRandomGenerator().randInt(0, 3) * 90;
	pigStartLocation.dir = 0;

	// Determine if agent should act randomly
	isStupid = currentGame->getRandomGenerator().randDouble() < 0.25;
}


bool Parasite::isValidStartConstellation(const Location& popStartLocation, const Location& parStartLocation, const Location& pigStartLocation)
{
	// Check if all locations are valid and the distance between them is always > 1.1
	return currentGame && currentGame->isFieldAllowed(popStartLocation.x, popStartLocation.y + 1, false) && currentGame->isFieldAllowed(parStartLocation.x, parStartLocation.y + 1, false) && currentGame->isFieldAllowed(pigStartLocation.x, pigStartLocation.y + 1, false) &&
		calcDistance(popStartLocation, parStartLocation) > 1.1f && calcDistance(parStartLocation, pigStartLocation) > 1.1f && calcDistance(popStartLocation, pigStartLocation) > 1.1f;
}

float Parasite::calcDistance(const Location& location1, const Location& location2)
{
	return sqrt(pow((float)location1.x - (float)location2.x, 2) + pow((float)location1.y - (float)location2.y, 2));
}

void Parasite::setToRandomLocation(Location& location)
{
	do
	{
		location.x = currentGame->getRandomGenerator().randInt(2, 6);
		location.y = currentGame->getRandomGenerator().randInt(1, 5);
	} while (!currentGame->isFieldAllowed(location.x, location.y + 1, false));
}

bool Parasite::getIsStupid() const
{
	return isStupid;
}

void Parasite::setIsStupid(bool isStupid)
{
	this->isStupid = isStupid;
}

const Location& Parasite::getParStartLocation() const
{
	return parStartLocation;
}

const Location& Parasite::getPopStartLocation() const
{
	return popStartLocation;
}

Location Parasite::getPigStartLocation() const
{
	return pigStartLocation;
}

void Parasite::setPigStartLocation(Location pigStartLocation)
{
	this->pigStartLocation = pigStartLocation;
}

void Parasite::setPopStartLocation(Location popStartLocation)
{
	this->popStartLocation = popStartLocation;
}

void Parasite::setParStartLocation(Location parStartLocation)
{
	this->parStartLocation = parStartLocation;
}

void Parasite::copyPropertiesFrom(AbstractIndividual& notUsedIndividual)
{
	AbstractDefaultIndividual::copyPropertiesFrom(notUsedIndividual);
	// Also copy starting positions
	Parasite& agent = dynamic_cast<Parasite&>(notUsedIndividual);
	parStartLocation = agent.parStartLocation;
	popStartLocation = agent.popStartLocation;
	pigStartLocation = agent.pigStartLocation;
	isStupid = agent.isStupid;
}
