#pragma once

#ifndef _AGENTMUTATIONALGORITHM_H_
#define _AGENTMUTATIONALGORITHM_H_

// Includes
#include "LightBulb/Learning/Evolution/EvolutionStrategy/MutationAlgorithm.hpp"

/**
 * \brief A mutation algorithm which mutates next to the neural network also the agents/pig start positions.
 */
class AgentMutationAlgorithm : public LightBulb::MutationAlgorithm
{
private:
public:
	AgentMutationAlgorithm(double mutationStrengthChangeSpeed_, double mutationStrengthMax_ = 50, double mutationStrengthMin_ = 0.000001f);
	// Inherited:
	void execute(LightBulb::AbstractIndividual& individual1) override;
	LightBulb::AbstractMutationAlgorithm* clone() const override;
};

#endif
