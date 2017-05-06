#pragma once

#ifndef _PONGGAMECONTROLLER_H_
#define _PONGGAMECONTROLLER_H_

// Includes
#include "LightBulbApp/Windows/AbstractCustomSubApp.hpp"
#include <memory>
#include "MinecraftWindow.hpp"
#include "Minecraft.hpp"


class MinecraftController : public LightBulb::AbstractCustomSubApp
{
private:
	std::unique_ptr<MinecraftWindow> window;
	Minecraft* environment;
	std::vector<std::vector<int>> field;
	int ai1X, ai1Y, ai1Dir, ai2X, ai2Y, ai2Dir;
public:
	int getAi1X() const;

	int getAi1Y() const;

	int getAi1Dir() const;

	int getAi2X() const;

	int getAi2Y() const;

	int getAi2Dir() const;

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
};

#endif
