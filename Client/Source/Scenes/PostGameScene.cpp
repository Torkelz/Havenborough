#include "PostGameScene.h"

PostGameScene::PostGameScene()
{
	m_SceneID = 0;
	m_Visible = false;
	m_NewSceneID = 0;
	m_ChangeScene = false;
	m_ChangeList = false;

	m_Graphics = nullptr;
}

PostGameScene::~PostGameScene()
{
	m_Graphics = nullptr;
}

bool PostGameScene::init(unsigned int p_SceneID, IGraphics *p_Graphics, ResourceManager *p_ResourceManager, IPhysics *p_Physics,
	Input *p_InputQueue, INetwork *p_Network)
{
	m_SceneID = p_SceneID;
	m_Graphics = p_Graphics;
	
	return true;
}

void PostGameScene::destroy()
{

}

void PostGameScene::onFrame(float p_DeltaTime, int* p_IsCurrentScene)
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

void PostGameScene::render()
{
	m_Graphics->setClearColor(Vector4(1, 0, 0, 1));
	m_Graphics->drawFrame(-1);
}

bool PostGameScene::getIsVisible()
{
	return m_Visible;
}

void PostGameScene::setIsVisible(bool p_SetVisible)
{
	m_Visible = p_SetVisible;
}

void PostGameScene::registeredInput(std::string p_Action, float p_Value)
{
	if(p_Action == "changeSceneN" && p_Value == 1)
	{
		if(m_SceneID == 0)
		{
			m_NewSceneID = (int)MenuScenes::OPTION;
		}
		else if(m_SceneID == 1)
		{
			m_NewSceneID = (int)MenuScenes::MAIN; 
		}
		m_ChangeScene = true;
	}
	if(p_Action == "goToMainMenu" && p_Value == 1)
	{
		m_ChangeList = true;
	}
	else if(p_Action == "changeSceneP" && p_Value == 1)
	{
		m_ChangeList = true;
	}
}

/*########## TEST FUNCTIONS ##########*/

int PostGameScene::getID()
{
	return m_SceneID;
}