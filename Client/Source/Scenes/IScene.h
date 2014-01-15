#pragma once

#include "IGraphics.h"
#include "IPhysics.h"
#include "../ClientExceptions.h"
#include "../GameLogic.h"
#include "../Input/Input.h"
#include "../ResourceManager.h"

#include <memory>
#include <string>

enum class MenuScenes
{
	MAIN,
	OPTION
};
enum class RunScenes
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
	* @param p_SceneID init the ID in order
	* @param p_Graphics pointer to the graphics engine
	* @param p_ResourceManager pointer to the resource manager
	* @param p_Physics p_InputQueue pointer to the keyboard and mouse input queue
	*/
	virtual bool init(unsigned int p_SceneID, IGraphics *p_Graphics, ResourceManager *p_ResourceManager,
		Input *p_InputQueue, GameLogic *p_GameLogic) = 0;

	/**
	* Destroy the scene.
	*/
	virtual void destroy() = 0;
	
	/**
	* Update scene every frame, used if the scene can change scene.
	* @param p_DeltaTime the time between two frames
	* @param p_IsCurrentScene is used if the scene contains buttons to switch scenes with,
	*	if p_IsCurrentScene is -1 this switches from game to menu vice versa
	*/
	virtual void onFrame(float p_DeltaTime, int* p_IsCurrentScene) = 0;
	
	/**
	* Render the scene to the screen.
	*/
	virtual void render() = 0;
	
	/**
	* Get if the scene is visible.
	* @return true if visible, false if not visible
	*/
	virtual bool getIsVisible() = 0;
	
	/**
	* Set scene activation and deactivation.
	* @param p_SetVisible true = Active scene, false = Inactive scene.
	*/
	virtual void setIsVisible(bool p_SetVisible) = 0;
	
	/**
	* Register and handle keyboard and mouse input.
	* @param p_Action the action to take
	* @param p_Value the value for the action
	*/
	virtual void registeredInput(std::string p_Action, float p_Value) = 0;

	/*########## TEST FUNCTIONS ##########*/

	virtual int	getID() = 0;
};