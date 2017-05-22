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
	popStartLocation.y += (int)zigguratGenerator->randDouble();
	
	Location parStartLocation = agent.getParStartLocation();
	parStartLocation.x += (int)zigguratGenerator->randDouble();
	parStartLocation.y += (int)zigguratGenerator->randDouble();

	Location pigStartLocation = agent.getPigStartLocation();
	pigStartLocation.x += (int)zigguratGenerator->randDouble();
	pigStartLocation.y += (int)zigguratGenerator->randDouble();

	if (agent.isValidStartConstelation(popStartLocation, parStartLocation, pigStartLocation))
	{
		agent.setPopStartLocation(popStartLocation);
		agent.setParStartLocation(parStartLocation);
		agent.setPigStartLocation(pigStartLocation);	

		popStartLocation.dir += (int)zigguratGenerator->randDouble() * 90;
		if (popStartLocation.dir < 0)
			popStartLocation.dir += 360;
		popStartLocation.dir %= 360;
		agent.setPopStartLocation(popStartLocation);

		parStartLocation.dir += (int)zigguratGenerator->randDouble() * 90;
		if (parStartLocation.dir < 0)
			parStartLocation.dir += 360;
		parStartLocation.dir %= 360;
		agent.setParStartLocation(parStartLocation);

		agent.setIsStupid(getRandomGenerator().randDouble() < 0.25);
	}
}



LightBulb::AbstractMutationAlgorithm* AgentMutationAlgorithm::clone() const
{
	return new AgentMutationAlgorithm(*this);
}


