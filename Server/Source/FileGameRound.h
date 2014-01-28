#pragma once

#include "GameRound.h"
#include "LevelBinaryLoader.h"
#include <tinyxml2/tinyxml2.h>

class FileGameRound : public GameRound
{
private:
	std::string m_FilePath;
	LevelBinaryLoader m_FileLoader;
public:
	void setup() override;
	void setFilePath(std::string p_FilePath);

private:
	void pushVector(tinyxml2::XMLPrinter& p_Printer, const std::string& p_ElementName, const Vector3& p_Vec);
	void sendLevel() override;
};