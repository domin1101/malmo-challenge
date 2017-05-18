// Includes
#include "MalmoHallOfFameAlgorithm.hpp"
#include "LightBulb/Learning/Evolution/AbstractCoevolutionEnvironment.hpp"
// Library includes
#include <algorithm>


void MalmoHallOfFameAlgorithm::evaluateIndividuals(std::vector<LightBulb::AbstractIndividual*>& individuals)
{
	if (amountOfCompetitionsPerIndividual > 0)
	{
		random_shuffle(members.begin(), members.end());

		if (!currentEnvironment->isParasiteEnvironment())
		{
			matchResults.resize(individuals.size());
			for (int memberIndex = 0; memberIndex < amountOfCompetitionsPerIndividual && memberIndex < members.size(); memberIndex++)
			{
				for (int r = 0; r < currentEnvironment->getRoundCount(); r++)
				{
					int index = 0;
					for (auto individual = individuals.begin(); individual != individuals.end(); individual++)
						matchResults[index++] = currentEnvironment->compareIndividuals(**individual, *members[memberIndex].get(), r);

					int max = matchResults.maxCoeff();
					index = 0;
					for (auto individual = individuals.begin(); individual != individuals.end(); individual++)
						setResult(**individual, memberIndex, r, matchResults[index++] == max);
				}
			}
		}
		else
		{
			matchResults.resize(std::min(amountOfCompetitionsPerIndividual, (int)members.size()));
			for (auto individual = individuals.begin(); individual != individuals.end(); individual++)
			{
				for (int r = 0; r < currentEnvironment->getRoundCount(); r++)
				{
					int index = 0;
					for (int memberIndex = 0; memberIndex < amountOfCompetitionsPerIndividual && memberIndex < members.size(); memberIndex++)
						matchResults[index++] = currentEnvironment->compareIndividuals(**individual, *members[memberIndex].get(), r);

					int max = matchResults.maxCoeff();
					index = 0;
					for (int memberIndex = 0; memberIndex < amountOfCompetitionsPerIndividual && memberIndex < members.size(); memberIndex++)
						setResult(**individual, memberIndex, r, matchResults[index++] < max);
				}
			}
		}
	}
}

void MalmoHallOfFameAlgorithm::setResult(LightBulb::AbstractIndividual& individual, int memberID, int round, bool firstPlayerHasWon)
{
	(*currentResults)[&individual][members[memberID].get()][round] = firstPlayerHasWon;
	(*currentResults)[members[memberID].get()][&individual][round] = !firstPlayerHasWon;	
}

MalmoHallOfFameAlgorithm::MalmoHallOfFameAlgorithm(int amountOfCompetitionsPerIndividual_)
	:LightBulb::RandomHallOfFameAlgorithm(amountOfCompetitionsPerIndividual_)
{
	amountOfCompetitionsPerIndividual = amountOfCompetitionsPerIndividual_;
}
