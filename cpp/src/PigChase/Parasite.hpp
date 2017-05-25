#pragma once

#ifndef _PARASITE_H_
#define _PARASITE_H_

// Library Includes

// Includes
#include "Agent.hpp"

// Forward declarations
class Malmo;

/**
 * \brief An agent which acts in the malmo environment.
 * \details Can act as a Focused/Random agent (parasite) or as a standard neural network agent.
 */
class Parasite : public Agent
{
	template <class Archive>
	friend void save(Archive& archive, Agent const& agent);
	template <class Archive>
	friend void load(Archive& archive, Agent& agent);
protected:
	/**
	 * \brief The starting position of the first agent.
	 * \note Used only if agent is parasite.
	 */
	Location popStartLocation;
	/**
	* \brief The starting position of the second agent (parasite).
	* \note Used only if agent is parasite.
	*/
	Location parStartLocation;
	/**
	* \brief The starting position of the second agent.
	* \note Used only if agent is parasite.
	*/
	Location pigStartLocation;
	/**
	 * \brief True, if the agent should act random.
	 */
	bool isStupid;
public:
	/**
	 * \brief Creates new agent.
	 * \param options The neural network options
	 * \param malmo_ The environment which should contain this agent.
	 * \param forceParasite True, if the agent should be a parasite.
	 */
	Parasite(LightBulb::FeedForwardNetworkTopologyOptions& options, Malmo& malmo_);
	Parasite() = default;
	/**
	 * \brief Calculates if the given start constelation is valid.
	 * \param popStartLocation The start location of the first agent.
	 * \param parStartLocation The start location of the second agent.
	 * \param pigStartLocation The start location of the pig.
	 * \return True, if valid.
	 */
	bool isValidStartConstellation(const Location& popStartLocation, const Location& parStartLocation, const Location& pigStartLocation);
	/**
	 * \brief Returns the euclidiean distance between two locations.
	 * \param location1 The first location.
	 * \param location2 The second location.
	 * \return The calculated distance.
	 */
	float calcDistance(const Location& location1, const Location& location2);
	/**
	 * \brief Sets the given location to a valid random location.
	 * \param location The location to randomize.
	 */
	void setToRandomLocation(Location& location);
	/**
	 * \brief Returns if the agent acts randomly.
	 * \return True, if randomly.
	 */
	bool getIsStupid() const;
	/**
	 * \brief Sets, if the agent should act randomly.
	 * \param isStupid True, if randomly.
	 */
	void setIsStupid(bool isStupid);
	/**
	* \brief Returns the start location of the first agent.
	* \return The start location of the first agent.
	*/
	const Location& getPopStartLocation() const;
	/**
	* \brief Returns the start location of the second agent.
	* \return The start location of the second agent.
	*/
	const Location& getParStartLocation() const;
	/**
	* \brief Returns the start location of the pig.
	* \return The start location of the pig.
	*/
	Location getPigStartLocation() const;
	/**
	* \brief Sets the start location of the pig.
	* \return The new start location of the pig.
	*/
	void setPigStartLocation(Location pigStartLocation);
	/**
	* \brief Sets the start location of the first agent.
	* \return The new start location of the first agent.
	*/
	void setPopStartLocation(Location popStartLocation);
	/**
	* \brief Sets the start location of the second agent.
	* \return The new start location of the second agent.
	*/
	void setParStartLocation(Location parStartLocation);
	/**
	 * \brief Randomizes all starting locations.
	 */
	void randomizeState();
	// Inherited:
	void copyPropertiesFrom(AbstractIndividual& notUsedIndividual) override;
};

#endif
