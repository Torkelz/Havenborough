#include "ResourceTranslator.h"



ResourceTranslator::ResourceTranslator()
{

}


ResourceTranslator::~ResourceTranslator()
{

}

std::string ResourceTranslator::translate(std::string p_ResourceType, std::string p_ResourceName)
{
	std::string tempPath;


	if(p_ResourceType == "model")
	{
		tempPath = "Resources\\model\\";
		if(p_ResourceName == "DZALA")
		{
			tempPath = tempPath + "dzala.tx";
		}
		if(p_ResourceName == "MARIM")
		{
			tempPath = tempPath + "marim.tx";
		}
		if(p_ResourceName == "HOUSE1")
		{
			tempPath = tempPath + "house1.tx";
		}
	}
	else if( p_ResourceType == "texture")
	{
		tempPath = "Resources\\texture\\";
	}
	else if ( p_ResourceType == "sound")
	{
		tempPath = "Resources\\sound\\";
	}
	else
	{
		tempPath = "";
	}

	return tempPath;
}