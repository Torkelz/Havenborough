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
		std::make_pair("RedBlueCrate", "assets/models/RedBlueCrate.btx"),
		std::make_pair("Test", "assets/IKtest.btx"),
		std::make_pair("RedCrate", "assets/models/RedCrate.btx"),
		std::make_pair("OrangeCrate", "assets/models/OrangeCrate.btx"),
		std::make_pair("LightBlueCrate", "assets/models/LightBlueCrate.btx"),
		std::make_pair("GreenCrate", "assets/models/GreenCrate.btx"),
		std::make_pair("BrownCrate", "assets/models/BrownCrate.btx"),
	};
	static const size_t numModels = sizeof(modelMap) / sizeof(modelMap[0]);

	static const std::pair<std::string, std::string> boundingMap[] =
	{
		std::make_pair("HOUSE1", "assets/volumes/house1.btx"),
		std::make_pair("BOX", "assets/volumes/Sample135.btx"),
		std::make_pair("SKYBOX", "assets/volumes/SkyBox.btx"),
		std::make_pair("RedBlueCrate", "assets/volumes/RedBlueCrate.btx"),
		std::make_pair("RedCrate", "assets/volumes/RedCrate.btx"),
		std::make_pair("OrangeCrate", "assets/volumes/OrangeCrate.btx"),
		std::make_pair("LightBlueCrate", "assets/volumes/LightBlueCrate.btx"),
		std::make_pair("GreenCrate", "assets/volumes/GreenCrate.btx"),
		std::make_pair("BrownCrate", "assets/volumes/BrownCrate.btx"),
	};
	static const size_t numBoundingVolumes = sizeof(boundingMap) / sizeof(boundingMap[0]);

	static const std::pair<std::string, std::string> textureMap[] =
	{
		std::make_pair("TEXTURE_NOT_FOUND", "assets/textures/TextureNotFound.png"),
	};
	static const size_t numTextures = sizeof(textureMap) / sizeof(textureMap[0]);

	static const std::pair<std::string, std::vector<const std::pair<std::string, std::string>>> resourceMaps[] =
	{
		std::make_pair("model", std::vector<const std::pair<std::string, std::string>>(modelMap, modelMap + numModels)),
		std::make_pair("texture", std::vector<const std::pair<std::string, std::string>>(textureMap, textureMap + numTextures)),
		std::make_pair("volume", std::vector<const std::pair<std::string, std::string>>(boundingMap, boundingMap + numBoundingVolumes)),
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