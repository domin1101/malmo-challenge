
#pragma once

#ifndef _MALMOEVOLUTION_H_
#define _MALMOEVOLUTION_H_

#include "LightBulbApp/TrainingPlans/AbstractCoevolutionTrainingPlan.hpp"
#include "LightBulb/NetworkTopology/FeedForwardNetworkTopology.hpp"
#include "LightBulb/Learning/Evolution/SharedSamplingCombiningStrategy.hpp"
#include "LightBulb/Learning/Evolution/AbstractHallOfFameAlgorithm.hpp"


class MalmoEvolution : public LightBulb::AbstractCoevolutionTrainingPlan
{
private:
	std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm> hof1;
	std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm> hof2;
	LightBulb::AbstractCombiningStrategy* cs1;
	LightBulb::AbstractCombiningStrategy* cs2;
protected:
	LightBulb::AbstractLearningRule* createLearningRate() override;
	LightBulb::FeedForwardNetworkTopologyOptions getNetworkOptions(int inputSize);
	LightBulb::AbstractEvolutionEnvironment* createEnvironment() override;
	LightBulb::AbstractEvolutionEnvironment* createParasiteEnvironment() override;
public:
	MalmoEvolution();
	std::string getOriginalName() const override;
	std::string getDescription() const override;
	LightBulb::AbstractTrainingPlan * createNewFromSameType() const override;
	std::string getLearningRuleName() const override;
};

USE_PARENT_SERIALIZATION_WITHOUT_NAMESPACE(MalmoEvolution, LightBulb::AbstractCoevolutionTrainingPlan);

#endif
