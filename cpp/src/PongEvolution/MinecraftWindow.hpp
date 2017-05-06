#pragma once

#ifndef _MINECRAFTWINDOW_H_
#define _MINECRAFTWINDOW_H_

// Includes
#include "LightBulbApp/Windows/AbstractSubAppWindow.hpp"

// Forward declarations
class MinecraftController;

wxDECLARE_EVENT(PONG_EVT_FIELD_CHANGED, wxThreadEvent);

class MinecraftWindow : public LightBulb::AbstractSubAppWindow
{
private:
	wxPanel* panel;
	wxToolBar* toolbar;
	MinecraftController& getController();
protected:
	DECLARE_EVENT_TABLE();
public:
	MinecraftWindow(MinecraftController& controller_, LightBulb::AbstractWindow& parent);
	void paintEvent(wxPaintEvent& evt);
	void resize(wxSizeEvent& evt);
	void paintNow();
	void refreshField(wxThreadEvent& evt);
	void toolBarClicked(wxCommandEvent& evt);
	void render(wxDC& dc);

	static std::string getLabel();

	void drawTriangleForDir(wxDC &dc, int dir, int offsetX, int offsetY, int sizeX, int sizeY);
};

#endif
