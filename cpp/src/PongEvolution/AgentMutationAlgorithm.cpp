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

	Location popStartLocation = agent.getPopStartLocation();

	popStartLocation.x += (int)zigguratGenerator->randDouble();
	popStartLocation = agent.setPopStartLocation(popStartLocation);

	popStartLocation.y += (int)zigguratGenerator->randDouble();
	popStartLocation = agent.setPopStartLocation(popStartLocation);

	popStartLocation.dir += (int)zigguratGenerator->randDouble() * 90;
	if (popStartLocation.dir < 0)
		popStartLocation.dir += 360;
	popStartLocation.dir %= 360;
	popStartLocation = agent.setPopStartLocation(popStartLocation);


	Location parStartLocation = agent.getParStartLocation();

	parStartLocation.x += (int)zigguratGenerator->randDouble();
	parStartLocation = agent.setParStartLocation(parStartLocation);

	parStartLocation.y += (int)zigguratGenerator->randDouble();
	parStartLocation = agent.setParStartLocation(parStartLocation);

	parStartLocation.dir += (int)zigguratGenerator->randDouble() * 90;
	if (parStartLocation.dir < 0)
		parStartLocation.dir += 360;
	parStartLocation.dir %= 360;
	parStartLocation = agent.setParStartLocation(parStartLocation);


	Location pigStartLocation = agent.getPigStartLocation();

	pigStartLocation.x += (int)zigguratGenerator->randDouble();
	pigStartLocation = agent.setPigStartLocation(pigStartLocation);

	pigStartLocation.y += (int)zigguratGenerator->randDouble();
	pigStartLocation = agent.setPigStartLocation(pigStartLocation);
}



LightBulb::AbstractMutationAlgorithm* AgentMutationAlgorithm::clone() const
{
	return new AgentMutationAlgorithm(*this);
}


