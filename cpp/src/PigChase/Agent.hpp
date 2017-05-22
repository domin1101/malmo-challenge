#pragma once

#ifndef _AGENT_H_
#define _AGENT_H_

// Library Includes

// Includes
#include "LightBulb/Learning/Evolution/AbstractDefaultIndividual.hpp"

// Forward declarations
class Malmo;

/**
 * \brief Desribes a location in the Malmo environment with x, y and direction.
 */
struct Location
{
	/**
	 * \brief The x-coordinate.
	 */
	int x;
	/**
	* \brief The y-coordinate.
	*/
	int y;
	/**
	* \brief The direction. (0, 90, 180 or 270)
	*/
	int dir;

	friend bool operator==(const Location& lhs, const Location& rhs)
	{
		return lhs.x == rhs.x
			&& lhs.y == rhs.y
			&& lhs.dir == rhs.dir;
	}

	/**
	 * \brief Compares two locations. (Useful for maps)
	 * \param other The other location.
	 * \return True, if the other location is "bigger" than this one.
	 */
	bool operator< (const Location& other) const
	{
		return (x < other.x || (x == other.x && y < other.y) || (x == other.x && y == other.y && dir < other.dir));
	}
};

/**
 * \brief An agent which acts in the malmo environment.
 * \details Can act as a Focused/Random agent (parasite) or as a standard neural network agent.
 */
class Agent : public LightBulb::AbstractDefaultIndividual
{
	template <class Archive>
	friend void save(Archive& archive, Agent const& agent);
	template <class Archive>
	friend void load(Archive& archive, Agent& agent);
protected:
	/**
	 * \brief An open list used for AStar calculation.
	 */
	std::vector<std::pair<int, Location>> openlist;
	/**
	 * \brief A pointer to the current environment, in which this agent should act at the moment.
	 */
	Malmo* currentGame;
	/**
	 * \brief The location of the agent.
	 */
	Location location;
	/**
	 * \brief The previous location of the agent.
	 */
	Location prevLocation;
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
	 * \brief True, if the agent acts as a parasite
	 */
	bool isParasite;
	/**
	 * \brief True, if the agent should act random.
	 */
	bool isStupid;
	/**
	 * \brief Executes one step.
	 * \param location The location which contains the current position.
	 * \return True, if step was successful.
	 */
	bool doStep(Location& location);
	/**
	 * \brief Turns left.
	 * \param location The location which contains the current position.
	 */
	void turnLeft(Location& location);
	/**
	* \brief Turns right.
	* \param location The location which contains the current position.
	*/
	void turnRight(Location& location);
	// Inherited:
	void interpretNNOutput(std::vector<double>& output) override;
	void getNNInput(std::vector<double>& input) override;
public:
	/**
	 * \brief Creates new agent.
	 * \param options The neural network options
	 * \param malmo_ The environment which should contain this agent.
	 * \param forceParasite True, if the agent should be a parasite.
	 */
	Agent(LightBulb::FeedForwardNetworkTopologyOptions& options, Malmo& malmo_, bool forceParasite = false);
	Agent() = default;
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
	 * \brief Sets the environment which will be used for simulating the next match.
	 * \param currentGame_ The environment.
	 */
	void setEnv(Malmo& currentGame_);
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
	 * \brief Returns the current location.
	 * \return The current location of the agent.
	 */
	const Location& getLocation() const;
	/**
	* \brief Returns the previous location.
	* \return The previous location of the agent.
	*/
	const Location& getPrevLocation() const;
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
	* \brief Sets the current location.
	* \param location The new location.
	*/
	void setLocation(Location location);
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
	void resetNN() override;
	void doNNCalculation() override;
	void copyPropertiesFrom(AbstractIndividual& notUsedIndividual) override;
};

#include "AgentIO.hpp"

#endif
