#pragma once

#ifndef _MALMOHALLOFFAMEALGORITHM_H_
#define _MALMOHALLOFFAMEALGORITHM_H_

// Library Includes

// Include
#include "LightBulb/Learning/Evolution/RandomHallOfFameAlgorithm.hpp"
#include <Eigen/Dense>
/**
* \brief Describes an hall of fame algorithm where individuals are evaluated against random members of the hall of fame.
*/
class MalmoHallOfFameAlgorithm : public LightBulb::RandomHallOfFameAlgorithm
{
private:
	Eigen::VectorXi matchResults;
	int amountOfCompetitionsPerIndividual;
protected:

	void evaluateIndividuals(std::vector<LightBulb::AbstractIndividual*>& individuals) override;
	void setResult(LightBulb::AbstractIndividual& individual, int memberID, int round, bool firstPlayerHasWon);
public:
	/**
		* \brief Creates a random hall of fame algorithm.
		* \param amountOfCompetitionsPerIndividual_ Determines the number of competitions per individual.
		*/
	MalmoHallOfFameAlgorithm(int amountOfCompetitionsPerIndividual_ = 0);
};

#endif
