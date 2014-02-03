#include "AnimationLoader.h"


AnimationLoader::AnimationLoader(void)
{
}


AnimationLoader::~AnimationLoader(void)
{
}

void AnimationLoader::clear()
{
	m_Joints.clear();
	m_Joints.shrink_to_fit();
}

AnimationLoader::Header AnimationLoader::readHeader(std::istream* p_Input)
{
	Header tempHeader;
	byteToString(p_Input, tempHeader.m_modelName);
	byteToInt(p_Input, tempHeader.m_numJoints);
	byteToInt(p_Input, tempHeader.m_numFrames);
	return tempHeader;
}

std::vector<Joint> AnimationLoader::readJointList(int p_NumberOfJoint, int p_NumberOfFrames, std::istream* p_Input)
{
	std::vector<Joint> readJoints;
	Joint temp;
	temp.m_JointAnimation.resize(p_NumberOfFrames);
	for(int i = 0; i < p_NumberOfJoint; i++)
	{
		byteToString(p_Input, temp.m_JointName);
		byteToInt(p_Input, temp.m_ID);
		byteToInt(p_Input, temp.m_Parent);
		p_Input->read(reinterpret_cast<char*>(&temp.m_TotalJointOffset), sizeof(DirectX::XMFLOAT4X4));
		p_Input->read(reinterpret_cast<char*>(temp.m_JointAnimation.data()), sizeof(KeyFrame) * p_NumberOfFrames);

		using namespace DirectX;
		
		XMMATRIX offset = XMLoadFloat4x4(&temp.m_TotalJointOffset);
		offset = XMMatrixTranspose(offset);
		XMStoreFloat4x4(&temp.m_TotalJointOffset, offset);

		// Precompute the total offset matrix for the joints
		if (temp.m_Parent == 0)
		{
			XMStoreFloat4x4(&temp.m_JointOffsetMatrix, XMMatrixTranspose(offset));
		}
		else
		{
			XMMATRIX parent = XMLoadFloat4x4(&readJoints[temp.m_Parent - 1].m_TotalJointOffset);
			parent = XMMatrixInverse(nullptr, parent);

			XMMATRIX sumOffset = XMMatrixMultiply(parent, offset);
			XMStoreFloat4x4(&temp.m_JointOffsetMatrix, XMMatrixTranspose(sumOffset));
		}

		readJoints.push_back(temp);
	}
	return readJoints;
}

void AnimationLoader::loadAnimationData(std::string p_FilePath)
{
	clearData();
	std::ifstream input(p_FilePath, std::istream::in | std::istream::binary);

	m_Joints = readJointList(m_FileHeader.m_numJoints, m_FileHeader.m_numFrames, &input);
}

const std::vector<Joint>& AnimationLoader::getJoints()
{
	return m_Joints;
}

void AnimationLoader::byteToString(std::istream* p_Input, std::string& p_Return)
{
	int strLength = 0;
	byteToInt(p_Input, strLength);
	std::vector<char> buffer(strLength);
	p_Input->read( buffer.data(), strLength);
	p_Return = std::string(buffer.data(), strLength);
}

void AnimationLoader::byteToInt(std::istream* p_Input, int& p_Return)
{
	p_Input->read((char*)&p_Return, sizeof(int));
}

void AnimationLoader::clearData()
{
	m_FileHeader.m_numFrames = 0;
	m_FileHeader.m_numJoints = 0;
	m_Joints.clear();
}