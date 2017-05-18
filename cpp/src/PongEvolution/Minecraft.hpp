#pragma once

#ifndef _MINECRAFT_H_
#define _MINECRAFT_H_

// Library Includes
#include <vector>
#include <map>
#include <array>
#include <mutex>

// Include
#include <LightBulb/Learning/Evolution/AbstractSimpleEvolutionEnvironment.hpp>
#include "LightBulb/IO/UseParentSerialization.hpp"
#include "LightBulb/NetworkTopology/FeedForwardNetworkTopology.hpp"
#include "Agent.hpp"

// Forward declarations
class Agent;
class AbstractTile;

#define DATASET_AVG_REWARD "Avg reward"
#define DATASET_BEST_REWARD "Best reward"
#define DATASET_AVG_MUT_STRENGTH "Average mutation strength"

enum MinecraftEvents
{
	EVT_FIELD_CHANGED
};

struct GamePreferences
{
	Location popStartLocation, parStartLocation, pigStartLocation;
};

class Minecraft : public LightBulb::AbstractSimpleEvolutionEnvironment, public LightBulb::Observable<MinecraftEvents, Minecraft>
{
private:
	std::vector<GamePreferences> gamePreferenceses;
	std::vector<std::vector<int>> fields;
	Agent* currentAgent;
	int currentPlayer;
	bool watchMode;
	std::map<int, int> grayPalette;
	int bestReward;
	int totalReward;
	int matchCount;
	bool isInteresting;
	Location pig;
	Location parasite;
	LightBulb::AbstractIndividual* lastBestIndividual;
protected:
	LightBulb::AbstractIndividual* createNewIndividual() override;
	void doParasiteStep();
	int simulateGame(Agent& agent, GamePreferences& gamePreferences);
	std::unique_ptr<LightBulb::FeedForwardNetworkTopologyOptions> options;	
	mutable std::map<const LightBulb::AbstractIndividual*, double> fitnessValues;
public:
	Minecraft(LightBulb::FeedForwardNetworkTopologyOptions& options_);
	Minecraft() = default;
	void setInputForAgent(std::vector<double>& input, int x, int y, int dir, int offset);
	void getNNInput(std::vector<double>& sight);

	bool isFieldBlockedForPig(int x, int y);
	bool isValidStartConstelation(const Location& popStartLocation, const Location& parStartLocation, const Location& pigStartLocation);
	float calcDistance(const Location& location1, const Location& location2);
	void setToRandomLocation(Location& location);

	int getReward(Agent &agent);

	bool isDone(const Location& ai1, const Location& ai2, int currentPlayer, int startPlayer);
	bool isPigCaught();

	bool isFieldAllowed(int x, int y, bool allowLapis = true);

	void setBlock(std::vector<double> &input, int x, int y, int dir, int value);

	void setBlock(std::vector<double> &input, int x, int y, int value);
	void startWatchMode();
	void stopWatchMode();

	const std::vector<std::vector<int>>& getField();
	const Agent& getCurrentAgent();
	const Location& getCurrentParasite();
	const Location& getPig();

	void generateRandomGamePreference(GamePreferences& gamePreferences);
	void startNewGame(Agent& agent, GamePreferences& gamePreferences);
	void doSimulationStep() override;
	double getFitness(const LightBulb::AbstractIndividual& individual) const override;
	std::vector<std::string> getDataSetLabels() const;

	int rateIndividual(LightBulb::AbstractIndividual &individual);
};

USE_EXISTING_PARENT_SERIALIZATION_WITHOUT_NAMESPACE(Minecraft, LightBulb::AbstractSimpleEvolutionEnvironment, LightBulb::AbstractEvolutionEnvironment);

#endif
