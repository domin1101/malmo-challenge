
#pragma once

#ifndef _PIGCHASEEVOLUTION_H_
#define _PIGCHASEEVOLUTION_H_

#include "LightBulbApp/TrainingPlans/AbstractCoevolutionTrainingPlan.hpp"
#include "LightBulb/NetworkTopology/FeedForwardNetworkTopology.hpp"
#include "LightBulb/Learning/Evolution/SharedSamplingCombiningStrategy.hpp"
#include "LightBulb/Learning/Evolution/AbstractHallOfFameAlgorithm.hpp"

/**
 * \brief Describes a training plan for the pig chase challenge.
 * \details Sets up coevolution learning rule for training agents in the malmo environment.
 */
class PigChaseEvolution : public LightBulb::AbstractCoevolutionTrainingPlan
{
private:
	/**
	 * \brief The first hall of fame algorithm to use.
	 */
	std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm> hof1;
	/**
	* \brief The second hall of fame algorithm to use.
	*/
	std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm> hof2;
	/**
	 * \brief The first combining strategy to use.
	 */
	LightBulb::AbstractCombiningStrategy* cs1;
	/**
	* \brief The second combining strategy to use.
	*/
	LightBulb::AbstractCombiningStrategy* cs2;
protected:
	// Inherited:
	LightBulb::AbstractLearningRule* createLearningRate() override;
	LightBulb::FeedForwardNetworkTopologyOptions getNetworkOptions(int inputSize);
	LightBulb::AbstractEvolutionEnvironment* createEnvironment() override;
	LightBulb::AbstractEvolutionEnvironment* createParasiteEnvironment() override;
public:
	PigChaseEvolution();
	std::string getOriginalName() const override;
	std::string getDescription() const override;
	LightBulb::AbstractTrainingPlan * createNewFromSameType() const override;
	std::string getLearningRuleName() const override;
};

USE_PARENT_SERIALIZATION_WITHOUT_NAMESPACE(PigChaseEvolution, LightBulb::AbstractCoevolutionTrainingPlan);

#endif
