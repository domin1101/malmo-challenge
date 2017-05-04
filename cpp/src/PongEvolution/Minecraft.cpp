// Includes
#include "PongEvolution/Minecraft.hpp"
#include "PongEvolution/Agent.hpp"
#include "LightBulb/Learning/Evolution/AbstractIndividual.hpp"
//Library includes
#include <iomanip>
#include <chrono>
#include <thread>

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
	return labels;
}


Minecraft::Minecraft(FeedForwardNetworkTopologyOptions& options_, bool isParasiteEnvironment_, AbstractCombiningStrategy* combiningStrategy_, AbstractCoevolutionFitnessFunction* fitnessFunction_, const std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm>* hallOfFameToAddAlgorithm_, const std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm>* hallOfFameToChallengeAlgorithm_)
	: AbstractCoevolutionEnvironment(isParasiteEnvironment_, combiningStrategy_, fitnessFunction_, hallOfFameToAddAlgorithm_, hallOfFameToChallengeAlgorithm_)
{
	options.reset(new FeedForwardNetworkTopologyOptions(options_));
	fields.resize(FIELD_SIZE, std::vector<int>(FIELD_SIZE, 0));
	fields[0][0] = 1;
	fields[1][0] = 1;
	fields[2][0] = 1;
	fields[3][0] = 1;
	fields[4][0] = 1;
	fields[5][0] = 1;
	fields[6][0] = 1;
	fields[7][0] = 1;
	fields[8][0] = 1;

	fields[0][1] = 1;
	fields[8][1] = 1;

	fields[0][2] = 1;
	fields[2][2] = 1;
	fields[3][2] = 1;
	fields[4][2] = 1;
	fields[5][2] = 1;
	fields[6][2] = 1;
	fields[8][2] = 1;

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

	fields[0][6] = 1;
	fields[2][6] = 1;
	fields[3][6] = 1;
	fields[4][6] = 1;
	fields[5][6] = 1;
	fields[6][6] = 1;
	fields[8][6] = 1;

	fields[0][7] = 1;
	fields[8][7] = 1;

	fields[0][8] = 1;
	fields[1][8] = 1;
	fields[2][8] = 1;
	fields[3][8] = 1;
	fields[4][8] = 1;
	fields[5][8] = 1;
	fields[6][8] = 1;
	fields[7][8] = 1;
	fields[8][8] = 1;

	grayPalette[0] = 255; // Sand
	grayPalette[1] = 200; // Grass
	grayPalette[2] = 150; // Lapis

	watchMode = false;
	bestReward = -25;
	totalReward = 0;
	matchCount = 0;
}

int Minecraft::doCompare(AbstractIndividual& obj1, AbstractIndividual& obj2, int round)
{
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

	currentPlayer = startPlayer;
	std::vector<int> rewards = {0, 0};

	for (int t = 0; t < 50; t++)
	{
		Agent& currentAgent = currentPlayer == 0 ? ai1 : ai2;

		currentAgent.doNNCalculation();
		rewards[currentPlayer] += getReward(currentAgent);

		currentPlayer = 1 - currentPlayer;

		if (watchMode)
		{
			throwEvent(EVT_FIELD_CHANGED, *this);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

		if (isDone(ai1, ai2, currentPlayer))
			break;
	}

	bestReward = std::max(bestReward, rewards[0]);
	totalReward += rewards[0];
	matchCount++;

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
	learningState->addData(std::string(parasiteEnvironment ? DATASET_PARASITE_PREFIX : "") + DATASET_BEST_REWARD, static_cast<double>(bestReward));
	learningState->addData(std::string(parasiteEnvironment ? DATASET_PARASITE_PREFIX : "") + DATASET_AVG_REWARD, static_cast<double>(totalReward) / matchCount);
	bestReward = -25;
	totalReward = 0;
	matchCount = 0;
}

bool Minecraft::isDone(Agent& ai1, Agent& ai2, int currentPlayer)
{
	return currentPlayer == 0 && (fields[ai1.getX()][ai1.getY() + 1] == 2 || fields[ai2.getX()][ai2.getY() + 1] == 2);
}

void Minecraft::startNewGame(Agent& ai1, Agent& ai2)
{
	ai1.setPositionAndDir(2, 3, 270);
	ai2.setPositionAndDir(2, 3, 270);
}

void Minecraft::getNNInput(std::vector<double>& input)
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
		setBlock(input, currentAi1->getX(), currentAi1->getY() + 1, currentAi1->getDir(), 50);
		setBlock(input, currentAi2->getX(), currentAi2->getY() + 1, currentAi2->getDir(), 100);
	}
	else
	{
		setBlock(input, currentAi2->getX(), currentAi2->getY() + 1, currentAi2->getDir(), 50);
		setBlock(input, currentAi1->getX(), currentAi1->getY() + 1, currentAi1->getDir(), 100);
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
	return -1 + (fields[agent.getX()][agent.getY() + 1] == 2 ? 5 : 0);
}

bool Minecraft::isFieldAllowed(int x, int y)
{
	return fields[x][y] != 0;
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