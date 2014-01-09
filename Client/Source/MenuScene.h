#pragma once

#include "IScene.h"

class MenuScene : public IScene
{
private:
	int m_SceneID;
	bool m_Visible;
	int  m_NewSceneID;
	bool m_ChangeScene;
	bool m_ChangeList;
public: 
	MenuScene();
	~MenuScene();
	/**
	* Initialize the scene.
	* @param p_SceneID init the ID in order.
	*/
	bool init(IGraphics *p_Graphics, ResourceManager *p_ResourceManager, IPhysics *p_Physics, Input *p_InputQueue, unsigned int p_SceneID) override;
	/**
	* Destroy the scene.
	*/
	void	destroy() override;
	/**
	* Update scene every frame, used if the scene can change scene.
	* @param p_IsCurrentScene is used if the scene contains buttons to switch scenes with.
	*/
	void onFrame(float p_Dt, int* p_IsCurrentScene) override;
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

	void	registeredKeyStroke(std::string p_Action, float p_Value) override;
	
	/*########## TEST FUNCTIONS ##########*/

	int		getID() override;
};