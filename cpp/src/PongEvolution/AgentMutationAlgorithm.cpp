// Includes
#include "AgentMutationAlgorithm.hpp"
#include "Agent.hpp"

AgentMutationAlgorithm::AgentMutationAlgorithm(double mutationStrengthChangeSpeed_, double mutationStrengthMax_, double mutationStrengthMin_)
	:LightBulb::MutationAlgorithm(mutationStrengthChangeSpeed_, mutationStrengthMax_, mutationStrengthMin_)
{
}

void AgentMutationAlgorithm::execute(LightBulb::AbstractIndividual& individual1)
{
	MutationAlgorithm::execute(individual1);
	Agent& agent = dynamic_cast<Agent&>(individual1);

	Location startLocation = agent.getLocation();

	startLocation.x += (int)zigguratGenerator->randDouble();
	startLocation = agent.setStartLocation(startLocation);

	startLocation.y += (int)zigguratGenerator->randDouble();
	startLocation = agent.setStartLocation(startLocation);

	startLocation.dir += (int)zigguratGenerator->randDouble() * 90;
	if (startLocation.dir < 0)
		startLocation.dir += 360;
	startLocation.dir %= 360;
	startLocation = agent.setStartLocation(startLocation);
}

LightBulb::AbstractMutationAlgorithm* AgentMutationAlgorithm::clone() const
{
	return new AgentMutationAlgorithm(*this);
}


