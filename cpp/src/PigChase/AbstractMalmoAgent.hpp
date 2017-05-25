#pragma once

#ifndef _ABSTRACTMALMOAGENT_H_
#define _ABSTRACTMALMOAGENT_H_

// Library Includes

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
class AbstractMalmoAgent
{
protected:
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
public:
	/**
	 * \brief Creates new agent.
	 * \param malmo_ The environment which should contain this agent.
	 */
	AbstractMalmoAgent(Malmo& malmo_);
	AbstractMalmoAgent() = default;
	/**
	 * \brief Sets the environment which will be used for simulating the next match.
	 * \param currentGame_ The environment.
	 */
	void setEnv(Malmo& currentGame_);
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
	* \brief Sets the current location.
	* \param location The new location.
	*/
	void setLocation(Location location);
	virtual void reset();
	// Inherited:
	virtual void doNNCalculation() = 0;
};

#endif
