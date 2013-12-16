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
	LevelBinaryLoader();
	~LevelBinaryLoader();
	void clear();
	bool loadBinaryFile(std::string p_FilePath);

protected:
	void byteToInt(std::istream* p_Input, int& p_Return);
	void byteToFloat(std::istream* p_Input, float& p_Return);
	void byteToString(std::istream* p_Input, std::string& p_Return);
	
	LevelBinaryLoader::Header readHeader(std::istream* p_Input);
	std::vector<LevelBinaryLoader::ModelData> readLevel(std::istream* p_Input);
	
private:
	void clearData();
};