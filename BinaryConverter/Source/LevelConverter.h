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
	int m_LevelDataSize;
public:
	/**
	 * Constructor
	 */
	LevelConverter();
	
	/**
	 * Deconstructor
	 */
	~LevelConverter();

	/**
	 * Call this function after setLevelHead and setLevelModelList
	 * or the file will be empty. 
	 *
	 * @param p_FileName, is the name on the file you whant to create. 
	 */
	bool writeFile(std::string p_FileName);

	/**
	 * Give information about the header. If this is empty the writeFile function will return false.
	 *
	 * @param p_Header, send in information about a header object.
	 */
	void setLevelHead(LevelLoader::LevelHeader p_Header);

	/**
	 * Sets information about the models used in the level. If this information is not set the file will be empty.
	 *
	 * @param p_LevelModelList, is a vector with models that is unsorted. It expects it to be in .txl format.
	 */
	void setLevelModelList(const std::vector<LevelLoader::LevelStruct>* p_LevelModelList);

	/**
	 * Clears the writer.
	 */
	void clear();
protected:
	void stringToByte(std::string p_String, std::ostream* p_Output);
	void intToByte(int p_Int, std::ostream* p_Output);

	void createHeader(std::ostream* p_Output);
	void createLevel(std::ostream* p_Output);
};