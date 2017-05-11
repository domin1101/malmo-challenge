#pragma once

#ifndef _AGENTMUTATIONALGORITHM_H_
#define _AGENTMUTATIONALGORITHM_H_

// Includes
#include "LightBulb/Learning/Evolution/EvolutionStrategy/MutationAlgorithm.hpp"

/**
	* \brief  A algorithm which mutates a given individual by executing the corresponding EvolutionStrategy algorithm.
	* \details The algorithm:\n\n
	* 1. All mutationStrength values are changed randomly.\n
	* 2. The new mutationStrength values are added to their corresponding weights.
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
