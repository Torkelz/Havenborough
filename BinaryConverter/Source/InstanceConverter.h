#pragma once

#include <sstream>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "InstanceLoader.h"

class InstanceConverter
{
private:
	struct ModelData
	{
		std::string m_MeshName;
		std::vector<DirectX::XMFLOAT3> m_Translation;
		std::vector<DirectX::XMFLOAT3> m_Rotation;
		std::vector<DirectX::XMFLOAT3> m_Scale;
	};
	struct Effects
	{
		std::string m_EffectName;
		std::vector<DirectX::XMFLOAT3> m_Translation;
		std::vector<DirectX::XMFLOAT3> m_Rotation;
	};
	DirectX::XMFLOAT3 m_LevelCheckPointStart;
	DirectX::XMFLOAT3 m_LevelCheckPointEnd;
	const std::vector<InstanceLoader::CheckPointStruct>* m_LevelCheckPointList;
	const std::vector<InstanceLoader::EffectStruct>* m_EffectList;
	const std::vector<InstanceLoader::ModelStruct>* m_Data;
	const std::vector<std::pair<InstanceLoader::LightData, InstanceLoader::DirectionalLight>>* m_LevelDirectionalLightList;
	const std::vector<std::pair<InstanceLoader::LightData, InstanceLoader::PointLight>>* m_LevelPointLightList;
	const std::vector<std::pair<InstanceLoader::LightData, InstanceLoader::SpotLight>>* m_LevelSpotLightList;
	const std::vector<InstanceLoader::ModelHeader>* m_ModelInformation;
	InstanceLoader::LevelHeader m_Header;
	int m_LevelDataSize, m_EffectDataSize;
public:
	/**
	 * Constructor
	 */
	InstanceConverter();
	
	/**
	 * Deconstructor
	 */
	~InstanceConverter();

	/**
	 * Call this function after setLevelHead and setLevelModelList
	 * or the file will be empty. 
	 *
	 * @param p_FileName, is the name on the file you whant to create. 
	 */
	bool writeFile(std::string p_FileName);

	/**
	 * Give information about the header. If this is empty the writeFile function will return false.
	 *
	 * @param p_Header, send in information about a header object.
	 */
	void setLevelHead(InstanceLoader::LevelHeader p_Header);

	/**
	 * Sets information about the models used in the level. If this information is not set the file will be empty.
	 *
	 * @param p_LevelModelList, is a vector with models that is unsorted. It expects it to be in .txl format.
	 */
	void setModelList(const std::vector<InstanceLoader::ModelStruct>* p_ModelList);

	/**
	 * Sets information about the directional lighting used in the level.
	 *
	 * @param p_LevelDirectionalLightList, is a vector with directional lights. It expects it to be in .txl format.
	 */
	void setLevelDirectionalLightList(const std::vector<std::pair<InstanceLoader::LightData, InstanceLoader::DirectionalLight>>* p_LevelDirectionalLightList);

	/**
	 * Sets information about the point lighting used in the level.
	 *
	 * @param p_LevelPointLightList, is a vector with point lights. It expects it to be in .txl format.
	 */
	void setLevelPointLightList(const std::vector<std::pair<InstanceLoader::LightData, InstanceLoader::PointLight>>* p_LevelPointLightList);

	/**
	 * Sets information about the spot lighting used in the level.
	 *
	 * @param p_LevelSpotLightList, is a vector with spot lights. It expects it to be in .txl format.
	 */
	void setLevelSpotLightList(const std::vector<std::pair<InstanceLoader::LightData, InstanceLoader::SpotLight>>* p_LevelSpotLightList);

	/**
	 * Sets the list of checkpoints for the track.
	 *
	 * @param p_LevelCheckPointList, is a list of checkpoints structs that contains positions.
	 */
	void setLevelCheckPointList(const std::vector<InstanceLoader::CheckPointStruct>* p_LevelCheckPointList);

	/**
	 * Set the checkpoint start value.
	 *
	 * @param p_LevelCheckPointStart, is a DirectX::XMFLOAT3.
	 */
	void setLevelCheckPointStart(DirectX::XMFLOAT3 p_LevelCheckPointStart);

	/**
	 * Set the checkpoint end value.
	 *
	 * @param p_LevelCheckPointEnd, is a DirectX::XMFLOAT3.
	 */
	void setLevelCheckPointEnd(DirectX::XMFLOAT3 p_LevelCheckPointEnd);

	/** 
	 * Set information about the models header.
	 *
	 * @param p_ModelInformation, is a vector of InstanceLoader ModelHeader
	 */
	void setModelInformation(const std::vector<InstanceLoader::ModelHeader>* p_ModelInformation);

	/**
	 * Sets the list of effects for the level.
	 *
	 * @param p_EffectList, is a list of effects structs that contains positions and rotations.
	 */
	void InstanceConverter::setEffectList(const std::vector<InstanceLoader::EffectStruct>* p_EffectList);

	/**
	 * Clears the writer.
	 */
	void clear();
protected:
	void stringToByte(std::string p_String, std::ostream* p_Output);
	void intToByte(int p_Int, std::ostream* p_Output);

	void createHeader(std::ostream* p_Output);
	void createLevel(std::ostream* p_Output);
	void createLighting(std::ostream* p_Output);
	void createCheckPoints(std::ostream* p_Output);
	void InstanceConverter::createEffect(std::ostream* p_Output);
};