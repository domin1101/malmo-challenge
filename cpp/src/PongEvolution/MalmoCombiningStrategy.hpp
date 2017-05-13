#pragma once

#ifndef _MALMOCOMBININGSTRATEGY_H_
#define _MALMOCOMBININGSTRATEGY_H_

// Include
#include "LightBulb/Learning/Evolution/SharedSamplingCombiningStrategy.hpp"
#include <Eigen/Dense>

class MalmoCombiningStrategy : public LightBulb::SharedSamplingCombiningStrategy
{
	template <class Archive>
	friend void serialize(Archive& archive, MalmoCombiningStrategy& sharedSamplingCombiningStrategy);
private:
	Eigen::VectorXi matchResults;
protected:
	// Inherited:
	void executeSample(LightBulb::AbstractCoevolutionEnvironment& simulationEnvironment, std::vector<LightBulb::AbstractIndividual*>& firstIndividuals, std::vector<LightBulb::AbstractIndividual*>& sample) override;
public:
	/**
		* \brief Creates the shared sampling combining strategy.
		* \param amountOfCompetitionsPerIndividual_ Determines the number of competitions per individual.
		* \param secondEnvironment_ The other environment.
		*/
	MalmoCombiningStrategy(int amountOfCompetitionsPerIndividual_ = 0, LightBulb::AbstractCoevolutionEnvironment* secondEnvironment_ = nullptr);
};

#endif
