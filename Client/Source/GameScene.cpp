#include "GameScene.h"

GameScene::GameScene()
{
	m_SceneID = 0;
	m_Visible = false;
	m_ChangeScene = false;
	m_ChangeList = false;
}

GameScene::~GameScene()
{
}

bool GameScene::init(unsigned int p_SceneID)
{
	m_SceneID = p_SceneID;
	return true;
}

void GameScene::destroy()
{
}

void GameScene::onFrame(int* p_IsCurrentScene)
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

void GameScene::render()
{
}

bool GameScene::getIsVisible()
{
	return m_Visible;
}

void GameScene::setIsVisible(bool p_SetVisible)
{
	m_Visible = p_SetVisible;
}

void GameScene::registeredKeyStroke(char* p_Key)
{
	if(*p_Key == 'L')
	{
		m_NewSceneID = GAMEPAUSE;
		m_ChangeScene = true;
	}
	else if(*p_Key == 'J')
	{
		m_ChangeList = true;
	}
}

/*########## TEST FUNCTIONS ##########*/

int GameScene::getID()
{
	return m_SceneID;
}