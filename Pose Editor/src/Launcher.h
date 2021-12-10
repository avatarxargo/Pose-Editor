/// <title>Launcher</title>
/// <desc>
///		Contains main and launches the apropriate application components.
///		Houses the ApplicationInstance wrapper around the View, Model, Controller components.
/// </desc>
/// <date>9/4/2021</date>
/// <version>1.0</version>
/// <author>David Hrusa</author>
/// <email>hrusadav@gmail.com</email>

#pragma once

#include <iostream>
#include <memory>

#include "view_glfw/ViewerGUI.h"
#include "model/PoseDataModel.h"
#include "controller/PoseController.h"

namespace PoseEditor {

	/// <summary>
	/// Wrapper class which holds the main application components. (In case we want to create multiple instances)
	/// </summary>
	class ApplicationInstance {
	private:
		bool running = false;
		std::shared_ptr<Model> model;
		std::shared_ptr<Viewer> viewer;
		std::shared_ptr<Controller> controller;

	public:
		/// <summary>
		/// Creates unique instances of the key application components.
		/// </summary>
		void initComponents(std::shared_ptr<Model> _model,
			std::shared_ptr<Viewer> _viewer,
			std::shared_ptr<Controller> _controller);

		/// <summary>
		/// Configures the components before starting the application proper.
		/// Assumes the components are valid.
		/// Returns false if the initialization fails.
		/// </summary>
		bool init();

		/// <summary>
		/// Begins the update loop. (Call initComponents(); and init(); prior to this)
		/// </summary>
		void start();
	};
}