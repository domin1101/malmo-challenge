#include "LightBulbApp/App.hpp"
#include <wx/wxprec.h>
#include "PigChase/PigChaseEvolution.hpp"
#include "PigChase/TensorflowExporter.hpp"

int main(int argc, char** argv)
{
	LightBulb::App* app = new LightBulb::App();
	app->addTrainingPlan(new PigChaseEvolution());
	app->addExporter(new TensorflowExporter());

	wxApp::SetInstance(app);
	wxEntry(argc, argv);

	return 0;
}
