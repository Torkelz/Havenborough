#pragma once
#include "IScene.h"
#include "Logger.h"
#include "Level.h"
#include "Player.h"

class GameScene : public IScene
{
private:
	int m_SceneID;
	bool m_Visible;
	int  m_NewSceneID;
	bool m_ChangeScene;
	bool m_ChangeList;
	
	IGraphics *m_Graphics;
	ResourceManager *m_ResourceManager;
	IPhysics *m_Physics;
	Level m_Level;
	Player m_Player;
	Input *m_InputQueue;

	BodyHandle m_Ground;

	std::vector<int> m_ResourceIDs;

	//DEBUG
	int currView;
	int climbBox;
	int jointBox;
	int skyBox;
	int ground;
	int circleWitch;
	int standingWitch;
	int wavingWitch;
	int ikTest;
	int slantedPlane;
	int OBBhouse1;
	int OBBhouse2;
	const static int NUM_BOXES = 16;
	int boxIds[NUM_BOXES];
	bool useIK_OnIK_Worm;
	float yaw;
	float yawSpeed;
	float pitch;
	float pitchSpeed;
	float roll;
	float rollSpeed;
	static const unsigned int numTowerBoxes = 5;
	int towerBoxes[numTowerBoxes];
	static const unsigned int numRotatedTowerBoxes = 5;
	int rotatedTowerBoxes[numRotatedTowerBoxes];
	float viewRot[2];
	float sensitivity;
public: 
	GameScene();
	~GameScene();
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
	void onFrame(float p_DeltaTime, int* p_IsCurrentScene) override;
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

private:
	void InitTemporaryStuff();
};