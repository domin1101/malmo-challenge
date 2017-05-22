// Includes
#include "PigChase/Agent.hpp"
#include "PigChase/Malmo.hpp"

using namespace LightBulb;

Agent::Agent(FeedForwardNetworkTopologyOptions& options, Malmo& malmo_, bool forceParasite)
	: AbstractDefaultIndividual(malmo_)
{
	currentGame = &malmo_;
	buildNeuralNetwork(options);
	isParasite = static_cast<AbstractCoevolutionEnvironment*>(environment)->isParasiteEnvironment() || forceParasite;
	
	randomizeState();
}

void Agent::randomizeState()
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

void Agent::resetNN()
{
	LightBulb::AbstractDefaultIndividual::resetNN();
	// Also reset previous location
	prevLocation.x = 0;
	prevLocation.y = 0;
	prevLocation.dir = 0;
}


bool Agent::isValidStartConstellation(const Location& popStartLocation, const Location& parStartLocation, const Location& pigStartLocation)
{
	// Check if all locations are valid and the distance between them is always > 1.1
	return currentGame && currentGame->isFieldAllowed(popStartLocation.x, popStartLocation.y + 1, false) && currentGame->isFieldAllowed(parStartLocation.x, parStartLocation.y + 1, false) && currentGame->isFieldAllowed(pigStartLocation.x, pigStartLocation.y + 1, false) &&
		calcDistance(popStartLocation, parStartLocation) > 1.1f && calcDistance(parStartLocation, pigStartLocation) > 1.1f && calcDistance(popStartLocation, pigStartLocation) > 1.1f;
}

float Agent::calcDistance(const Location& location1, const Location& location2)
{
	return sqrt(pow((float)location1.x - (float)location2.x, 2) + pow((float)location1.y - (float)location2.y, 2));
}

void Agent::setToRandomLocation(Location& location)
{
	do
	{
		location.x = currentGame->getRandomGenerator().randInt(2, 6);
		location.y = currentGame->getRandomGenerator().randInt(1, 5);
	} while (!currentGame->isFieldAllowed(location.x, location.y + 1, false));
}


void Agent::getNNInput(std::vector<double>& input)
{
	currentGame->getNNInput(input);
}

bool Agent::doStep(Location& location)
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

void Agent::turnLeft(Location& location)
{
	location.dir -= 90;
	if (location.dir < 0)
		location.dir += 360;
}

void Agent::turnRight(Location& location)
{
	location.dir += 90;
	location.dir %= 360;
}

const Location& Agent::getPrevLocation() const
{
	return prevLocation;
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
	// If this not a parasite just ask the NN what to do
	if (!isParasite)
		LightBulb::AbstractDefaultIndividual::doNNCalculation();
	else
	{
		// If the agent should act randomly (This is currently not working and ends in not cooperative agents)
		if (false && isStupid)
		{
			// Choose a random action.
			int action = currentGame->getRandomGenerator().randInt(0, 2);
			if (action == 0)
				doStep(location);
			else if (action == 1)
				turnLeft(location);
			else if (action == 2)
				turnRight(location);
			return;
		}

		// Check if we already arrived at the pig.
		const Location& pig = currentGame->getPig();
		if (std::abs(pig.x - location.x) + std::abs(pig.y - location.y) == 1)
		{
			turnRight(location);
			return;
		}

		static std::vector<std::pair<int, Location>> openlist;
		static std::map<Location, std::map<Location, int>> cache;

		// Check if the pathfinding has already been cached
		if (cache[location][pig] != 0)
		{
			if (cache[location][pig] == 1)
				doStep(location);
			else if (cache[location][pig] == 2)
				turnLeft(location);
			else if (cache[location][pig] == 3)
				turnRight(location);
			return;
		}

		// Execute A*:
		openlist.clear();
		int index = 0;
		openlist.push_back(std::make_pair(-1, location));
		while (index < openlist.size())
		{
			// If goal has been reached
			if (std::abs(pig.x - openlist[index].second.x) + std::abs(pig.y - openlist[index].second.y) == 1)
			{
				// Determine first action, execute it and abort A*
				cache[location][pig] = openlist[index].first + 1;
				if (openlist[index].first == 0)
					doStep(location);
				else if (openlist[index].first == 1)
					turnLeft(location);
				else if (openlist[index].first == 2)
					turnRight(location);
				return;
			}

			// Check if doing a step would be successful
			Location nextStep = openlist[index].second;
			if (doStep(nextStep))
				openlist.push_back(std::make_pair(openlist[index].first != -1 ? openlist[index].first : 0, nextStep));

			// Add a "rotate left" to the open list
			openlist.push_back(std::make_pair(openlist[index].first != -1 ? openlist[index].first : 1, openlist[index].second));
			turnLeft(openlist.back().second);

			// Add a "rotate right" to the open list
			openlist.push_back(std::make_pair(openlist[index].first != -1 ? openlist[index].first : 2, openlist[index].second));
			turnRight(openlist.back().second);

			index++;
		}
	}
}

bool Agent::getIsStupid() const
{
	return isStupid;
}

void Agent::setIsStupid(bool isStupid)
{
	this->isStupid = isStupid;
}

void Agent::setEnv(Malmo &currentGame_)
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

void Agent::setPigStartLocation(Location pigStartLocation)
{
	this->pigStartLocation = pigStartLocation;
}

void Agent::setPopStartLocation(Location popStartLocation)
{
	this->popStartLocation = popStartLocation;
}

void Agent::setParStartLocation(Location parStartLocation)
{
	this->parStartLocation = parStartLocation;
}

void Agent::copyPropertiesFrom(AbstractIndividual& notUsedIndividual)
{
	AbstractDefaultIndividual::copyPropertiesFrom(notUsedIndividual);
	// Also copy starting positions
	Agent& agent = dynamic_cast<Agent&>(notUsedIndividual);
	parStartLocation = agent.parStartLocation;
	popStartLocation = agent.popStartLocation;
}
