#pragma once 
#include "TestScene.h"
#include "GameScene.h"
#include <vector>

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
	//Graphics* m_graphics
public:
	SceneManager();
	~SceneManager();
	/**
	* Initialize the scenes and scenemanager.
	*/
	bool		init(/*graphics* p_graphics*/); //ska ta emot en pekare till graphics för att utföra render skickas vidare till IScene scener
	/**
	* Destroy scenes when shutdown.
	*/
	void		destroy();
	/**
	* Update active scenes every frame.
	*/
	void		onFrame();
	/**
	* Render active scenes.
	*/
	void		render();
	/**
	* Set pause screen when game is running.
	*/
	void		setPause();

	/*########## TEST FUNCTIONS ##########*/

	std::vector<IScene::ptr> getScene();

	std::vector<IScene::ptr> testOnFrame(int p_testScene);

private:
	/**
	* Change the scene 
	* @param p_NowShowing gives visibility to a new scene.
	*/
	void		changeScene(int p_NowShowing);
	/**
	* Switching to run scene list.
	*/
	void		startGame();
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
};