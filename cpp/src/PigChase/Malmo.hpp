#pragma once

#ifndef _MALMO_H_
#define _MALMO_H_

// Library Includes
#include <vector>
#include <map>
#include <array>
#include <mutex>

// Include
#include "LightBulb/Learning/Evolution/AbstractCoevolutionEnvironment.hpp"
#include "LightBulb/IO/UseParentSerialization.hpp"
#include "LightBulb/NetworkTopology/FeedForwardNetworkTopology.hpp"
#include "Agent.hpp"
#include "ChallengeAgent.hpp"
#include "Parasite.hpp"

// Forward declarations
class Agent;
class AbstractTile;

#define DATASET_AVG_REWARD "Avg reward"
#define DATASET_BEST_REWARD "Best reward"
#define DATASET_AVG_MUT_STRENGTH "Average mutation strength"
#define DATASET_CHALLENGE "Challenge performance"
#define DATASET_LAPIS_ENDINGS "Lapis endings"
#define DATASET_CAUGHT_ENDINGS "Caught endings"

enum MalmoEvents
{
	EVT_FIELD_CHANGED
};

/**
 * \brief An enumaration describing the different possibilites how a match can end.
 */
enum Ending
{
	TIMEOUT,
	CAUGHT,
	LAPIS
};

/**
 * \brief A mock of the Minecraft Malmo environment. Is used to simulate matches much faster.
 */
class Malmo : public LightBulb::AbstractCoevolutionEnvironment, public LightBulb::Observable<MalmoEvents, Malmo>
{
private:
	std::unique_ptr<ChallengeAgent> challengeAgent;
	/**
	 * \brief Stores the basic structure of the environment. (0: fence, 1: walkable, 2: lapis)
	 */
	std::vector<std::vector<int>> fields;
	/**
	 * \brief Points to the current first agent.
	 */
	AbstractMalmoAgent* currentAi1;
	/**
	* \brief Points to the current second agent.
	*/
	AbstractMalmoAgent* currentAi2;
	/**
	 * \brief Contains the current player (0 => first agent, 1 => second agent)
	 */
	int currentPlayer;
	/**
	 * \brief True, if watch mode is activated.
	 */
	bool watchMode;
	/**
	 * \brief Stores the best reward achieved in the current iteration.
	 */
	int bestReward;
	/**
	* \brief Stores the total reward achieved in the current iteration.
	*/
	int totalReward;
	/**
	* \brief Stores the number of matches in the current iteration.
	*/
	int matchCount;
	/**
	* \brief Stores the number of steps in the current match.
	*/
	int stepCounter;
	/**
	 * \brief True, if the current match is for rating purposes.
	 */
	bool inRating;
	/**
	 * \brief Contains the type of the last match ending.
	 */
	Ending lastEnding;
	/**
	 * \brief Stores the location of the pig.
	 */
	Location pig;
	/**
	 * \brief Points to the best individual of the last iteration.
	 */
	AbstractMalmoAgent* lastBestIndividual;
	/**
	 * \brief Cache used for the A* algorithm.
	 */
	std::map<Location, std::map<Location, int>> aStarCache;
	/**
	* \brief The neural network options used for creating new agents.
	*/
	std::unique_ptr<LightBulb::FeedForwardNetworkTopologyOptions> options;
protected:
	/**
	 * \brief Simulates one match.
	 * \param ai1 The first agent.
	 * \param ai2 The second agent.
	 * \param startPlayer Determines which player should start (0 = first, 1 = second)
	 * \return The total reward of the non parasite.
	 */
	int simulateGame(AbstractMalmoAgent& ai1, AbstractMalmoAgent& ai2, Parasite& parasite, int startPlayer);
	// Inherited:
	LightBulb::AbstractIndividual* createNewIndividual() override;
	int doCompare(LightBulb::AbstractIndividual& obj1, LightBulb::AbstractIndividual& obj2, int round) override;
public:
	/**
	 * \brief Creates a new environment.
	 * \param options_ The neural network options used for creating new agents. 
	 * \param isParasiteEnvironment True, if this should be the parasite environment.
	 * \param combiningStrategy_ The combining strategy which determines which individuals should compete.
	 * \param fitnessFunction_ The fitness function which computes the fitness values from the combining results.
	 * \param hallOfFameToAddAlgorithm_ The algorithm where to add new hall of fame members.
	 * \param hallOfFameToChallengeAlgorithm_ The algorithm which should be used for challenging.
	 */
	Malmo(LightBulb::FeedForwardNetworkTopologyOptions& options_, bool isParasiteEnvironment, LightBulb::AbstractCombiningStrategy* combiningStrategy_, LightBulb::AbstractCoevolutionFitnessFunction* fitnessFunction_, const std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm>* hallOfFameToAddAlgorithm_ = nullptr, const std::shared_ptr<LightBulb::AbstractHallOfFameAlgorithm>* hallOfFameToChallengeAlgorithm_ = nullptr);
	Malmo() = default;
	/**
	 * \brief Adds the input for one entity with the given position to the input array.
	 * \param input The input vector to use.
	 * \param x The x coordinate of the agent.
	 * \param y The y coordinate of the agent.
	 * \param dir The direction of the agent.
	 * \param offset The offset where to put the new input data inside the input vector.
	 */
	void setInputForAgent(std::vector<double>& input, int x, int y, int dir, int offset);
	/**
	 * \brief Returns the input for the neural network.
	 * \param input The vector where the input data should be filled in.
	 */
	void getNNInput(std::vector<double>& input);
	/**
	 * \brief Returns if the given position is blocked for the pig.
	 * \param x The x coordinate.
	 * \param y The y coordinate.
	 * \return True, if the field is blocked.
	 */
	bool isFieldBlockedForPig(int x, int y);
	/**
	 * \brief Should be called after an agent moved.
	 * \param x The new x position.
	 * \param y The new y position.
	 * \param dx The delta x value which was used for the movement.
	 * \param dy The delta y value which was used for the movement.
	 */
	void agentMovedTo(int x, int y, int dx, int dy);
	/**
	 * \brief Sets up a new game.
	 */
	void startNewGame(Parasite& parasite);
	/**
	 * \brief Returns the A* cache which is used by all agents.
	 * \return The A* cache.
	 */
	std::map<Location, std::map<Location, int>>& getAStarCache();
	/**
	 * \brief Returns the reward for the last step of the given agent.
	 * \param agent The agent.
	 * \return The reward for the agent.
	 */
	int getReward(AbstractMalmoAgent &agent);
	/**
	 * \brief Returns if the current match is over.
	 * \param ai1 The first agent.
	 * \param ai2 The second agent.
	 * \param currentPlayer The current player.
	 * \param startPlayer The player which started.
	 * \return True, if match is over.
	 */
	bool isDone(AbstractMalmoAgent &ai1, AbstractMalmoAgent &ai2, int currentPlayer, int startPlayer);
	/**
	 * \brief Returns if the pig has been caught.
	 * \return True, if it is caught.
	 */
	bool isPigCaught();
	/**
	 * \brief Returns if the given positon is walkable.
	 * \param x The x-coordinate.
	 * \param y The y-coordinate.
	 * \param allowLapis True, if lapis lazuli should be walkable.
	 * \return True, if walkable.
	 */
	bool isFieldAllowed(int x, int y, bool allowLapis = true);
	/**
	 * \brief Starts watch mode.
	 */
	void startWatchMode();
	/**
	 * \brief Stops watch mode.
	 */
	void stopWatchMode();
	/**
	 * \brief Returns the field.
	 * \return The field as a two dimensional vector. (0: fence, 1: walkable, 2: lapis)
	 */
	const std::vector<std::vector<int>>& getField();
	/**
	 * \brief Returns the current first agent.
	 * \return The first agent.
	 */
	const AbstractMalmoAgent& getAgent1();
	/**
	 * \brief Returns the current second agent.
	 * \return The second agent. 
	 */
	const AbstractMalmoAgent& getAgent2();
	/**
	 * \brief Returns the location of the pig.
	 * \return The location of the pig.
	 */
	const Location& getPig();
	/**
	 * \brief Returns the number of steps the non-parasite agent has taken in the current match.
	 * \return The number of step.
	 */
	int getStepCounter() const;
	// Inherited:
	int rateIndividual(LightBulb::AbstractIndividual &individual) override;
	std::vector<std::string> getDataSetLabels() const override;
	int getRoundCount() const override;
};

USE_EXISTING_PARENT_SERIALIZATION_WITHOUT_NAMESPACE(Malmo, LightBulb::AbstractCoevolutionEnvironment, LightBulb::AbstractEvolutionEnvironment);

#endif
