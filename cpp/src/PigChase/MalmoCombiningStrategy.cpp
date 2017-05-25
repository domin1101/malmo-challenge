// Includes
#include "MalmoCombiningStrategy.hpp"
#include "LightBulb/Learning/Evolution/AbstractIndividual.hpp"
#include "LightBulb/Learning/Evolution/AbstractCoevolutionEnvironment.hpp"
//Library includes


void MalmoCombiningStrategy::combine(LightBulb::AbstractCoevolutionEnvironment& simulationEnvironment, std::vector<LightBulb::AbstractIndividual*>& firstIndividuals, std::vector<LightBulb::AbstractIndividual*>& secondIndividuals)
{
	if (!otherCombiningStrategy)
		throw std::invalid_argument("SharedSamplingCombiningStrategy only works with two environments.");

	std::vector<LightBulb::AbstractIndividual*> sample;
	std::map<LightBulb::AbstractIndividual*, std::map<int, double>> beat;
	std::map<LightBulb::AbstractIndividual*, double> sampleFitness;
	MalmoCombiningResults prevResults = dynamic_cast<const MalmoCombiningResults&>(otherCombiningStrategy->getPrevResults());

	// Do while we have not reached the given sample size yet
	while (sample.size() < amountOfCompetitionsPerIndividual)
	{
		LightBulb::AbstractIndividual* bestIndividual = nullptr;

		// Go through all opponents
		for (auto secondPlayer = secondIndividuals.begin(); secondPlayer != secondIndividuals.end(); secondPlayer++)
		{
			if (sampleFitness[*secondPlayer] != -1)
			{
				sampleFitness[*secondPlayer] = 0;
				// Go through all their last matches
				for (auto resultsPerSecondPlayer = prevResults.floatingResults[*secondPlayer].begin(); resultsPerSecondPlayer != prevResults.floatingResults[*secondPlayer].end(); resultsPerSecondPlayer++)
				{
					for (auto result = resultsPerSecondPlayer->second.begin(); result != resultsPerSecondPlayer->second.end(); result++)
					{
						// Accumulate their score 
						sampleFitness[*secondPlayer] += result->second * (1.0 / (1 + beat[resultsPerSecondPlayer->first][result->first]));
					}
				}
				if (bestIndividual == nullptr || sampleFitness[bestIndividual] < sampleFitness[*secondPlayer])
					bestIndividual = *secondPlayer;
			}
		}

		// If no new individual has been found, abort
		if (!bestIndividual)
			break;

		// Add individual to sample
		sample.push_back(bestIndividual);
		sampleFitness[bestIndividual] = -1;
		// Remember which opponent it has beaten in the last round
		for (auto resultsPerSecondPlayer = prevResults.floatingResults[bestIndividual].begin(); resultsPerSecondPlayer != prevResults.floatingResults[bestIndividual].end(); resultsPerSecondPlayer++)
		{
			for (auto result = resultsPerSecondPlayer->second.begin(); result != resultsPerSecondPlayer->second.end(); result++)
			{
				beat[resultsPerSecondPlayer->first][result->first] += result->second;
			}
		}
	}

	// Run the sample
	executeSample(simulationEnvironment, firstIndividuals, sample);
}

void MalmoCombiningStrategy::executeSample(class LightBulb::AbstractCoevolutionEnvironment& simulationEnvironment, std::vector<LightBulb::AbstractIndividual*>& firstIndividuals, std::vector<LightBulb::AbstractIndividual*>& sample)
{
	// Go through all opponents
	for (auto secondPlayer = sample.begin(); secondPlayer != sample.end(); secondPlayer++)
	{
		for (int r = 0; r < simulationEnvironment.getRoundCount(); r++)
		{
			// Simulate the current parasite agains all agents
			for (auto firstPlayer = firstIndividuals.begin(); firstPlayer != firstIndividuals.end(); firstPlayer++)
				setResult(**firstPlayer, **secondPlayer, r, false, (simulationEnvironment.compareIndividuals(**firstPlayer, **secondPlayer, r) + 25) / 50.0);
		}
	}	
}


void MalmoCombiningStrategy::setResult(LightBulb::AbstractIndividual& firstPlayer, LightBulb::AbstractIndividual& secondPlayer, int round, bool firstPlayerWon, double floatingReward)
{
	(*results)[&firstPlayer][&secondPlayer][round] = firstPlayerWon;
	results->floatingResults[&firstPlayer][&secondPlayer][round] = floatingReward;
	firstPlayerWins += firstPlayerWon;
}

LightBulb::CombiningStrategyResults& MalmoCombiningStrategy::execute(LightBulb::AbstractCoevolutionEnvironment& environment)
{
	results.reset(new MalmoCombiningResults());
	firstPlayerWins = 0;

	combine(environment, environment.getIndividuals(), secondEnvironment ? secondEnvironment->getIndividuals() : environment.getIndividuals());

	return *results.get();
}


const LightBulb::CombiningStrategyResults& MalmoCombiningStrategy::getPrevResults() const
{
	return *results.get();
}



MalmoCombiningStrategy::MalmoCombiningStrategy(int amountOfCompetitionsPerIndividual_, LightBulb::AbstractCoevolutionEnvironment* secondEnvironment_)
	:LightBulb::SharedSamplingCombiningStrategy(amountOfCompetitionsPerIndividual_, secondEnvironment_)
{
	results.reset(new MalmoCombiningResults());
}
