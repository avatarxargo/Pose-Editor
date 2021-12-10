/// <title>Pose Data Util</title>
/// <desc>
///		I/O and other manipulation for PoseData.
/// </desc>
/// <date>9/5/2021</date>
/// <version>1.0</version>
/// <author>David Hrusa</author>
/// <email>hrusadav@gmail.com</email>

#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../ModelInterface.h"
#include "../PoseData.h"

/// <summary>
/// PoseDataUtil holds a combination of utility functions for running more elaborate tests on BonePawn, file IO
/// as well as smaller helper functions for manipulating strings and converting quaternions.
/// </summary>
namespace PoseDataUtil {

	// === File IO ===

	/// <summary>
	/// Safe file opener. Atempts to parse the provided file into a proper BonePawn.
	/// </summary>
	/// <returns>parsed file. When an error occurs, the returned file has loaded set to false.</returns>
	PoseData::BonePawn openFile(std::string path);

	/// <summary>
	/// Encodes the pawn into the provided path. If the path is empty, path from pawn is used.
	/// </summary>
	/// <returns>true if successful.</returns>
	bool saveFile(PoseData::BonePawn pawn, std::string path = "");

	// === MISC ===

	/// <summary>
	/// converts euler angles in degrees into a quaternion.
	/// </summary>
	glm::quat eulerToQuat(glm::vec3 euler);
	/// <summary>
	/// converts quaternion into euler angles in degrees.
	/// </summary>
	glm::vec3 quatToEuler(glm::quat quat);

	/// <summary>
	/// Helper function to parse out filename from a full path.
	/// </summary>
	std::string parseFilename(std::string arg);

	/// <summary>
	/// Helper function to add .csv at the end of file paths.
	/// </summary>
	std::string addExtension(std::string arg);

	/// <returns>generates a unique bone name, not yet present in the pawn.</returns>
	std::string getUniqueBoneName(const PoseData::BonePawn pawn);

	/// <returns>generates a unique bone id, not yet present in the pawn.</returns>
	ID getUniqueBoneId(const PoseData::BonePawn pawn);

	/// <summary>
	/// Finds the offset in the pawn's bone array of a bone with the provided name.
	/// </summary>
	/// <returns>offset or -1 if not found</returns>
	int pawnFindBoneName(const PoseData::BonePawn pawn, std::string boneName);

	/// <summary>
	/// Finds the offset in the pawn's bone array of a bone with the provided ID.
	/// </summary>
	/// <returns>offset or -1 if not found</returns>
	int pawnFindBoneId(const PoseData::BonePawn pawn, ID id);

	/// <summary>
	/// tests whether startBone has an infinite loop of parents within pawn.
	/// </summary>
	/// <returns>false if the an infinite loop is found.</returns>
	bool pawnTestBoneParentLoop(const PoseData::BonePawn pawn, ID startBone);

	/// <summary>
	/// Provides a list of all bones which have the given bone ID as aparent
	/// </summary>
	std::vector<int> pawnGetBoneChildren(const PoseData::BonePawn pawn, ID parent);

	// === Bone Operations ===

	/// <summary>
	/// Hides the decision making of parsing ints, floats and strings and determining whether each type is parsable.
	/// </summary>
	/// <param name="bone">bone to write into.</param>
	/// <param name="line_counter">which field on the parsed line is being given.</param>
	/// <param name="value">raw string contents of the CSV field to parse.</param>
	/// <returns>false if unparsable</returns>
	bool boneParseCSVField(PoseData::BoneData& bone, int line_counter, std::string value);

	/// <summary>
	/// Creates a proper deep copy.
	/// </summary>
	PoseData::BoneData boneDeepCopy(const PoseData::BoneData& source);

	// === Pawn Operations ===

	/// <summary>
	/// Inserts a new bone into the pawn without any validity checking. 
	/// </summary>
	/// <param name="pawn">target of the operation.</param>
	/// <param name="bone">bone to be added.</param>
	/// <param name="index">optional, inserts the bone at the specified location in the storage vector.</param>
	void pawnInsertBone(PoseData::BonePawn& pawn, PoseData::BoneData bone, int index = -1);

	/// <summary>
	/// Creates a proper deep copy of the provided pawn including BoneData.
	/// </summary>
	PoseData::BonePawn pawnDeepCopy(const PoseData::BonePawn& source);
}