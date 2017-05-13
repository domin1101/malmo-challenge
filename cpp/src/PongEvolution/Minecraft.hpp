#pragma once

#ifndef _MINECRAFT_H_
#define _MINECRAFT_H_

// Library Includes
#include <vector>
#include <map>
#include <array>
#include <mutex>

// Include
#include "LightBulb/Learning/Evolution/AbstractCoevolutionEnvironment.hpp"
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

class Minecraft : public LightBulb::AbstractCoevolutionEnvironment, public LightBulb::Observable<MinecraftEvents, Minecraft>
{
private:
	std::vector<std::vector<int>> fields;
	Agent* currentAi1;
	Agent* currentAi2;
	int currentPlayer;
	bool watchMode;
	std::map<int, int> grayPalette;
	int bestReward;
	int totalReward;
	int matchCount;
	bool isInteresting;
	Location pig;
	LightBulb::AbstractIndividual* lastBestIndividual;
protected:
	LightBulb::AbstractIndividual* createNewIndividual() override;
	int simulateGame(Agent& ai1, Agent& ai2, int startPlayer);
	std::unique_ptr<LightBulb::FeedForwardNetworkTopologyOptions> options;
	int doCompare(LightBulb::AbstractIndividual& obj1, LightBulb::AbstractIndividual& obj2, int round) override;
public:
	Minecraft(LightBulb::FeedForwardNetworkTopologyOptions& options_, bool isParasiteEnvironment, LightBulb::AbstractCombiningStrategy* combiningStrategy_, LightBulb::AbstractCoevolutionFitnessFunction* fitnessFunction_, const std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm>* hallOfFameToAddAlgorithm_ = nullptr, const std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm>* hallOfFameToChallengeAlgorithm_ = nullptr);
	Minecraft() = default;
	void setInputForAgent(std::vector<double>& input, int x, int y, int dir, int offset);
	void getNNInput(std::vector<double>& sight);
	int getRoundCount() const override;

	bool isFieldBlockedForPig(int x, int y);
	void startNewGame(Agent &ai1, Agent &ai2);
	void getNNInputFull(std::vector<double>& input);

	int getReward(Agent &agent);

	bool isDone(Agent &ai1, Agent &ai2, int currentPlayer, int startPlayer);
	bool isPigCaught();

	bool isFieldAllowed(int i, int i1);

	void setBlock(std::vector<double> &input, int x, int y, int dir, int value);

	void setBlock(std::vector<double> &input, int x, int y, int value);
	void startWatchMode();
	void stopWatchMode();

	const std::vector<std::vector<int>>& getField();
	const Agent& getAgent1();
	const Agent& getAgent2();
	const Location& getPig();

	std::vector<std::string> getDataSetLabels() const;

	int rateIndividual(LightBulb::AbstractIndividual &individual);
};

// USE_EXISTING_PARENT_SERIALIZATION_WITHOUT_NAMESPACE(Pong, LightBulb::AbstractCoevolutionEnvironment, LightBulb::AbstractEvolutionEnvironment);

#endif
