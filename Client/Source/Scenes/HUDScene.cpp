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
	m_TimePositionFade = 1.f;
	m_TimeFlashFade = 0.f;
	m_TimeFlashFadeMax = 0.5f;
	m_FadeOutFlash = false;
	m_FeedbackManabarTime = 0.f;
	m_FeedbackManabarTimeMax = 0.5f;
	m_FeedbackManabar = false;
	m_FeedbackCastable = false;
	m_FeedbackFade = false;
	m_ManabarScale = Vector3(0.f, 0.f, 0.f);

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
	m_GUIFont = "StoneHenge";
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
	m_EventManager->addListener(EventListenerDelegate(this, &HUDScene::onFinish), FinishRaceEventData::sk_EventType);

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
		position.z = 1;
		float proc = m_TimePositionCurrent / m_TimePositionFade;
		if(proc < 1.0f)
		{
			position.x = m_TimePosition.x * proc;
			position.y = 150.f * (1 - proc) + m_TimePosition.y * proc;
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

		Vector3 tempScale = Vector3(0.45f, 0.45f, 0.45f);

		m_Graphics->set2D_ObjectScale(m_GUI["Time"], tempScale);
		m_Graphics->setTextColor(m_TextHandle["Time"], Vector4(m_Color, percentage));
		m_Graphics->set2D_ObjectPosition(m_GUI["Time"], position);

		m_Graphics->set2D_ObjectScale(m_GUI["TimeBG"], tempScale);
		m_Graphics->setTextColor(m_TextHandle["TimeBG"], Vector4(m_BGColor, percentage));
		m_Graphics->set2D_ObjectPosition(m_GUI["TimeBG"], Vector3(position.x-2, position.y-2, position.z+1));
	}

	if(m_FadeOutFlash)
	{
		m_TimeFlashFade += p_Dt;

		float percentage = m_TimeFlashFade / m_TimeFlashFadeMax;

		Vector3 scale(2.f, 2.f, 2.f);

		scale.x = m_TimeScale.x + (scale.x - m_TimeScale.x) * percentage;
		scale.y = m_TimeScale.y + (scale.y - m_TimeScale.y) * percentage;
		scale.z = m_TimeScale.z + (scale.z - m_TimeScale.z) * percentage;

		m_Graphics->set2D_ObjectScale(m_GUI["TimeFlash"], scale);
		m_Graphics->setTextColor(m_TextHandle["TimeFlash"], Vector4(0.f, 0.f, 0.f, 0.5f - 0.5f * percentage));

		if(m_TimeFlashFade >= m_TimeFlashFadeMax)
		{
			m_FadeOutFlash = false;
		}
	}


	if(m_FeedbackCastable)
	{
		if(!m_FeedbackFade)
			m_FeedbackManabarTime += p_Dt;
		else
			m_FeedbackManabarTime -= p_Dt;

		float per = m_FeedbackManabarTime / m_FeedbackManabarTimeMax;

		m_Graphics->set2D_ObjectColor(m_GUI["ManabarFeedback"], Vector4(29.f/256.f * per, 76.f/256.f * per, 215.f / 256.f * per, per));

		Vector3 tempScale = Vector3(1.f, 2.f, 2.f);

		tempScale.x = m_ManabarScale.x + (tempScale.x - m_ManabarScale.x) * per;
		tempScale.y = m_ManabarScale.y + (tempScale.y - m_ManabarScale.y) * per;
		tempScale.z = m_ManabarScale.z + (tempScale.z - m_ManabarScale.z) * per;

		m_Graphics->set2D_ObjectScale(m_GUI["ManabarFeedback"], tempScale);

		if(m_FeedbackManabarTime < 0.f)
		{
			m_FeedbackManabarTime = 0.f;
			m_FeedbackCastable = false;
			m_FeedbackFade = false;
		}

		if(m_FeedbackManabarTime >= m_FeedbackManabarTimeMax)
		{
			m_FeedbackFade = true;
		}
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
		m_Graphics->render2D_Object(m_GUI["ManabarFeedback"]);
		m_Graphics->render2D_Object(m_GUI["Time"]);
		m_Graphics->render2D_Object(m_GUI["TimeBG"]);
		m_Graphics->render2D_Object(m_GUI["TimeFlash"]);
		m_Graphics->render2D_Object(m_GUI["RacePos"]);
		m_Graphics->render2D_Object(m_GUI["RacePosBG"]);
		m_Graphics->render2D_Object(m_GUI["Checkpoints"]);
		m_Graphics->render2D_Object(m_GUI["CheckpointsBG"]);
		m_Graphics->render2D_Object(m_GUI["Crosshair"]);
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

void HUDScene::setHUDSettings(std::map<std::string, Settings::HUDSettings> p_Settings, Vector2 p_ScreenResolution)
{
	releasePreLoadedModels();
	m_HUDSettings = p_Settings;
	m_Resolution = p_ScreenResolution;
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


	if(data->getCurrentMana() * 100 >= data->getManaCost())
	{
		if(!m_FeedbackManabar)
			m_FeedbackCastable = true;
		m_Graphics->set2D_ObjectColor(m_GUI["ManabarChange"], Vector4(29.f/256.f, 76.f/256.f, 215.f / 256.f, 1.f));
		
		m_FeedbackManabar = true;
	}
	else
	{
		m_Graphics->set2D_ObjectColor(m_GUI["ManabarChange"], Vector4(1.f, 1.f, 1.f, 1.f));

		m_FeedbackManabar = false;
	}

	Vector2 barSize = m_Graphics->get2D_ObjectHalfSize(m_GUI["ManabarChange"]);
	barSize.x *= s.x;
	Vector3 barPos = m_Graphics->get2D_ObjectPosition(m_GUI["ManabarChange"]);
	//m_Graphics->set2D_ObjectPosition(m_GUI["ManabarCounter"], Vector3(barPos.x, barPos.y, 2));

	barPos = Vector3(barPos.x + (data->getDiffCurrPrevious() * barSize.x), barPos.y, barPos.z);
	m_Graphics->set2D_ObjectPosition(m_GUI["ManabarChange"], barPos);
}

void HUDScene::updatePlayerTime(IEventData::Ptr p_Data)
{
	std::shared_ptr<UpdatePlayerTimeEventData> data = std::static_pointer_cast<UpdatePlayerTimeEventData>(p_Data);

	std::wstring playerTimeText;
	
	float timeDiff = fabs(data->getTime());

	int minutes = (int)timeDiff / 60;
	float seconds = timeDiff - minutes * 60;
	wchar_t buffer[64];
	std::swprintf(buffer, L"+" L"%02.2d" L":" L"%05.2f\n", minutes, seconds);
	playerTimeText += buffer;

	if (!playerTimeText.empty())
	{
		playerTimeText = playerTimeText.substr(0, playerTimeText.length() - 1);
	}

	m_Graphics->updateText(m_TextHandle["Time"], playerTimeText.data());
	m_Graphics->setTextColor(m_TextHandle["Time"], Vector4(m_Color, 1.0f));
	m_Graphics->updateText(m_TextHandle["TimeBG"], playerTimeText.data());
	m_Graphics->setTextColor(m_TextHandle["TimeBG"], Vector4(m_Color, 1.0f));
	m_Graphics->updateText(m_TextHandle["TimeFlash"], playerTimeText.data());
	m_Graphics->setTextColor(m_TextHandle["TimeFlash"], Vector4(m_Color, 1.0f));
	m_TimeTimerCurrent = m_TimeTimerMax;
	m_TimeScale = Vector3(0.45f, 0.45f, 0.45f);
	m_TimePositionCurrent = 0;
	m_FadeOut = true;

	m_FadeOutFlash = true;
	m_TimeFlashFade = 0.f;
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
	m_CheckpointPosition.y = m_CheckpointPosition.y - 20.f;
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
		m_Graphics->render2D_Object(m_GUI["Checkpoints"]);
	m_Graphics->updateText(m_TextHandle["Checkpoints"], std::wstring(m_TakenCheckpoints.begin(), m_TakenCheckpoints.end()).c_str());
	m_Graphics->updateText(m_TextHandle["CheckpointsBG"], std::wstring(m_TakenCheckpoints.begin(), m_TakenCheckpoints.end()).c_str());
}

void HUDScene::onFinish(IEventData::Ptr p_Data)
{
	m_ChangeScene = true;
	m_NewSceneID = (int)RunScenes::POST_GAME;
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
	m_Graphics->updateText(m_TextHandle["CheckpointsBG"], std::wstring(m_TakenCheckpoints.begin(), m_TakenCheckpoints.end()).c_str());
}

void HUDScene::preLoadModels()
{
	static const std::string preloadedTextures[] =
	{
		"TEXTURE_NOT_FOUND",
		"MANA_BAR",
		"MANA_BARCHANGE",
		"Crosshair",
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
	adjustHUDPosition(pos);
	createGUIElement("Arrow",m_Graphics->create2D_Object(pos, scale, 0.f, "Arrow1"));

	pos = Vector3(-400, -320, 3);
	scale = Vector3(1.0f, 1.0f, 1.0f);
	id = "Manabar";
	getHUDSettings(id, pos, scale);
	adjustHUDPosition(pos);

	createGUIElement("ManabarChange", m_Graphics->create2D_Object(Vector3(pos.x, pos.y, 3), Vector2(140, 28), scale, 0.0f, "MANA_BARCHANGE"));
	createGUIElement("Manabar", m_Graphics->create2D_Object(Vector3(pos.x, pos.y - 0.5f, 4), Vector2(144, 30), scale, 0.0f, "MANA_BAR"));
	m_Graphics->set2D_ObjectColor(m_GUI["Manabar"], Vector4(0.f, 0.f, 0.f, 1.f));

	createGUIElement("ManabarFeedback", m_Graphics->create2D_Object(Vector3(pos.x, pos.y, 4), Vector2(144, 28), scale, 0.0f, "MANA_BARCHANGE"));
	m_ManabarScale = scale;
	m_Graphics->set2D_ObjectColor(m_GUI["ManabarFeedback"], Vector4(0.f, 0.f, 0.f, 0.f));

	createTextElement("ManabarCounter", m_Graphics->createText(L"", Vector2(130,65), m_GUIFont.c_str(), 20.f, Vector4(1,1,1,1), Vector3(0,0,0), 1.0f, 0.f));
	createGUIElement("ManabarCounter", m_Graphics->create2D_Object(Vector3(pos.x, pos.y, 2), Vector3(1,1,1), 0.f, m_TextHandle["ManabarCounter"]));

	Vector4 crosshairColor(1.f, 1.f, 1.f, 1.f);
	Vector3 crosshairPosition(0.f, 0.f, 0.f);
	Vector3 crosshairScale(1.f, 1.f, 1.f);
	getHUDSettings("Crosshair", crosshairPosition, crosshairScale);
	getHUDColor("Crosshair", crosshairColor);
	createGUIElement("Crosshair", m_Graphics->create2D_Object(crosshairPosition, Vector2(2.f, 2.f), crosshairScale, 0.f, "Crosshair"));
	m_Graphics->set2D_ObjectColor(m_GUI["Crosshair"], crosshairColor);

	pos = Vector3(0, 0, 0);
	scale = Vector3(2.0f, 2.0f, 2.0f);
	id = "Countdown";
	getHUDSettings(id, pos, scale);
	adjustHUDPosition(pos);
	
	createTextElement("Countdown", m_Graphics->createText(L"", Vector2(180,120), m_GUIFont.c_str(), 72.f, Vector4(1,0,0,1), Vector3(0,0,0), 1.0f, 0.f));
	createGUIElement("Countdown", m_Graphics->create2D_Object(pos, scale, 0.f, m_TextHandle["Countdown"]));

	pos = Vector3(420, 250, 1);
	scale = Vector3(1.f, 1.f, 1.f);
	id = "Time";
	getHUDSettings(id,pos,scale);
	adjustHUDPosition(pos);

	m_TimePosition = pos;
	createTextElement("Time", m_Graphics->createText(L"0.00", Vector2(300.f, 80.f), m_GUIFont.c_str(), 72.f, Vector4(m_Color, 0.f), Vector3(0.f, 0.f, 0.f), 1.f, 0.f));
	createGUIElement("Time", m_Graphics->create2D_Object(m_TimePosition, scale, 0.f, m_TextHandle["Time"]));
	m_Graphics->setTextBackgroundColor(m_TextHandle["Time"], Vector4(m_Color, 0.0f));
	createTextElement("TimeBG", m_Graphics->createText(L"0.00", Vector2(300.f, 80.f), m_GUIFont.c_str(), 72.f, Vector4(m_Color, 0.f), Vector3(0.f, 0.f, 0.f), 1.f, 0.f));
	createGUIElement("TimeBG", m_Graphics->create2D_Object(Vector3(m_TimePosition.x-2, m_TimePosition.y-2, m_TimePosition.z+1), scale, 0.f, m_TextHandle["TimeBG"]));

	createTextElement("TimeFlash", m_Graphics->createText(L"0.00", Vector2(500.f, 300.f), m_GUIFont.c_str(), 72.f, Vector4(1.f, 1.f, 1.f, 0.f), Vector3(0.f, 0.f, 0.f), 1.f, 0.f));
	createGUIElement("TimeFlash", m_Graphics->create2D_Object(Vector3(0.f, 150.f, 0.f), scale, 0.f, m_TextHandle["TimeFlash"]));

	pos = Vector3(-450, 320, 3);
	scale = Vector3(1.0f, 1.0f, 1.0f);
	id = "RacePos";
	getHUDSettings(id,pos,scale);
	adjustHUDPosition(pos);

	createTextElement("RacePos", m_Graphics->createText(L"1st", Vector2(200, 65), m_GUIFont.c_str(), 42, Vector4(m_Color, 1.f), Vector3(0.0f, 0.0f, 0.0f), 1.0f, 0.f));
	createGUIElement("RacePos", m_Graphics->create2D_Object(pos, scale, 0.f, m_TextHandle["RacePos"]));

	createTextElement("RacePosBG", m_Graphics->createText(L"1st", Vector2(204, 69), m_GUIFont.c_str(), 42, Vector4(m_BGColor, 0.8f), Vector3(0.0f, 0.0f, 0.0f), 1.0f, 0.f));
	createGUIElement("RacePosBG", m_Graphics->create2D_Object(Vector3(pos.x-2, pos.y-2, 2), Vector3(1,1,1), 0.f, m_TextHandle["RacePosBG"]));

	pos = Vector3(418, 318, 3);
	scale = Vector3(1.0f, 1.0f, 1.0f);
	id = "Checkpoints";
	getHUDSettings(id,pos,scale);
	adjustHUDPosition(pos);

	createTextElement("Checkpoints", m_Graphics->createText(L"0/0", Vector2(204, 69), m_GUIFont.c_str(), 42, Vector4(m_Color, 0.8f), Vector3(0.0f, 0.0f, 0.0f), 1.0f, 0.f));
	createGUIElement("Checkpoints", m_Graphics->create2D_Object(pos, scale, 0.f, m_TextHandle["Checkpoints"]));
	
	createTextElement("CheckpointsBG", m_Graphics->createText(L"0/0", Vector2(204, 69), m_GUIFont.c_str(), 42, Vector4(m_BGColor, 0.8f), Vector3(0.0f, 0.0f, 0.0f), 1.0f, 0.f));
	createGUIElement("CheckpointsBG", m_Graphics->create2D_Object(Vector3(pos.x-2, pos.y-2, 4), Vector3(1,1,1), 0.f, m_TextHandle["CheckpointsBG"]));


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

	for (int res : m_ResourceIDs)
	{
		m_ResourceManager->releaseResource(res);
	}
	m_ResourceIDs.clear();
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

void HUDScene::getHUDSettings( std::string p_Id, Vector3 &p_Position, Vector3 &p_Scale )
{
	if(m_HUDSettings.count(p_Id) > 0)
	{
		p_Position = m_HUDSettings.at(p_Id).position;
		float s = m_HUDSettings.at(p_Id).scale;
		p_Scale = Vector3(s, s, s);
	}
}

void HUDScene::getHUDColor(const std::string& p_Id, Vector4& p_Color)
{
	const auto it = m_HUDSettings.find(p_Id);
	if (it != m_HUDSettings.end())
	{
		p_Color = it->second.color;
	}
}

void HUDScene::adjustHUDPosition(Vector3 &p_Position)
{
	if(m_Resolution.x > 0.f && m_Resolution.y > 0.f)
	{
		Vector2 origSize(1280,720);
		float percentageX, percentageY;

		percentageX  = (p_Position.x + origSize.x*0.5f) / origSize.x;
		percentageY  = (p_Position.y + origSize.y*0.5f) / origSize.y;
		percentageX = (percentageX > 1.f) ? 1.f : percentageX;
		percentageY = (percentageY > 1.f) ? 1.f : percentageY;

		p_Position.x = percentageX * m_Resolution.x - m_Resolution.x * 0.5f;
		p_Position.y = percentageY * m_Resolution.y - m_Resolution.y * 0.5f;
	}
}
