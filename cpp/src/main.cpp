#include "LightBulbApp/App.hpp"
#include <wx/wxprec.h>
#include "PongEvolution/MalmoEvolution.hpp"

int main(int argc, char** argv)
{
	LightBulb::App* app = new LightBulb::App();
	app->addTrainingPlan(new MalmoEvolution());

	wxApp::SetInstance(app);
	wxEntry(argc, argv);

	return 0;
}
