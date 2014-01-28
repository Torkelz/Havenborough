#include "Level.h"

const DirectX::XMFLOAT3 &Level::getStartPosition(void) const
{
	return m_StartPosition;
}

void Level::setStartPosition(const DirectX::XMFLOAT3 &p_StartPosition)
{
	m_StartPosition = p_StartPosition;
}

const DirectX::XMFLOAT3 &Level::getGoalPosition(void) const
{
	return m_GoalPosition;
}

void Level::setGoalPosition(const DirectX::XMFLOAT3 &p_GoalPosition)
{
	m_GoalPosition = p_GoalPosition;
}

Level::Level(ResourceManager* p_Resources, IPhysics* p_Physics, ActorFactory* p_ActorFactory)
{
	m_Resources = p_Resources;
	m_Physics = p_Physics;
	m_ActorFactory = p_ActorFactory;

	m_StartPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_GoalPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

Level::~Level()
{
	m_Resources = nullptr;
	m_Physics = nullptr;

	m_StartPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_GoalPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Level::releaseLevel()
{
	m_LevelData.clear();
	m_LevelData.shrink_to_fit();
	m_LevelCollisionData.clear();
	m_LevelCollisionData.shrink_to_fit();
	m_Resources = nullptr;
}

bool Level::loadLevel(std::istream* p_LevelData, std::istream* p_CollisionData, std::vector<Actor::ptr>& p_ActorOut)
{
	m_LevelLoader.readStreamData(p_LevelData);	

	m_LevelData = m_LevelLoader.getModelData();
	for(unsigned int i = 0; i < m_LevelData.size(); i++)
	{
		LevelStreamReader::ModelData& model = m_LevelData.at(i);
		std::string meshName = model.m_MeshName;

		for(unsigned int j = 0; j < m_LevelData.at(i).m_Translation.size(); j++)
		{
			Vector3 translation = model.m_Translation.at(j);
			Vector3 rotation = model.m_Rotation.at(j);
			Vector3 scale = model.m_Scale.at(j);	

			p_ActorOut.push_back(createObjectActor(model.m_MeshName, translation, rotation, scale));
		}
	}
	
	p_LevelData->seekg(0);

	// This will be implemented at a later stage when physics has what it takes!
	m_CollisionLoader.readStreamData(p_CollisionData);
	m_LevelCollisionData = m_CollisionLoader.getModelData();
	for(unsigned int i = 0; i < m_LevelCollisionData.size(); i++)
	{
		LevelStreamReader::ModelData& collisionData = m_LevelCollisionData.at(i);
		std::string meshName = collisionData.m_MeshName;
		
		for(unsigned int j = 0; j < collisionData.m_Translation.size(); j++)
		{
			Vector3 translation = collisionData.m_Translation.at(j);
			Vector3 rotation = collisionData.m_Rotation.at(j);
			Vector3 scale = collisionData.m_Scale.at(j);

			p_ActorOut.push_back(createCollisionActor(meshName, translation, rotation, scale));
		}
	}

	p_CollisionData->seekg(0);

	return true;
}

static void pushVector(tinyxml2::XMLPrinter& p_Printer, const std::string& p_ElementName, Vector3 p_Vec)
{
	p_Printer.OpenElement(p_ElementName.c_str());
	p_Printer.PushAttribute("x", p_Vec.x);
	p_Printer.PushAttribute("y", p_Vec.y);
	p_Printer.PushAttribute("z", p_Vec.z);
	p_Printer.CloseElement();
}

void Level::drawLevel()
{
	//for(unsigned int i = 0; i < m_DrawID.size(); i++)
	//{
	//	m_Graphics->renderModel(m_DrawID.at(i));
	//}
}

Actor::ptr Level::createObjectActor(std::string p_MeshName, Vector3 p_Position, Vector3 p_Rotation, Vector3 p_Scale)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", p_MeshName.c_str());
	pushVector(printer, "Scale", p_Scale);
	printer.CloseElement();
	printer.CloseElement();

	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr());

	Actor::ptr actor = m_ActorFactory->createActor(doc.FirstChildElement("Object"));
	actor->setPosition(p_Position);
	actor->setRotation(p_Rotation);

	return actor;
}

Actor::ptr Level::createCollisionActor(std::string p_MeshName, Vector3 p_Translation, Vector3 p_Rotation, Vector3 p_Scale)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("MeshPhysics");
	printer.PushAttribute("Mesh", p_MeshName.c_str());
	pushVector(printer, "Scale", p_Scale);
	printer.CloseElement();
	printer.CloseElement();

	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr());

	Actor::ptr actor = m_ActorFactory->createActor(doc.FirstChildElement("Object"));
	actor->setPosition(p_Translation);
	actor->setRotation(p_Rotation);

	return actor;
}
