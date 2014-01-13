#pragma once
#include "IScene.h"
//#include "../Logger.h"
#include "../GameLogic.h"

class GameScene : public IScene
{
private:
	int m_SceneID;
	bool m_Visible;
	int  m_NewSceneID;
	bool m_ChangeScene;
	bool m_ChangeList;
	
	IGraphics *m_Graphics;
	IPhysics *m_Physics;
	ResourceManager *m_ResourceManager;
	Input *m_InputQueue;

	GameLogic *m_GameLogic;

public: 
	GameScene();
	~GameScene();

	bool init(unsigned int p_SceneID, IGraphics *p_Graphics, ResourceManager *p_ResourceManager,
		IPhysics *p_Physics, Input *p_InputQueue) override;
	void destroy() override;

	void onFrame(float p_DeltaTime, int* p_IsCurrentScene) override;
	void render() override;

	bool getIsVisible() override;
	void setIsVisible(bool p_SetVisible) override;

	void registeredInput(std::string p_Action, float p_Value) override;

	void initializeGameLogic(void);

	int	getID() override;
};