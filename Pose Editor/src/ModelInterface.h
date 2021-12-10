/// <title>Model Interface</title>
/// <desc>
///		Maintains the underlying datastructure of the currently open file. The only way to change the state of the data is through the command methods.
///		Additionally, the delta flag is maintained. Commands resulting in transformative actions will dirty this flag. This allows the Controller to
///		minimize update calls to the Viewer when no change occurs.
/// 
///		Provides a range of command methods (prefix cmd) which respond to controller input.
/// </desc>
/// <date>9/4/2021</date>
/// <version>1.0</version>
/// <author>David Hrusa</author>
/// <email>hrusadav@gmail.com</email>

#pragma once

#include "PoseData.h"

namespace PoseEditor {

	/// <summary>
	///		Maintains the underlying datastructure of the currently open file. The only way to change the state of the data is through the command methods.
	///		Additionally, the delta flag is maintained. Commands resulting in transformative actions will dirty this flag. This allows the Controller to
	///		minimize update calls to the Viewer when no change occurs.
	/// 
	///		Provides a range of command methods (prefix cmd) which respond to controller input.
	/// </summary>
	class Model {
	public:
		// === runtime functions ===

		/// <returns>
		/// true if model values have changed. (view should be refreshed)
		/// </returns>
		virtual bool isDelta() = 0;

		/// <summary>
		/// Resets delta to false once updates have been propagated.
		/// </summary>
		virtual void resetDelta() = 0;

		/// <summary>
		/// Provides a const reference to the current pawn for other parts of the program.
		/// </summary>
		const virtual PoseData::BonePawn& getCurrentPawn() = 0;

		// === command functions ===

		/// <summary>
		/// called by Controller when new BonePawn is to be inserted into the model.
		/// </summary>
		virtual void cmdSetPawn(PoseData::BonePawn pawn) = 0;
		/// <summary>
		/// called by Controller to set current BonePawn's path of saving. (including the filename)
		/// </summary>
		virtual void cmdSetFilePath(std::string path) = 0;
		/// <summary>
		/// called by Controller to set current BonePawn's filename to save to.
		/// </summary>
		virtual void cmdSetFileName(std::string name) = 0;
		/// <summary>
		/// called by Controller to set whether the current BonePawn has a valid file path to save to. Generated pawns do not have one.
		/// </summary>
		virtual void cmdSetLoaded(bool arg) = 0;
		/// <summary>
		/// called by Controller to set whether the current BonePawn has been saved. This value will be dirtied by any transformative opearation.
		/// </summary>
		virtual void cmdSetSaved(bool arg) = 0;

		/// <summary>
		/// called by Controller to add a new bone.
		/// </summary>
		/// <param name="parentid">id of the new bone's parent. Pass -1 to create a root bone.</param>
		virtual void cmdBoneAdd(ID parentid) = 0;
		/// <summary>
		/// called by Controller to remove the provided bone. All children are assigned to this bone's parent.
		/// </summary>
		/// <param name="boneid">bone to be removed.</param>
		virtual void cmdBoneRemove(ID boneid) = 0;
		/// <summary>
		/// called by Controller to move the bone up in the csv ordering.
		/// </summary>
		/// <param name="boneid">bone to be moved.</param>
		virtual void cmdBoneMoveUp(ID boneid) = 0;
		/// <summary>
		/// called by Controller to move the bone down in the csv ordering.
		/// </summary>
		/// <param name="boneid">bone to be moved.</param>
		virtual void cmdBoneMoveDown(ID boneid) = 0;

		/// <summary>
		/// called by Controller to set the provided bone to new rotation values. This method converts the euler angles to quaternions
		/// and updates both records for this bone. Furthermore, it converts the new quaternion values back to euler as the new euler value.
		/// Since euler angles are prown to gimbal lock and flipping at certain configurations, such process ensures that the user observes 
		/// this interpretation immediately, instead of finding data stored differently than presented by the View at runtime.
		/// </summary>
		/// <param name="boneid">bone to be changed</param>
		/// <param name="euler">yaw, pitch, roll euler angles</param>
		virtual void cmdBoneSetRotation(ID boneid, glm::vec3 euler) = 0;
		/// <summary>
		/// called by Controller to give a bone a different display name. The operation will not change the name
		/// if a bone with this name already exists. (Though the View may not reflect this)
		/// </summary>
		/// <param name="boneid">bone to be changed.</param>
		/// <param name="name">the new name</param>
		virtual void cmdBoneSetName(ID boneid, std::string name) = 0;
		/// <summary>
		/// called by Controller to assign a bone to a new parrent. This operation will not proceed if such name
		/// already exists.
		/// </summary>
		/// <param name="boneid">bone to be changed.</param>
		/// <param name="parentid">new parent id, pass -1 to make the provided bone a root bone.</param>
		virtual void cmdBoneSetParent(ID boneid, ID parentid) = 0;
	};
}