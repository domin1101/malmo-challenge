// Includes
#include "MalmoCombiningStrategy.hpp"
#include "LightBulb/Learning/Evolution/AbstractIndividual.hpp"
#include "LightBulb/Learning/Evolution/AbstractCoevolutionEnvironment.hpp"
//Library includes


void MalmoCombiningStrategy::executeSample(LightBulb::AbstractCoevolutionEnvironment& simulationEnvironment, std::vector<LightBulb::AbstractIndividual*>& firstIndividuals, std::vector<LightBulb::AbstractIndividual*>& sample)
{
	if (!simulationEnvironment.isParasiteEnvironment())
	{
		matchResults.resize(firstIndividuals.size());
		for (auto secondPlayer = sample.begin(); secondPlayer != sample.end(); secondPlayer++)
		{
			for (int r = 0; r < simulationEnvironment.getRoundCount(); r++)
			{
				int index = 0;
				for (auto firstPlayer = firstIndividuals.begin(); firstPlayer != firstIndividuals.end(); firstPlayer++)
					matchResults[index++] = simulationEnvironment.compareIndividuals(**firstPlayer, **secondPlayer, r);

				int max = matchResults.maxCoeff();
				index = 0;
				for (auto firstPlayer = firstIndividuals.begin(); firstPlayer != firstIndividuals.end(); firstPlayer++)
					setResult(**firstPlayer, **secondPlayer, r, matchResults[index++] == max);
			}
		}
	}
	else
	{
		matchResults.resize(sample.size());
		for (auto firstPlayer = firstIndividuals.begin(); firstPlayer != firstIndividuals.end(); firstPlayer++)
		{
			for (int r = 0; r < simulationEnvironment.getRoundCount(); r++)
			{
				int index = 0;
				for (auto secondPlayer = sample.begin(); secondPlayer != sample.end(); secondPlayer++)
					matchResults[index++] = simulationEnvironment.compareIndividuals(**firstPlayer, **secondPlayer, r);

				int max = matchResults.maxCoeff();
				index = 0;
				for (auto secondPlayer = sample.begin(); secondPlayer != sample.end(); secondPlayer++)
					setResult(**firstPlayer, **secondPlayer, r, matchResults[index++] < max);
			}
		}
	}
}

MalmoCombiningStrategy::MalmoCombiningStrategy(int amountOfCompetitionsPerIndividual_, LightBulb::AbstractCoevolutionEnvironment* secondEnvironment_)
	:LightBulb::SharedSamplingCombiningStrategy(amountOfCompetitionsPerIndividual_, secondEnvironment_)
{
}
