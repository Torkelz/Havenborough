#include "Level.h"
#include "LevelBinaryLoader.h"
#include <XMLHelper.h>

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
	m_Resources = nullptr;
}

bool Level::loadLevel(std::istream& p_LevelData, std::vector<Actor::ptr>& p_ActorOut)
{
	LevelBinaryLoader levelLoader;
	levelLoader.readStreamData(p_LevelData);	

	std::vector<LevelBinaryLoader::ModelData> m_LevelData = levelLoader.getModelData();
	for(unsigned int i = 0; i < m_LevelData.size(); i++)
	{
		LevelBinaryLoader::ModelData& model = m_LevelData.at(i);
		std::string meshName = model.m_MeshName;

		for(unsigned int j = 0; j < m_LevelData.at(i).m_Translation.size(); j++)
		{
			Vector3 translation = model.m_Translation.at(j);
			Vector3 rotation = model.m_Rotation.at(j);
			Vector3 scale = model.m_Scale.at(j);	

			p_ActorOut.push_back(createObjectActor(model.m_MeshName, translation, rotation, scale));
		}
	}
	
	p_LevelData.seekg(0, p_LevelData.beg);
		
	Actor::ptr directionalActor;
	Actor::ptr pointActor;
	Actor::ptr spotActor;
	for (const auto& directionalLight : levelLoader.getDirectionalLightData())
	{
		directionalActor = m_ActorFactory->createDirectionalLight(directionalLight.m_Direction, directionalLight.m_Color);
		p_ActorOut.push_back(directionalActor);
	}
	for (const auto& pointLight : levelLoader.getPointLightData())
	{
		pointActor = m_ActorFactory->createPointLight(pointLight.m_Translation, pointLight.m_Intensity * 5000, pointLight.m_Color);
		p_ActorOut.push_back(pointActor);
	}
	Vector2 minMaxAngle;
	for (const auto& spotLight : levelLoader.getSpotLightData())
	{
		minMaxAngle.x = cosf(spotLight.m_ConeAngle); minMaxAngle.y = cosf(spotLight.m_ConeAngle + spotLight.m_PenumbraAngle);
		spotActor = m_ActorFactory->createSpotLight(spotLight.m_Translation, spotLight.m_Direction, minMaxAngle, spotLight.m_Intensity * 5000, spotLight.m_Color);
		p_ActorOut.push_back(spotActor);
	}

	return true;
}

Actor::ptr Level::createObjectActor(std::string p_MeshName, Vector3 p_Position, Vector3 p_Rotation, Vector3 p_Scale)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	pushVector(printer, p_Position);
	pushRotation(printer, p_Rotation);
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", p_MeshName.c_str());
	pushVector(printer, "Scale", p_Scale);
	printer.CloseElement();
	printer.OpenElement("MeshPhysics");
	printer.PushAttribute("Mesh", p_MeshName.c_str());
	pushVector(printer, "Scale", p_Scale);
	printer.CloseElement();
	printer.CloseElement();

	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr());

	Actor::ptr actor = m_ActorFactory->createActor(doc.FirstChildElement("Object"));

	return actor;
}

const Vector3 &Level::getStartPosition(void) const
{
	return m_StartPosition;
}

void Level::setStartPosition(const Vector3 &p_StartPosition)
{
	m_StartPosition = p_StartPosition;
}

const Vector3 &Level::getGoalPosition(void) const
{
	return m_GoalPosition;
}

void Level::setGoalPosition(const Vector3 &p_GoalPosition)
{
	m_GoalPosition = p_GoalPosition;
}
