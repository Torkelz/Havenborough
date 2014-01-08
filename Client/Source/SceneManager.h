#pragma once 
#include "MenuScene.h"
#include "GameScene.h"
#include <vector>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/*TESTING*/
#include <iostream>

class SceneManager
{
private:
	std::vector<IScene::ptr>		m_MenuSceneList;
	std::vector<IScene::ptr>		m_RunSceneList;
	unsigned int					m_NumberOfMenuScene;
	unsigned int					m_NumberOfRunScene;
	int								m_NowShowing;
	/**
	* Changes render and update lists of scenes.
	* true = Menu(m_MenuSceneList)
	* false = Run(m_RunSceneList)
	*/
	bool							m_IsMenuState;
	IGraphics						*m_Graphics;
	ResourceManager					*m_ResourceManager;
	IPhysics						*m_Physics;
	Input							*m_InputQueue;
public:
	SceneManager();
	~SceneManager();
	/**
	* Initialize the scenes and scenemanager.
	* @return true if initialize worked, false if the initialization failed.
	*/
	bool init(IGraphics *p_Graphics, ResourceManager *p_ResourceManager, IPhysics *p_Physics,
		Input *p_InputQueue);
	/**
	* Destroy scenes when shutdown.
	*/
	void destroy();
	/**
	* Update active scenes every frame.
	*/
	void onFrame(float p_DeltaTime);
	/**
	* Render active scenes.
	*/
	void render();
	/**
	* Pass along keystroke to a scene.
	* @param p_WParam = Keyvalue.
	* @param p_LParam Unused but needed.
	* @param p_Result Unused but needed.
	*/
	bool keyStroke(std::string p_Action, float p_Value);
	/*########## TEST FUNCTIONS ##########*/

	std::vector<IScene::ptr> getScene();

private:
	/**
	* Change the scene 
	* @param p_NowShowing gives visibility to a new scene.
	*/
	void		changeScene(float p_DeltaTime, int p_NowShowing);
	/**
	* Switching to run scene list.
	*/
	void		startRun();
	/**
	* Switching to menu scene list.
	*/
	void		startMenu();
	/**
	* Update menu scenes
	*/
	void		onFrameMenu();
	/**
	* Update game scenes
	*/
	void		onFrameRun();
	/**
	* Set pause screen when game is running.
	*/
	void		setPause();
	/**
	* Passes along the a keystroke to active scene
	* @param p_Key key pressed.
	*/
	void passKeyStroke(std::string p_Action, float p_Value);
};