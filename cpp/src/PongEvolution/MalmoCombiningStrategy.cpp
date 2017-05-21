// Includes
#include "MalmoCombiningStrategy.hpp"
#include "LightBulb/Learning/Evolution/AbstractIndividual.hpp"
#include "LightBulb/Learning/Evolution/AbstractCoevolutionEnvironment.hpp"
#include "Minecraft.hpp"
//Library includes


void MalmoCombiningStrategy::combine(LightBulb::AbstractCoevolutionEnvironment& simulationEnvironment, std::vector<LightBulb::AbstractIndividual*>& firstIndividuals, std::vector<LightBulb::AbstractIndividual*>& secondIndividuals)
{
	if (!otherCombiningStrategy)
		throw std::invalid_argument("SharedSamplingCombiningStrategy only works with two environments.");

	std::vector<LightBulb::AbstractIndividual*> sample;
	std::map<LightBulb::AbstractIndividual*, std::map<int, double>> beat;
	std::map<LightBulb::AbstractIndividual*, double> sampleFitness;
	MalmoCombiningResults prevResults = dynamic_cast<const MalmoCombiningResults&>(otherCombiningStrategy->getPrevResults());
	while (sample.size() < amountOfCompetitionsPerIndividual)
	{
		LightBulb::AbstractIndividual* bestIndividual = nullptr;
		for (auto secondPlayer = secondIndividuals.begin(); secondPlayer != secondIndividuals.end(); secondPlayer++)
		{
			if (sampleFitness[*secondPlayer] != -1)
			{
				sampleFitness[*secondPlayer] = 0;
				for (auto resultsPerSecondPlayer = prevResults.floatingResults[*secondPlayer].begin(); resultsPerSecondPlayer != prevResults.floatingResults[*secondPlayer].end(); resultsPerSecondPlayer++)
				{
					for (auto result = resultsPerSecondPlayer->second.begin(); result != resultsPerSecondPlayer->second.end(); result++)
					{
						sampleFitness[*secondPlayer] += result->second * (1.0 / (1 + beat[resultsPerSecondPlayer->first][result->first]));
					}
				}
				if (bestIndividual == nullptr || sampleFitness[bestIndividual] < sampleFitness[*secondPlayer])
					bestIndividual = *secondPlayer;
			}
		}
		if (!bestIndividual)
			break;

		sample.push_back(bestIndividual);
		sampleFitness[bestIndividual] = -1;
		for (auto resultsPerSecondPlayer = prevResults.floatingResults[bestIndividual].begin(); resultsPerSecondPlayer != prevResults.floatingResults[bestIndividual].end(); resultsPerSecondPlayer++)
		{
			for (auto result = resultsPerSecondPlayer->second.begin(); result != resultsPerSecondPlayer->second.end(); result++)
			{
				beat[resultsPerSecondPlayer->first][result->first] += result->second;
			}
		}
	}

	executeSample(simulationEnvironment, firstIndividuals, sample);
}

void MalmoCombiningStrategy::executeSample(class LightBulb::AbstractCoevolutionEnvironment& simulationEnvironment, std::vector<LightBulb::AbstractIndividual*>& firstIndividuals, std::vector<LightBulb::AbstractIndividual*>& sample)
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
					matchResults[index++] = simulationEnvironment.compareIndividuals(**firstPlayer, **secondPlayer, r) / (double)static_cast<Minecraft&>(simulationEnvironment).getStepCounter();

				int max = matchResults.maxCoeff();
				index = 0;
				for (auto firstPlayer = firstIndividuals.begin(); firstPlayer != firstIndividuals.end(); firstPlayer++)
				{
					setResult(**firstPlayer, **secondPlayer, r, matchResults[index] > 0, (matchResults[index] + 1) / 25.0);
					index++;
				}
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
				{
					setResult(**firstPlayer, **secondPlayer, r, matchResults[index] < 0, (max - matchResults[index]) / 25.0);
					index++;
				}
			}
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
