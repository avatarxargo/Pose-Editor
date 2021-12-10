/// <title>Controller Interface</title>
/// <desc>
///		Controller receives prompts from the View and issues commands to the Model and View accordingly.
///		Provides a range of command methods (prefix cmd) which respond to user input.
/// </desc>
/// <date>9/4/2021</date>
/// <version>1.0</version>
/// <author>David Hrusa</author>
/// <email>hrusadav@gmail.com</email>

#pragma once

#include <memory>
#include <string>
#include <glm/vec3.hpp>

#include "ModelInterface.h"
#include "ViewerInterface.h"

namespace PoseEditor {

	class Model;
	class Viewer;

	/// <summary>
	///	Controller receives prompts from the View and issues commands to the Model and View accordingly.
	///	Provides a range of command methods (prefix cmd) which respond to user input.
	/// </summary>
	class Controller {
	public:
		// === system functions ===

		/// <summary>
		/// Called before the update loop begins.
		/// Returns false if the initialization fails.
		/// </summary>
		virtual bool init() = 0;

		/// <summary>
		/// Updates the controller logic during update loop.
		/// If the Model has dirty delta, it propagates this information to the VIew.
		/// </summary>
		virtual void update() = 0;

		/// <summary>
		/// Cleans up the controller.
		/// </summary>
		virtual void cleanUp() = 0;

		/// <summary>
		/// Use during initialization to pass pointer to the Model component.
		/// </summary>
		virtual void setModel(std::shared_ptr<Model> _model) = 0;
		/// <summary>
		/// Use during initialization to pass pointer to the Viewer component.
		/// </summary>
		virtual void setViewer(std::shared_ptr<Viewer> _viewer) = 0;

		// === runtime functions ===

		/// <summary>
		/// When set to false, closes the application.
		/// </summary>
		virtual void setApplicationActive(bool arg) = 0;

		/// <returns>
		/// true if the application should shut down.
		/// </returns>
		virtual bool getApplicationActive() = 0;

		// === command functions ===

		/// <summary>
		/// call when the UI logic determines a blank new file should be created.
		/// </summary>
		virtual void cmdNewFile() = 0;
		/// <summary>
		/// call when the UI logic determines a new model should be parsed from the provided path.
		/// </summary>
		/// <returns>true if the opening succeeded</returns>
		virtual bool cmdOpenFile(std::string path) = 0;
		/// <summary>
		/// call when the UI logic determines the current state of the model should be saved to the provided path.
		/// </summary>
		/// <returns>true if the saving succeeded</returns>
		virtual bool cmdSaveFile(std::string path) = 0;

		/// <summary>
		/// call when the UI logic determines a new bone should be added.
		/// </summary>
		/// <param name="parentid">id of the new bone's parent. Pass -1 to create a root bone.</param>
		virtual void cmdBoneAdd(ID parentid) = 0;
		/// <summary>
		/// call when the UI logic determines the provided bone should be removed. All children are assigned to this bone's parent.
		/// </summary>
		/// <param name="boneid">bone to be removed.</param>
		virtual void cmdBoneRemove(ID boneid) = 0;
		/// <summary>
		/// call when the UI logic determines a new bone should be shifted up in the csv ordering.
		/// </summary>
		/// <param name="boneid">bone to be moved.</param>
		virtual void cmdBoneMoveUp(ID boneid) = 0;
		/// <summary>
		/// call when the UI logic determines a new bone should be shifted down in the csv ordering.
		/// </summary>
		/// <param name="boneid">bone to be moved.</param>
		virtual void cmdBoneMoveDown(ID boneid) = 0;

		/// <summary>
		/// call when the UI logic determines the provided bone should assume new rotation values. This method converts the euler angles to quaternions
		/// and updates both records for this bone. Furthermore, it converts the new quaternion values back to euler as the new euler value.
		/// Since euler angles are prown to gimbal lock and flipping at certain configurations, such process ensures that the user observes 
		/// this interpretation immediately, instead of finding data stored differently than presented by the View at runtime.
		/// </summary>
		/// <param name="boneid">bone to be changed</param>
		/// <param name="euler">yaw, pitch, roll euler angles</param>
		virtual void cmdBoneSetRotation(ID boneid, glm::vec3 euler) = 0;
		/// <summary>
		/// call when the UI logic determines the given bone should have a different display name. The operation will not change the name
		/// if a bone with this name already exists. (Though the View may not reflect this)
		/// </summary>
		/// <param name="boneid">bone to be changed.</param>
		/// <param name="name">the new name</param>
		virtual void cmdBoneSetName(ID boneid, std::string name) = 0;
		/// <summary>
		/// call when the UI logic determines the given bone should be assigned to a new parrent. This operation will not proceed if such name
		/// already exists.
		/// </summary>
		/// <param name="boneid">bone to be changed.</param>
		/// <param name="parentid">new parent id, pass -1 to make the provided bone a root bone.</param>
		virtual void cmdBoneSetParent(ID boneid, ID parentid) = 0;
	};
}