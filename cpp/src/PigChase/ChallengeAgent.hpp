#pragma once

#ifndef _CHALLENGEAGENT_H_
#define _CHALLENGEAGENT_H_

// Library Includes

// Includes
#include "AbstractMalmoAgent.hpp"
#include <utility>
#include <vector>
#include <map>

/**
 * \brief An agent which acts in the malmo environment.
 * \details Can act as a Focused/Random agent (parasite) or as a standard neural network agent.
 */
class ChallengeAgent : public AbstractMalmoAgent
{
protected:
	/**
	 * \brief An open list used for AStar calculation.
	 */
	std::vector<std::pair<int, Location>> openlist;
	std::map<Location, std::map<Location, int>> cache;
	/**
	 * \brief True, if the agent should act random.
	 */
	bool isStupid;
public:
	ChallengeAgent(Malmo& malmo_);
	/**
	 * \brief Sets, if the agent should act randomly.
	 * \param isStupid True, if randomly.
	 */
	void setIsStupid(bool isStupid);
	// Inherited:
	void doNNCalculation() override;
	bool getIsStupid() const;
};

#endif
