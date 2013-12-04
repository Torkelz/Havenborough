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
		if(p_ResourceName == "Dzala")
		{
			tempPath = tempPath + "character\\Dzala\\dzala.obj";
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