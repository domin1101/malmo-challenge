#pragma once

#ifndef _AGENT_H_
#define _AGENT_H_

// Library Includes

// Includes
#include "LightBulb/Learning/Evolution/AbstractDefaultIndividual.hpp"
#include "AbstractMalmoAgent.hpp"

// Forward declarations
class Malmo;

/**
 * \brief An agent which acts in the malmo environment.
 * \details Can act as a Focused/Random agent (parasite) or as a standard neural network agent.
 */
class Agent : public LightBulb::AbstractDefaultIndividual, public AbstractMalmoAgent
{
	template <class Archive>
	friend void save(Archive& archive, Agent const& agent);
	template <class Archive>
	friend void load(Archive& archive, Agent& agent);
protected:
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
	Agent(LightBulb::FeedForwardNetworkTopologyOptions& options, Malmo& malmo_);
	Agent() = default;
	void reset() override;
	// Inherited:
	void resetNN() override;
	void doNNCalculation() override;
};

#include "AgentIO.hpp"

#endif
