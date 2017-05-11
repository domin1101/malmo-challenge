#pragma once

#ifndef _AGENT_H_
#define _AGENT_H_

// Library Includes

// Includes
#include "LightBulb/Learning/Evolution/AbstractDefaultIndividual.hpp"
#include "LightBulb/IO/UseParentSerialization.hpp"

// Forward declarations
class Minecraft;

struct Location
{
	int x;
	int y;
	int dir;
};

class Agent : public LightBulb::AbstractDefaultIndividual
{
protected:
	Minecraft* currentGame;
	Location location;
	Location startLocation; 
	void getNNInput(std::vector<double>& input) override;
	void interpretNNOutput(std::vector<double>& output) override;
public:
	Agent(LightBulb::FeedForwardNetworkTopologyOptions& options, Minecraft& pong_);
	Agent() = default;
	void setEnv(Minecraft& currentGame_);

	Location getLocation() const;
	Location getStartLocation() const;
	void setLocation(Location location);
	Location setStartLocation(Location startLocation);
	void copyPropertiesFrom(AbstractIndividual& notUsedIndividual) override;
};

// USE_PARENT_SERIALIZATION_WITHOUT_NAMESPACE(PongAI, LightBulb::AbstractDefaultIndividual);

#endif
