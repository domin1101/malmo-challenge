#pragma once

#ifndef _MALMOCOMBININGSTRATEGY_H_
#define _MALMOCOMBININGSTRATEGY_H_

// Include
#include "LightBulb/Learning/Evolution/SharedSamplingCombiningStrategy.hpp"
#include <Eigen/Dense>

/**
 * \brief Declares a new result structure which supports floating values.
 */
struct MalmoCombiningResults : LightBulb::CombiningStrategyResults
{
	std::map<LightBulb::AbstractIndividual*, std::map<LightBulb::AbstractIndividual*, std::map<int, double>>> floatingResults;
};

/**
 * \brief A combining strategy used for the malmo environment.
 * \details This combining strategy works with rewards.
 * A parasite agent gets its result value depending on the difference between the reward of the opponent and the maximum reward possible to achieve against this agent.
 * The non-parasite agents gets its result value just depending on its reward.
 * The result values are normalized between 0 and 1.
 */
class MalmoCombiningStrategy : public LightBulb::SharedSamplingCombiningStrategy
{
	template <class Archive>
	friend void serialize(Archive& archive, MalmoCombiningStrategy& sharedSamplingCombiningStrategy);
private:
	/**
	 * \brief Stores all results of the current iteration.
	 */
	std::unique_ptr<MalmoCombiningResults> results;
	/**
	 * \brief Stores the match rewards of the non-parasite agent in the current iteration.
	 */
	Eigen::VectorXi matchResults;
protected:	
	// Inherited:
	void combine(LightBulb::AbstractCoevolutionEnvironment& simulationEnvironment, std::vector<LightBulb::AbstractIndividual*>& firstIndividuals, std::vector<LightBulb::AbstractIndividual*>& secondIndividuals) override;
	void executeSample(LightBulb::AbstractCoevolutionEnvironment& simulationEnvironment, std::vector<LightBulb::AbstractIndividual*>& firstIndividuals, std::vector<LightBulb::AbstractIndividual*>& sample) override;
	void setResult(LightBulb::AbstractIndividual& firstPlayer, LightBulb::AbstractIndividual& secondPlayer, int round, bool firstPlayerWon, double floatingReward);
	LightBulb::CombiningStrategyResults& execute(LightBulb::AbstractCoevolutionEnvironment& environment) override;
public:
	/**
		* \brief Creates the malmo combining strategy.
		* \param amountOfCompetitionsPerIndividual_ Determines the number of competitions per individual.
		* \param secondEnvironment_ The other environment.
		*/
	MalmoCombiningStrategy(int amountOfCompetitionsPerIndividual_ = 0, LightBulb::AbstractCoevolutionEnvironment* secondEnvironment_ = nullptr);
	// Inherited:
	const LightBulb::CombiningStrategyResults& getPrevResults() const override;
};

#endif
