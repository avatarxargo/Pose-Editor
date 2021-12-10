/// <title>Pose Controller</title>
/// <desc>
///		Implementation of abstract ControllerInterface. 
/// </desc>
/// <date>9/4/2021</date>
/// <version>1.0</version>
/// <author>David Hrusa</author>
/// <email>hrusadav@gmail.com</email>

#pragma once

#include <memory>
#include <string>

#include "../ControllerInterface.h"
#include "../ModelInterface.h"
#include "../ViewerInterface.h"
#include "../model/PoseDataUtil.h"

namespace PoseController {

	/// <summary>
	///	Implementation of abstract ControllerInterface. 
	/// </summary>
	class PoseController : public PoseEditor::Controller {
	private:
		/// <summary>
		/// Application exits when this is set to false by any View.
		/// </summary>
		bool m_ApplicationActive = true;
		/// <summary>Pointer to the Model application component to propagate user actions onto.</summary>
		std::shared_ptr<PoseEditor::Model> m_Model;
		/// <summary>Pointer to the Viewer application component to propagate model changes onto.</summary>
		std::shared_ptr<PoseEditor::Viewer> m_Viewer;

	public:

		// === system functions ===

		/// <summary>
		/// Called before the update loop begins.
		/// Returns false if the initialization fails.
		/// </summary>
		bool init() override;

		/// <summary>
		/// Updates the controller logic during update loop.
		/// If the Model has dirty delta, it propagates this information to the VIew.
		/// </summary>
		void update() override;

		/// <summary>
		/// Cleans up the controller.
		/// </summary>
		void cleanUp() override;

		/// <summary>
		/// Use during initialization to pass pointer to the Model component.
		/// </summary>
		void setModel(std::shared_ptr<PoseEditor::Model> _model) override;
		/// <summary>
		/// Use during initialization to pass pointer to the Viewer component.
		/// </summary>
		void setViewer(std::shared_ptr<PoseEditor::Viewer> _viewer) override;

		// === runtime functions ===

		/// <summary>
		/// When set to false, closes the application.
		/// </summary>
		void setApplicationActive(bool arg) override;
		/// <returns>
		/// true if the application should shut down.
		/// </returns>
		bool getApplicationActive() override;

		// === command functions ===

		/// <summary>
		/// call when the UI logic determines a blank new file should be created.
		/// </summary>
		void cmdNewFile() override;
		/// <summary>
		/// call when the UI logic determines a new model should be parsed from the provided path.
		/// </summary>
		/// <returns>true if the opening succeeded</returns>
		bool cmdOpenFile(std::string path) override;
		/// <summary>
		/// call when the UI logic determines the current state of the model should be saved to the provided path.
		/// </summary>
		/// <returns>true if the saving succeeded</returns>
		bool cmdSaveFile(std::string path) override;

		/// <summary>
		/// call when the UI logic determines a new bone should be added.
		/// </summary>
		/// <param name="parentid">id of the new bone's parent. Pass -1 to create a root bone.</param>
		void cmdBoneAdd(ID parentid) override;
		/// <summary>
		/// call when the UI logic determines the provided bone should be removed. All children are assigned to this bone's parent.
		/// </summary>
		/// <param name="boneid">bone to be removed.</param>
		void cmdBoneRemove(ID boneid) override;
		/// <summary>
		/// call when the UI logic determines a new bone should be shifted up in the csv ordering.
		/// </summary>
		/// <param name="boneid">bone to be moved.</param>
		void cmdBoneMoveUp(ID boneid) override;
		/// <summary>
		/// call when the UI logic determines a new bone should be shifted down in the csv ordering.
		/// </summary>
		/// <param name="boneid">bone to be moved.</param>
		void cmdBoneMoveDown(ID boneid) override;

		/// <summary>
		/// call when the UI logic determines the provided bone should assume new rotation values. This method converts the euler angles to quaternions
		/// and updates both records for this bone. Furthermore, it converts the new quaternion values back to euler as the new euler value.
		/// Since euler angles are prown to gimbal lock and flipping at certain configurations, such process ensures that the user observes 
		/// this interpretation immediately, instead of finding data stored differently than presented by the View at runtime.
		/// </summary>
		/// <param name="boneid">bone to be changed</param>
		/// <param name="euler">yaw, pitch, roll euler angles</param>
		void cmdBoneSetRotation(ID boneid, glm::vec3 euler) override;
		/// <summary>
		/// call when the UI logic determines the given bone should have a different display name. The operation will not change the name
		/// if a bone with this name already exists. (Though the View may not reflect this)
		/// </summary>
		/// <param name="boneid">bone to be changed.</param>
		/// <param name="name">the new name</param>
		void cmdBoneSetName(ID boneid, std::string name) override;
		/// <summary>
		/// call when the UI logic determines the given bone should be assigned to a new parrent. This operation will not proceed if such name
		/// already exists.
		/// </summary>
		/// <param name="boneid">bone to be changed.</param>
		/// <param name="parentid">new parent id, pass -1 to make the provided bone a root bone.</param>
		void cmdBoneSetParent(ID boneid, ID parentid) override;
	};
}