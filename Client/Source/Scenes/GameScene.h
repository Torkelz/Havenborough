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
	bool m_RenderDebugBV;

	IGraphics *m_Graphics;
	ResourceManager *m_ResourceManager;
	Input *m_InputQueue;

	GameLogic *m_GameLogic;
	EventManager *m_EventManager;

	ActorFactory m_ActorFactory;

	std::vector<int> m_ResourceIDs;
	std::vector<Light> m_Lights;

	struct ReachIK
	{
		std::string rootJoint;
		std::string bendJoint;
		std::string reachJoint;
		Vector3 target;
	};

	struct MeshBinding
	{
		unsigned int meshId;
		int resourceId;
		IGraphics::InstanceId modelId;
		std::vector<ReachIK> activeIKs;
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

	void registeredInput(std::string p_Action, float p_Value, float p_PrevValue) override;

	int	getID() override;

private:
	void addLight(IEventData::Ptr p_Data);
	void createMesh(IEventData::Ptr p_Data);
	void updateModelPosition(IEventData::Ptr p_Data);
	void updateModelRotation(IEventData::Ptr p_Data);
	void updateModelScale(IEventData::Ptr p_Data);
	void playAnimation(IEventData::Ptr p_Data);
	void addReachIK(IEventData::Ptr p_Data);
	void removeReachIK(IEventData::Ptr p_Data);
	
	void renderBoundingVolume(BodyHandle p_BoundingVolume);

	void loadSandboxModels();
	void releaseSandboxModels();
};