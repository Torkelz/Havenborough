#pragma once

#include <fstream>
#include <sstream>
#include <DirectXMath.h>
#include <memory>
#include <vector>

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
		int m_numMaterialBuffer;
		int m_numJoints;
		int m_numFrames;
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

protected:	
	void byteToInt(std::istream* p_Input, int& p_Return);
	void byteToString(std::istream* p_Input, std::string& p_Return);

	BVLoader::Header readHeader(std::istream* p_Input);
	std::vector<BVLoader::BoundingVolume> readBoundingVolume(int p_NumberOfVertex, std::istream* p_Input);

private:
	void clearData();
};