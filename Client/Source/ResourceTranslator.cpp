#include "ResourceTranslator.h"
#include "ClientExceptions.h"

#include <vector>

ResourceTranslator::ResourceTranslator()
{

}


ResourceTranslator::~ResourceTranslator()
{

}

std::string ResourceTranslator::translate(std::string p_ResourceType, std::string p_ResourceName)
{
	static const std::pair<std::string, std::string> modelMap[] =
	{
		std::make_pair("DZALA", "assets/models/character_witch.btx"),
		std::make_pair("MARIM", "assets/models/marim.btx"),
		std::make_pair("HOUSE1", "assets/models/house1.btx"),
		std::make_pair("BOX", "assets/models/Sample135.btx"),
		std::make_pair("SKYBOX", "assets/models/SkyBox.btx"),
	};
	static const size_t numModels = sizeof(modelMap) / sizeof(modelMap[0]);

	static const std::pair<std::string, std::string> textureMap[] =
	{
		std::make_pair("TEXTURE_NOT_FOUND", "assets/textures/TextureNotFound.png"),
	};
	static const size_t numTextures = sizeof(textureMap) / sizeof(textureMap[0]);

	static const std::pair<std::string, std::string> animatedModelMap[] =
	{
		std::make_pair("Test", "assets/IKtest.btx"),
	};
	static const size_t numAnimatedModels = sizeof(animatedModelMap) / sizeof(animatedModelMap[0]);

	static const std::pair<std::string, std::vector<const std::pair<std::string, std::string>>> resourceMaps[] =
	{
		std::make_pair("model", std::vector<const std::pair<std::string, std::string>>(modelMap, modelMap + numModels)),
		std::make_pair("texture", std::vector<const std::pair<std::string, std::string>>(textureMap, textureMap + numTextures)),
		std::make_pair("animatedModel", std::vector<const std::pair<std::string, std::string>>(animatedModelMap, animatedModelMap + numAnimatedModels)),
	};

	for (const auto& map : resourceMaps)
	{
		if (p_ResourceType == map.first)
		{
			for (const auto& m : map.second)
			{
				if (m.first == p_ResourceName)
				{
					return m.second;
				}
			}
		}
	}
	
	throw ResourceManagerException("Unknown resource: '" + p_ResourceType + ":" + p_ResourceName + "'", __LINE__, __FILE__);
}