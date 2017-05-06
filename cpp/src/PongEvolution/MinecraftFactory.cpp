// Includes
#include "PongEvolution/MinecraftFactory.hpp"
#include "MinecraftController.hpp"

using namespace LightBulb;

AbstractCustomSubApp* MinecraftFactory::createCustomSupApp(AbstractMainApp& mainApp, AbstractTrainingPlan& trainingPlan, AbstractWindow& parent) const
{
	MinecraftController* controller = new MinecraftController(mainApp, trainingPlan, parent);
	controller->getWindow().Show();
	return controller;
}

std::string MinecraftFactory::getLabel() const
{
	return MinecraftController::getLabel();
}
