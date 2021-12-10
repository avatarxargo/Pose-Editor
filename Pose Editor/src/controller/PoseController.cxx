/// <title>Pose Controller</title>
/// <desc>
///		Implementation of abstract ControllerInterface. 
/// </desc>
/// <date>9/4/2021</date>
/// <version>1.0</version>
/// <author>David Hrusa</author>
/// <email>hrusadav@gmail.com</email>

#include "PoseController.h"

bool PoseController::PoseController::init() {
	// start the scene with a new file.
	cmdNewFile();
	return true;
}

void PoseController::PoseController::update() {
	m_Viewer->update();
	// if the model changed, update view:
	if (m_Model->isDelta()) {
		m_Model->resetDelta();
		m_Viewer->updateView(m_Model->getCurrentPawn());
	}
}

void PoseController::PoseController::cleanUp() {
	// nothing
}

void PoseController::PoseController::setModel(std::shared_ptr<PoseEditor::Model> _model) { m_Model = _model; }
void PoseController::PoseController::setViewer(std::shared_ptr<PoseEditor::Viewer> _viewer) { m_Viewer = _viewer; }

void PoseController::PoseController::setApplicationActive(bool arg) {
	m_ApplicationActive = arg;
}
bool PoseController::PoseController::getApplicationActive() {
	return m_ApplicationActive;
}

void PoseController::PoseController::cmdNewFile() {
	PoseData::BonePawn blankPawn;
	blankPawn.loaded = false;
	blankPawn.originalFileName = "Untitled";
	m_Model->cmdSetPawn(blankPawn);
	m_Model->cmdSetSaved(true); // a blank file doesn't need saving.
}

bool PoseController::PoseController::cmdOpenFile(std::string path) {
	PoseData::BonePawn filePawn = PoseDataUtil::openFile(path);
	if (filePawn.loaded) {
		m_Model->cmdSetPawn(filePawn);
		m_Model->cmdSetSaved(true); // a newly loaded file is saved on the disk.
	}
	return filePawn.loaded;
}

bool PoseController::PoseController::cmdSaveFile(std::string _path) {
	std::string path = PoseDataUtil::addExtension(_path);
	if (PoseDataUtil::saveFile(m_Model->getCurrentPawn(), path)) {
		// since the save succeeded update the model to reflect the new file path.
		m_Model->cmdSetFilePath(path);
		m_Model->cmdSetFileName(PoseDataUtil::parseFilename(path));
		m_Model->cmdSetLoaded(true);
		m_Model->cmdSetSaved(true);
		std::cout << "Saved filename: " << path << "\n";
	}
	return false;
}

void PoseController::PoseController::cmdBoneAdd(ID parentid) { m_Model->cmdBoneAdd(parentid); }
void PoseController::PoseController::cmdBoneRemove(ID boneid) { m_Model->cmdBoneRemove(boneid); }
void PoseController::PoseController::cmdBoneMoveUp(ID boneid) { m_Model->cmdBoneMoveUp(boneid); }
void PoseController::PoseController::cmdBoneMoveDown(ID boneid) { m_Model->cmdBoneMoveDown(boneid); }

void PoseController::PoseController::cmdBoneSetRotation(ID boneid, glm::vec3 euler) { m_Model->cmdBoneSetRotation(boneid, euler); }
void PoseController::PoseController::cmdBoneSetName(ID boneid, std::string name) { m_Model->cmdBoneSetName(boneid, name); }
void PoseController::PoseController::cmdBoneSetParent(ID boneid, ID parentid) { m_Model->cmdBoneSetParent(boneid, parentid); }