#pragma once
#pragma warning(disable : 4996)
#include "..\..\Graphics\Source\Joint.h"
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

	/**
	 * Returns a vector of joints for the animation. 
	 ** This will be empty if the source file does not contain any animation.
	 *
	 * @returns a vector of the struct Joint.
	 */
	const std::vector<Joint>& getJoints();

	/**
	 * Opens a binary file then reads the information stream and saves the information in vectors of structs.
	 * 
	 * @param p_FilePath, the absolute path to the source file.
	 */
	void loadAnimationData(std::string p_FilePath);

protected:
	void byteToInt(std::istream* p_Input, int& p_Return);
	void byteToString(std::istream* p_Input, std::string& p_Return);

	Header readHeader(std::istream* p_Input);
	std::vector<Joint> readJointList(int p_NumberOfJoint, int p_NumberOfFrames, std::istream* p_Input);
private:
	void clearData();
};

