#pragma once

#ifndef _MALMOFITNESSFUNCTION_H_
#define _MALMOFITNESSFUNCTION_H_

// Library Includes

// Include
#include "LightBulb/Learning/Evolution/AbstractCoevolutionFitnessFunction.hpp"


/**
* \brief A fitness function which rewards individuals that win against opponents which are defeated by fewer other individuals.
* \details Describes \n\n
* \f$ f_j = \sum{\frac{1}{N_o}} \f$ \n \n
* Where \f$N_o\f$ is the number of individuals that can win against opponent \f$o\f$.
*/
class MalmoFitnessFunction : public LightBulb::AbstractCoevolutionFitnessFunction
{
protected:
public:
	// Inherited:
	std::map<const LightBulb::AbstractIndividual*, double>* execute(const LightBulb::CombiningStrategyResults& results) override;
};

#endif
