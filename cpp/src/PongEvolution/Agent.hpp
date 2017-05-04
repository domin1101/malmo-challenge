#pragma once

#ifndef _AGENT_H_
#define _AGENT_H_

// Library Includes

// Includes
#include "LightBulb/Learning/Evolution/AbstractDefaultIndividual.hpp"
#include "LightBulb/IO/UseParentSerialization.hpp"

// Forward declarations
class Minecraft;

class Agent : public LightBulb::AbstractDefaultIndividual
{
protected:
	Minecraft* currentGame;
	int x;
	int y;
	int dir;
	void getNNInput(std::vector<double>& input) override;
	void interpretNNOutput(std::vector<double>& output) override;
public:
	Agent(LightBulb::FeedForwardNetworkTopologyOptions& options, Minecraft& pong_);
	Agent() = default;
	void setEnv(Minecraft& currentGame_);
	int getX()const;
	int getY()const;

	void setPositionAndDir(int x_, int y_, int dir_);

	int getDir()const;
};

// USE_PARENT_SERIALIZATION_WITHOUT_NAMESPACE(PongAI, LightBulb::AbstractDefaultIndividual);

#endif
