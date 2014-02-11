#include "AnimationLoader.h"

#include "AnimationClipLoader.h"

#include <algorithm>
#include <boost/filesystem.hpp>

AnimationLoader::AnimationLoader(void)
{
}


AnimationLoader::~AnimationLoader(void)
{
	clear();
}

void AnimationLoader::clear()
{
	m_Joints.clear();
	m_Joints.shrink_to_fit();
}

AnimationLoader::Header AnimationLoader::readHeader(std::istream* p_Input)
{
	Header tempHeader;
	byteToString(p_Input, tempHeader.m_ModelName);
	byteToInt(p_Input, tempHeader.m_NumJoints);
	byteToInt(p_Input, tempHeader.m_NumFrames);
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
	m_FileHeader = readHeader(&input);
	m_Joints = readJointList(m_FileHeader.m_NumJoints, m_FileHeader.m_NumFrames, &input);

	input.close();
}

bool AnimationLoader::loadAnimationDataResource(const char* p_ResourceName, const char* p_FilePath)
{
	AnimationData::ptr data(new AnimationData);
	loadAnimationData(p_FilePath);
	data->joints = getJoints();
	clearData();

	LoadedAnimationData loadedData;
	loadedData.animationData = data;
	loadedData.filename = p_FilePath;
	loadedData.resourceName = p_ResourceName;

	boost::filesystem::path filepath(p_FilePath);
	boost::filesystem::path mlxPath(filepath.replace_extension("mlx"));

	data->animationClips = MattiasLucaseXtremeLoader::loadAnimationClip(mlxPath.string());
	data->ikGroups = MattiasLucaseXtremeLoader::loadIKGroup(mlxPath.string());
	data->animationPath = MattiasLucaseXtremeLoader::loadAnimationPath(mlxPath.string());

	m_LoadedAnimations.push_back(loadedData);

	return true;
}

bool AnimationLoader::releaseAnimationData(const char* p_ResourceName)
{
	auto it = std::remove_if(m_LoadedAnimations.begin(), m_LoadedAnimations.end(),
		[p_ResourceName] (const LoadedAnimationData& p_Data)
	{
		return p_Data.resourceName == p_ResourceName;
	});

	m_LoadedAnimations.erase(it, m_LoadedAnimations.end());

	return true;
}

AnimationData::ptr AnimationLoader::getAnimationData(const char* p_ResourceName) const
{
	auto it = std::find_if(m_LoadedAnimations.begin(), m_LoadedAnimations.end(),
		[p_ResourceName] (const LoadedAnimationData& p_Data)
	{
		return p_Data.resourceName == p_ResourceName;
	});

	if (it == m_LoadedAnimations.end())
	{
		return AnimationData::ptr();
	}
	else
	{
		return it->animationData;
	}
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
	m_FileHeader.m_ModelName = "";
	m_FileHeader.m_NumFrames = 0;
	m_FileHeader.m_NumJoints = 0;
	m_Joints.clear();
}