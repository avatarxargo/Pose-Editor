/// <title>Pose Data Model</title>
/// <desc>
///		Implementation of the abstract ModelInterface. Responds to prompts from the controler and maintains PoseData.
///		Util included in PoseDataUtil.
/// </desc>
/// <date>9/4/2021</date>
/// <version>1.0</version>
/// <author>David Hrusa</author>
/// <email>hrusadav@gmail.com</email>

#include "PoseDataModel.h"

inline void PoseModel::PoseModel::delta() {
	m_Delta = true;
	m_BonePawn.saved = false;
}

bool PoseModel::PoseModel::boneInRange(int boneid) {
	return boneid < m_BonePawn.bones.size() && boneid >= 0;
}

bool PoseModel::PoseModel::isDelta() {
	return m_Delta;
}
void PoseModel::PoseModel::resetDelta() {
	m_Delta = false;
}

const PoseData::BonePawn& PoseModel::PoseModel::getCurrentPawn() {
	return m_BonePawn;
}

void PoseModel::PoseModel::cmdSetPawn(PoseData::BonePawn pawn) {
	m_BonePawn = pawn;
	delta();
}

void PoseModel::PoseModel::cmdSetFilePath(std::string path) {
	m_BonePawn.originalFilePath = path;
	delta();
}

void PoseModel::PoseModel::cmdSetFileName(std::string name) {
	m_BonePawn.originalFileName = name;
	delta();
}

void PoseModel::PoseModel::cmdSetLoaded(bool arg) {
	m_BonePawn.loaded = arg;
	delta();
}

void PoseModel::PoseModel::cmdSetSaved(bool arg) {
	m_BonePawn.saved = arg;
	m_Delta = true;
}

void PoseModel::PoseModel::cmdBoneAdd(ID parentid) {
	delta();
	PoseData::BoneData bone;
	bone.id = PoseDataUtil::getUniqueBoneId(m_BonePawn);
	bone.parent = parentid;
	bone.displayName = PoseDataUtil::getUniqueBoneName(m_BonePawn);
	if (parentid > 0) {
		int parentCoord = PoseDataUtil::pawnFindBoneId(m_BonePawn, parentid);
		if (parentCoord >= 0) {
			parentCoord += parentCoord < m_BonePawn.bones.size() ? 1 : 0;
			m_BonePawn.bones.insert(m_BonePawn.bones.begin() + parentCoord, bone);
			return;
		}
	}
	m_BonePawn.bones.push_back(bone);
}
void PoseModel::PoseModel::cmdBoneRemove(ID boneid) {
	int coord = PoseDataUtil::pawnFindBoneId(m_BonePawn, boneid);
	if (coord >= 0) { // found
		/* first set any child node's parent to the deleted node's parent */
		auto children = PoseDataUtil::pawnGetBoneChildren(m_BonePawn, boneid);
		for (auto child : children) {
			m_BonePawn.bones[child].parent = m_BonePawn.bones[coord].parent;
		}
		m_BonePawn.bones.erase(m_BonePawn.bones.begin() + coord);
		m_Delta = true;
	}
}
void PoseModel::PoseModel::cmdBoneMoveUp(ID boneid) {
	int coord = PoseDataUtil::pawnFindBoneId(m_BonePawn, boneid);
	if (coord > 0) { // not the first element
		PoseData::BoneData tmp = m_BonePawn.bones[coord];
		m_BonePawn.bones[coord] = m_BonePawn.bones[coord - 1];
		m_BonePawn.bones[coord - 1] = tmp;
		delta();
	}
}
void PoseModel::PoseModel::cmdBoneMoveDown(ID boneid) {
	int coord = PoseDataUtil::pawnFindBoneId(m_BonePawn, boneid);
	if (coord >= 0 && coord < (m_BonePawn.bones.size() - 1)) { // not the last element
		PoseData::BoneData tmp = m_BonePawn.bones[coord];
		m_BonePawn.bones[coord] = m_BonePawn.bones[coord + 1];
		m_BonePawn.bones[coord + 1] = tmp;
		delta();
	}
}

void PoseModel::PoseModel::cmdBoneSetRotation(ID boneid, glm::vec3 euler) {
	int coord = PoseDataUtil::pawnFindBoneId(m_BonePawn, boneid);
	if (coord >= 0) { // found
		delta();
		m_BonePawn.bones[coord].eulerRotation = euler;
		m_BonePawn.bones[coord].quaternion = PoseDataUtil::eulerToQuat(m_BonePawn.bones[coord].eulerRotation);
		/* The next steo is a little redundant, but the idea is to expose any edge cases in quaternion/euler conversion
		instead of concealing them and saving corrupt data into the file. This way it will propagate back to UI immediately.*/
		m_BonePawn.bones[coord].eulerRotation = PoseDataUtil::quatToEuler(m_BonePawn.bones[coord].quaternion);
	}
}
void PoseModel::PoseModel::cmdBoneSetName(ID boneid, std::string name) {
	int coord = PoseDataUtil::pawnFindBoneId(m_BonePawn, boneid);
	int nameCoord = PoseDataUtil::pawnFindBoneName(m_BonePawn, name);
	if (coord >= 0 && nameCoord < 0) { // found && name not used
		delta();
		m_BonePawn.bones[coord].displayName = name;
	}
}

void PoseModel::PoseModel::cmdBoneSetParent(ID boneid, ID parentid) {
	int coord = PoseDataUtil::pawnFindBoneId(m_BonePawn, boneid);
	if (coord >= 0) { // found
		/* only set it if it won't create an infinite loop */
		auto originalParent = m_BonePawn.bones[coord].parent;
		m_BonePawn.bones[coord].parent = parentid;
		if (PoseDataUtil::pawnTestBoneParentLoop(m_BonePawn, boneid)) {
			delta();
		}
		else {
			m_BonePawn.bones[coord].parent = originalParent;
		}
	}
}