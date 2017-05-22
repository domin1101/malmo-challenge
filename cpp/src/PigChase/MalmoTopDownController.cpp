// Includes
#include "PigChase/MalmoTopDownController.hpp"
#include "LightBulbApp/TrainingPlans/AbstractTrainingPlan.hpp"
#include "PigChaseEvolution.hpp"
#include "Malmo.hpp"
#include "Agent.hpp"
// Library includes
#include <exception>
#include <vector>

using namespace LightBulb;

MalmoTopDownController::MalmoTopDownController(AbstractMainApp& mainApp, AbstractTrainingPlan& trainingPlan_, AbstractWindow& parent)
	:AbstractCustomSubApp(mainApp, trainingPlan_)
{
	environment = static_cast<Malmo*>(&static_cast<PigChaseEvolution*>(trainingPlan)->getEnvironment());
	window.reset(new MalmoTopDownWindow(*this, parent));
}

Location MalmoTopDownController::getAi1Location() const
{
	return ai1Location;
}

Location MalmoTopDownController::getAi2Location() const
{
	return ai2Location;
}

void MalmoTopDownController::prepareClose()
{
	stopWatchMode();
}

MalmoTopDownWindow& MalmoTopDownController::getWindow()
{
	return *window.get();
}

void MalmoTopDownController::stopWatchMode()
{
	environment->stopWatchMode();
	environment->removeObserver(EVT_FIELD_CHANGED, &MalmoTopDownController::fieldChanged, *this);
}


void MalmoTopDownController::startWatchMode()
{
	environment->startWatchMode();
	environment->registerObserver(EVT_FIELD_CHANGED, &MalmoTopDownController::fieldChanged, *this);
}


std::string MalmoTopDownController::getLabel()
{
	return "PigChaseChallenge";
}

const std::vector<std::vector<int>>& MalmoTopDownController::getField()
{
	return field;
}


Location MalmoTopDownController::getPig() const
{
	return pig;
}

void MalmoTopDownController::fieldChanged(Malmo& malmo)
{
	if (field.size() == 0)
		field = malmo.getField();

	ai1Location = malmo.getAgent1().getLocation();
	ai2Location = malmo.getAgent2().getLocation();
	pig = malmo.getPig();

	wxThreadEvent evt(MALMO_EVT_FIELD_CHANGED);
	window->GetEventHandler()->QueueEvent(evt.Clone());
}
