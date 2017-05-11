// Includes
#include "PongEvolution/MinecraftWindow.hpp"
#include "MinecraftController.hpp"
#include <wx/artprov.h>

BEGIN_EVENT_TABLE(MinecraftWindow, wxFrame)
EVT_PAINT(MinecraftWindow::paintEvent)
EVT_SIZE(MinecraftWindow::resize)
END_EVENT_TABLE()

wxDEFINE_EVENT(PONG_EVT_FIELD_CHANGED, wxThreadEvent);

enum
{
	TOOLBAR_STOP_WATCHMODE,
	TOOLBAR_START_WATCHMODE,
};

MinecraftWindow::MinecraftWindow(MinecraftController& controller_, LightBulb::AbstractWindow& parent)
	:AbstractSubAppWindow(controller_, getLabel(), parent)
{
	panel = nullptr;
	Bind(PONG_EVT_FIELD_CHANGED, &MinecraftWindow::refreshField, this);

	toolbar = CreateToolBar();
	toolbar->AddTool(TOOLBAR_STOP_WATCHMODE, "WatchMode Off", wxArtProvider::GetBitmap(wxART_GO_FORWARD));
	toolbar->AddTool(TOOLBAR_START_WATCHMODE, "WatchMode On", wxArtProvider::GetBitmap(wxART_DELETE));
	toolbar->Bind(wxEVT_TOOL, wxCommandEventFunction(&MinecraftWindow::toolBarClicked), this);
	toolbar->Realize();

	toolbar->EnableTool(TOOLBAR_START_WATCHMODE, true);
	toolbar->EnableTool(TOOLBAR_STOP_WATCHMODE, false);

	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	panel = new wxPanel(this);
	panel->SetMinSize(wxSize(603, 603));
	
	sizer->Add(panel, 1, wxEXPAND);
	SetSizerAndFit(sizer);
}

void MinecraftWindow::paintEvent(wxPaintEvent& evt)
{
	paintNow();
}

void MinecraftWindow::resize(wxSizeEvent& evt)
{
	Layout();
	paintNow();
}

void MinecraftWindow::paintNow()
{
	if (panel)
	{
		wxClientDC dc(panel);
		render(dc);
	}
}

void MinecraftWindow::refreshField(wxThreadEvent& evt)
{
	paintNow();
}

void MinecraftWindow::toolBarClicked(wxCommandEvent& evt)
{
	if (evt.GetId() == TOOLBAR_STOP_WATCHMODE)
	{
		toolbar->EnableTool(TOOLBAR_STOP_WATCHMODE, false);
		getController().stopWatchMode();
		toolbar->EnableTool(TOOLBAR_START_WATCHMODE, true);

		paintNow();
	}
	else if (evt.GetId() == TOOLBAR_START_WATCHMODE)
	{
		toolbar->EnableTool(TOOLBAR_START_WATCHMODE, false);
		getController().startWatchMode();
		toolbar->EnableTool(TOOLBAR_STOP_WATCHMODE, true);
		wxThreadEvent tEvt;
		refreshField(tEvt);
	}
	
}

void MinecraftWindow::render(wxDC& dc)
{ 
	// draw some text
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.SetBackgroundMode(wxSOLID);

	dc.Clear();

	auto& fields = getController().getField();
	Location ai1Location = getController().getAi1Location();
	Location ai2Location = getController().getAi2Location();

	if (fields.size())
	{
		float sizex = (float)panel->GetSize().GetWidth() / fields.size();
		float sizey = (float)panel->GetSize().GetHeight() / fields[0].size();

		for (int x = 0; x < fields.size(); x++)
		{
			for (int y = 0; y < fields[x].size(); y++)
			{
				if (fields[x][y] == 1)
					dc.SetBrush(*wxGREEN_BRUSH);
				else if (fields[x][y] == 0)
					dc.SetBrush(*wxYELLOW_BRUSH);
				else if (fields[x][y] == 2)
					dc.SetBrush(*wxBLACK_BRUSH);
				dc.DrawRectangle(sizex * x, sizey * y, sizex, sizey);

				if (x == ai1Location.x && y == ai1Location.y + 1)
				{
					dc.SetBrush(*wxRED_BRUSH);
					drawTriangleForDir(dc, ai1Location.dir, sizex * x, sizey * y, sizex, sizey);
				}
				if (x == ai2Location.x && y == ai2Location.y + 1)
				{
					dc.SetBrush(*wxBLUE_BRUSH);
					drawTriangleForDir(dc, ai2Location.dir, sizex * x, sizey * y, sizex, sizey);
				}
			}
		}
	}
}

void MinecraftWindow::drawTriangleForDir(wxDC& dc, int dir, int offsetX, int offsetY, int sizeX, int sizeY)
{
	std::vector<wxPoint> pointList;
	if (dir == 0)
	{
		pointList.push_back(wxPoint(offsetX, offsetY));
		pointList.push_back(wxPoint(offsetX + sizeX, offsetY));
		pointList.push_back(wxPoint(offsetX + sizeX / 2, offsetY + sizeY));
	}
	else if (dir == 90)
	{
		pointList.push_back(wxPoint(offsetX + sizeX, offsetY));
		pointList.push_back(wxPoint(offsetX + sizeX, offsetY + sizeY));
		pointList.push_back(wxPoint(offsetX, offsetY + sizeY / 2));
	}
	else if (dir == 180)
	{
		pointList.push_back(wxPoint(offsetX, offsetY + sizeY));
		pointList.push_back(wxPoint(offsetX + sizeX, offsetY + sizeY));
		pointList.push_back(wxPoint(offsetX + sizeX / 2, offsetY));
	}
	else if (dir == 270)
	{
		pointList.push_back(wxPoint(offsetX, offsetY));
		pointList.push_back(wxPoint(offsetX, offsetY + sizeY));
		pointList.push_back(wxPoint(offsetX + sizeX, offsetY+ sizeY / 2));
	}
	dc.DrawPolygon(pointList.size(), pointList.data());
}

std::string MinecraftWindow::getLabel()
{
	return "PongGame";
}

MinecraftController& MinecraftWindow::getController()
{
	return static_cast<MinecraftController&>(*controller);
}
