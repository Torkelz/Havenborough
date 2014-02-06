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
	struct ModelHeader
	{
		std::string m_MeshName;
		int m_Animated;
		int m_Transparency;
		int m_Collidable;
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
		DirectX::XMFLOAT3 m_Translation;
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
	std::vector<ModelHeader> m_ModelHeaders;
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
	const std::vector<LevelLoader::ModelStruct>& getLevelModelList() const;

	/**
	 * Returns a vector with information about the levels directional lighting.
	 *
	 * @return DirectionalLight list.
	 */
	const std::vector<std::pair<LevelLoader::LightData, LevelLoader::DirectionalLight>>& getLevelDirectionalLightList() const;

	/**
	 * Returns a vector with information about the levels directional lighting.
	 *
	 * @return PointLight list.
	 */
	const std::vector<std::pair<LevelLoader::LightData, LevelLoader::PointLight>>& getLevelPointLightList() const;

	/**
	 * Returns a vector with information about the levels directional lighting.
	 *
	 * @return PointLight list.
	 */
	const std::vector<std::pair<LevelLoader::LightData, LevelLoader::SpotLight>>& getLevelSpotLightList() const;

	/**
	 * Returns a vector with information about the levels check points.
	 * To get information about start and end use function getCheckPointStart/getCheckPointEnd.
	 *
	 * @return CheckPointStruct list.
	 */
	const std::vector<LevelLoader::CheckPointStruct>& getLevelCheckPointList() const;

	/**
	 * Returns the start point of the map.
	 *
	 * @return DirectX::XMFLOAT3 with information about the start position.
	 */
	DirectX::XMFLOAT3 getLevelCheckPointStart() const;
	
	/**
	 * Returns the end point of the map.
	 *
	 * @return DirectX::XMFLOAT3 with information about the end position.
	 */
	DirectX::XMFLOAT3 getLevelCheckPointEnd() const;

	/**
	 * Returns information about models headers.
	 *
	 * @return a vector containing information about model headers.
	 */
	const std::vector<ModelHeader> getModelInformation() const;

protected:
	void byteToInt(std::istream& p_Input, int& p_Return);
	void byteToString(std::istream& p_Input, std::string& p_Return);
	std::string getPath(std::string p_FilePath);

	void startReading(std::istream& p_Input);
	void readModelHeaders(std::string p_FilePath);
	int readHeader(std::istream& p_Input);
	void readMeshList(std::istream& p_Input);
	void readLightList(std::istream& p_Input);
	void readCheckPointList(std::istream& p_Input);

private:
	void clearData();
};