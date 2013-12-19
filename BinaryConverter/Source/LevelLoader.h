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
	const std::vector<LevelLoader::LevelStruct>& getLevelModelList();

protected:
	void startReading(std::istream& p_Input);
	void readHeader(std::istream& p_Input);
	void readMeshList(std::istream& p_Input);

private:
	void clearData();
};