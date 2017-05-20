// Includes
#include "MalmoFitnessFunction.hpp"
#include "MalmoCombiningStrategy.hpp"

std::map<const LightBulb::AbstractIndividual*, double>* MalmoFitnessFunction::execute(const LightBulb::CombiningStrategyResults& results)
{
	std::map<LightBulb::AbstractIndividual*, std::map<int, double>> rewardCounter;
	const MalmoCombiningResults& malmoResults = dynamic_cast<const MalmoCombiningResults&>(results);

	for (auto resultsPerIndividual = malmoResults.floatingResults.begin(); resultsPerIndividual != malmoResults.floatingResults.end(); resultsPerIndividual++)
	{
		for (auto resultsPerCombination = resultsPerIndividual->second.begin(); resultsPerCombination != resultsPerIndividual->second.end(); resultsPerCombination++)
		{
			for (auto result = resultsPerCombination->second.begin(); result != resultsPerCombination->second.end(); result++)
			{
				rewardCounter[resultsPerCombination->first][result->first] += result->second;
			}
		}
	}

	std::map<const LightBulb::AbstractIndividual*, double>* fitnessValues = new std::map<const LightBulb::AbstractIndividual*, double>();

	for (auto resultsPerIndividual = malmoResults.floatingResults.begin(); resultsPerIndividual != malmoResults.floatingResults.end(); resultsPerIndividual++)
	{
		for (auto resultsPerCombination = resultsPerIndividual->second.begin(); resultsPerCombination != resultsPerIndividual->second.end(); resultsPerCombination++)
		{
			for (auto result = resultsPerCombination->second.begin(); result != resultsPerCombination->second.end(); result++)
			{
				if (rewardCounter[resultsPerCombination->first][result->first] > 0)
				{
					(*fitnessValues)[resultsPerIndividual->first] += result->second *1.0 / rewardCounter[resultsPerCombination->first][result->first];
				}
			}
		}
	}

	return fitnessValues;
}
