// Includes
#include "PigChase/MalmoTopDownWindowFactory.hpp"
#include "MalmoTopDownController.hpp"

using namespace LightBulb;

AbstractCustomSubApp* MalmoTopDownWindowFactory::createCustomSupApp(AbstractMainApp& mainApp, AbstractTrainingPlan& trainingPlan, AbstractWindow& parent) const
{
	MalmoTopDownController* controller = new MalmoTopDownController(mainApp, trainingPlan, parent);
	controller->getWindow().Show();
	return controller;
}

std::string MalmoTopDownWindowFactory::getLabel() const
{
	return MalmoTopDownController::getLabel();
}
