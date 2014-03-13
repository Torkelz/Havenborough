#pragma once
#include "IScene.h"
#include "ISound.h"
#include "../GameLogic.h"

#include <random>
#include <LightClass.h>

class GameScene : public IScene
{
private:
	int m_ExtraParticleID;
	int m_SceneID;
	bool m_Visible;
	int  m_NewSceneID;
	bool m_ChangeScene;
	bool m_ChangeList;
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
	std::vector<LightClass> m_Lights;

	unsigned int m_CurrentDebugView;
	std::vector<IGraphics::RenderTarget> m_SelectableRenderTargets;

	std::vector<std::string> m_BackGroundSoundsList;
	std::string m_SoundFolderPath;
	std::string m_SoundPath;
	std::default_random_engine m_RandomEngine;
	ISound *m_SoundManager;
	bool m_SoundExist;

	float m_ViewSensitivity;

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

	//Text Component id, Graphics Text id
	std::map<unsigned int, unsigned int> m_WorldText;

	bool m_RenderPause;
	unsigned int m_PauseId;
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
	
	/**
	* Sets the sensitivity factor for the view movement.
	* @param p_Value the factor to use
	*/
	void setMouseSensitivity(float p_Value);

	void setSoundManager(ISound *p_SoundManager);
private:
	std::string changeBackGroundSound(const std::string& p_FontFolderPath);

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
	void spellHit(IEventData::Ptr p_Data);
	void create3DSound(IEventData::Ptr p_Data);
	void update3DSound(IEventData::Ptr p_Data);
	void release3DSound(IEventData::Ptr p_Data);


	void createWorldText(IEventData::Ptr p_Data);
	void removeWorldText(IEventData::Ptr p_Data);
	void updateWorldTextPosition(IEventData::Ptr p_Data);

	void renderBoundingVolume(BodyHandle p_BoundingVolume);

	void preLoadModels();
	void releasePreLoadedModels();
};