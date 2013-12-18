#include "LevelBinaryLoader.h"
#include "IGraphics.h"
#include "IPhysics.h"
#include "ResourceManager.h"

class Level
{
public:
private:
	IGraphics* m_Graphic;
	IPhysics* m_Physics;
	ResourceManager* m_Resource;
	LevelBinaryLoader m_LevelLoader, m_CollisionLoader;
	std::vector<LevelBinaryLoader::ModelData> m_LevelData, m_LevelCollisionData;
	std::vector<int> m_ResourceID, m_BVResourceID;
	std::vector<int> m_DrawID;
public:
	Level(){}
	Level(IGraphics* p_Graphic, ResourceManager* p_Resource, IPhysics* p_Physics);
	~Level();
	void releaseLevel();
	bool loadLevel(std::string p_LevelFilePath, std::string p_CollisionFilePath);
	void drawLevel();
private:
};