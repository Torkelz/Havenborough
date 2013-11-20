#pragma once
#include <memory>
enum MENUSCENES
{
	MAINMENU,
	MENUOPTION
};
enum RUNSCENES
{
	GAME,
	HUD,
	PAUSE
};
class IScene
{
private:
public: 
	typedef std::shared_ptr<IScene> ptr;
	/**
	* Initialize the scene.
	* @param p_SceneID init the ID in order.
	*/
	virtual bool	init(unsigned int p_SceneID) = 0; 
	/**
	* Destroy the scene.
	*/
	virtual void	destroy() = 0;
	/**
	* Update scene every frame, used if the scene can change scene.
	* @param p_IsCurrentScene is used if the scene contains buttons to switch scenes with.
	*/
	virtual void	onFrame(int* p_IsCurrentScene) = 0;
	/**
	* Update scene every frame, used for scene that can not change scene.
	*/
	virtual void	onFrame() = 0;
	/**
	* Render the scene to the screen.
	*/
	virtual void	render() = 0;
	/**
	* Get if the scene is visible.
	*/
	virtual bool	getIsVisible() = 0;
	/**
	* Set scene activation and deactivation.
	* @param p_SetVisible true = Active scene, false = deactive scene.
	*/
	virtual void	setIsVisible(bool p_SetVisible) = 0;

	/*########## TEST FUNCTIONS ##########*/

	virtual int		testOnFrame(int* p_testChange) = 0;

	virtual int		getID() = 0;

private:
};