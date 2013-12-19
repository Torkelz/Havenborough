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
	};
	struct ModelData
	{
		std::string m_MeshName;
		std::vector<DirectX::XMFLOAT3> m_Translation;
		std::vector<DirectX::XMFLOAT3> m_Rotation;
		std::vector<DirectX::XMFLOAT3> m_Scale;
	};
private: 
	std::vector<LevelBinaryLoader::ModelData> m_LevelData;
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
	const std::vector<LevelBinaryLoader::ModelData>& LevelBinaryLoader::getModelData();
protected:
	void byteToInt(std::istream* p_Input, int& p_Return);
	void byteToString(std::istream* p_Input, std::string& p_Return);
	
	LevelBinaryLoader::Header readHeader(std::istream* p_Input);
	std::vector<LevelBinaryLoader::ModelData> readLevel(std::istream* p_Input);
	
private:
	void clearData();
};