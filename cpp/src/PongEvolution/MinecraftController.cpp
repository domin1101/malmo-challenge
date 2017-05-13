// Includes
#include "PongEvolution/MinecraftController.hpp"
#include "LightBulbApp/TrainingPlans/AbstractTrainingPlan.hpp"
#include "MalmoEvolution.hpp"
#include "Minecraft.hpp"
#include "Agent.hpp"
// Library includes
#include <exception>
#include <vector>

using namespace LightBulb;

MinecraftController::MinecraftController(AbstractMainApp& mainApp, AbstractTrainingPlan& trainingPlan_, AbstractWindow& parent)
	:AbstractCustomSubApp(mainApp, trainingPlan_)
{
	environment = static_cast<Minecraft*>(&static_cast<MalmoEvolution*>(trainingPlan)->getEnvironment());
	window.reset(new MinecraftWindow(*this, parent));
}

Location MinecraftController::getAi1Location() const
{
	return ai1Location;
}

Location MinecraftController::getAi2Location() const
{
	return ai2Location;
}

void MinecraftController::prepareClose()
{
	stopWatchMode();
}

MinecraftWindow& MinecraftController::getWindow()
{
	return *window.get();
}

void MinecraftController::stopWatchMode()
{
	environment->stopWatchMode();
	environment->removeObserver(EVT_FIELD_CHANGED, &MinecraftController::fieldChanged, *this);
}


void MinecraftController::startWatchMode()
{
	environment->startWatchMode();
	environment->registerObserver(EVT_FIELD_CHANGED, &MinecraftController::fieldChanged, *this);
}


std::string MinecraftController::getLabel()
{
	return "PongGame";
}

const std::vector<std::vector<int>>& MinecraftController::getField()
{
	return field;
}


Location MinecraftController::getPig() const
{
	return pig;
}

void MinecraftController::fieldChanged(Minecraft& minecraft)
{
	if (field.size() == 0)
		field = minecraft.getField();

	ai1Location = minecraft.getAgent1().getLocation();
	ai2Location = minecraft.getAgent2().getLocation();
	pig = minecraft.getPig();

	wxThreadEvent evt(PONG_EVT_FIELD_CHANGED);
	window->GetEventHandler()->QueueEvent(evt.Clone());
}
