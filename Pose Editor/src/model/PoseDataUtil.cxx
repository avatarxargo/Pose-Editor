/// <title>Pose Data Util</title>
/// <desc>
///		I/O and other manipulation for PoseData.
/// </desc>
/// <date>9/5/2021</date>
/// <version>1.0</version>
/// <author>David Hrusa</author>
/// <email>hrusadav@gmail.com</email>

#define LOAD_FAILED { {}, path, parseFilename(path), false }
#define ARG_COUNT 7
#define MAX_BONE_LIMIT 1000000
#define STR(X) #X

#include "PoseDataUtil.h"

PoseData::BonePawn PoseDataUtil::openFile(std::string path) {
	std::ifstream ifile;
	ifile.open(path.c_str(), std::ios::in);

	if (ifile.is_open()) {
		std::string line;
		std::string field;
		PoseData::BonePawn pawn = {};
		pawn.originalFilePath = path;
		pawn.originalFileName = parseFilename(path);
		pawn.loaded = true;
		PoseData::BoneData bone;
		std::stringstream lineStream;
		int row_counter = 0; // index of the current row.
		int line_counter = 0; // number of fields read on current line.

		while (std::getline(ifile, line)) {
			// parse line (create a new Bone and start reading into it):
			lineStream = std::stringstream(line);
			line_counter = 0;
			bone = {};
			while (lineStream.good() && line_counter < ARG_COUNT) {
				std::getline(lineStream, field, ',');
				if (!boneParseCSVField(bone, line_counter, field)) {
					std::fprintf(stderr, "Trouble reading '%s' [row %d, col %d]: Value could not be parsed.", path.c_str(), row_counter, line_counter);
					ifile.close();
					return LOAD_FAILED;
				}
				line_counter++;
			}
			if (line_counter < ARG_COUNT) {
				std::fprintf(stderr, "Trouble reading '%s' [row %d, col %d]: Premature end of line. Expected %d items.", path.c_str(), row_counter, line_counter, ARG_COUNT);
				ifile.close();
				return LOAD_FAILED;
			}
			// the bone was read successfuly: Add it to the pawn.
			bone.eulerRotation = quatToEuler(bone.quaternion);
			PoseDataUtil::pawnInsertBone(pawn, bone);
			row_counter++;
		}
		// all lines read, return pawn:
		ifile.close();
		pawn.saved = true;
		return pawn;
	}
	else {
		std::fprintf(stderr, "Trouble reading '%s': Could not open file.", path.c_str());
		return LOAD_FAILED;
	}
}

bool PoseDataUtil::saveFile(const PoseData::BonePawn pawn, std::string path) {
	std::ofstream ofile;
	ofile.open(path.c_str(), std::ios::out);

	if (ofile.is_open()) {
		for (int i = 0; i < pawn.bones.size(); i++) {
			const PoseData::BoneData& bone = pawn.bones[i];
			ofile << bone.id << ", " << bone.parent << ", " <<
				bone.quaternion[0] << ", " <<
				bone.quaternion[1] << ", " <<
				bone.quaternion[2] << ", " <<
				bone.quaternion[3] << ", " <<
				bone.displayName <<
				(i < (pawn.bones.size() - 1) ? "\n" : "");
		}
		ofile.close();
		return true;
	}
	else {
		std::fprintf(stderr, "Trouble writing to '%s': Could not open file.", path.c_str());
		return false;
	}
}


glm::quat PoseDataUtil::eulerToQuat(glm::vec3 euler) {
	return glm::quat(glm::radians(euler));
}

glm::vec3 PoseDataUtil::quatToEuler(glm::quat quat) {
	return glm::degrees(glm::eulerAngles(glm::normalize(quat)));
}

std::string PoseDataUtil::parseFilename(std::string arg) {
	size_t last = arg.find_last_of("/\\");
	if (last != std::string::npos)
		return arg.substr(last + 1);
	return arg;
}

std::string PoseDataUtil::addExtension(std::string arg) {
	size_t last = arg.find_last_of(".");
	if (last != std::string::npos)
		return arg.substr(0, last).append(".csv");
	return arg.append(".csv");
}

std::string PoseDataUtil::getUniqueBoneName(const PoseData::BonePawn pawn) {
	for (int id = 1; id < MAX_BONE_LIMIT; id++) {
		std::string name = "bone (" + std::to_string(id) + ")";
		if (pawnFindBoneName(pawn, name) < 0) { //not foud
			return name;
		}
	}
	// somehow there is a million bones with the default name, it's probably better to throw an exception to stop a loop:
	std::fprintf(stderr, "Error: getUniqueBoneName() found more than %d bones with default names.", MAX_BONE_LIMIT);
	throw std::runtime_error("getUniqueBoneName() found more than" STR(MAX_BONE_LIMIT) "bones with default names.");
}

ID PoseDataUtil::getUniqueBoneId(const PoseData::BonePawn pawn) {
	for (int id = 1; id < MAX_BONE_LIMIT; id++) {
		if (pawnFindBoneId(pawn, id) < 0) { //not foud
			return id;
		}
	}
	// somehow there is a million bones with the default name, it's probably better to throw an exception to stop a loop:
	std::fprintf(stderr, "Error: getUniqueBoneId() found more than %d bones.", MAX_BONE_LIMIT);
	throw std::runtime_error("getUniqueBoneId() found more than" STR(MAX_BONE_LIMIT) "bones.");
}

int PoseDataUtil::pawnFindBoneName(const PoseData::BonePawn pawn, std::string boneName) {
	int pos = 0;
	for (PoseData::BoneData bone : pawn.bones) {
		if (boneName.compare(bone.displayName) == 0)
			return pos;
		pos++;
	}
	return -1;
}

int PoseDataUtil::pawnFindBoneId(const PoseData::BonePawn pawn, ID id) {
	int pos = 0;
	for (PoseData::BoneData bone : pawn.bones) {
		if (bone.id == id) {
			return pos;
		}
		pos++;
	}
	return -1;
}

bool PoseDataUtil::pawnTestBoneParentLoop(const PoseData::BonePawn pawn, ID startBone) {
	/* if we keep finding parents for more than the count of elements, there is a loop.*/
	ID current = startBone;
	for (int i = 0; i <= pawn.bones.size(); ++i) {
		int idx = pawnFindBoneId(pawn, current);
		if (idx < 0) //root reached
			return true;
		current = pawn.bones[idx].parent;
	}
	return false;
}

std::vector<int> PoseDataUtil::pawnGetBoneChildren(const PoseData::BonePawn pawn, ID parent) {
	auto list = std::vector<int>();
	int pos = 0;
	for (PoseData::BoneData bone : pawn.bones) {
		if (bone.parent == parent) {
			list.push_back(pos);
		}
		pos++;
	}
	return list;
}

bool PoseDataUtil::boneParseCSVField(PoseData::BoneData& bone, int line_counter, std::string value) {
	try {
		switch (line_counter) {
		case 0: //bone id
			bone.id = std::stoi(value);
			return true;
		case 1: //parent bone id
			bone.parent = std::stoi(value);
			return true;
		case 2: //quaternion X
		case 3: //quaternion Y
		case 4: //quaternion Z
		case 5: //quaternion W
			bone.quaternion[line_counter - 2] = std::stof(value);
			return true;
		case 6: //name
			bone.displayName = value.substr(1);
			return true;
		default:
			// this index doesn't have a corresponding bone field.
			return false;
		}
	}
	catch (const std::exception&)
	{
		// we could detect specific exceptions, but it only matters that reading failed.
		return false;
	}

}

PoseData::BoneData PoseDataUtil::boneDeepCopy(const PoseData::BoneData& source) {
	PoseData::BoneData bone;
	bone.id = source.id;
	bone.parent = source.parent;
	bone.quaternion = source.quaternion;
	bone.eulerRotation = source.eulerRotation;
	bone.displayName = source.displayName;
	return bone;
}


void PoseDataUtil::pawnInsertBone(PoseData::BonePawn& pawn, PoseData::BoneData bone, int index) {
	if (index < 0)
		pawn.bones.push_back(bone);
	else
		pawn.bones.insert(pawn.bones.begin() + index, bone);
}

PoseData::BonePawn PoseDataUtil::pawnDeepCopy(const PoseData::BonePawn& source) {
	PoseData::BonePawn pawn;
	for (PoseData::BoneData bone : source.bones)
		pawn.bones.push_back(boneDeepCopy(bone));
	pawn.originalFilePath = source.originalFilePath;
	pawn.originalFileName = source.originalFileName;
	pawn.loaded = source.loaded;
	pawn.saved = source.saved;
	return pawn;
}
