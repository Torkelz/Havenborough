#pragma once
#include "IScene.h"
#include "../GameLogic.h"
#include <Light.h>

class GameScene : public IScene
{
private:
	int m_SceneID;
	bool m_Visible;
	int  m_NewSceneID;
	bool m_ChangeScene;
	bool m_ChangeList;
	IGraphics::RenderTarget m_CurrentDebugView;
	IGraphics::Object2D_Id m_GUI_ArrowId;
	bool m_RenderDebugBV;
	int m_SkyboxID;

	bool m_UseThirdPersonCamera;
	bool m_UseFlippedCamera;
	bool m_DebugAnimations;

	IGraphics *m_Graphics;
	ResourceManager *m_ResourceManager;
	Input *m_InputQueue;
	INetwork *m_Network;

	GameLogic *m_GameLogic;
	EventManager *m_EventManager;

	std::vector<int> m_ResourceIDs;
	std::vector<Light> m_Lights;

	struct ReachIK
	{
		std::string group;
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

	struct ParticleBinding
	{
		std::string effectName;
		IGraphics::InstanceId instance;
	};
	std::map<unsigned int, ParticleBinding> m_Particles;

	struct SpellBinding
	{
		unsigned int spellId;
		int resourceId;
		BodyHandle body;
	};
	std::vector<SpellBinding> m_Spells;

public: 
	GameScene();
	~GameScene();

	bool init(unsigned int p_SceneID, IGraphics *p_Graphics, ResourceManager *p_ResourceManager,
		Input *p_InputQueue, GameLogic *p_GameLogic, EventManager *p_EventManager) override;
	void destroy() override;

	void onFrame(float p_DeltaTime, int* p_IsCurrentScene) override;
	void onFocus() override;
	void render() override;

	bool getIsVisible() override;
	void setIsVisible(bool p_SetVisible) override;

	void registeredInput(std::string p_Action, float p_Value, float p_PrevValue) override;

	int	getID() override;

private:
	void addLight(IEventData::Ptr p_Data);
	void removeLight(IEventData::Ptr p_Data);
	void createMesh(IEventData::Ptr p_Data);
	void removeMesh(IEventData::Ptr p_Data);
	void updateModelPosition(IEventData::Ptr p_Data);
	void updateModelRotation(IEventData::Ptr p_Data);
	void updateModelScale(IEventData::Ptr p_Data);
	void updateAnimation(IEventData::Ptr p_Data);
	void changeColorTone(IEventData::Ptr p_Data);
	void createParticleEffect(IEventData::Ptr p_Data);
	void removeParticleEffectInstance(IEventData::Ptr p_Data);
	void updateParticlePosition(IEventData::Ptr p_Data);
	void updateParticleRotation(IEventData::Ptr p_Data);
	void updateParticleBaseColor(IEventData::Ptr p_Data);
	
	void renderBoundingVolume(BodyHandle p_BoundingVolume);

	void preLoadModels();
	void releasePreLoadedModels();
};