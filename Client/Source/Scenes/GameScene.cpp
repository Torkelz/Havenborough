#include "GameScene.h"

GameScene::GameScene()
{
	m_SceneID = 0;
	m_Visible = false;
	m_NewSceneID = 0;
	m_ChangeScene = false;
	m_ChangeList = false;
	
	m_GameLogic = nullptr;
	m_Graphics = nullptr;
	m_Physics = nullptr;
	m_InputQueue = nullptr;
	m_Network = nullptr;
}

GameScene::~GameScene()
{
	SAFE_SHUTDOWN(m_GameLogic);
	m_Graphics = nullptr;
	m_InputQueue = nullptr;
}

bool GameScene::init(unsigned int p_SceneID, IGraphics *p_Graphics, ResourceManager *p_ResourceManager,
	IPhysics *p_Physics, Input *p_InputQueue, INetwork *p_Network)
{
	m_SceneID = p_SceneID;
	m_Graphics = p_Graphics;
	m_Physics = p_Physics;
	m_InputQueue = p_InputQueue;
	m_ResourceManager = p_ResourceManager;
	m_Network = p_Network;
	
	return true;
}

void GameScene::destroy()
{
	SAFE_SHUTDOWN(m_GameLogic);
}

void GameScene::onFrame(float p_DeltaTime, int* p_IsCurrentScene)
{
	m_GameLogic->onFrame(p_DeltaTime);
	if(m_GameLogic->getChangeScene() != GameLogic::GoToScene::NONE)
	{
		m_ChangeScene = true;
		m_NewSceneID = (int)m_GameLogic->getChangeScene();
		
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
}

void GameScene::render()
{
	if(m_GameLogic)
		m_GameLogic->render();
}

bool GameScene::getIsVisible()
{
	return m_Visible;
}

void GameScene::setIsVisible(bool p_SetVisible)
{
	m_Visible = p_SetVisible;
}

void GameScene::registeredInput(std::string p_Action, float p_Value)
{
	if(p_Action == "changeSceneN" && p_Value == 1)
	{
		m_NewSceneID = (int)RunScenes::GAMEPAUSE;
		m_ChangeScene = true;
	}
	else if(p_Action == "changeSceneP" && p_Value == 1)
	{
		m_ChangeList = true;
	}

	m_GameLogic->registeredInput(p_Action, p_Value);
}

void GameScene::initializeGameLogic(void)
{
	if(m_GameLogic)
	{
		m_GameLogic->shutdown();
		SAFE_DELETE(m_GameLogic);
	}
	m_GameLogic = new GameLogic();
	m_GameLogic->initialize(m_Graphics, m_ResourceManager, m_Physics, m_InputQueue, m_Network);
}

/*########## TEST FUNCTIONS ##########*/

int GameScene::getID()
{
	return m_SceneID;
}

GameLogic* GameScene::getGameLogic()
{
	return m_GameLogic;
}
