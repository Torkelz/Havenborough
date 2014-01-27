#include "ResourceTranslator.h"
#include "ClientExceptions.h"

#include <vector>

ResourceTranslator::ResourceTranslator(){}

ResourceTranslator::~ResourceTranslator(){}

std::string ResourceTranslator::translate(std::string p_ResourceType, std::string p_ResourceName)
{
	static const std::pair<std::string, std::string> modelMap[] =
	{
		std::make_pair("Arrow", "assets/models/Arrow.btx"),
		std::make_pair("Barrel1", "assets/models/Barrel1.btx"),
		std::make_pair("Checkpoint1", "assets/models/Checkpoint.btx"),
		std::make_pair("Crate1", "assets/models/Crate1.btx"),
		std::make_pair("House1", "assets/models/house1.btx"),
		std::make_pair("House2", "assets/models/house2.btx"),
		std::make_pair("House3", "assets/models/house3.btx"),
		std::make_pair("MarketStand1", "assets/models/MarketStand1.btx"),
		std::make_pair("MarketStand2", "assets/models/MarketStand2.btx"),
		std::make_pair("DZALA", "assets/models/Ninta2.btx"),
		std::make_pair("BOX", "assets/models/Sample135.btx"),
		std::make_pair("Sidewalk1", "assets/models/Sidewalk1.btx"),
		std::make_pair("Stair1", "assets/models/Stair1.btx"),
		std::make_pair("StoneBrick2", "assets/models/StoneBrick2.btx"),
		std::make_pair("Street1", "assets/models/Street1.btx"),
		std::make_pair("IKTest", "assets/models/testRig.btx"),
		std::make_pair("Tree1", "assets/models/Tree1.btx"),
		std::make_pair("WITCH", "assets/models/Witch_5_Running.btx"),
		std::make_pair("WoodenShed1", "assets/models/WoodenShed.btx"),
	};

	static const size_t numModels = sizeof(modelMap) / sizeof(modelMap[0]);

	static const std::pair<std::string, std::string> boundingMap[] =
	{
		std::make_pair("Barrel1", "assets/volumes/CB_Barrel1.txc"),
        std::make_pair("Crate1", "assets/volumes/CB_Crate1.txc"),
        std::make_pair("House1", "assets/volumes/CB_House1.txc"),
		std::make_pair("House2", "assets/volumes/CB_House2.txc"),
        std::make_pair("House3", "assets/volumes/CB_House3.txc"),
        std::make_pair("MarketStand1", "assets/volumes/CB_MarketStand1.txc"),
        std::make_pair("MarketStand2", "assets/volumes/CB_MarketStand2.txc"),
        std::make_pair("Sidewalk1", "assets/volumes/CB_Sidewalk1.txc"),		
        std::make_pair("Stair1", "assets/volumes/CB_Stair1.txc"),
        std::make_pair("StoneBrick2", "assets/volumes/CB_StoneBrick2.txc"),
        std::make_pair("Street1", "assets/volumes/CB_Street1.txc"),
        std::make_pair("Tree1", "assets/volumes/CB_Tree1.txc"),
        std::make_pair("WoodenShed1", "assets/volumes/CB_WoodenShed1.txc"),
	};
	static const size_t numBoundingVolumes = sizeof(boundingMap) / sizeof(boundingMap[0]);

	static const std::pair<std::string, std::string> textureMap[] =
	{
		std::make_pair("TEXTURE_NOT_FOUND", "assets/textures/TextureNotFound.png"),
		std::make_pair("SKYBOXDDS", "assets/textures/d3d_skybox_9.dds"),
	};
	static const size_t numTextures = sizeof(textureMap) / sizeof(textureMap[0]);

	static const std::pair<std::string, std::string> soundMap[] =
	{
		std::make_pair("Background", "assets/sounds/Mainbackground.mp3"),
	};
	static const size_t numSounds = sizeof(soundMap) / sizeof(soundMap[0]);

	static const std::pair<std::string, std::vector<const std::pair<std::string, std::string>>> resourceMaps[] =
	{
		std::make_pair("model", std::vector<const std::pair<std::string, std::string>>(modelMap, modelMap + numModels)),
		std::make_pair("texture", std::vector<const std::pair<std::string, std::string>>(textureMap, textureMap + numTextures)),
		std::make_pair("volume", std::vector<const std::pair<std::string, std::string>>(boundingMap, boundingMap + numBoundingVolumes)),
		std::make_pair("sound", std::vector<const std::pair<std::string, std::string>>(soundMap, soundMap + numSounds)),
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