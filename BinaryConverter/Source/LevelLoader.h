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
	struct LightStruct
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
private:
	std::vector<ModelStruct> m_LevelModelList;
	std::vector<std::pair<LightStruct,DirectionalLight>> m_LevelDirectionalLightList;
	std::vector<std::pair<LightStruct,PointLight>> m_LevelPointLightList;
	std::vector<std::pair<LightStruct,SpotLight>> m_LevelSpotLightList;
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
	 * @return LevelStruct struct.
	 */
	const std::vector<LevelLoader::ModelStruct>& getLevelModelList();

protected:
	void startReading(std::istream& p_Input);
	void readHeader(std::istream& p_Input);
	void readLightHeader(std::istream& p_Input);
	void readCheckPointHeader(std::istream& p_Input);
	void readMeshList(std::istream& p_Input);
	void readLightList(std::istream& p_Input);

private:
	void clearData();
};