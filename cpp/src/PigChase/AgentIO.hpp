#pragma once

#ifndef _AGENTIO_H_
#define _AGENTIO_H_

// Includes
#include "Agent.hpp"

// Libraray includes
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

template <class Archive>
void serialize(Archive& archive, Location& location)
{
	archive(cereal::make_nvp("x", location.x));
	archive(cereal::make_nvp("y", location.y));
	archive(cereal::make_nvp("dir", location.dir));
}

template <class Archive>
void save(Archive& archive, Agent const& agent)
{
	archive(cereal::base_class<LightBulb::AbstractDefaultIndividual>(&agent));
	//archive(cereal::make_nvp("parStartLocation", agent.parStartLocation));
	//archive(cereal::make_nvp("popStartLocation", agent.popStartLocation));
	//archive(cereal::make_nvp("pigStartLocation", agent.pigStartLocation));
	//archive(cereal::make_nvp("isStupid", agent.isStupid));
}

template <class Archive>
void load(Archive& archive, Agent& agent)
{
	archive(cereal::base_class<LightBulb::AbstractDefaultIndividual>(&agent));
	//archive(cereal::make_nvp("parStartLocation", agent.parStartLocation));
	//archive(cereal::make_nvp("popStartLocation", agent.popStartLocation));
	//archive(cereal::make_nvp("pigStartLocation", agent.pigStartLocation));
	//archive(cereal::make_nvp("isStupid", agent.isStupid));
}

#include "LightBulb/IO/UsedArchives.hpp"

CEREAL_REGISTER_TYPE(Agent);

#endif