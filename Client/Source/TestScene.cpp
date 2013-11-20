#include "TestScene.h"

TestScene::TestScene()
{
	m_SceneID = 0;
	m_Visible = false;
	m_NewSceneID = 0;
	m_ChangeScene = false;
	m_ChangeList = false;
}

TestScene::~TestScene()
{
}

bool TestScene::init(unsigned int p_SceneID)
{
	m_SceneID = p_SceneID;
	return true;
}

void TestScene::destroy()
{
}

void TestScene::onFrame(int* p_IsCurrentScene)
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

void TestScene::render()
{
}

bool TestScene::getIsVisible()
{
	return m_Visible;
}

void TestScene::setIsVisible(bool p_SetVisible)
{
	m_Visible = p_SetVisible;
}

void TestScene::registeredKeyStroke(char* p_Key)
{
	if(*p_Key == 'L')
	{
		if(m_SceneID == 0)
		{
			m_NewSceneID = MENUOPTION;
		}
		else if(m_SceneID == 1)
		{
			m_NewSceneID = MENUMAIN; 
		}
		m_ChangeScene = true;
	}
	else if(*p_Key == 'J')
	{
		m_ChangeList = true;
	}
}

/*########## TEST FUNCTIONS ##########*/

int TestScene::getID()
{
	return m_SceneID;
}

