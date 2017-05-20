// Includes
#include "PongEvolution/Minecraft.hpp"
#include "PongEvolution/Agent.hpp"
#include "LightBulb/Learning/Evolution/AbstractIndividual.hpp"
//Library includes
#include <iomanip>
#include <chrono>
#include <thread>
#include <numeric>

#define FIELD_SIZE 9
#define DOUBLEFIELD_SIZE 2 * FIELD_SIZE

using namespace LightBulb;

AbstractIndividual* Minecraft::createNewIndividual()
{
	return new Agent(*options, *this);
}

std::vector<std::string> Minecraft::getDataSetLabels() const
{
	auto labels = AbstractCoevolutionEnvironment::getDataSetLabels();
	labels.push_back(std::string(parasiteEnvironment ? DATASET_PARASITE_PREFIX : "") + DATASET_AVG_REWARD);
	labels.push_back(std::string(parasiteEnvironment ? DATASET_PARASITE_PREFIX : "") + DATASET_BEST_REWARD);
	labels.push_back(std::string(parasiteEnvironment ? DATASET_PARASITE_PREFIX : "") + DATASET_AVG_MUT_STRENGTH);
	if (!parasiteEnvironment)
		labels.push_back(DATASET_CHALLENGE);
	return labels;
}


Minecraft::Minecraft(FeedForwardNetworkTopologyOptions& options_, bool isParasiteEnvironment_, AbstractCombiningStrategy* combiningStrategy_, AbstractCoevolutionFitnessFunction* fitnessFunction_, const std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm>* hallOfFameToAddAlgorithm_, const std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm>* hallOfFameToChallengeAlgorithm_)
	: AbstractCoevolutionEnvironment(isParasiteEnvironment_, combiningStrategy_, fitnessFunction_, hallOfFameToAddAlgorithm_, hallOfFameToChallengeAlgorithm_)
{
	options.reset(new FeedForwardNetworkTopologyOptions(options_));
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

	grayPalette[0] = 255; // Sand
	grayPalette[1] = 200; // Grass
	grayPalette[2] = 150; // Lapis

	watchMode = false;
	bestReward = -25;
	totalReward = 0;
	matchCount = 0;
	lastBestIndividual = nullptr;
}

int Minecraft::doCompare(AbstractIndividual& obj1, AbstractIndividual& obj2, int round)
{
	/*for (int i = 0; i < 3; i++)
	{
		int reward = simulateGame(static_cast<Agent&>(obj1), static_cast<Agent&>(obj2), round);
		if (isParasiteEnvironment() && reward == 1)
			return 1;
		else if (!isParasiteEnvironment() && reward == -1)
			return -1;
	}
	return isParasiteEnvironment() ? -1 : 1;*/
	return simulateGame(static_cast<Agent&>(obj1), static_cast<Agent&>(obj2), round);
}


int Minecraft::getRoundCount() const
{
	return 2;
}


int Minecraft::simulateGame(Agent& ai1, Agent& ai2, int startPlayer)
{
	currentAi1 = &ai1;
	currentAi2 = &ai2;

	ai2.resetNN();
	ai1.resetNN();

	ai1.setEnv(*this);
	ai2.setEnv(*this);

	startNewGame(ai1, ai2);

	if (parasiteEnvironment)
		startPlayer = 1 - startPlayer;
	currentPlayer = startPlayer;
	std::vector<int> rewards = {0, 0};
	int noPosDirChangesInRow = 0;
	int noPosChangesInRow = 0;
	stepCounter = 0;

	for (int t = 0; t < 50; t++)
	{
		Agent& currentAgent = currentPlayer == 0 ? ai1 : ai2;

		Location prevLocation = currentAgent.getLocation();
		currentAgent.doNNCalculation();
		rewards[currentPlayer] += getReward(currentAgent);

		if (currentPlayer == 0)
			stepCounter++;

		if (prevLocation.x == currentAgent.getLocation().x && prevLocation.y == currentAgent.getLocation().y)
		{
			if (prevLocation.dir == currentAgent.getLocation().dir)
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

		currentPlayer = 1 - currentPlayer;

		if (watchMode && lastBestIndividual == &ai1)
		{
			throwEvent(EVT_FIELD_CHANGED, *this);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		if (isDone(ai1, ai2, currentPlayer, startPlayer))
			break;
	}
	
	if (isPigCaught())
		rewards[currentPlayer] += 25;

	bestReward = std::max(bestReward, rewards[0]);
	totalReward += rewards[0];
	matchCount++;

	if (parasiteEnvironment)
		return rewards[1];
	else
		return rewards[0];

	if (rewards[0] == rewards[1]) {
		if (parasiteEnvironment)
			return -1;
		else
			return 1;
	}
	else
		return rewards[0] > rewards[1] ? 1 : -1;
}

int Minecraft::rateIndividual(AbstractIndividual& individual)
{
	lastBestIndividual = &individual;
	learningState->addData(std::string(parasiteEnvironment ? DATASET_PARASITE_PREFIX : "") + DATASET_BEST_REWARD, static_cast<double>(bestReward));
	learningState->addData(std::string(parasiteEnvironment ? DATASET_PARASITE_PREFIX : "") + DATASET_AVG_REWARD, static_cast<double>(totalReward) / matchCount);
	learningState->addData(std::string(parasiteEnvironment ? DATASET_PARASITE_PREFIX : "") + DATASET_AVG_MUT_STRENGTH, std::accumulate(individual.getMutationStrength().begin(), individual.getMutationStrength().end(), 0.0) / individual.getMutationStrength().size());
	bestReward = -25;
	totalReward = 0;
	matchCount = 0;


	if (!isParasiteEnvironment())
	{
		Agent parasite(*options, *this, true);
		int rewards = 0;
		int steps = 0;

		for (int i = 0; i < 100; i++)
		{
			parasite.randomizeState();
			rewards += simulateGame(static_cast<Agent&>(individual), parasite, getRandomGenerator().randInt(0, 1));
			steps += stepCounter;
		}

		learningState->addData(DATASET_CHALLENGE, rewards / (float)steps);
	}


	return 0;
}

bool Minecraft::isDone(Agent& ai1, Agent& ai2, int currentPlayer, int startPlayer)
{
	return isPigCaught() || (currentPlayer == startPlayer && (fields[ai1.getLocation().x][ai1.getLocation().y + 1] == 2 || fields[ai2.getLocation().x][ai2.getLocation().y + 1] == 2));
}

bool Minecraft::isPigCaught()
{
	return isFieldBlockedForPig(pig.x, pig.y + 2) && isFieldBlockedForPig(pig.x, pig.y) && isFieldBlockedForPig(pig.x - 1, pig.y + 1) && isFieldBlockedForPig(pig.x + 1, pig.y + 1);
}

bool Minecraft::isFieldBlockedForPig(int x, int y)
{
	return !isFieldAllowed(x, y) || (currentAi1->getLocation().x == x && currentAi1->getLocation().y + 1 == y) || (currentAi2->getLocation().x == x && currentAi2->getLocation().y + 1 == y);
}

void Minecraft::agentMovedTo(int x, int y, int dx, int dy)
{
	if (pig.x == x && pig.y == y && !isFieldBlockedForPig(x + dx, y + dy + 1))
	{
		pig.x += dx;
		pig.y += dy;
	}
}

void Minecraft::startNewGame(Agent& ai1, Agent& ai2)
{
	Location popStartLocation, parStartLocation;
	if (isParasiteEnvironment())
	{
		parStartLocation = ai1.getParStartLocation();
		popStartLocation = ai1.getPopStartLocation();
		pig = ai1.getPigStartLocation();
	}
	else
	{
		parStartLocation = ai2.getParStartLocation();
		popStartLocation = ai2.getPopStartLocation();
		pig = ai2.getPigStartLocation();
	}

	//pig.x = 2;
	//pig.y = 1;

	//parStartLocation = popStartLocation;

	ai1.setLocation(isParasiteEnvironment() ? parStartLocation : popStartLocation);
	ai2.setLocation(isParasiteEnvironment() ? popStartLocation : parStartLocation);

	isInteresting = (isParasiteEnvironment() && ai1.getLocation().dir == 0 && ai2.getLocation().dir == 90);
	/*
	do
	{
		ai1.setPositionAndDir(getRandomGenerator().randInt(2, 6), 3, getRandomGenerator().randInt(0, 3) * 90);
	} while (!isFieldAllowed(ai1.getLocation().x, ai1.getLocation().y + 1));

	do
	{
		ai2.setPositionAndDir(getRandomGenerator().randInt(2, 6), 3, getRandomGenerator().randInt(0, 3) * 90);
	} while (!isFieldAllowed(ai2.getLocation().x, ai2.getLocation().y + 1));*/
}

void Minecraft::getNNInputFull(std::vector<double>& input)
{
	input.resize(FIELD_SIZE * FIELD_SIZE * 4, 0);

	for (int x = 0; x < FIELD_SIZE; x++)
	{
		for (int y = 0; y < FIELD_SIZE; y++)
		{
			setBlock(input, x, y, grayPalette[fields[x][y]]);
		}
	}

	if (currentPlayer == 0)
	{
		setBlock(input, currentAi1->getLocation().x, currentAi1->getLocation().y + 1, currentAi1->getLocation().dir, 50);
		//setBlock(input, currentAi2->getLocation().x, currentAi2->getLocation().y + 1, currentAi2->getLocation().dir, 100);
	}
	else
	{
		setBlock(input, currentAi2->getLocation().x, currentAi2->getLocation().y + 1, currentAi2->getLocation().dir, 50);
		//setBlock(input, currentAi1->getLocation().x, currentAi1->getLocation().y + 1, currentAi1->getLocation().dir, 100);
	}

	/*
	std::string output = "";
	for (int y = 0; y < DOUBLEFIELD_SIZE; y++)
	{
		for (int x = 0; x < DOUBLEFIELD_SIZE; x++)
		{
			output += std::to_string((int)input[x + y * DOUBLEFIELD_SIZE]) + " ";
		}
		output += "\n";
	}
	std::cout << output;*/

	std::for_each(input.begin(), input.end(), [](double &n){ n /= 255; });
}


void Minecraft::setInputForAgent(std::vector<double>& input, int x, int y, int dir, int offset)
{
	x -= 1;
	y -= 1;
	input[offset + 0] = x == 1 || x == 3 || x == 5 || x == 7;
	input[offset + 1] = x == 2 || x == 3 || x == 6 || x == 7;
	input[offset + 2] = x >= 4;
	input[offset + 3] = y == 1 || y == 3 || y == 5 || y == 7;
	input[offset + 4] = y == 2 || y == 3 || y == 6 || y == 7;
	input[offset + 5] = y >= 4;
	input[offset + 6] = dir == 180 || dir == 270;
	input[offset + 7] = dir == 90 || dir == 270;
}

void Minecraft::getNNInput(std::vector<double>& input)
{
	bool isParasite = currentPlayer == 0 && isParasiteEnvironment() || currentPlayer == 1 && !isParasiteEnvironment();
	input.resize(isParasite ? 16 : 24, 0);

	if (currentPlayer == 0)
	{
		setInputForAgent(input, pig.x, pig.y + 1, 0, 0);
		setInputForAgent(input, currentAi1->getLocation().x, currentAi1->getLocation().y + 1, currentAi1->getLocation().dir, 8);
		if (!isParasite)
			setInputForAgent(input, currentAi2->getLocation().x, currentAi2->getLocation().y + 1, currentAi2->getLocation().dir, 16);
			
	}
	else
	{
		setInputForAgent(input, pig.x, pig.y + 1, 0, 0);
		setInputForAgent(input, currentAi2->getLocation().x, currentAi2->getLocation().y + 1, currentAi2->getLocation().dir, 8);
		if (!isParasite)
			setInputForAgent(input, currentAi1->getLocation().x, currentAi1->getLocation().y + 1, currentAi1->getLocation().dir, 16);
	}
}




void Minecraft::setBlock(std::vector<double>& input, int x, int y, int value)
{
	input[x * 2 + y * DOUBLEFIELD_SIZE * 2] = value;
	input[(x * 2 + 1) + y * DOUBLEFIELD_SIZE * 2] = value;
	input[x * 2 + (y * 2 + 1) * DOUBLEFIELD_SIZE] = value;
	input[(x * 2 + 1) + (y * 2 + 1) * DOUBLEFIELD_SIZE] = value;
}

void Minecraft::setBlock(std::vector<double>& input, int x, int y, int dir, int value)
{
	if (dir == 180)
	{
		input[x * 2 + (y * 2 + 1) * DOUBLEFIELD_SIZE] = value;
		input[(x * 2 + 1) + (y * 2 + 1) * DOUBLEFIELD_SIZE] = value;
		input[x * 2 + y * DOUBLEFIELD_SIZE * 2] = (input[x * 2 + y * DOUBLEFIELD_SIZE * 2] + value) / 2;
		input[(x * 2 + 1) + y * DOUBLEFIELD_SIZE * 2] = (input[(x * 2 + 1) + y * DOUBLEFIELD_SIZE * 2] + value) / 2;
	}
	else if (dir == 270)
	{
		input[(x * 2 + 1) + y * DOUBLEFIELD_SIZE * 2] = value;
		input[(x * 2 + 1) + (y * 2 + 1) * DOUBLEFIELD_SIZE] = value;
		input[x * 2 + y * DOUBLEFIELD_SIZE * 2] = (input[x * 2 + y * DOUBLEFIELD_SIZE * 2] + value) / 2;
		input[x * 2 + (y * 2 + 1) * DOUBLEFIELD_SIZE] = (input[x * 2 + (y * 2 + 1) * DOUBLEFIELD_SIZE] + value) / 2;
	}
	else if (dir == 0)
	{
		input[x * 2 + y * DOUBLEFIELD_SIZE * 2] = value;
		input[(x * 2 + 1) + y * DOUBLEFIELD_SIZE * 2] = value;
		input[x * 2 + (y * 2 + 1) * DOUBLEFIELD_SIZE] = (input[x * 2 + (y * 2 + 1) * DOUBLEFIELD_SIZE] + value) / 2;
		input[(x * 2 + 1) + (y * 2 + 1) * DOUBLEFIELD_SIZE] = (input[(x * 2 + 1) + (y * 2 + 1) * DOUBLEFIELD_SIZE] + value) / 2;
	}
	else if (dir == 90)
	{
		input[x * 2 + y * DOUBLEFIELD_SIZE * 2] = value;
		input[x * 2 + (y * 2 + 1) * DOUBLEFIELD_SIZE] = value;
		input[(x * 2 + 1) + y * DOUBLEFIELD_SIZE * 2] = (input[(x * 2 + 1) + y * DOUBLEFIELD_SIZE * 2] + value) / 2;
		input[(x * 2 + 1) + (y * 2 + 1) * DOUBLEFIELD_SIZE] = (input[(x * 2 + 1) + (y * 2 + 1) * DOUBLEFIELD_SIZE] + value) / 2;
	}
}

int Minecraft::getReward(Agent &agent)
{
	return -1 + (isPigCaught() ? 25 : 0) +(fields[agent.getLocation().x][agent.getLocation().y + 1] == 2 && (isParasiteEnvironment() && currentAi1->getIsStupid() || !isParasiteEnvironment() && currentAi2->getIsStupid()) ? 5 : 0);
}

bool Minecraft::isFieldAllowed(int x, int y, bool allowLapis)
{
	return x >= 0 && y >= 0 && x < FIELD_SIZE && y < FIELD_SIZE && fields[x][y] != 0 && (allowLapis || fields[x][y] != 2);
}

void Minecraft::startWatchMode()
{
	watchMode = true;
}

void Minecraft::stopWatchMode()
{
	watchMode = false;
}

const std::vector<std::vector<int>> &Minecraft::getField()
{
	return fields;
}

const Agent &Minecraft::getAgent1()
{
	return *currentAi1;
}

const Agent &Minecraft::getAgent2()
{
	return *currentAi2;
}

const Location& Minecraft::getPig()
{
	return pig;
}