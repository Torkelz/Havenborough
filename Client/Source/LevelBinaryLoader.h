#pragma once

#include <fstream>
#include <sstream>
#include <DirectXMath.h>
#include <memory>
#include <vector>

class LevelBinaryLoader
{
public:
	struct Header
	{
		int m_NumberOfModels;
		int m_NumberOfLights;
		int m_NumberOfCheckPoints;
	};
	struct ModelData
	{
		std::string m_MeshName;
		std::vector<DirectX::XMFLOAT3> m_Translation;
		std::vector<DirectX::XMFLOAT3> m_Rotation;
		std::vector<DirectX::XMFLOAT3> m_Scale;
	};
	
	struct LightData
	{
		DirectX::XMFLOAT3 m_Translation;
		DirectX::XMFLOAT3 m_Color;
		int m_Type;
	};
	struct DirectionalLight
	{
		float m_Intensity;
		DirectX::XMFLOAT3 m_Direction;
	};
	struct PointLight
	{
		float m_Intensity;
	};
	struct SpotLight
	{
		float m_Intensity;
		DirectX::XMFLOAT3 m_Direction;
		float m_ConeAngle;
		float m_PenumbraAngle;
	};
	struct CheckPointStruct
	{
		int m_Number;
		DirectX::XMFLOAT3 m_Transaltion;
	};
private: 
	std::vector<LevelBinaryLoader::ModelData> m_LevelData;
	std::vector<std::pair<LevelBinaryLoader::LightData, LevelBinaryLoader::DirectionalLight>> m_LevelDirectionalLightList;
	std::vector<std::pair<LevelBinaryLoader::LightData, LevelBinaryLoader::PointLight>> m_LevelPointLightList;
	std::vector<std::pair<LevelBinaryLoader::LightData, LevelBinaryLoader::SpotLight>> m_LevelSpotLightList;
	std::vector<LevelBinaryLoader::CheckPointStruct> m_LevelCheckPointList;
	DirectX::XMFLOAT3 m_LevelCheckPointStart;
	DirectX::XMFLOAT3 m_LevelCheckPointEnd;
	Header m_Header;
public:
	/**
	 * Constructor.
	 */
	LevelBinaryLoader();

	/**
	 * Deconstructor.
	 */
	~LevelBinaryLoader();

	/**
	 * Clears all vectors and de-allocate memory.
	 */
	void clear();

	/**
	 * Reads a binary level file of the .btxl format. 
	 * The file is compressed so that all information about the level and models comes instanced.
	 * 
	 * @param p_FilePath is the complete path to the source file.
	 * @return bool success = true | fail = false
	 */
	bool loadBinaryFile(std::string p_FilePath);

	/**
	 * Returns information about the models in the level.
	 *
	 * @return a vector of ModelData struct. 
	 */
	const std::vector<LevelBinaryLoader::ModelData>& getModelData();

	/**
	 * Returns information about the directional lights in the level.
	 *
	 * @return a vector of LightData struct paired with DirectionalLight struct. 
	 */
	const std::vector<std::pair<LevelBinaryLoader::LightData, LevelBinaryLoader::DirectionalLight>>& getDirectionalLightData();

	/**
	 * Returns information about the point lights in the level.
	 *
	 * @return a vector of LightData struct paired with PointLight struct. 
	 */
	const std::vector<std::pair<LevelBinaryLoader::LightData, LevelBinaryLoader::PointLight>>& getPointLightData();

	/**
	 * Returns information about the spot lights in the level.
	 *
	 * @return a vector of LightData struct paired with SpotLight struct.  
	 */
	const std::vector<std::pair<LevelBinaryLoader::LightData, LevelBinaryLoader::SpotLight>>& getSpotLightData();

	/**
	 * Returns information about the start checkpoint in the level.
	 *
	 * @return a DirectX::XMFLOAT3 as a position. 
	 */
	DirectX::XMFLOAT3 getCheckPointStart();

	/**
	 * Returns information about the end checkpoint in the level.
	 *
	 * @return a DirectX::XMFLOAT3 as a position. 
	 */
	DirectX::XMFLOAT3 getCheckPointEnd();

	/**
	 * Returns information about the checkpoints in the level.
	 *
	 * @return a vector of CheckPoint struct. 
	 */
	const std::vector<LevelBinaryLoader::CheckPointStruct>& getCheckPointData();

protected:
	void byteToInt(std::istream* p_Input, int& p_Return);
	void byteToString(std::istream* p_Input, std::string& p_Return);
	
	LevelBinaryLoader::Header readHeader(std::istream* p_Input);
	std::vector<LevelBinaryLoader::ModelData> readLevel(std::istream* p_Input);
	void readLevelLighting(std::istream* p_Input);
	void readLevelCheckPoint(std::istream* p_Input);

private:
	void clearData();
};