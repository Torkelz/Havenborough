#include "HUDScene.h"
#include "../ClientExceptions.h"
#include <EventData.h>
#include <sstream>
HUDScene::HUDScene()
{
	m_SceneID = 0;
	m_Visible = false;
	m_NewSceneID = 0;
	m_ChangeScene = false;
	m_ChangeList = false;
	m_FadeOut = false;
	m_Color = Vector3(0.0274509803921569f, 0.2313725490196078f, 0.3764705882352941f);
	m_BGColor = Vector3(0.8156862745098039f, 0.8156862745098039f, 0.8156862745098039f);
	m_TimeTimerMax = 10.0f;
	m_TimeTimerStartFade = 5.0f;
	m_TimePositionFade = 1.0f;
	m_Graphics = nullptr;
	m_EventManager = nullptr;
	m_ResourceManager = nullptr;
	m_RenderHUD = true;
	m_ShowDebugInfo = false;
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
	m_EventManager->addListener(EventListenerDelegate(this, &HUDScene::updatePlayerTime), UpdatePlayerTimeEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &HUDScene::updatePlayerRacePosition), UpdatePlayerRaceEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &HUDScene::setNrOfCheckpoints), GetNrOfCheckpoints::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &HUDScene::activateHUD), activateHUDEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &HUDScene::updateTakenCheckpoints), UpdateTakenCheckpoints::sk_EventType);

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
	if(m_FadeOut)
	{
		m_TimeTimerCurrent -= p_Dt;
		m_TimePositionCurrent += p_Dt;
		Vector3 position;
		Vector3 scale(0.45f,0.45f,0.45f);
		float proc = m_TimePositionCurrent / m_TimePositionFade;
		if(proc < 1.0f)
		{
			position.x = m_TimePosition.x * proc;
			position.y = m_TimePosition.y * proc;
			position.z = m_TimePosition.z * proc;
			if(m_TimeScale.x * (1 - proc) > 1)
			{
				scale.x = m_TimeScale.x * (1 - proc);
				scale.y = m_TimeScale.y * (1 - proc);
				scale.z = m_TimeScale.z * (1 - proc);
			}
		}
		else
		{
			position = m_TimePosition;
		}

		float percentage = m_TimeTimerCurrent/m_TimeTimerStartFade;

		if(m_TimeTimerCurrent < 0.0f)
		{
			percentage = 0.0f;
			m_FadeOut = false;
		}
		else if(m_TimeTimerCurrent > m_TimeTimerStartFade)
		{
			percentage = 1.0f;
		}
		m_Graphics->set2D_ObjectScale(m_GUI["Time"], scale);
		m_Graphics->setTextColor(m_TextHandle["Time"], Vector4(m_Color, percentage));
		m_Graphics->set2D_ObjectPosition(m_GUI["Time"], position);
	}
	

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

	if (m_ShowDebugInfo)
	{
		std::string debugTextKey;
		for (const auto& val : m_DebugInfo.getInfoList())
		{
			debugTextKey += val.first + '\n';
		}
		std::vector<wchar_t> wText(debugTextKey.length() + 1);
		mbstowcs(wText.data(), debugTextKey.data(), debugTextKey.length() + 1);
		m_Graphics->updateText(m_TextHandle["DebugTextKey"], wText.data());

		std::string debugTextValue;
		for (const auto& val : m_DebugInfo.getInfoList())
		{
			debugTextValue += val.second + '\n';
		}
		wText.resize(debugTextValue.length() + 1);
		mbstowcs(wText.data(), debugTextValue.data(), debugTextValue.length() + 1);
		m_Graphics->updateText(m_TextHandle["DebugTextValue"], wText.data());
	}
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
	if(m_RenderHUD)
	{
		m_Graphics->render2D_Object(m_GUI["Arrow"]);
		m_Graphics->render2D_Object(m_GUI["Manabar"]);
		m_Graphics->render2D_Object(m_GUI["ManabarChange"]);
		m_Graphics->render2D_Object(m_GUI["ManabarCounter"]);
		m_Graphics->render2D_Object(m_GUI["Time"]);
		m_Graphics->render2D_Object(m_GUI["RacePos"]);
		m_Graphics->render2D_Object(m_GUI["RacePosBG"]);
		m_Graphics->render2D_Object(m_GUI["Checkpoints"]);
		if (m_ShowDebugInfo)
		{
			m_Graphics->render2D_Object(m_GUI["DebugTextKey"]);
			m_Graphics->render2D_Object(m_GUI["DebugTextValue"]);
		}

			if(m_RenderCountdown)
			{
				m_Graphics->render2D_Object(m_GUI["Countdown"]);
				m_RenderCountdown = false;
			}
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
	if (p_Value > 0.5f && p_PrevValue <= 0.5f)
	{
		if (p_Action == "toggleDebugInfo")
		{
			m_ShowDebugInfo = !m_ShowDebugInfo;
		}
	}
}

void HUDScene::setHUDSettings(std::map<std::string, Settings::HUDSettings> p_Settings)
{
	releasePreLoadedModels();
	m_HUDSettings = p_Settings;
	preLoadModels();
}

DebugInfo& HUDScene::getDebugInfo()
{
	return m_DebugInfo;
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

void HUDScene::updatePlayerTime(IEventData::Ptr p_Data)
{
	std::shared_ptr<UpdatePlayerTimeEventData> data = std::static_pointer_cast<UpdatePlayerTimeEventData>(p_Data);

	std::stringstream ss;
	ss.precision(2);
	float timeDiff = data->getTime();
	float floorTimeDiff = floorf(timeDiff);
	float timeDiffFrac = (timeDiff - floorTimeDiff) * 100.f;

	ss << floorTimeDiff << "." << timeDiffFrac;
	std::string string = ss.str();

	m_Graphics->updateText(m_TextHandle["Time"], std::wstring(string.begin(), string.end()).c_str());
	m_Graphics->setTextColor(m_TextHandle["Time"], Vector4(m_Color, 1.0f));
	m_TimeTimerCurrent = m_TimeTimerMax;
	m_TimeScale = Vector3(10,10,10);
	m_TimePositionCurrent = 0;
	m_FadeOut = true;
}

void HUDScene::updatePlayerRacePosition(IEventData::Ptr p_Data)
{
	std::shared_ptr<UpdatePlayerRaceEventData> data = std::static_pointer_cast<UpdatePlayerRaceEventData>(p_Data);

	int racePos = data->getPosition();
	static const char* positions[] = {"st","nd","rd","th"};
	std::string position;
	position.append(std::to_string(racePos));
	if(racePos >= 4)
	{
		position.append(positions[3]);
	}
	else
	{
		position.append(positions[racePos-1]);
	}

	m_Graphics->updateText(m_TextHandle["RacePos"], std::wstring(position.begin(), position.end()).c_str());
	m_Graphics->updateText(m_TextHandle["RacePosBG"], std::wstring(position.begin(), position.end()).c_str());
}

void HUDScene::updateCheckpointPosition(IEventData::Ptr p_Data)
{
	std::shared_ptr<UpdateCheckpointPositionEventData> data = std::static_pointer_cast<UpdateCheckpointPositionEventData>(p_Data);

	m_CheckpointPosition = data->getPosition();
}

void HUDScene::activateHUD(IEventData::Ptr p_Data)
{
	std::shared_ptr<activateHUDEventData> data = std::static_pointer_cast<activateHUDEventData>(p_Data);

	m_RenderHUD = data->getState();
}

void HUDScene::setNrOfCheckpoints(IEventData::Ptr p_Data)
{
	std::shared_ptr<GetNrOfCheckpoints> data = std::static_pointer_cast<GetNrOfCheckpoints>(p_Data);
	m_TakenCheckpoints = "0 / ";
	m_NumberOfCheckpoints = data->getNumberOfCheckpoints();
	m_TakenCheckpoints.append(std::to_string(m_NumberOfCheckpoints));
	m_Graphics->updateText(m_TextHandle["Checkpoints"], std::wstring(m_TakenCheckpoints.begin(), m_TakenCheckpoints.end()).c_str());
}

void HUDScene::updateTakenCheckpoints(IEventData::Ptr p_Data)
{
	std::shared_ptr<UpdateTakenCheckpoints> data = std::static_pointer_cast<UpdateTakenCheckpoints>(p_Data);
	unsigned int number = data->getNumberOfCheckpointsTaken();
	m_TakenCheckpoints.clear();
	m_TakenCheckpoints = std::to_string(number);
	m_TakenCheckpoints.append(" / ");
	m_TakenCheckpoints.append(std::to_string(m_NumberOfCheckpoints));
	m_Graphics->updateText(m_TextHandle["Checkpoints"], std::wstring(m_TakenCheckpoints.begin(), m_TakenCheckpoints.end()).c_str());
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
	getHUDSettings(id, pos, scale);

	createGUIElement("Arrow",m_Graphics->create2D_Object(pos, scale, 0.f, "Arrow1"));

	pos = Vector3(-400, -320, 3);
	scale = Vector3(1.0f, 1.0f, 1.0f);
	id = "Manabar";
	getHUDSettings(id, pos, scale);

	createGUIElement("ManabarChange", m_Graphics->create2D_Object(Vector3(pos.x, pos.y, 3), Vector2(140, 30), scale, 0.0f, "MANA_BARCHANGE"));
	createGUIElement("Manabar", m_Graphics->create2D_Object(Vector3(pos.x, pos.y, 4), Vector2(144, 28), scale, 0.0f, "MANA_BAR"));

	createTextElement("ManabarCounter", m_Graphics->createText(L"", Vector2(130,65), "Aniron", 20.f, Vector4(1,1,1,1), Vector3(0,0,0), 1.0f, 0.f));
	createGUIElement("ManabarCounter", m_Graphics->create2D_Object(Vector3(pos.x, pos.y, 2), Vector3(1,1,1), 0.f, m_TextHandle["ManabarCounter"]));

	pos = Vector3(0, 0, 0);
	scale = Vector3(2.0f, 2.0f, 2.0f);
	id = "Countdown";
	getHUDSettings(id, pos, scale);

	
	createTextElement("Countdown", m_Graphics->createText(L"", Vector2(130,65), "Aniron", 72.f, Vector4(1,0,0,1), Vector3(0,0,0), 1.0f, 0.f));
	createGUIElement("Countdown", m_Graphics->create2D_Object(pos, scale, 0.f, m_TextHandle["Countdown"]));

	pos = Vector3(420, 250, 1);
	scale = Vector3(1.0f, 1.0f, 1.0f);
	id = "Time";
	getHUDSettings(id,pos,scale);
	m_TimePosition = pos;
	createTextElement("Time", m_Graphics->createText(L"0.00", Vector2(120.f, 70.f), "Aniron", 72.f, Vector4(m_Color, 0.f), Vector3(0.f, 100.f, 0.f), 1.f, 0.f));
	createGUIElement("Time", m_Graphics->create2D_Object(m_TimePosition, scale, 0.f, m_TextHandle["Time"]));
	m_Graphics->setTextBackgroundColor(m_TextHandle["Time"], Vector4(m_Color, 0.f));

	pos = Vector3(-450, 320, 2);
	scale = Vector3(1.0f, 1.0f, 1.0f);
	id = "RacePos";
	getHUDSettings(id,pos,scale);
	createTextElement("RacePos", m_Graphics->createText(L"1st", Vector2(200, 65), "Aniron", 42, Vector4(m_Color, 1.f), Vector3(0.0f, 0.0f, 0.0f), 1.0f, 0.f));
	createGUIElement("RacePos", m_Graphics->create2D_Object(pos, scale, 0.f, m_TextHandle["RacePos"]));

	createTextElement("RacePosBG", m_Graphics->createText(L"1st", Vector2(204, 69), "Aniron", 42, Vector4(m_BGColor, 0.8f), Vector3(0.0f, 0.0f, 0.0f), 1.0f, 0.f));
	createGUIElement("RacePosBG", m_Graphics->create2D_Object(Vector3(pos.x-2, pos.y-2, 3), Vector3(1,1,1), 0.f, m_TextHandle["RacePosBG"]));

	pos = Vector3(418, 318, 3);
	scale = Vector3(1.0f, 1.0f, 1.0f);
	id = "Checkpoints";
	getHUDSettings(id,pos,scale);
	createTextElement("Checkpoints", m_Graphics->createText(L"0/0", Vector2(204, 69), "Aniron", 42, Vector4(m_Color, 0.8f), Vector3(0.0f, 0.0f, 0.0f), 1.0f, 0.f));
	createGUIElement("Checkpoints", m_Graphics->create2D_Object(pos, scale, 0.f, m_TextHandle["Checkpoints"]));
	
	createTextElement("CheckpointsBG", m_Graphics->createText(L"1st", Vector2(204, 69), "Aniron", 42, Vector4(m_BGColor, 0.8f), Vector3(0.0f, 0.0f, 0.0f), 1.0f, 0.f));
	createGUIElement("CheckpointsBG", m_Graphics->create2D_Object(Vector3(pos.x-2, pos.y-2, 3), Vector3(1,1,1), 0.f, m_TextHandle["CheckpointsBG"]));


	createTextElement("DebugTextKey", m_Graphics->createText(L"", Vector2(300.f, 400.f), "Segoe UI", 30, Vector4(0.8f, 0.8f, 0.8f, 1.f), Vector3(0.0f, 0.0f, 0.0f), 1.0f, 0.f));
	m_Graphics->setTextAlignment(m_TextHandle["DebugTextKey"], TEXT_ALIGNMENT::LEADING);
	m_Graphics->setTextParagraphAlignment(m_TextHandle["DebugTextKey"], PARAGRAPH_ALIGNMENT::NEAR_ALIGNMENT);
	m_Graphics->setTextBackgroundColor(m_TextHandle["DebugTextKey"], Vector4(0.f, 0.f, 0.f, 0.4f));
	createGUIElement("DebugTextKey", m_Graphics->create2D_Object(Vector3(-490.f, 160.f, 4.f), Vector3(1,1,1), 0.f, m_TextHandle["DebugTextKey"]));
	
	createTextElement("DebugTextValue", m_Graphics->createText(L"", Vector2(300.f, 400.f), "Segoe UI", 30, Vector4(0.8f, 0.8f, 0.8f, 1.f), Vector3(0.0f, 0.0f, 0.0f), 1.0f, 0.f));
	m_Graphics->setTextAlignment(m_TextHandle["DebugTextValue"], TEXT_ALIGNMENT::LEADING);
	m_Graphics->setTextParagraphAlignment(m_TextHandle["DebugTextValue"], PARAGRAPH_ALIGNMENT::NEAR_ALIGNMENT);
	m_Graphics->setTextBackgroundColor(m_TextHandle["DebugTextValue"], Vector4(0.f, 0.f, 0.f, 0.4f));
	createGUIElement("DebugTextValue", m_Graphics->create2D_Object(Vector3(-190.f, 160.f, 4.f), Vector3(1,1,1), 0.f, m_TextHandle["DebugTextValue"]));
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

void HUDScene::getHUDSettings( std::string id, Vector3 &pos, Vector3 &scale )
{
	if(m_HUDSettings.count(id) > 0)
	{
		pos = m_HUDSettings.at(id).position;
		float s = m_HUDSettings.at(id).scale;
		scale = Vector3(s, s, s);
	}
}
