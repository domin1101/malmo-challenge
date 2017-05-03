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

// Forward declarations
class Agent;
class AbstractTile;

#define DATASET_PONG_RATING "Pong rating"

class Minecraft : public LightBulb::AbstractCoevolutionEnvironment
{
private:
	std::vector<std::vector<int>> fields;
	Agent* currentAi1;
	Agent* currentAi2;
	int currentPlayer;
	std::map<int, int> grayPalette;
protected:
	LightBulb::AbstractIndividual* createNewIndividual() override;
	int simulateGame(Agent& ai1, Agent& ai2);
	std::unique_ptr<LightBulb::FeedForwardNetworkTopologyOptions> options;
	int doCompare(LightBulb::AbstractIndividual& obj1, LightBulb::AbstractIndividual& obj2, int round) override;
public:
	Minecraft(LightBulb::FeedForwardNetworkTopologyOptions& options_, bool isParasiteEnvironment, LightBulb::AbstractCombiningStrategy* combiningStrategy_, LightBulb::AbstractCoevolutionFitnessFunction* fitnessFunction_, const std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm>* hallOfFameToAddAlgorithm_ = nullptr, const std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm>* hallOfFameToChallengeAlgorithm_ = nullptr);
	Minecraft() = default;
	void getNNInput(std::vector<double>& sight);
	int getRoundCount() const override;

	void startNewGame(Agent &ai1, Agent &ai2);

	int getReward(Agent &agent);

	bool isDone(Agent &ai1, Agent &ai2, int currentPlayer);

	bool isFieldAllowed(int i, int i1);

	void setBlock(std::vector<double> &input, int x, int y, int dir, int value);

	void setBlock(std::vector<double> &input, int x, int y, int value);
};

// USE_EXISTING_PARENT_SERIALIZATION_WITHOUT_NAMESPACE(Pong, LightBulb::AbstractCoevolutionEnvironment, LightBulb::AbstractEvolutionEnvironment);

#endif
