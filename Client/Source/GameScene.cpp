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

void GameScene::onFrame(int* p_IsCurrentState)
{
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

/*########## TEST FUNCTIONS ##########*/

int GameScene::testOnFrame(int* p_testChange)
{
	if(*p_testChange == 1)
	{
		m_Visible = false;
		return *p_testChange-1;
	}
	else if(*p_testChange == 2)
	{
		m_Visible = false;
		return -1;
	}
	else if(*p_testChange == 3)
	{
		return 0;
	}
	else
	{
		m_Visible = false;
		return 42;
	}
}

int GameScene::getID()
{
	return m_SceneID;
}