#pragma once

#include <sstream>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "LevelLoader.h"

class LevelConverter
{
private:
	struct ModelData
	{
		std::string m_MeshName;
		std::vector<DirectX::XMFLOAT3> m_Translation;
		std::vector<DirectX::XMFLOAT3> m_Rotation;
		std::vector<DirectX::XMFLOAT3> m_Scale;
	};

	const std::vector<LevelLoader::LevelStruct>* m_LevelData;
	LevelLoader::LevelHeader m_Header;
public:
	LevelConverter();
	~LevelConverter();
	bool writeFile(std::string p_FileName);
	void setLevelHead(LevelLoader::LevelHeader p_Header);
	void setLevelModelList(const std::vector<LevelLoader::LevelStruct>* p_LevelModelList);
	void clear();
protected:
	void stringToByte(std::string p_String, std::ostream* p_Output);
	void intToByte(int p_Int, std::ostream* p_Output);
	void floatToByte(float p_Float, std::ostream* p_Output);

	void createHeader(std::ostream* p_Output);
	void createLevel(std::ostream* p_Output);
};