#pragma once

#include "IScene.h"

class TestScene : public IScene
{
private:
	int m_SceneID;
	bool m_Visible;
public: 
	TestScene();
	~TestScene();
	/**
	* Initialize the scene.
	* @param p_SceneID init the ID in order.
	*/
	bool	init(unsigned int p_SceneID) override;
	/**
	* Destroy the scene.
	*/
	void	destroy() override;
	/**
	* Update scene every frame, used if the scene can change scene.
	* @param p_IsCurrentScene is used if the scene contains buttons to switch scenes with.
	*/
	void	onFrame(int* p_IsCurrentScene);
	/**
	* Update scene every frame, used for scene that can not change scene.
	*/
	void	onFrame();
	/**
	* Render the scene to the screen.
	*/
	void	render() override;
	/**
	* Get if the scene is visible.
	*/
	bool	getIsVisible() override;
	/**
	* Set scene activation and deactivation.
	* @param p_SetVisible true = Active scene, false = deactive scene.
	*/
	void	setIsVisible(bool p_SetVisible) override;
	
	/*########## TEST FUNCTIONS ##########*/
	
	int		testOnFrame(int* p_testChange) override;

	int		getID() override;

private:
};