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
	Minecraft* currentGame;
	Location location;
	Location popStartLocation;
	Location parStartLocation;
	Location pigStartLocation;
	void getNNInput(std::vector<double>& input) override;
	void interpretNNOutput(std::vector<double>& output) override;
public:
	Agent(LightBulb::FeedForwardNetworkTopologyOptions& options, Minecraft& pong_);
	bool isValidStartConstelation(const Location& popStartLocation, const Location& parStartLocation, const Location& pigStartLocation);
	float calcDistance(const Location& location1, const Location& location2);
	void setToRandomLocation(Location& location);
	Agent() = default;
	void setEnv(Minecraft& currentGame_);

	const Location& getLocation() const;
	const Location& getPopStartLocation() const;
	const Location& getParStartLocation() const;
	void setLocation(Location location);
	Location getPigStartLocation() const;
	void setPigStartLocation(Location pigStartLocation);
	void setPopStartLocation(Location popStartLocation);
	void setParStartLocation(Location parStartLocation);
	void copyPropertiesFrom(AbstractIndividual& notUsedIndividual) override;
};

#include "AgentIO.hpp"

#endif
