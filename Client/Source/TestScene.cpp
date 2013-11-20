#include "TestScene.h"

TestScene::TestScene()
{
	m_SceneID = 0;
	m_Visible = false;
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
}

void TestScene::onFrame()
{ 
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

/*########## TEST FUNCTIONS ##########*/

int TestScene::testOnFrame(int* p_testChange)
{
	if(*p_testChange == 1)
	{
		m_Visible = false;
		return *p_testChange;
	}
	else if(*p_testChange == 2 || *p_testChange == 3)
	{
		m_Visible = false;
		return -1;
	}
	else
	{
		m_Visible = false;
		return 42;
	}
}

int TestScene::getID()
{
	return m_SceneID;
}