/// <title>Viewer Interface</title>
/// <desc>
///		Viewer handles user interaction and presents the state of the Model to them.
///		The updateView method is to be called by the Controller to reflect changes in the Model.
///		Without it, the Viewer has no means of fetching updates.
/// </desc>
/// <date>9/4/2021</date>
/// <version>1.0</version>
/// <author>David Hrusa</author>
/// <email>hrusadav@gmail.com</email>

#pragma once

#include <memory>
#include "ModelInterface.h"
#include "ControllerInterface.h"
#include "PoseData.h"

namespace PoseEditor {

	class Controller;

	/// <summary>
	///		Viewer handles user interaction and presents the state of the Model to them.
	///		The updateView method is to be called by the Controller to reflect changes in the Model.
	///		Without it, the Viewer has no means of fetching updates.
	/// </summary>
	class Viewer {
	public:
		// === system functions ===

		/// <summary>
		/// Called before the update loop begins. Configures the UI and render pipeline.
		/// Returns false if the initialization fails.
		/// </summary>
		virtual bool init() = 0;

		/// <summary>
		/// Updates the view logic during update loop.
		/// </summary>
		virtual void update() = 0;

		/// <summary>
		/// Destroys this view and cleans up.
		/// </summary>
		virtual void cleanUp() = 0;

		/// <summary>
		/// Use during initialization to pass pointer to the Controller component.
		/// This is needed, because this specific viewer also handles the input callbacks of the window.
		/// </summary>
		virtual void setController(std::shared_ptr<Controller> _controller) = 0;

		// === runtime functions ===

		/// <summary>
		/// Called by the controller whenever the model changes.
		/// It is important that this reference is const to avoid improper application design.
		/// </summary>
		/// <param name="currentPawn">Const reference to the active pawn in the model.</param>
		virtual void updateView(const PoseData::BonePawn currentPawn) = 0;
	};
}