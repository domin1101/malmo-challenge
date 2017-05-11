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

	agent.setStartX(agent.getStartX() + (int)zigguratGenerator->randDouble());
	agent.setStartY(agent.getStartY() + (int)zigguratGenerator->randDouble());
	int newDir = (agent.getStartDir() + (int)zigguratGenerator->randDouble() * 90) % 360;
	if (newDir < 0)
		newDir += 360;
	agent.setStartDir(newDir);
}

LightBulb::AbstractMutationAlgorithm* AgentMutationAlgorithm::clone() const
{
	return new AgentMutationAlgorithm(*this);
}


