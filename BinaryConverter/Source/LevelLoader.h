#pragma once

#include <fstream>
#include <sstream>
#include <DirectXMath.h>
#include <memory>
#include <vector>

class LevelLoader
{
public:
	struct LevelHeader
	{
		int m_NumberOfModels;
		int m_NumberOfLights;
		int m_NumberOfCheckPoints;
	};
	struct ModelStruct
	{
		std::string m_MeshName;
		DirectX::XMFLOAT3 m_Translation;
		DirectX::XMFLOAT3 m_Rotation;
		DirectX::XMFLOAT3 m_Scale;
	};
	struct LightData
	{
		std::string m_LightName;
		DirectX::XMFLOAT3 m_Translation;
		DirectX::XMFLOAT3 m_Color;
		int m_Type;
	};
	struct DirectionalLight
	{
		int m_Intensity;
		DirectX::XMFLOAT3 m_Direction;
	};
	struct PointLight
	{
		int m_Intensity;
	};
	struct SpotLight
	{
		int m_Intensity;
		DirectX::XMFLOAT3 m_Direction;
		int m_ConeAngle;
	};
	struct CheckPointStruct
	{
		int m_Number;
		DirectX::XMFLOAT3 m_Transaltion;
	};
private:
	DirectX::XMFLOAT3 m_CheckPointStart;
	DirectX::XMFLOAT3 m_CheckPointEnd;
	std::vector<CheckPointStruct> m_LevelCheckPointList;
	std::vector<ModelStruct> m_LevelModelList;
	std::vector<std::pair<LightData,DirectionalLight>> m_LevelDirectionalLightList;
	std::vector<std::pair<LightData,PointLight>> m_LevelPointLightList;
	std::vector<std::pair<LightData,SpotLight>> m_LevelSpotLightList;
	std::stringstream m_Stringstream;
	LevelHeader m_Header;
public:
	/**
	 * Constructor.
	 */
	LevelLoader();

	/**
	 * Deconstructor, calls clear(). 
	 */
	~LevelLoader();

	/**
	 * Opens a .txl format file. If file not found this returns false.
	 * 
	 * @param p_FilePath, is the complete path to the file.
	 */
	bool loadLevel(std::string p_FilePath);

	/**
	 * Clears out all information about the previous file. This is also done in the deconstructor.
	 */
	void clear();

	/**
	 * Returns all header information about the file. 
	 *
	 * @return LevelHeader struct.
	 */
	LevelLoader::LevelHeader getLevelHeader();

	/**
	 * Returns a vector with information about the levels models.
	 *
	 * @return ModelStruct list.
	 */
	const std::vector<LevelLoader::ModelStruct>& getLevelModelList();

	/**
	 * Returns a vector with information about the levels directional lighting.
	 *
	 * @return DirectionalLight list.
	 */
	const std::vector<LevelLoader::DirectionalLight> getLevelDirectionalLightList();

	/**
	 * Returns a vector with information about the levels directional lighting.
	 *
	 * @return PointLight list.
	 */
	const std::vector<LevelLoader::PointLight> getLevelPointLightList();

	/**
	 * Returns a vector with information about the levels directional lighting.
	 *
	 * @return PointLight list.
	 */
	const std::vector<LevelLoader::SpotLight> getLevelSpotLightList();

	/**
	 * Returns a vector with information about the levels check points.
	 * To get information about start and end use function getCheckPointStart/getCheckPointEnd.
	 *
	 * @return CheckPointStruct list.
	 */
	const std::vector<LevelLoader::CheckPointStruct>& getLevelCheckPointList();

	/**
	 * Returns the start point of the map.
	 *
	 * @return DirectX::XMFLOAT3 with information about the start position.
	 */
	DirectX::XMFLOAT3 getLevelCheckPointStart();
	
	/**
	 * Returns the end point of the map.
	 *
	 * @return DirectX::XMFLOAT3 with information about the end position.
	 */
	DirectX::XMFLOAT3 getLevelCheckPointEnd();

protected:
	void startReading(std::istream& p_Input);
	void readHeader(std::istream& p_Input);
	void readLightHeader(std::istream& p_Input);
	void readCheckPointHeader(std::istream& p_Input);
	void readMeshList(std::istream& p_Input);
	void readLightList(std::istream& p_Input);
	void readCheckPointList(std::istream& p_Input);

private:
	void clearData();
};