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

Minecraft::Minecraft(FeedForwardNetworkTopologyOptions& options_, bool isParasiteEnvironment_, AbstractCombiningStrategy* combiningStrategy_, AbstractCoevolutionFitnessFunction* fitnessFunction_, const std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm>* hallOfFameToAddAlgorithm_, const std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm>* hallOfFameToChallengeAlgorithm_)
	: AbstractCoevolutionEnvironment(isParasiteEnvironment_, combiningStrategy_, fitnessFunction_, hallOfFameToAddAlgorithm_, hallOfFameToChallengeAlgorithm_)
{
	options.reset(new FeedForwardNetworkTopologyOptions(options_));
	fields.resize(FIELD_SIZE, std::vector<int>(FIELD_SIZE, 0));
	fields[2][1] = 1;
	fields[3][1] = 1;
	fields[4][1] = 1;
	fields[5][1] = 1;
	fields[6][1] = 1;

	fields[2][2] = 1;
	fields[4][2] = 1;
	fields[6][2] = 1;

	fields[1][3] = 2;
	fields[2][3] = 1;
	fields[3][3] = 1;
	fields[4][3] = 1;
	fields[5][3] = 1;
	fields[6][3] = 1;
	fields[7][3] = 2;

	fields[2][4] = 1;
	fields[4][4] = 1;
	fields[6][4] = 1;

	fields[2][5] = 1;
	fields[3][5] = 1;
	fields[4][5] = 1;
	fields[5][5] = 1;
	fields[6][5] = 1;

	grayPalette[0] = 255; // Sand
	grayPalette[1] = 200; // Grass
	grayPalette[2] = 150; // Lapis
}

int Minecraft::doCompare(AbstractIndividual& obj1, AbstractIndividual& obj2, int round)
{
	return simulateGame(static_cast<Agent&>(obj1), static_cast<Agent&>(obj2));
}


int Minecraft::getRoundCount() const
{
	return 1;
}


int Minecraft::simulateGame(Agent& ai1, Agent& ai2)
{
	currentAi1 = &ai1;
	currentAi2 = &ai2;

	ai2.resetNN();
	ai1.resetNN();

	ai1.setEnv(*this);
	ai2.setEnv(*this);

	startNewGame(ai1, ai2);

	currentPlayer = 0;
	std::vector<int> rewards = {0, 0};

	for (int t = 0; t < 50; t++)
	{
		Agent& currentAgent = currentPlayer == 0 ? ai1 : ai2;

		currentAgent.doNNCalculation();
		rewards[currentPlayer] += getReward(currentAgent);

		currentPlayer = 1 - currentPlayer;

		if (isDone(ai1, ai2, currentPlayer))
			break;
	}

	if (rewards[0] == rewards[1]) {
		if (parasiteEnvironment)
			return -1;
		else
			return 1;
	}
	else
		return rewards[0] > rewards[1] ? 1 : -1;
}

bool Minecraft::isDone(Agent& ai1, Agent& ai2, int currentPlayer)
{
	return currentPlayer == 0 && (fields[ai1.getX()][ai1.getY()] == 2 || fields[ai2.getX()][ai2.getY()] == 2);
}

void Minecraft::startNewGame(Agent& ai1, Agent& ai2)
{
	ai1.setPositionAndDir(2, 3, 90);
	ai2.setPositionAndDir(3, 3, 90);
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
		setBlock(input, currentAi1->getX(), currentAi1->getX(), 100);
		setBlock(input, currentAi2->getX(), currentAi2->getX(), 50);
	}
	else
	{
		setBlock(input, currentAi2->getX(), currentAi2->getX(), 100);
		setBlock(input, currentAi1->getX(), currentAi1->getX(), 50);
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
	dir = ((((dir - 45) % 360) / 90) - 1) % 4;
	if (dir == 0)
	{
		input[(x * 2 + 1) + y * DOUBLEFIELD_SIZE * 2] = value;
		input[(x * 2 + 1) + (y * 2 + 1) * DOUBLEFIELD_SIZE] = value;
		input[x * 2 + y * DOUBLEFIELD_SIZE * 2] = (input[x * 2 + y * DOUBLEFIELD_SIZE * 2] + value) / 2;
		input[x * 2 + (y * 2 + 1) * DOUBLEFIELD_SIZE] = (input[x * 2 + (y * 2 + 1) * DOUBLEFIELD_SIZE] + value) / 2;
	}
	else if (dir == 1)
	{
		input[x * 2 + (y * 2 + 1) * DOUBLEFIELD_SIZE] = value;
		input[(x * 2 + 1) + (y * 2 + 1) * DOUBLEFIELD_SIZE] = value;
		input[x * 2 + y * DOUBLEFIELD_SIZE * 2] = (input[x * 2 + y * DOUBLEFIELD_SIZE * 2] + value) / 2;
		input[(x * 2 + 1) + y * DOUBLEFIELD_SIZE * 2] = (input[(x * 2 + 1) + y * DOUBLEFIELD_SIZE * 2] + value) / 2;
	}
	else if (dir == 2)
	{
		input[x * 2 + y * DOUBLEFIELD_SIZE * 2] = value;
		input[x * 2 + (y * 2 + 1) * DOUBLEFIELD_SIZE] = value;
		input[(x * 2 + 1) + y * DOUBLEFIELD_SIZE * 2] = (input[(x * 2 + 1) + y * DOUBLEFIELD_SIZE * 2] + value) / 2;
		input[(x * 2 + 1) + (y * 2 + 1) * DOUBLEFIELD_SIZE] = (input[(x * 2 + 1) + (y * 2 + 1) * DOUBLEFIELD_SIZE] + value) / 2;
	}
	else if (dir == 3)
	{
		input[x * 2 + y * DOUBLEFIELD_SIZE * 2] = value;
		input[(x * 2 + 1) + y * DOUBLEFIELD_SIZE * 2] = value;
		input[x * 2 + (y * 2 + 1) * DOUBLEFIELD_SIZE] = (input[x * 2 + (y * 2 + 1) * DOUBLEFIELD_SIZE] + value) / 2;
		input[(x * 2 + 1) + (y * 2 + 1) * DOUBLEFIELD_SIZE] = (input[(x * 2 + 1) + (y * 2 + 1) * DOUBLEFIELD_SIZE] + value) / 2;
	}
}

int Minecraft::getReward(Agent &agent) {
	return -1 + (fields[agent.getX()][agent.getY()] == 2 ? 5 : 0);
}

bool Minecraft::isFieldAllowed(int x, int y) {
	return fields[x][y] != 0;
}
