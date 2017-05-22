#pragma once

#ifndef _MALMOTOPDOWNWINDOW_H_
#define _MALMOTOPDOWNWINDOW_H_

// Includes
#include "LightBulbApp/Windows/AbstractSubAppWindow.hpp"

// Forward declarations
class MalmoTopDownController;

wxDECLARE_EVENT(MALMO_EVT_FIELD_CHANGED, wxThreadEvent);

/**
 * \brief Displays a window which can show the top down view of the current match.
 */
class MalmoTopDownWindow : public LightBulb::AbstractSubAppWindow
{
private:
	/**
	 * \brief The panel which is used as canvase.
	 */
	wxPanel* panel;
	/**
	 * \brief The toolbar for starting and stoping watch mode.
	 */
	wxToolBar* toolbar;
	/**
	 * \brief Draws a triangle in a tile depending on the given direction.
	 * \param dc The drawing tools.
	 * \param dir The direction.
	 * \param offsetX The x-offset of the tile.
	 * \param offsetY The y-offset of the tile.
	 * \param sizeX The x-size of the tile.
	 * \param sizeY The y-size of the tile.
	 */
	void drawTriangleForDir(wxDC &dc, int dir, int offsetX, int offsetY, int sizeX, int sizeY);
	DECLARE_EVENT_TABLE();
public:
	/**
	 * \brief Returns the window.
	 * \param controller_ The controller which manages the window.
	 * \param parent The parent window.
	 */
	MalmoTopDownWindow(MalmoTopDownController& controller_, LightBulb::AbstractWindow& parent);
	/**
	 * \brief Repaints the window.
	 * \param evt The event.
	 */
	void paintEvent(wxPaintEvent& evt);
	/**
	* \brief Repaints the window.
	* \param evt The event.
	*/
	void resize(wxSizeEvent& evt);
	/**
	* \brief Repaints the window.
	*/
	void paintNow();
	/**
	* \brief Repaints the window.
	*/
	void refreshField(wxThreadEvent& evt);
	/**
	 * \brief Called after a button of the toolbar has been clicked.
	 * \param evt The event.
	 */
	void toolBarClicked(wxCommandEvent& evt);
	/**
	 * \brief Renders the top down view.
	 * \param dc The drawing tool.
	 */
	void render(wxDC& dc);
	/**
	 * \brief Returns the label of the window.
	 * \return The label.
	 */
	static std::string getLabel();
	/**
	 * \brief Returns the controller
	 * \return The controller which manages the window.
	 */
	MalmoTopDownController& getController();
};

#endif
