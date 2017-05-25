// Includes
#include "PigChase/ChallengeAgent.hpp"
#include "Malmo.hpp"

using namespace LightBulb;

ChallengeAgent::ChallengeAgent(Malmo& malmo_)
	: AbstractMalmoAgent(malmo_)
{

}

void ChallengeAgent::doNNCalculation()
{
	prevLocation = location;
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

bool ChallengeAgent::getIsStupid() const
{
	return isStupid;
}

void ChallengeAgent::setIsStupid(bool isStupid)
{
	this->isStupid = isStupid;
}
