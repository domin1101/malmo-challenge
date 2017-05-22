#pragma once

#ifndef _MALMOTOPDOWNCONTROLLER_H_
#define _MALMOTOPDOWNCONTROLLER_H_

// Includes
#include "LightBulbApp/Windows/AbstractCustomSubApp.hpp"
#include <memory>
#include "MalmoTopDownWindow.hpp"
#include "Malmo.hpp"
#include "Agent.hpp"

/**
 * \brief The controller of the malmo top down window.
 */
class MalmoTopDownController : public LightBulb::AbstractCustomSubApp
{
private:
	/**
	 * \brief Contains the window.
	 */
	std::unique_ptr<MalmoTopDownWindow> window;
	/**
	 * \brief Points to the environment which should be drawn.
	 */
	Malmo* environment;
	/**
	 * \brief Stores the current field.
	 */
	std::vector<std::vector<int>> field;
	/**
	 * \brief Stores the current location of the first agent.
	 */
	Location ai1Location;
	/**
	 * \brief Stores the current location of the second agent.
	 */
	Location ai2Location;
	/**
	 * \brief Stores the current location of the pig.
	 */
	Location pig;
protected:
	// Inherited:
	void prepareClose() override;
public:
	/**
	 * \brief Creates the controller.
	 * \param mainApp The main app.
	 * \param trainingPlan The training plan which runs the pig chase experiment.
	 * \param parent The parent window.
	 */
	MalmoTopDownController(LightBulb::AbstractMainApp& mainApp, LightBulb::AbstractTrainingPlan& trainingPlan, LightBulb::AbstractWindow& parent);
	/**
	 * \brief Returns the window.
	 * \return The window.
	 */
	MalmoTopDownWindow& getWindow();
	/**
	 * \brief Stops watch mode.
	 */
	void stopWatchMode();
	/**
	 * \brief Starts watch mode.
	 */
	void startWatchMode();
	/**
	 * \brief Returns the label of the sub app.
	 * \return The label.
	 */
	static std::string getLabel();
	/**
	 * \brief Returns the current field.
	 * \return The field.
	 */
	const std::vector<std::vector<int>>& getField();
	/**
	 * \brief Is called when the field has been changed.
	 * \param malmo The environment which has changed.
	 */
	void fieldChanged(Malmo& malmo);
	/**
	 * \brief Returns the current location of the first agent.
	 * \return The current location of the first agent.
	 */
	Location getAi1Location() const;
	/**
	* \brief Returns the current location of the second agent.
	* \return The current location of the second agent.
	*/
	Location getAi2Location() const;
	/**
	* \brief Returns the current location of the pig.
	* \return The current location of the pig.
	*/
	Location getPig() const;
};

#endif
