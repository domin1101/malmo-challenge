#pragma once

#ifndef _PONGGAMECONTROLLER_H_
#define _PONGGAMECONTROLLER_H_

// Includes
#include "LightBulbApp/Windows/AbstractCustomSubApp.hpp"
#include <memory>
#include "MinecraftWindow.hpp"
#include "Minecraft.hpp"
#include "Agent.hpp"

class MinecraftController : public LightBulb::AbstractCustomSubApp
{
private:
	std::unique_ptr<MinecraftWindow> window;
	Minecraft* environment;
	std::vector<std::vector<int>> field;
	Location ai1Location;
	Location ai2Location;
protected:
	void prepareClose() override;
public:
	MinecraftController(LightBulb::AbstractMainApp& mainApp, LightBulb::AbstractTrainingPlan& trainingPlan, LightBulb::AbstractWindow& parent);
	MinecraftWindow& getWindow();
	void stopWatchMode();
	void startWatchMode();
	static std::string getLabel();
	const std::vector<std::vector<int>>& getField();
	void fieldChanged(Minecraft& pong);
	Location getAi1Location() const;
	Location getAi2Location() const;
};

#endif
