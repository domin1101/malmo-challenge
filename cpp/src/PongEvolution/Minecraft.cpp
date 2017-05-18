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
	auto labels = AbstractSimpleEvolutionEnvironment::getDataSetLabels();
	labels.push_back(DATASET_AVG_REWARD);
	labels.push_back(DATASET_BEST_REWARD);
	labels.push_back(DATASET_AVG_MUT_STRENGTH);
	return labels;
}


Minecraft::Minecraft(FeedForwardNetworkTopologyOptions& options_)
	: AbstractSimpleEvolutionEnvironment()
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


int Minecraft::simulateGame(Agent& agent, GamePreferences& gamePreferences)
{
	currentAgent = &agent;

	agent.resetNN();

	startNewGame(agent, gamePreferences);

	currentPlayer = 0;
	int reward = 0;
	int noPosDirChangesInRow = 0;
	int noPosChangesInRow = 0;

	for (int t = 0; t < 50; t++)
	{
		if (currentPlayer == 0)
		{
			agent.doNNCalculation();
			reward += getReward(agent);
		}
		else
			doParasiteStep();

		currentPlayer = 1 - currentPlayer;

		if (watchMode)
		{
			throwEvent(EVT_FIELD_CHANGED, *this);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		if (isDone(agent.getLocation(), parasite, currentPlayer, 0))
			break;
	}

	if (isPigCaught())
		reward += 25;

	bestReward = std::max(bestReward, reward);
	totalReward += reward;
	matchCount++;

	return reward;
}


void Minecraft::doSimulationStep()
{
	if (gamePreferenceses.size() == 0)
	{
		gamePreferenceses.resize(100);
		for (int i = 0; i < 100; i++)
			generateRandomGamePreference(gamePreferenceses[i]);
	}

	for (auto individual = individuals.begin(); individual != individuals.end(); individual++)
	{
		fitnessValues[*individual] = 0;
		for (int i = 0; i < 100; i++)
			fitnessValues[*individual] += simulateGame(static_cast<Agent&>(**individual), gamePreferenceses[i]);
	}
	learningState->addData(DATASET_BEST_REWARD, static_cast<double>(bestReward));
	learningState->addData(DATASET_AVG_REWARD, static_cast<double>(totalReward) / matchCount);
	totalReward = 0;
	matchCount = 0;
	bestReward = -25;
}

double Minecraft::getFitness(const LightBulb::AbstractIndividual& individual) const
{
	return fitnessValues[&individual];
}

void Minecraft::doParasiteStep()
{
}

int Minecraft::rateIndividual(AbstractIndividual& individual)
{
	lastBestIndividual = &individual;	
	return 0;
}

bool Minecraft::isDone(const Location& ai1, const Location& ai2, int currentPlayer, int startPlayer)
{
	return isPigCaught() || (currentPlayer == startPlayer && (fields[ai1.x][ai1.y + 1] == 2 || fields[ai2.x][ai2.y + 1] == 2));
}

bool Minecraft::isPigCaught()
{
	return false && isFieldBlockedForPig(pig.x, pig.y + 2) && isFieldBlockedForPig(pig.x, pig.y) && isFieldBlockedForPig(pig.x - 1, pig.y + 1) && isFieldBlockedForPig(pig.x + 1, pig.y + 1);
}

bool Minecraft::isFieldBlockedForPig(int x, int y)
{
	return !isFieldAllowed(x, y) || (currentAgent->getLocation().x == x && currentAgent->getLocation().y + 1 == y) || (parasite.x == x && parasite.y + 1 == y);
}


void Minecraft::generateRandomGamePreference(GamePreferences& gamePreferences)
{
	do
	{
		setToRandomLocation(gamePreferences.popStartLocation);
		setToRandomLocation(gamePreferences.parStartLocation);
		setToRandomLocation(gamePreferences.pigStartLocation);
	} while (!isValidStartConstelation(gamePreferences.popStartLocation, gamePreferences.parStartLocation, gamePreferences.pigStartLocation));

	gamePreferences.popStartLocation.dir = getRandomGenerator().randInt(0, 3) * 90;
	gamePreferences.parStartLocation.dir = getRandomGenerator().randInt(0, 3) * 90;
}

void Minecraft::startNewGame(Agent& agent, GamePreferences& gamePreferences)
{
	

	//pig.x = 2;
	//pig.y = 1;

	//parStartLocation = popStartLocation;

	agent.setLocation(gamePreferences.popStartLocation);
	parasite = gamePreferences.parStartLocation;
	pig = gamePreferences.pigStartLocation;

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


bool Minecraft::isValidStartConstelation(const Location& popStartLocation, const Location& parStartLocation, const Location& pigStartLocation)
{
	return  isFieldAllowed(popStartLocation.x, popStartLocation.y + 1, false) && isFieldAllowed(parStartLocation.x, parStartLocation.y + 1, false) && isFieldAllowed(pigStartLocation.x, pigStartLocation.y + 1, false) &&
		calcDistance(popStartLocation, parStartLocation) > 1.1f && calcDistance(parStartLocation, pigStartLocation) > 1.1f && calcDistance(popStartLocation, pigStartLocation) > 1.1f;
}

float Minecraft::calcDistance(const Location& location1, const Location& location2)
{
	return sqrt(pow((float)location1.x - (float)location2.x, 2) + pow((float)location1.y - (float)location2.y, 2));
}

void Minecraft::setToRandomLocation(Location& location)
{
	do
	{
		location.x = getRandomGenerator().randInt(2, 6);
		location.y = getRandomGenerator().randInt(1, 5);
	} while (!isFieldAllowed(location.x, location.y + 1, false));
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
	input.resize(24, 0);

	setInputForAgent(input, pig.x, pig.y + 1, 0, 0);
	setInputForAgent(input, currentAgent->getLocation().x, currentAgent->getLocation().y + 1, currentAgent->getLocation().dir, 8);
	setInputForAgent(input, parasite.x, parasite.y + 1, parasite.dir, 16);
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
	return -1 + (isPigCaught() ? 25 : 0) +(fields[agent.getLocation().x][agent.getLocation().y + 1] == 2 ? 5 : 0);
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

const Agent &Minecraft::getCurrentAgent()
{
	return *currentAgent;
}

const Location& Minecraft::getCurrentParasite()
{
	return parasite;
}

const Location& Minecraft::getPig()
{
	return pig;
}