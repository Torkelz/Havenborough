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
}

void PostGameScene::onFocus()
{
	std::shared_ptr<MouseEventDataShow> showMouse(new MouseEventDataShow(true));
	m_EventManager->queueEvent(showMouse);
	std::shared_ptr<MouseEventDataLock> lockMouse(new MouseEventDataLock(false));
	m_EventManager->queueEvent(lockMouse);
}

void PostGameScene::render()
{

}

bool PostGameScene::getIsVisible()
{
	return m_Visible;
}

void PostGameScene::setIsVisible(bool p_SetVisible)
{
	m_ChangeList = false;

	m_Visible = p_SetVisible;
}

void PostGameScene::registeredInput(std::string p_Action, float p_Value, float p_PrevValue)
{
}

void PostGameScene::createGUIElement(std::string p_GUIIdentifier, int p_Id)
{
}

void PostGameScene::createTextElement(std::string p_TextIdentifier, int p_Id)
{
}

void PostGameScene::preLoadModels()
{
	static const std::string preloadedTextures[] =
	{
		"TEXTURE_NOT_FOUND",
	};
	for (const std::string &texture : preloadedTextures)
	{
		m_ResourceIDs.push_back(m_ResourceManager->loadResource("texture", texture));
	}
}

void PostGameScene::releasePreLoadedModels()
{
	for (int res : m_ResourceIDs)
	{
		m_ResourceManager->releaseResource(res);
	}
	m_ResourceIDs.clear();
}

/*########## TEST FUNCTIONS ##########*/
int PostGameScene::getID()
{
	return m_SceneID;
}
