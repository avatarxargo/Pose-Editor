/// <title>Pose Data</title>
/// <desc>
///		Represents the core data structure.
///		File I/O and other utility included in PoseDataUtil to keep this file simple.
/// </desc>
/// <date>9/4/2021</date>
/// <version>1.0</version>
/// <author>David Hrusa</author>
/// <email>hrusadav@gmail.com</email>

#pragma once

#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

typedef int ID;

namespace PoseData {

	/// <summary>
	/// Data corresponding to an individual bone. Additionally, keeps track of euler angles for the use in View.
	/// </summary>
	struct BoneData {
		//[BoneID] [ParentBoneID] [Quaternion X] [Quaternion Y] [Quaternion Z] [Quaternion W] [Name]
		ID id;
		ID parent;
		/// <summary>Should be kept synchronized with eulerRotation.</summary>
		glm::quat quaternion = glm::quat(0, 0, 0, 1);
		/// <summary>Euler angles of rotation (in degrees) Should be kept synchronized with quaternion.</summary>
		glm::vec3 eulerRotation = glm::vec3(0, 0, 0); //generated value
		std::string displayName;
	};

	/// <summary>
	/// BonePawn is mostly just a vector of bones. The idea is that you can include meta information such as the original file path
	/// in case your controller can handle multiple editor windows, etc.
	/// </summary>
	struct BonePawn {
		std::vector<BoneData> bones;
		/// <summary>
		/// The full path this pawn was loaded from. (including file name) Empty if generated or failed.
		/// </summary>
		std::string originalFilePath = "";
		/// <summary>
		/// The filename from originalFilePath.
		/// </summary>
		std::string originalFileName = "";
		/// <summary>
		/// true if the file was opened properly. This is used to determine whether originalFilePath would be a valid path for saving.
		/// </summary>
		bool loaded = false;
		/// <summary>
		/// false if any change occured on this pawn which has not yet been recorded to a permanent file.
		/// </summary>
		bool saved = false;
	};

}