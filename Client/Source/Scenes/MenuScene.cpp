#include "MenuScene.h"

MenuScene::MenuScene()
{
	m_SceneID = 0;
	m_Visible = false;
	m_NewSceneID = 0;
	m_ChangeScene = false;
	m_ChangeList = false;

	m_Graphics = nullptr;
}

MenuScene::~MenuScene()
{
	m_Graphics = nullptr;
}

bool MenuScene::init(IGraphics *p_Graphics, ResourceManager *p_ResourceManager, IPhysics *p_Physics, Input *p_InputQueue, unsigned int p_SceneID)
{
	m_SceneID = p_SceneID;

	m_Graphics = p_Graphics;
	return true;
}

void MenuScene::destroy(){}

void MenuScene::onFrame(float p_Dt, int* p_IsCurrentScene)
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

void MenuScene::render()
{
	m_Graphics->drawFrame(3);
}

bool MenuScene::getIsVisible()
{
	return m_Visible;
}

void MenuScene::setIsVisible(bool p_SetVisible)
{
	m_Visible = p_SetVisible;
}

void MenuScene::registeredKeyStroke(std::string p_Action, float p_Value)
{
	if(p_Action == "changeSceneN" && p_Value == 1)
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
	else if(p_Action == "changeSceneP" && p_Value == 1)
	{
		m_ChangeList = true;
	}
}

/*########## TEST FUNCTIONS ##########*/

int MenuScene::getID()
{
	return m_SceneID;
}

