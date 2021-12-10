/// <title>Pose Data Model</title>
/// <desc>
///		Implementation of the abstract ModelInterface. Responds to prompts from the controler and maintains PoseData.
///		Util included in PoseDataUtil.
/// </desc>
/// <date>9/4/2021</date>
/// <version>1.0</version>
/// <author>David Hrusa</author>
/// <email>hrusadav@gmail.com</email>

#pragma once

#include "../ModelInterface.h"
#include "../PoseData.h"
#include "PoseDataUtil.h"

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace PoseModel {

	/// <summary>
	///	Implementation of the abstract ModelInterface. Responds to prompts from the controler and maintains PoseData.
	///	Util included in PoseDataUtil.
	/// </summary>
	class PoseModel : public PoseEditor::Model {
	private:
		/// <summary>maintains the pawn being modified through commands.</summary>
		PoseData::BonePawn m_BonePawn = { {}, "","", false };
		/// <summary>true if model values have changed.</summary>
		bool m_Delta = false;
		/// <returns>true if pawn contains given bone.</returns>
		bool boneInRange(int boneid);
		/// <summary>marks dirty bits for delta and saved.</summary>
		inline void delta();
	public:

		/// <returns>
		/// true if model values have changed. (view should be refreshed)
		/// </returns>
		bool isDelta() override;
		/// <summary>
		/// Resets delta to false once updates have been propagated.
		/// </summary>
		void resetDelta() override;
		/// <summary>
		/// Provides a const reference to the current pawn for other parts of the program.
		/// </summary>
		const PoseData::BonePawn& getCurrentPawn() override;

		/// <summary>
		/// called by Controller when new BonePawn is to be inserted into the model.
		/// </summary>
		void cmdSetPawn(PoseData::BonePawn pawn) override;
		/// <summary>
		/// called by Controller to set current BonePawn's path of saving. (including the filename)
		/// </summary>
		void cmdSetFilePath(std::string path) override;
		/// <summary>
		/// called by Controller to set current BonePawn's filename to save to.
		/// </summary>
		void cmdSetFileName(std::string name) override;
		/// <summary>
		/// called by Controller to set whether the current BonePawn has a valid file path to save to. Generated pawns do not have one.
		/// </summary>
		void cmdSetLoaded(bool arg) override;
		/// <summary>
		/// called by Controller to set whether the current BonePawn has been saved. This value will be dirtied by any transformative opearation.
		/// </summary>
		void cmdSetSaved(bool arg) override;

		/// <summary>
		/// called by Controller to add a new bone.
		/// </summary>
		/// <param name="parentid">id of the new bone's parent. Pass -1 to create a root bone.</param>
		void cmdBoneAdd(ID parentid) override;
		/// <summary>
		/// called by Controller to remove the provided bone. All children are assigned to this bone's parent.
		/// </summary>
		/// <param name="boneid">bone to be removed.</param>
		void cmdBoneRemove(ID boneid) override;
		/// <summary>
		/// called by Controller to move the bone up in the csv ordering.
		/// </summary>
		/// <param name="boneid">bone to be moved.</param>
		void cmdBoneMoveUp(ID boneid) override;
		/// <summary>
		/// called by Controller to move the bone down in the csv ordering.
		/// </summary>
		/// <param name="boneid">bone to be moved.</param>
		void cmdBoneMoveDown(ID boneid) override;

		/// <summary>
		/// called by Controller to set the provided bone to new rotation values. This method converts the euler angles to quaternions
		/// and updates both records for this bone. Furthermore, it converts the new quaternion values back to euler as the new euler value.
		/// Since euler angles are prown to gimbal lock and flipping at certain configurations, such process ensures that the user observes 
		/// this interpretation immediately, instead of finding data stored differently than presented by the View at runtime.
		/// </summary>
		/// <param name="boneid">bone to be changed</param>
		/// <param name="euler">yaw, pitch, roll euler angles</param>
		void cmdBoneSetRotation(ID boneid, glm::vec3 euler) override;
		/// <summary>
		/// called by Controller to give a bone a different display name. The operation will not change the name
		/// if a bone with this name already exists. (Though the View may not reflect this)
		/// </summary>
		/// <param name="boneid">bone to be changed.</param>
		/// <param name="name">the new name</param>
		void cmdBoneSetName(ID boneid, std::string name) override;
		/// <summary>
		/// called by Controller to assign a bone to a new parrent. This operation will not proceed if such name
		/// already exists.
		/// </summary>
		/// <param name="boneid">bone to be changed.</param>
		/// <param name="parentid">new parent id, pass -1 to make the provided bone a root bone.</param>
		void cmdBoneSetParent(ID boneid, ID parentid) override;
	};

}