#pragma once

#ifndef _MALMOFITNESSFUNCTION_H_
#define _MALMOFITNESSFUNCTION_H_

// Library Includes

// Include
#include "LightBulb/Learning/Evolution/AbstractCoevolutionFitnessFunction.hpp"


/**
* \brief The fitness function used for the malmo environment.
* \details This fitness function works exactly like the shared coevolution fitness function, but can also work with floating results.
*/
class MalmoFitnessFunction : public LightBulb::AbstractCoevolutionFitnessFunction
{
protected:
public:
	// Inherited:
	std::map<const LightBulb::AbstractIndividual*, double>* execute(const LightBulb::CombiningStrategyResults& results) override;
};

#endif
