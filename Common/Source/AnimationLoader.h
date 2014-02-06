#pragma once
#pragma warning(disable : 4996)
#include "AnimationData.h"
#include "Joint.h"

#include <fstream>
#include <vector>
#include <string>

class AnimationLoader
{
public:
	struct Header
	{
		std::string m_ModelName;
		int m_NumJoints;
		int m_NumFrames;
	};

private:
	std::vector<Joint> m_Joints;
	Header m_FileHeader;

	struct LoadedAnimationData
	{
		std::string resourceName;
		std::string filename;
		AnimationData::ptr animationData;
	};

	std::vector<LoadedAnimationData> m_LoadedAnimations;

public:
	/**
	 * Constructor.
	 */
	AnimationLoader(void);

	/**
	 * Destructor.
	 */
	~AnimationLoader(void);

	/**
	 * Use this function to release the memory in loader vectors.
	 */
	void clear();

	bool loadAnimationDataResource(const char* p_resourceName, const char* p_FilePath);
	bool releaseAnimationData(const char* p_FilePath);
	AnimationData::ptr getAnimationData(const char* p_ResourceName) const;

protected:
	void byteToInt(std::istream* p_Input, int& p_Return);
	void byteToString(std::istream* p_Input, std::string& p_Return);

	Header readHeader(std::istream* p_Input);
	std::vector<Joint> readJointList(int p_NumberOfJoint, int p_NumberOfFrames, std::istream* p_Input);
private:
	void clearData();

	/**
	 * Opens a binary file then reads the information stream and saves the information in vectors of structs.
	 * 
	 * @param p_FilePath, the absolute path to the source file.
	 */
	void loadAnimationData(std::string p_FilePath);

	/**
	 * Returns a vector of joints for the animation. 
	 ** This will be empty if the source file does not contain any animation.
	 *
	 * @returns a vector of the struct Joint.
	 */
	const std::vector<Joint>& getJoints();
};

