#include "PostGameScene.h"

PostGameScene::PostGameScene()
{
	m_SceneID = 0;
	m_Visible = false;
	m_NewSceneID = 0;
	m_ChangeScene = false;
	m_ChangeList = false;

	m_Graphics = nullptr;
	m_EventManager = nullptr;
	m_ResourceManager = nullptr;

	m_GoalList.push_back(std::make_pair("Player1", "now"));
	m_GoalList.push_back(std::make_pair("Player2", "now+1"));
	m_GoalList.push_back(std::make_pair("Player3", "now+2"));
	m_GoalList.push_back(std::make_pair("Player4", "now+3"));
	m_GoalList.push_back(std::make_pair("Player5", "now+4"));
	m_GoalList.push_back(std::make_pair("Player6", "now+5"));
	m_GoalList.push_back(std::make_pair("Player7", "now+6"));
	m_GoalList.push_back(std::make_pair("Player8", "now+7"));
}

PostGameScene::~PostGameScene()
{
	m_Graphics = nullptr;
	m_EventManager = nullptr;
	m_ResourceManager = nullptr;
}

bool PostGameScene::init(unsigned int p_SceneID, IGraphics *p_Graphics, ResourceManager *p_ResourceManager,
	Input *p_InputQueue, GameLogic *p_GameLogic, EventManager *p_EventManager)
{
	m_SceneID = p_SceneID;

	m_Graphics = p_Graphics;
	m_EventManager = p_EventManager;
	m_ResourceManager = p_ResourceManager;

	//m_EventManager->addListener(EventListenerDelegate(this, &PostGameScene::updateGraphicalCountdown), UpdateGraphicalCountdownEventData::sk_EventType);

	preLoadModels();

	return true;
}

void PostGameScene::destroy()
{
	releasePreLoadedModels();
}

void PostGameScene::onFrame(float p_Dt, int* p_IsCurrentScene)
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

	std::string playerNameText;
	std::string playerTimeText;
	for (const auto& val : m_GoalList)
	{
		playerNameText += val.first + '\n';
		playerTimeText += val.second + '\n';
	}

	std::vector<wchar_t> wText(playerNameText.length() + 1);
	mbstowcs(wText.data(), playerNameText.data(), playerNameText.length() + 1);
	m_Graphics->updateText(m_PlayerNamesText, wText.data());

	wText.resize(playerTimeText.length() + 1);
	mbstowcs(wText.data(), playerTimeText.data(), playerTimeText.length() + 1);
	m_Graphics->updateText(m_PlayerTimesText, wText.data());
}

void PostGameScene::onFocus()
{
	std::shared_ptr<MouseEventDataShow> showMouse(new MouseEventDataShow(false));
	m_EventManager->queueEvent(showMouse);
	std::shared_ptr<MouseEventDataLock> lockMouse(new MouseEventDataLock(true));
	m_EventManager->queueEvent(lockMouse);
}

void PostGameScene::render()
{
	m_Graphics->render2D_Object(m_GoalObject);
	m_Graphics->render2D_Object(m_GoalShadowObject);
	m_Graphics->render2D_Object(m_PlayerNamesObject);
	m_Graphics->render2D_Object(m_PlayerTimesObject);
}

bool PostGameScene::getIsVisible()
{
	return true;
}

void PostGameScene::setIsVisible(bool p_SetVisible)
{
	m_ChangeList = false;

	m_Visible = p_SetVisible;
}

void PostGameScene::registeredInput(std::string p_Action, float p_Value, float p_PrevValue)
{
}

void PostGameScene::preLoadModels()
{
	static const Vector3 darkBlue(0.0274509803921569f, 0.2313725490196078f, 0.3764705882352941f);
	static const Vector3 shadowColor(0.8156862745098039f, 0.8156862745098039f, 0.8156862745098039f);

	static const wchar_t* headerText = L"Goal time";
	static const char* headerFont = "StoneHenge";
	static const float headerFontSize = 100.f;
	static const Vector3 headerPos(0.f, 250.f, 2.f);
	static const Vector3 shadowOffset(-3.f, -2.f, 1.f);

	static const char* listFont = "StoneHenge";
	static const float listFontSize = 24.f;
	static const Vector4 listForground(1.f, 1.f, 1.f, 1.f);
	static const Vector4 listBackground(darkBlue, 0.4f);
	static const Vector2 textureSize(300.f, 580.f);
	static const float listYCenter = -140.f;
	static const float listXCenter = 0.f;
	static const float listXSperatorDistance = 100.f;
	static const float listXOffset = (textureSize.x + listXSperatorDistance) * 0.5f;

	m_GoalText = m_Graphics->createText(headerText, headerFont, headerFontSize, Vector4(darkBlue, 1.f), Vector3(0.f, 0.f, 0.f), 1.0f, 0.f);
	m_Graphics->setTextBackgroundColor(m_GoalText, Vector4(darkBlue, 0.f));
	m_GoalObject = m_Graphics->create2D_Object(headerPos, Vector3(1.f, 1.f, 1.f), 0.f, m_GoalText);
	
	m_GoalShadowText = m_Graphics->createText(headerText, headerFont, headerFontSize, Vector4(shadowColor, 1.f), Vector3(0.f, 0.f, 0.f), 1.0f, 0.f);
	m_Graphics->setTextBackgroundColor(m_GoalShadowText, Vector4(shadowColor, 0.f));
	m_GoalShadowObject = m_Graphics->create2D_Object(headerPos + shadowOffset, Vector3(1.f, 1.f, 1.f), 0.f, m_GoalShadowText);
	
	m_PlayerNamesText = m_Graphics->createText(L"PlayerName", textureSize, listFont, listFontSize, listForground, Vector3(0.f, 0.f, 0.f), 1.f, 0.f);
	m_Graphics->setTextBackgroundColor(m_PlayerNamesText, listBackground);
	m_Graphics->setTextAlignment(m_PlayerNamesText, TEXT_ALIGNMENT::LEADING);
	m_Graphics->setTextParagraphAlignment(m_PlayerNamesText, PARAGRAPH_ALIGNMENT::NEAR_ALIGNMENT);
	m_PlayerNamesObject = m_Graphics->create2D_Object(Vector3(listXCenter - listXOffset, listYCenter, 3.f), Vector3(1.f, 1.f, 1.f), 0.f, m_PlayerNamesText);

	m_PlayerTimesText = m_Graphics->createText(L"Some time", textureSize, listFont, listFontSize, listForground, Vector3(0.f, 0.f, 0.f), 1.f, 0.f);
	m_Graphics->setTextBackgroundColor(m_PlayerTimesText, listBackground);
	m_Graphics->setTextAlignment(m_PlayerTimesText, TEXT_ALIGNMENT::LEADING);
	m_Graphics->setTextParagraphAlignment(m_PlayerTimesText, PARAGRAPH_ALIGNMENT::NEAR_ALIGNMENT);
	m_PlayerTimesObject = m_Graphics->create2D_Object(Vector3(listXCenter + listXOffset, listYCenter, 3.f), Vector3(1.f, 1.f, 1.f), 0.f, m_PlayerTimesText);
}

void PostGameScene::releasePreLoadedModels()
{
	m_Graphics->release2D_Model(m_GoalObject);
	m_Graphics->deleteText(m_GoalText);

	m_Graphics->release2D_Model(m_GoalShadowObject);
	m_Graphics->deleteText(m_GoalShadowText);
	
	m_Graphics->release2D_Model(m_PlayerNamesObject);
	m_Graphics->deleteText(m_PlayerNamesText);

	m_Graphics->release2D_Model(m_PlayerTimesObject);
	m_Graphics->deleteText(m_PlayerTimesText);
}

/*########## TEST FUNCTIONS ##########*/
int PostGameScene::getID()
{
	return m_SceneID;
}
