#pragma once
#include <fstream>
#include <DirectXMath.h>
#include <vector>
#include <string>

class BVLoader
{
public:
	struct BoundingVolume
	{
		DirectX::XMFLOAT4 m_Postition;
	};

	struct Header
	{
		std::string m_modelName;
		int m_numMaterial;
		int m_numVertex;
		int m_numFaces;
	};

private:	
	std::vector<BoundingVolume> m_BoundingVolume;
	Header m_FileHeader;
public:
	BVLoader(void);
	~BVLoader(void);

	/**
	 * Opens a binary file then reads the information stream and saves the information in vectors of structs.
	 * 
	 * @param p_FilePath, the absolut path to the source file.
	 */
	bool loadBinaryFile(std::string p_FilePath);
	
	/**
	 * Use this function to de-allocate the memory in loader vectors.
	 */
	void clear();

	/**
	 * Returns all header information about the file. 
	 *
	 * @return LevelHeader struct.
	 */
	BVLoader::Header getLevelHeader();

	/**
	 * Returns information about the boundingvolume for a specific model.
	 *
	 * @returns a vector of the struct BoundingVolume.
	 */
	const std::vector<BVLoader::BoundingVolume>& getBoundingVolumes();
	//void byteToInt(std::istream* p_Input, int& p_Return);
	//void byteToString(std::istream* p_Input, std::string& p_Return);

	void readHeader(std::istream* p_Input);
	void readBoundingVolume(std::istream* p_Input);

private:
	void clearData();
};