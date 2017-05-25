// Includes
#include "PigChase/Malmo.hpp"
#include "PigChase/Agent.hpp"
#include "LightBulb/Learning/Evolution/AbstractIndividual.hpp"
//Library includes
#include <iomanip>
#include <chrono>
#include <thread>
#include <numeric>
#include "Parasite.hpp"

#define FIELD_SIZE 9
#define DOUBLEFIELD_SIZE 2 * FIELD_SIZE

using namespace LightBulb;

AbstractIndividual* Malmo::createNewIndividual()
{
	if (isParasiteEnvironment())	
		return new Parasite(*options, *this);
	else
		return new Agent(*options, *this);
}

std::vector<std::string> Malmo::getDataSetLabels() const
{
	auto labels = AbstractCoevolutionEnvironment::getDataSetLabels();
	labels.push_back(std::string(parasiteEnvironment ? DATASET_PARASITE_PREFIX : "") + DATASET_AVG_REWARD);
	labels.push_back(std::string(parasiteEnvironment ? DATASET_PARASITE_PREFIX : "") + DATASET_BEST_REWARD);
	labels.push_back(std::string(parasiteEnvironment ? DATASET_PARASITE_PREFIX : "") + DATASET_AVG_MUT_STRENGTH);
	if (!parasiteEnvironment)
	{
		labels.push_back(DATASET_CHALLENGE);
		labels.push_back(DATASET_LAPIS_ENDINGS);
		labels.push_back(DATASET_CAUGHT_ENDINGS);
	}
	return labels;
}


Malmo::Malmo(FeedForwardNetworkTopologyOptions& options_, bool isParasiteEnvironment_, AbstractCombiningStrategy* combiningStrategy_, AbstractCoevolutionFitnessFunction* fitnessFunction_, const std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm>* hallOfFameToAddAlgorithm_, const std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm>* hallOfFameToChallengeAlgorithm_)
	: AbstractCoevolutionEnvironment(isParasiteEnvironment_, combiningStrategy_, fitnessFunction_, hallOfFameToAddAlgorithm_, hallOfFameToChallengeAlgorithm_)
{
	options.reset(new FeedForwardNetworkTopologyOptions(options_));
	challengeAgent.reset(new ChallengeAgent(*this));

	// Build the default field structure
	fields.resize(FIELD_SIZE, std::vector<int>(FIELD_SIZE, 0));
	
	fields[2][2] = 1;
	fields[3][2] = 1;
	fields[4][2] = 1;
	fields[5][2] = 1;
	fields[6][2] = 1;

	fields[2][3] = 1;
	fields[4][3] = 1;
	fields[6][3] = 1;

	fields[1][4] = 2;
	fields[2][4] = 1;
	fields[3][4] = 1;
	fields[4][4] = 1;
	fields[5][4] = 1;
	fields[6][4] = 1;
	fields[7][4] = 2;

	fields[2][5] = 1;
	fields[4][5] = 1;
	fields[6][5] = 1;

	fields[2][6] = 1;
	fields[3][6] = 1;
	fields[4][6] = 1;
	fields[5][6] = 1;
	fields[6][6] = 1;
	
	watchMode = false;
	bestReward = -25;
	totalReward = 0;
	matchCount = 0;
	lastBestIndividual = nullptr;
	inRating = false;
}

int Malmo::doCompare(AbstractIndividual& obj1, AbstractIndividual& obj2, int round)
{
	AbstractIndividual& parasite = isParasiteEnvironment() ? obj1 : obj2;
	AbstractIndividual& nonparasite = !isParasiteEnvironment() ? obj1 : obj2;
	int nonparasiteReward = simulateGame(static_cast<Agent&>(nonparasite), *challengeAgent, static_cast<Parasite&>(parasite), round);
	int parasiteReward = simulateGame(static_cast<Agent&>(parasite), *challengeAgent, static_cast<Parasite&>(parasite), round);
	if (isParasiteEnvironment())
		return parasiteReward > nonparasiteReward ? 1 : -1;
	else
		return parasiteReward <= nonparasiteReward ? 1 : -1;
}

int Malmo::getRoundCount() const
{
	// We want to do two rounds per agents combination - so every agent can start once
	return 2;
}

int Malmo::simulateGame(AbstractMalmoAgent& ai1, AbstractMalmoAgent& ai2, Parasite& parasite, int startPlayer)
{
	currentAi1 = &ai1;
	currentAi2 = &ai2;

	// Reset AIs.
	ai1.reset();
	ai1.setEnv(*this);
	ai2.reset();
	ai2.setEnv(*this);

	// Reset the game
	startNewGame(parasite);

	// Set start player as new current player
	currentPlayer = startPlayer;

	// Reset state
	std::vector<int> rewards = {0, 0};
	int noPosDirChangesInRow = 0;
	int noPosChangesInRow = 0;
	stepCounter = 0;
	lastEnding = TIMEOUT;

	// Do maximal 2x 25 steps
	for (int t = 0; t < 50; t++)
	{
		// Determine the current agent
		AbstractMalmoAgent& currentAgent = currentPlayer == 0 ? ai1 : ai2;

		// Let the agent act
		currentAgent.doNNCalculation();
		// Receive the agents reward
		rewards[currentPlayer] += getReward(currentAgent);

		// Increase the step counter
		if (currentPlayer == 0)
			stepCounter++;

		// Check if we have run into a never ending loop and stop the match if thats the case (Performance!)
		if (currentAgent.getPrevLocation().x == currentAgent.getLocation().x && currentAgent.getPrevLocation().y == currentAgent.getLocation().y)
		{
			if (currentAgent.getPrevLocation().dir == currentAgent.getLocation().dir)
				noPosDirChangesInRow++;
			else
				noPosDirChangesInRow = 0;
			noPosChangesInRow++;
			if (noPosDirChangesInRow >= 2 || noPosChangesInRow >= 8)
			{
				stepCounter = 25;
				rewards = { -25, -25 };
				break;
			}
		}
		else
		{
			noPosDirChangesInRow = 0;
			noPosChangesInRow = 0;
		}

		// Switch the player
		currentPlayer = 1 - currentPlayer;

		// If we are in watch mode
		if (watchMode && lastBestIndividual == &ai1)
		{
			// Notify the window
			throwEvent(EVT_FIELD_CHANGED, *this);
			// Sleep a while
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		// If the match if over, exit the loop
		if (isDone(ai1, ai2, currentPlayer, startPlayer))
		{
			lastEnding = LAPIS;
			break;
		}
	}
	
	// If the pig has been caught, add 25 points the next player. (The other one already got 25)
	if (isPigCaught())
	{
		lastEnding = CAUGHT;
		rewards[currentPlayer] += 25;
	}

	if (isParasiteEnvironment() && &ai1 == &parasite || !isParasiteEnvironment() && &ai1 != &parasite)
	{
		// Calculate statistics.
		bestReward = std::max(bestReward, rewards[0]);
		totalReward += rewards[0];
		matchCount++;
	}

	return rewards[0];
}

int Malmo::rateIndividual(AbstractIndividual& individual)
{
	lastBestIndividual = &static_cast<Agent&>(individual);
	// Add statistics
	learningState->addData(std::string(parasiteEnvironment ? DATASET_PARASITE_PREFIX : "") + DATASET_BEST_REWARD, static_cast<double>(bestReward));
	learningState->addData(std::string(parasiteEnvironment ? DATASET_PARASITE_PREFIX : "") + DATASET_AVG_REWARD, static_cast<double>(totalReward) / matchCount);
	learningState->addData(std::string(parasiteEnvironment ? DATASET_PARASITE_PREFIX : "") + DATASET_AVG_MUT_STRENGTH, std::accumulate(individual.getMutationStrength().begin(), individual.getMutationStrength().end(), 0.0) / individual.getMutationStrength().size());
	// Reset statistics
	bestReward = -25;
	totalReward = 0;
	matchCount = 0;
	
	// If this is not the parasite environment, do a rating of the best individual
	if (!isParasiteEnvironment())
	{
		inRating = true;
		Parasite parasite(*options, *this);
		int rewards = 0;
		int steps = 0;
		int lapisEndings = 0;
		int caughtEndings = 0;

		// Run 100 matches agains random opponents
		for (int i = 0; i < 100; i++)
		{
			parasite.randomizeState();
			rewards += simulateGame(static_cast<Agent&>(individual), *challengeAgent, parasite, getRandomGenerator().randInt(0, 1));
			steps += stepCounter;
			if (lastEnding == CAUGHT)
				caughtEndings++;
			else if (lastEnding == LAPIS)
				lapisEndings++;
		}

		// Log the data
		learningState->addData(DATASET_CHALLENGE, rewards / (float)steps);
		learningState->addData(DATASET_LAPIS_ENDINGS, lapisEndings / 100.0);
		learningState->addData(DATASET_CAUGHT_ENDINGS, caughtEndings / 100.0);
		inRating = false;
	}

	return 0;
}

int Malmo::getStepCounter() const
{
	return stepCounter;
}

bool Malmo::isDone(AbstractMalmoAgent& ai1, AbstractMalmoAgent& ai2, int currentPlayer, int startPlayer)
{
	// Game is over when the pig is caught or one player has step into a lapis lazuli field
	return isPigCaught() || (currentPlayer == startPlayer && (fields[ai1.getLocation().x][ai1.getLocation().y + 1] == 2 || fields[ai2.getLocation().x][ai2.getLocation().y + 1] == 2));
}

bool Malmo::isPigCaught()
{
	// Check if all four directions are blocked
	return false && isFieldBlockedForPig(pig.x, pig.y + 2) && isFieldBlockedForPig(pig.x, pig.y) && isFieldBlockedForPig(pig.x - 1, pig.y + 1) && isFieldBlockedForPig(pig.x + 1, pig.y + 1);
}

bool Malmo::isFieldBlockedForPig(int x, int y)
{
	// Check if field is blocked by a fence or by an agent.
	return !isFieldAllowed(x, y) || (currentAi1->getLocation().x == x && currentAi1->getLocation().y + 1 == y) || (currentAi2->getLocation().x == x && currentAi2->getLocation().y + 1 == y);
}

void Malmo::agentMovedTo(int x, int y, int dx, int dy)
{
	// If the pig is located on the field
	if (pig.x == x && pig.y == y && !isFieldBlockedForPig(x + dx, y + dy + 1))
	{
		// Move the pig
		pig.x += dx;
		pig.y += dy;
	}
}

void Malmo::startNewGame(Parasite& parasite)
{
	// Retrieve start locations from the current parasite
	Location popStartLocation, parStartLocation;

	parStartLocation = parasite.getParStartLocation();
	popStartLocation = parasite.getPopStartLocation();
	pig = parasite.getPigStartLocation();

	/*popStartLocation.x = 4;
	popStartLocation.y = 5;
	popStartLocation.dir = 0;*/

	// Set locations
	currentAi1->setLocation(popStartLocation);
	currentAi2->setLocation(parStartLocation);

	static_cast<ChallengeAgent*>(currentAi2)->setIsStupid(parasite.getIsStupid());
}

std::map<Location, std::map<Location, int>>& Malmo::getAStarCache()
{
	return aStarCache;
}

void Malmo::setInputForAgent(std::vector<double>& input, int x, int y, int dir, int offset)
{
	// Subtract 1 from both coordinates so we get values between 0 and 7
	x -= 1;
	y -= 1;
	// Add x binary encoded
	input[offset + 0] = x == 1 || x == 3 || x == 5 || x == 7;
	input[offset + 1] = x == 2 || x == 3 || x == 6 || x == 7;
	input[offset + 2] = x >= 4;
	// Add y binary encoded
	input[offset + 3] = y == 1 || y == 3 || y == 5 || y == 7;
	input[offset + 4] = y == 2 || y == 3 || y == 6 || y == 7;
	input[offset + 5] = y >= 4;
	// Add dir binary encoded
	input[offset + 6] = dir == 180 || dir == 270;
	input[offset + 7] = dir == 90 || dir == 270;
}

void Malmo::getNNInput(std::vector<double>& input)
{
	// Resize the input to the proper size
	input.resize(8);

	// Add all positions to the input depending on the current player
	if (currentPlayer == 0)
	{
		//setInputForAgent(input, pig.x, pig.y + 1, 0, 0);
		setInputForAgent(input, currentAi1->getLocation().x, currentAi1->getLocation().y + 1, currentAi1->getLocation().dir, 0);
		//setInputForAgent(input, currentAi2->getLocation().x, currentAi2->getLocation().y + 1, currentAi2->getLocation().dir, 16);
		//setInputForAgent(input, currentAi2->getPrevLocation().x, currentAi2->getPrevLocation().y + 1, currentAi2->getPrevLocation().dir, 24);
	}
	else
	{
		//setInputForAgent(input, pig.x, pig.y + 1, 0, 0);
		setInputForAgent(input, currentAi2->getLocation().x, currentAi2->getLocation().y + 1, currentAi2->getLocation().dir, 0);
		//setInputForAgent(input, currentAi1->getLocation().x, currentAi1->getLocation().y + 1, currentAi1->getLocation().dir, 16);
		//setInputForAgent(input, currentAi1->getPrevLocation().x, currentAi1->getPrevLocation().y + 1, currentAi1->getPrevLocation().dir, 24);		
	}
}

int Malmo::getReward(AbstractMalmoAgent &agent)
{
	// Return always -1 + 25 if the pig is caught + 5 if the agent stands on lapis
	return -1 + (isPigCaught() ? 25 : 0) + (fields[agent.getLocation().x][agent.getLocation().y + 1] == 2 ? 5 : 0);
}

bool Malmo::isFieldAllowed(int x, int y, bool allowLapis)
{
	// If given field is declared as walkable (0 or 2e)
	return x >= 0 && y >= 0 && x < FIELD_SIZE && y < FIELD_SIZE && fields[x][y] != 0 && (allowLapis || fields[x][y] != 2);
}

void Malmo::startWatchMode()
{
	watchMode = true;
}

void Malmo::stopWatchMode()
{
	watchMode = false;
}

const std::vector<std::vector<int>> &Malmo::getField()
{
	return fields;
}

const AbstractMalmoAgent &Malmo::getAgent1()
{
	return *currentAi1;
}

const AbstractMalmoAgent &Malmo::getAgent2()
{
	return *currentAi2;
}

const Location& Malmo::getPig()
{
	return pig;
}