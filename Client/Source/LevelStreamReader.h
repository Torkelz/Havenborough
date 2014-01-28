#pragma once

#include <fstream>
#include <sstream>
#include <DirectXMath.h>
#include <memory>
#include <vector>

class LevelStreamReader
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
	std::vector<LevelStreamReader::ModelData> m_LevelData;
	std::vector<LevelStreamReader::DirectionalLight> m_LevelDirectionalLightList;
	std::vector<LevelStreamReader::PointLight> m_LevelPointLightList;
	std::vector<LevelStreamReader::SpotLight> m_LevelSpotLightList;
	std::vector<LevelStreamReader::CheckPointStruct> m_LevelCheckPointList;
	DirectX::XMFLOAT3 m_LevelCheckPointStart;
	DirectX::XMFLOAT3 m_LevelCheckPointEnd;
	Header m_Header;
public:
	/**
	 * Constructor.
	 */
	LevelStreamReader();

	/**
	 * Deconstructor.
	 */
	~LevelStreamReader();

	/**
	 * Clears all vectors and de-allocate memory.
	 */
	void clear();

	/**
	 * Reads information from a stringstream and converts it to vectors of information.
	 * 
	 * @param p_Input, input is a istream of compressed information.
	 */
	void readStreamData(std::istream* p_Input);

	/**
	 * Returns information about the models in the level.
	 *
	 * @return a vector of ModelData struct. 
	 */
	const std::vector<LevelStreamReader::ModelData>& getModelData() const;

	/**
	 * Returns information about the directional lights in the level.
	 *
	 * @return a vector of LightData struct paired with DirectionalLight struct. 
	 */
	const std::vector<LevelStreamReader::DirectionalLight>& getDirectionalLightData() const;

	/**
	 * Returns information about the point lights in the level.
	 *
	 * @return a vector of LightData struct paired with PointLight struct. 
	 */
	const std::vector<LevelStreamReader::PointLight>& getPointLightData() const;

	/**
	 * Returns information about the spot lights in the level.
	 *
	 * @return a vector of LightData struct paired with SpotLight struct.  
	 */
	const std::vector<LevelStreamReader::SpotLight>& getSpotLightData() const;

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
	const std::vector<LevelStreamReader::CheckPointStruct>& getCheckPointData() const;

protected:
	void byteToInt(std::istream* p_Input, int& p_Return);
	void byteToString(std::istream* p_Input, std::string& p_Return);
	
	LevelStreamReader::Header readHeader(std::istream* p_Input);
	std::vector<LevelStreamReader::ModelData> readLevel(std::istream* p_Input);
	void readLevelLighting(std::istream* p_Input);
	void readLevelCheckPoint(std::istream* p_Input);

private:
	void clearData();
};