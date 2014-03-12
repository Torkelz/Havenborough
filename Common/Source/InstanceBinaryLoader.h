#pragma once
#include <fstream>
#include <DirectXMath.h>
#include <vector>

class InstanceBinaryLoader
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
		bool m_Animated;
		bool m_Transparent;
		bool m_CollideAble;
		std::vector<DirectX::XMFLOAT3> m_Translation;
		std::vector<DirectX::XMFLOAT3> m_Rotation;
		std::vector<DirectX::XMFLOAT3> m_Scale;
	};
	struct DirectionalLight
	{
		DirectX::XMFLOAT3 m_Translation;
		DirectX::XMFLOAT3 m_Color;
		int m_Type;
		float m_Intensity;
		DirectX::XMFLOAT3 m_Direction;
	};
	struct PointLight
	{
		DirectX::XMFLOAT3 m_Translation;
		DirectX::XMFLOAT3 m_Color;
		int m_Type;
		float m_Intensity;
	};
	struct SpotLight
	{
		DirectX::XMFLOAT3 m_Translation;
		DirectX::XMFLOAT3 m_Color;
		int m_Type;
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
	std::vector<InstanceBinaryLoader::ModelData> m_LevelData;
	std::vector<InstanceBinaryLoader::DirectionalLight> m_LevelDirectionalLightList;
	std::vector<InstanceBinaryLoader::PointLight> m_LevelPointLightList;
	std::vector<InstanceBinaryLoader::SpotLight> m_LevelSpotLightList;
	std::vector<std::vector<InstanceBinaryLoader::CheckPointStruct>> m_LevelCheckPointList;
	DirectX::XMFLOAT3 m_LevelCheckPointStart;
	DirectX::XMFLOAT3 m_LevelCheckPointEnd;
	Header m_Header;

	std::ifstream m_Input;
public:
	/**
	 * Constructor.
	 */
	InstanceBinaryLoader();

	/**
	 * Deconstructor.
	 */
	~InstanceBinaryLoader();

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
	void loadBinaryFile(std::string p_FilePath);

	/**
	 * Reads information from a stringstream and converts it to vectors of information.
	 * 
	 * @param p_Input, input is a istream of compressed information.
	 */
	void readStreamData(std::istream& p_Input);

	/**
	 * Returns information about the models in the level.
	 *
	 * @return a vector of ModelData struct. 
	 */
	const std::vector<InstanceBinaryLoader::ModelData>& getModelData() const;

	/**
	 * Returns information about the directional lights in the level.
	 *
	 * @return a vector of LightData struct paired with DirectionalLight struct. 
	 */
	const std::vector<InstanceBinaryLoader::DirectionalLight>& getDirectionalLightData() const;

	/**
	 * Returns information about the point lights in the level.
	 *
	 * @return a vector of LightData struct paired with PointLight struct. 
	 */
	const std::vector<InstanceBinaryLoader::PointLight>& getPointLightData() const;

	/**
	 * Returns information about the spot lights in the level.
	 *
	 * @return a vector of LightData struct paired with SpotLight struct.  
	 */
	const std::vector<InstanceBinaryLoader::SpotLight>& getSpotLightData() const;

	/**
	 * Returns information about the start checkpoint in the level.
	 *
	 * @return a DirectX::XMFLOAT3 as a position. 
	 */
	DirectX::XMFLOAT3 getCheckPointStart() const;

	/**
	 * Returns information about the end checkpoint in the level.
	 *
	 * @return a DirectX::XMFLOAT3 as a position. 
	 */
	DirectX::XMFLOAT3 getCheckPointEnd() const;

	/**
	 * Returns information about the checkpoints in the level.
	 *
	 * @return a vector of CheckPoint struct. 
	 */
	const std::vector<std::vector<InstanceBinaryLoader::CheckPointStruct>>& getCheckPointData() const;

	/**
	 * Get the stream information about the file.
	 *
	 * @return a const char pointer with binary information, dont forget to get the size of the stream.
	 */
	std::string getDataStream();

protected:
	void byteToInt(std::istream& p_Input, int& p_Return);
	void byteToString(std::istream& p_Input, std::string& p_Return);
	void byteToBool(std::istream& p_Input, bool& p_Return);
	
	InstanceBinaryLoader::Header readHeader(std::istream& p_Input);
	std::vector<InstanceBinaryLoader::ModelData> readLevel(std::istream& p_Input);
	void readLevelLighting(std::istream& p_Input);
	void readLevelCheckPoint(std::istream& p_Input);

private:
	void clearData();
};