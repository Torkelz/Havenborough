#include "GameScene.h"

GameScene::GameScene()
{
	m_SceneID = 0;
	m_Visible = false;
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
		p_IsCurrentScene = &m_NewSceneID;
		m_Visible = false;
		m_ChangeScene = false;
	}
}

void GameScene::onFrame()
{
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

void GameScene::registeredKeyStroke(char p_Key)
{
	if(p_Key == 'L')
	{
		m_NewSceneID = GAMEPAUSE;
		m_ChangeScene = true;
	}
}

/*########## TEST FUNCTIONS ##########*/

int GameScene::getID()
{
	return m_SceneID;
}