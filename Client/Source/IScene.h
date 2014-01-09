#pragma once
#include <memory>
#include <string>
#include "IGraphics.h"
#include "ResourceManager.h"
#include "IPhysics.h"
#include "Input\Input.h"
enum MENUSCENES
{
	MENUMAIN,
	MENUOPTION
};
enum RUNSCENES
{
	GAMEMAIN,
	GAMEHUD,
	GAMEPAUSE
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
	virtual bool init(IGraphics *p_Graphics, ResourceManager *p_ResourceManager, IPhysics *p_Physics, Input *p_InputQueue, unsigned int p_SceneID) = 0; 
	/**
	* Destroy the scene.
	*/
	virtual void	destroy() = 0;
	/**
	* Update scene every frame, used if the scene can change scene.
	* @param p_IsCurrentScene is used if the scene contains buttons to switch scenes with. 
	* If p_IsCurrentScene is -1 this switches from game to menu vice versa. 
	*/
	virtual void onFrame(float p_DeltaTime, int* p_IsCurrentScene) = 0;
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
	/**
	* Register and handle key event.
	* @param p_Key key pressed.
	*/
	virtual void	registeredKeyStroke(std::string p_Action, float p_Value) = 0;

	/*########## TEST FUNCTIONS ##########*/

	virtual int		getID() = 0;
};