#pragma once

#ifndef _MALMOTOPDOWNWINDOWFACTORY_H_
#define _MALMOTOPDOWNWINDOWFACTORY_H_

// Includes
#include "LightBulbApp/Windows/AbstractCustomSubAppFactory.hpp"

/**
 * \brief A factory for creating new top down windows.
 */
class MalmoTopDownWindowFactory : public LightBulb::AbstractCustomSubAppFactory
{
protected:
public:
	// Inherited:
	LightBulb::AbstractCustomSubApp* createCustomSupApp(LightBulb::AbstractMainApp& mainApp, LightBulb::AbstractTrainingPlan& trainingPlan, LightBulb::AbstractWindow& parent) const override;
	std::string getLabel() const override;
};

#endif
