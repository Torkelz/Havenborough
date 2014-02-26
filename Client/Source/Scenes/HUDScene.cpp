#include "HUDScene.h"
#include "../ClientExceptions.h"
#include <EventData.h>

HUDScene::HUDScene()
{
	m_SceneID = 0;
	m_Visible = false;
	m_NewSceneID = 0;
	m_ChangeScene = false;
	m_ChangeList = false;

	m_Graphics = nullptr;
	m_EventManager = nullptr;
	m_ResourceManager = nullptr;
}

HUDScene::~HUDScene()
{
	m_Graphics = nullptr;
	m_EventManager = nullptr;
	m_ResourceManager = nullptr;
}

bool HUDScene::init(unsigned int p_SceneID, IGraphics *p_Graphics, ResourceManager *p_ResourceManager,
	Input *p_InputQueue, GameLogic *p_GameLogic, EventManager *p_EventManager)
{
	m_SceneID = p_SceneID;

	m_Graphics = p_Graphics;
	m_EventManager = p_EventManager;
	m_ResourceManager = p_ResourceManager;

	m_EventManager->addListener(EventListenerDelegate(this, &HUDScene::updateGraphicalCountdown), UpdateGraphicalCountdownEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &HUDScene::updateGraphicalManabar), UpdateGraphicalManabarEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &HUDScene::updateCheckpointPosition), UpdateCheckpointPositionEventData::sk_EventType);

	m_CheckpointPosition = Vector3(0,0,0);
	m_RenderCountdown = false;

	preLoadModels();

	return true;
}

void HUDScene::destroy()
{
	releasePreLoadedModels();
}

void HUDScene::onFrame(float p_Dt, int* p_IsCurrentScene)
{
	if(m_ChangeScene)
	{
		*p_IsCurrentScene = m_NewSceneID;
		m_Visible = false;
		m_ChangeScene = false;
	}
	else if(m_ChangeList)
	{
		*p_IsCurrentScene = -1;
		m_ChangeList = false;
	}

	m_Graphics->set2D_ObjectLookAt(m_GUI["Arrow"], m_CheckpointPosition);
}

void HUDScene::onFocus()
{
	std::shared_ptr<MouseEventDataShow> showMouse(new MouseEventDataShow(true));
	m_EventManager->queueEvent(showMouse);
	std::shared_ptr<MouseEventDataLock> lockMouse(new MouseEventDataLock(false));
	m_EventManager->queueEvent(lockMouse);
}

void HUDScene::render()
{
	m_Graphics->render2D_Object(m_GUI["Arrow"]);
	m_Graphics->render2D_Object(m_GUI["Manabar"]);
	m_Graphics->render2D_Object(m_GUI["ManabarChange"]);
	m_Graphics->render2D_Object(m_GUI["ManabarCounter"]);

	if(m_RenderCountdown)
	{
		m_Graphics->render2D_Object(m_GUI["Countdown"]);
		m_RenderCountdown = false;
	}
}

bool HUDScene::getIsVisible()
{
	return m_Visible;
}

void HUDScene::setIsVisible(bool p_SetVisible)
{
	m_ChangeList = false;

	m_Visible = p_SetVisible;
}

void HUDScene::registeredInput(std::string p_Action, float p_Value, float p_PrevValue)
{

}

void HUDScene::setHUDSettings(std::map<std::string, Settings::HUDSettings> p_Settings)
{
	releasePreLoadedModels();
	m_HUDSettings = p_Settings;
	preLoadModels();
}

void HUDScene::createGUIElement(std::string p_GUIIdentifier, int p_Id)
{
	if(m_GUI.count(p_GUIIdentifier) > 0)
		throw SceneException("Failed to create a GUI Element with an already existing id: " + p_GUIIdentifier, __LINE__, __FILE__);

	m_GUI.insert(std::pair<std::string, int>(p_GUIIdentifier, p_Id));
}

void HUDScene::createTextElement(std::string p_TextIdentifier, int p_Id)
{
	if(m_TextHandle.count(p_TextIdentifier) > 0)
		throw SceneException("Failed to create a Text Element with an already existing id: " + p_TextIdentifier, __LINE__, __FILE__);

	m_TextHandle.insert(std::pair<std::string, int>(p_TextIdentifier, p_Id));
}

void HUDScene::updateGraphicalCountdown(IEventData::Ptr p_Data)
{
	std::shared_ptr<UpdateGraphicalCountdownEventData> data = std::static_pointer_cast<UpdateGraphicalCountdownEventData>(p_Data);

	m_Graphics->updateText(m_TextHandle["Countdown"], data->getText().c_str());
	m_Graphics->setTextColor(m_TextHandle["Countdown"], data->getColor());
	m_Graphics->set2D_ObjectScale(m_GUI["Countdown"], data->getScale());
	m_RenderCountdown = true;
}

void HUDScene::updateGraphicalManabar(IEventData::Ptr p_Data)
{
	std::shared_ptr<UpdateGraphicalManabarEventData> data = std::static_pointer_cast<UpdateGraphicalManabarEventData>(p_Data);
	Vector3 s = Vector3(1,1,1);
	std::string id = "Manabar";
	if(m_HUDSettings.count(id) > 0)
	{
		float ss = m_HUDSettings.at(id).scale;
		s = Vector3(ss,ss,ss);
	}
	m_Graphics->set2D_ObjectScale(m_GUI["ManabarChange"], Vector3(data->getCurrentMana() * s.x, s.y, 0.f));

	m_Graphics->updateText(m_TextHandle["ManabarCounter"], std::to_wstring((int)(data->getCurrentMana() * 100)).c_str());

	Vector2 barSize = m_Graphics->get2D_ObjectHalfSize(m_GUI["ManabarChange"]);
	barSize.x *= s.x;
	Vector3 barPos = m_Graphics->get2D_ObjectPosition(m_GUI["ManabarChange"]);
	m_Graphics->set2D_ObjectPosition(m_GUI["ManabarCounter"], Vector3(barPos.x, barPos.y, 2));

	barPos = Vector3(barPos.x + (data->getDiffCurrPrevious() * barSize.x), barPos.y, barPos.z);
	m_Graphics->set2D_ObjectPosition(m_GUI["ManabarChange"], barPos);
}

void HUDScene::updateCheckpointPosition(IEventData::Ptr p_Data)
{
	std::shared_ptr<UpdateCheckpointPositionEventData> data = std::static_pointer_cast<UpdateCheckpointPositionEventData>(p_Data);

	m_CheckpointPosition = data->getPosition();
}

void HUDScene::preLoadModels()
{
	static const std::string preloadedTextures[] =
	{
		"TEXTURE_NOT_FOUND",
		"MANA_BAR",
		"MANA_BARCHANGE",
	};
	for (const std::string &texture : preloadedTextures)
	{
		m_ResourceIDs.push_back(m_ResourceManager->loadResource("texture", texture));
	}
	m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", "Arrow1"));

	Vector3 pos = Vector3(0, 300, 150.f);
	Vector3 scale = Vector3(0.3f, 0.3f, 0.3f);
	std::string id = "Arrow";
	if(m_HUDSettings.count(id) > 0)
	{
		pos = m_HUDSettings.at(id).position;
		float s = m_HUDSettings.at(id).scale;
		scale = Vector3(s, s, s);
	}
	createGUIElement("Arrow",m_Graphics->create2D_Object(pos, scale, 0.f, "Arrow1"));

	pos = Vector3(-400, -320, 3);
	scale = Vector3(1.0f, 1.0f, 1.0f);
	id = "Manabar";
	if(m_HUDSettings.count(id) > 0)
	{
		pos = m_HUDSettings.at(id).position;
		float s = m_HUDSettings.at(id).scale;
		scale = Vector3(s, s, s);
	}
	createGUIElement("ManabarChange", m_Graphics->create2D_Object(Vector3(pos.x, pos.y, 3), Vector2(140, 30), scale, 0.0f, "MANA_BARCHANGE"));
	createGUIElement("Manabar", m_Graphics->create2D_Object(Vector3(pos.x, pos.y, 4), Vector2(144, 28), scale, 0.0f, "MANA_BAR"));

	createTextElement("ManabarCounter", m_Graphics->createText(L"", Vector2(130,65), "Segoe UI", 20.f, Vector4(1,1,1,1), Vector3(0,0,0), 1.0f, 0.f));
	createGUIElement("ManabarCounter", m_Graphics->create2D_Object(Vector3(pos.x, pos.y, 2), Vector3(1,1,1), 0.f, m_TextHandle["ManabarCounter"]));

	pos = Vector3(0, 0, 0);
	scale = Vector3(2.0f, 2.0f, 2.0f);
	id = "Countdown";
	if(m_HUDSettings.count(id) > 0)
	{
		pos = m_HUDSettings.at(id).position;
		float s = m_HUDSettings.at(id).scale;
		scale = Vector3(s, s, s);
	}
	createTextElement("Countdown", m_Graphics->createText(L"", Vector2(130,65), "Segoe UI", 72.f, Vector4(1,0,0,1), Vector3(0,0,0), 1.0f, 0.f));
	createGUIElement("Countdown", m_Graphics->create2D_Object(pos, scale, 0.f, m_TextHandle["Countdown"]));
}

void HUDScene::releasePreLoadedModels()

{
	for (int res : m_ResourceIDs)
	{
		m_ResourceManager->releaseResource(res);
	}
	m_ResourceIDs.clear();

	for(auto id : m_GUI)
	{
		m_Graphics->release2D_Model(id.second);
	}
	m_GUI.clear();
	for(auto id : m_TextHandle)
	{
		m_Graphics->deleteText(id.second);
	}
	m_TextHandle.clear();
	m_HUDSettings.clear();
}
/*########## TEST FUNCTIONS ##########*/

int HUDScene::getID()
{
	return m_SceneID;
}

void HUDScene::createGUIElementTest(std::string p_GUIIdentifier, int p_Id)
{
	createGUIElement(p_GUIIdentifier, p_Id);
}
void HUDScene::createTextElementTest(std::string p_TextIdentifier, int p_Id)
{
	createTextElement(p_TextIdentifier, p_Id);
}