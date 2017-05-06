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



void MinecraftController::fieldChanged(Minecraft& minecraft)
{
	if (field.size() == 0)
		field = minecraft.getField();

	ai1X = minecraft.getAgent1().getX();
	ai1Y = minecraft.getAgent1().getY();
	ai1Dir = minecraft.getAgent1().getDir();

	ai2X = minecraft.getAgent2().getX();
	ai2Y = minecraft.getAgent2().getY();
	ai2Dir = minecraft.getAgent2().getDir();

	wxThreadEvent evt(PONG_EVT_FIELD_CHANGED);
	window->GetEventHandler()->QueueEvent(evt.Clone());
}

int MinecraftController::getAi1X() const
{
	return ai1X;
}

int MinecraftController::getAi1Y() const
{
	return ai1Y;
}

int MinecraftController::getAi1Dir() const
{
	return ai1Dir;
}

int MinecraftController::getAi2X() const
{
	return ai2X;
}

int MinecraftController::getAi2Y() const
{
	return ai2Y;
}

int MinecraftController::getAi2Dir() const
{
	return ai2Dir;
}
