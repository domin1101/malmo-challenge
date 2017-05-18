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

	friend bool operator==(const Location& lhs, const Location& rhs)
	{
		return lhs.x == rhs.x
			&& lhs.y == rhs.y
			&& lhs.dir == rhs.dir;
	}
};

class Agent : public LightBulb::AbstractDefaultIndividual
{
	template <class Archive>
	friend void save(Archive& archive, Agent const& agent);
	template <class Archive>
	friend void load(Archive& archive, Agent& agent);
protected:
	Location location;
	void getNNInput(std::vector<double>& input) override;
	void interpretNNOutput(std::vector<double>& output) override;
public:
	Agent(LightBulb::FeedForwardNetworkTopologyOptions& options, Minecraft& pong_);
	Agent() = default;
	const Location& getLocation() const;
	void setLocation(Location location);
};

USE_PARENT_SERIALIZATION_WITHOUT_NAMESPACE(Agent, LightBulb::AbstractDefaultIndividual);

#endif
