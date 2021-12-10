/// <title>Launcher</title>
/// <desc>
///		Contains main and launches the apropriate application components.
///		Houses the ApplicationInstance wrapper around the View, Model, Controller components.
/// </desc>
/// <date>9/4/2021</date>
/// <version>1.0</version>
/// <author>David Hrusa</author>
/// <email>hrusadav@gmail.com</email>

#include "Launcher.h"

PoseEditor::ApplicationInstance* app;

int main(int argc, char* argv[]) {
	std::printf("Welcome to pose editor\n");

	// At this stage you could introduce any other combination of components using command line arguments:
	app = new PoseEditor::ApplicationInstance();
	app->initComponents(
		/* dynamic cast here is needed because due to the interdependence of the interfaces and forward declaration,
		the implementations don't realize they are children of their interfaces in this scope. */
		std::dynamic_pointer_cast<PoseEditor::Model>(std::make_shared<PoseModel::PoseModel>()),
		std::dynamic_pointer_cast<PoseEditor::Viewer>(std::make_shared<ViewerGUI::ViewerGLFW>()),
		std::dynamic_pointer_cast<PoseEditor::Controller>(std::make_shared<PoseController::PoseController>()));
	app->init();
	app->start();

	return 0;
}

void PoseEditor::ApplicationInstance::initComponents(
	std::shared_ptr<Model> _model,
	std::shared_ptr<Viewer> _viewer,
	std::shared_ptr<Controller> _controller) {
	model = _model;
	viewer = _viewer;
	controller = _controller;

	/* Introduce components to each other: */
	controller->setModel(model);
	controller->setViewer(viewer);
	viewer->setController(controller);
}

bool PoseEditor::ApplicationInstance::init() {
	controller->init();
	viewer->init();
	return true;
}

void PoseEditor::ApplicationInstance::start() {
	running = true;
	while (running) {
		controller->update();
		running = controller->getApplicationActive();
	}
	viewer->cleanUp();
	controller->cleanUp();
}
