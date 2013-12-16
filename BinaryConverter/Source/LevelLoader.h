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
	};
	struct LevelStruct
	{
		std::string m_MeshName;
		DirectX::XMFLOAT3 m_Translation;
		DirectX::XMFLOAT3 m_Rotation;
		DirectX::XMFLOAT3 m_Scale;
	};
private:
	std::vector<LevelStruct> m_LevelModelList;
	std::stringstream m_Stringstream;
	LevelHeader m_Header;
public:
	LevelLoader();
	~LevelLoader();
	bool loadLevel(std::string p_FilePath);
	void clear();
	LevelLoader::LevelHeader getLevelHeader();
	const std::vector<LevelLoader::LevelStruct>& getLevelModelList();
protected:
	void startReading(std::istream& p_Input);
	void readHeader(std::istream& p_Input);
	void readMeshList(std::istream& p_Input);

private:
	void clearData();
};