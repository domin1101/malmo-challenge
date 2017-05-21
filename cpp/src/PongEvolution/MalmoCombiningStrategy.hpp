#pragma once

#ifndef _MALMOCOMBININGSTRATEGY_H_
#define _MALMOCOMBININGSTRATEGY_H_

// Include
#include "LightBulb/Learning/Evolution/SharedSamplingCombiningStrategy.hpp"
#include <Eigen/Dense>


struct MalmoCombiningResults : LightBulb::CombiningStrategyResults
{
	std::map<LightBulb::AbstractIndividual*, std::map<LightBulb::AbstractIndividual*, std::map<int, double>>> floatingResults;
};

class MalmoCombiningStrategy : public LightBulb::SharedSamplingCombiningStrategy
{
	template <class Archive>
	friend void serialize(Archive& archive, MalmoCombiningStrategy& sharedSamplingCombiningStrategy);
private:
	std::unique_ptr<MalmoCombiningResults> results;
	Eigen::VectorXd matchResults;
protected:
	void combine(LightBulb::AbstractCoevolutionEnvironment& simulationEnvironment, std::vector<LightBulb::AbstractIndividual*>& firstIndividuals, std::vector<LightBulb::AbstractIndividual*>& secondIndividuals) override;
	// Inherited:
	void executeSample(LightBulb::AbstractCoevolutionEnvironment& simulationEnvironment, std::vector<LightBulb::AbstractIndividual*>& firstIndividuals, std::vector<LightBulb::AbstractIndividual*>& sample) override;
	void setResult(LightBulb::AbstractIndividual& firstPlayer, LightBulb::AbstractIndividual& secondPlayer, int round, bool firstPlayerWon, double floatingReward);
	LightBulb::CombiningStrategyResults& execute(LightBulb::AbstractCoevolutionEnvironment& environment);
public:
	const LightBulb::CombiningStrategyResults& getPrevResults() const;
	/**
		* \brief Creates the shared sampling combining strategy.
		* \param amountOfCompetitionsPerIndividual_ Determines the number of competitions per individual.
		* \param secondEnvironment_ The other environment.
		*/
	MalmoCombiningStrategy(int amountOfCompetitionsPerIndividual_ = 0, LightBulb::AbstractCoevolutionEnvironment* secondEnvironment_ = nullptr);
};

#endif
