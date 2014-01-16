#pragma once
#include "IScene.h"
//#include "../Logger.h"
#include "../GameLogic.h"
#include "../LightStructs.h"

class GameScene : public IScene
{
private:
	int m_SceneID;
	bool m_Visible;
	int  m_NewSceneID;
	bool m_ChangeScene;
	bool m_ChangeList;
	int m_CurrentDebugView;
	
	IGraphics *m_Graphics;
	IPhysics *m_Physics;
	ResourceManager *m_ResourceManager;
	Input *m_InputQueue;

	GameLogic *m_GameLogic;
	EventManager *m_EventManager;

	ActorFactory m_ActorFactory;

	std::vector<int> m_ResourceIDs;
	std::vector<Light> m_Lights;

	struct MeshBinding
	{
		unsigned int meshId;
		IGraphics::InstanceId modelId;
	};
	std::vector<MeshBinding> m_Models;

public: 
	GameScene();
	~GameScene();

	bool init(unsigned int p_SceneID, IGraphics *p_Graphics, ResourceManager *p_ResourceManager,
		Input *p_InputQueue, GameLogic *p_GameLogic, EventManager *p_EventManager) override;
	void destroy() override;

	void onFrame(float p_DeltaTime, int* p_IsCurrentScene) override;
	void render() override;

	bool getIsVisible() override;
	void setIsVisible(bool p_SetVisible) override;

	void registeredInput(std::string p_Action, float p_Value) override;

	int	getID() override;

private:
	void addLight(IEventData::Ptr p_Data);
	void createMesh(IEventData::Ptr p_Data);
	void updateModelPosition(IEventData::Ptr p_Data);
	void updateModelRotation(IEventData::Ptr p_Data);
	void updateModelScale(IEventData::Ptr p_Data);
	void loadSandboxModels();
	void releaseSandboxModels();
};