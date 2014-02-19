#include "ResourceTranslator.h"
#include "CommonExceptions.h"

#include <vector>

ResourceTranslator::ResourceTranslator(){}

ResourceTranslator::~ResourceTranslator(){}

std::string ResourceTranslator::translate(std::string p_ResourceType, std::string p_ResourceName)
{
	static const std::pair<std::string, std::string> modelMap[] =
	{
		std::make_pair("Aqueduct1", "assets/models/Aqueduct1.btx"),
		std::make_pair("AqueducttEnd1", "assets/models/AqueducttEnd1.btx"),
		std::make_pair("Arrow", "assets/models/Arrow.btx"),
		std::make_pair("Arrow1", "assets/models/Arrow1.btx"),
		std::make_pair("Barrel1", "assets/models/Barrel1.btx"),
		std::make_pair("Checkpoint1", "assets/models/Checkpoint.btx"),
		std::make_pair("Crate1", "assets/models/Crate1.btx"),
		std::make_pair("Grass1", "assets/models/Grass1.btx"),
		std::make_pair("House1", "assets/models/house1.btx"),
		std::make_pair("House2", "assets/models/house2.btx"),
		std::make_pair("House3", "assets/models/house3.btx"),
		std::make_pair("House4", "assets/models/house4.btx"),
		std::make_pair("House5", "assets/models/house5.btx"),
		std::make_pair("House6", "assets/models/house6.btx"),
		std::make_pair("MarketStand1", "assets/models/MarketStand1.btx"),
		std::make_pair("MarketStand2", "assets/models/MarketStand2.btx"),
		std::make_pair("Road1", "assets/models/Road1.btx"),
		std::make_pair("Road2", "assets/models/Road2.btx"),
		std::make_pair("Road3", "assets/models/Road3.btx"),
		std::make_pair("Road4", "assets/models/Road4.btx"),
		std::make_pair("Road5", "assets/models/Road5.btx"),
		std::make_pair("Sidewalk1", "assets/models/Sidewalk1.btx"),
		std::make_pair("Stair1", "assets/models/Stair1.btx"),
		std::make_pair("Stallning1", "assets/models/Stallning1.btx"),
		std::make_pair("Stallning2", "assets/models/Stallning2.btx"),
		std::make_pair("Stallning3", "assets/models/Stallning3.btx"),
		std::make_pair("Stallning4", "assets/models/Stallning4.btx"),
		std::make_pair("StoneBrick1", "assets/models/StoneBrick1.btx"),
		std::make_pair("StoneBrick2", "assets/models/StoneBrick2.btx"),
		std::make_pair("Vege1", "assets/models/Vege1.btx"),
		std::make_pair("Vege2", "assets/models/Vege2.btx"),
		std::make_pair("Street1", "assets/models/Street1.btx"),
		std::make_pair("Tree1", "assets/models/Tree1.btx"),
		std::make_pair("WITCH", "assets/models/Dzala.btx"),
		std::make_pair("WoodenShed1", "assets/models/WoodenShed.btx"),
		std::make_pair("Zane", "assets/models/ZaneFirewind.btx"),
	};

	static const size_t numModels = sizeof(modelMap) / sizeof(modelMap[0]);

	static const std::pair<std::string, std::string> boundingMap[] =
	{
		std::make_pair("Barrel1", "assets/volumes/CB_Barrel1.txc"),
        std::make_pair("Crate1", "assets/volumes/CB_Crate1.txc"),
		std::make_pair("Grass1", "assets/volumes/temp/CB_Grass1.txc"),
        std::make_pair("House1", "assets/volumes/CB_House1.txc"),
		std::make_pair("House2", "assets/volumes/CB_House2.txc"),
        std::make_pair("House3", "assets/volumes/CB_House3.txc"),
		std::make_pair("House4", "assets/volumes/CB_House4.txc"),
		std::make_pair("House5", "assets/volumes/CB_House5.txc"),
        std::make_pair("House6", "assets/volumes/CB_House6.txc"),
        std::make_pair("MarketStand1", "assets/volumes/CB_MarketStand1.txc"),
        std::make_pair("MarketStand2", "assets/volumes/CB_MarketStand2.txc"),
		std::make_pair("Road1", "assets/volumes/CB_Road1.txc"),
		std::make_pair("Road2", "assets/volumes/CB_Road2.txc"),
		std::make_pair("Road3", "assets/volumes/CB_Road3.txc"),
		std::make_pair("Road4", "assets/volumes/CB_Road4.txc"),
		std::make_pair("Road5", "assets/volumes/CB_Road5.txc"),
        std::make_pair("Sidewalk1", "assets/volumes/CB_Sidewalk1.txc"),		
        std::make_pair("Stair1", "assets/volumes/CB_Stair1.txc"),
		std::make_pair("Stallning1", "assets/volumes/CB_Stallning1.txc"),
		std::make_pair("Stallning2", "assets/volumes/CB_Stallning2.txc"),
		std::make_pair("Stallning3", "assets/volumes/CB_Stallning3.txc"),
		std::make_pair("Stallning4", "assets/volumes/CB_Stallning4.txc"),
        std::make_pair("StoneBrick2", "assets/volumes/CB_StoneBrick2.txc"),
        std::make_pair("Street1", "assets/volumes/CB_Street1.txc"),
        std::make_pair("Tree1", "assets/volumes/CB_Tree1.txc"),
		std::make_pair("Vege1", "assets/volumes/temp/CB_Vege1.txc"),
		std::make_pair("Vege2", "assets/volumes/temp/CB_Vege2.txc"),
        std::make_pair("WoodenShed1", "assets/volumes/CB_WoodenShed1.txc"),
	};
	static const size_t numBoundingVolumes = sizeof(boundingMap) / sizeof(boundingMap[0]);

	static const std::pair<std::string, std::string> textureMap[] =
	{
		std::make_pair("TEXTURE_NOT_FOUND", "assets/textures/TextureNotFound_COLOR.dds"),
		std::make_pair("MANA_BAR", "assets/textures/manabar.dds"),
		std::make_pair("SKYBOXDDS", "assets/textures/Skybox1_COLOR.dds"),
		std::make_pair("Particle1", "assets/textures/Particle1.dds"),
	};
	static const size_t numTextures = sizeof(textureMap) / sizeof(textureMap[0]);

	static const std::pair<std::string, std::string> soundMap[] =
	{
		std::make_pair("Background", "assets/sounds/Mainbackground.mp3"),
	};
	static const size_t numSounds = sizeof(soundMap) / sizeof(soundMap[0]);

	static const std::pair<std::string, std::string> particleMap[] =
	{
		std::make_pair("fire", "assets/particles/fire.xml"),
	};
	static const size_t numParticles = sizeof(particleMap) / sizeof(particleMap[0]);

	static const std::pair<std::string, std::string> spellMap[] =
	{
		std::make_pair("TestSpell", "assets/spells/TestSpell.xml"),
	};
	static const size_t numSpells = sizeof(spellMap) / sizeof(spellMap[0]);

	static const std::pair<std::string, std::string> animationMap[] =
	{
		std::make_pair("WITCH", "assets/animations/Dzala.atx"),
	};
	static const size_t numAnimations = sizeof(animationMap) / sizeof(animationMap[0]);

	static const std::pair<std::string, std::vector<const std::pair<std::string, std::string>>> resourceMaps[] =
	{
		std::make_pair("model", std::vector<const std::pair<std::string, std::string>>(modelMap, modelMap + numModels)),
		std::make_pair("texture", std::vector<const std::pair<std::string, std::string>>(textureMap, textureMap + numTextures)),
		std::make_pair("volume", std::vector<const std::pair<std::string, std::string>>(boundingMap, boundingMap + numBoundingVolumes)),
		std::make_pair("sound", std::vector<const std::pair<std::string, std::string>>(soundMap, soundMap + numSounds)),
		std::make_pair("particleSystem", std::vector<const std::pair<std::string, std::string>>(particleMap, particleMap + numParticles)),
		std::make_pair("spell", std::vector<const std::pair<std::string, std::string>>(spellMap, spellMap + numSpells)),
		std::make_pair("animation", std::vector<const std::pair<std::string, std::string>>(animationMap, animationMap + numAnimations)),
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
	
	if (p_ResourceType == "model")
	{
		return "assets/models/" + p_ResourceName + ".btx";
	}
	else if (p_ResourceType == "volume")
	{
		return "assets/volumes/CB_" + p_ResourceName + ".txc";
	}
	throw ResourceManagerException("Unknown resource: '" + p_ResourceType + ":" + p_ResourceName + "'", __LINE__, __FILE__);
}