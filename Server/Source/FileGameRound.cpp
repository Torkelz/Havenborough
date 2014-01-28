#include "FileGameRound.h"

#include <Logger.h>

void FileGameRound::setup()
{
	m_FileLoader.loadBinaryFile(m_FilePath);
}

void FileGameRound::setFilePath(std::string p_Filepath)
{
	m_FilePath = p_Filepath;
}