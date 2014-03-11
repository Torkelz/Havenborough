#pragma once
#include "IScene.h"
#include "../DebugInfo.h"
#include "../Settings.h"

class HUDScene : public IScene
{
private:
	int m_SceneID;
	bool m_Visible;
	int  m_NewSceneID;
	bool m_ChangeScene;
	bool m_ChangeList;
	bool m_ShowDebugInfo;
	unsigned int m_NumberOfCheckpoints;
	
	std::string m_TakenCheckpoints;
	Vector3 m_Color, m_BGColor, m_TimePosition, m_TimeScale;
	bool m_FadeOut;
	float m_TimeTimerMax, m_TimeTimerStartFade, m_TimePositionFade;
	float m_TimeTimerCurrent, m_TimePositionCurrent;
	float m_TimeFlashFade, m_TimeFlashFadeMax;
	bool m_FadeOutFlash;
	float m_ManaCost, m_CurrentMana;
	float m_TimePulse, m_TimePulseFade, m_TimePulseMax;
	bool m_Pulse, m_Hej;

	std::string m_GUIFont;
	IGraphics *m_Graphics;
	EventManager *m_EventManager;
	ResourceManager *m_ResourceManager;

	std::vector<int> m_ResourceIDs;

	std::map<std::string, int> m_GUI;
	std::map<std::string, int> m_TextHandle;
	std::map<std::string, Settings::HUDSettings> m_HUDSettings;

	DebugInfo m_DebugInfo;

	bool m_RenderCountdown;
	bool m_RenderHUD;
	Vector3 m_CheckpointPosition;
	Vector2 m_Resolution;
public: 
	HUDScene();
	~HUDScene();

	bool init(unsigned int p_SceneID, IGraphics *p_Graphics, ResourceManager *p_ResourceManager,
		Input *p_InputQueue, GameLogic *p_GameLogic, EventManager *p_EventManager) override;

	void destroy() override;

	void onFrame(float p_DeltaTime, int* p_IsCurrentScene) override;
	void onFocus() override;

	void render() override;
		 
	bool getIsVisible() override;
		 
	void setIsVisible(bool p_SetVisible) override;
		 
	void registeredInput(std::string p_Action, float p_Value, float p_PrevValue) override;


	/*
	 * Sets settings read from Settings.cpp to use with the hud.
	 * @param p_Settings all settings read from Useroptions.xml
	 */
	void setHUDSettings(std::map<std::string, Settings::HUDSettings> p_Settings, Vector2 p_ScreenResolution);

	/**
	 * Gets the debug info 
	 *
	 * @return the debug info object used to print info to the hud
	 */
	DebugInfo& getDebugInfo();

private:
	void createGUIElement(std::string p_GUIIdentifier, int p_Id);
	void createTextElement(std::string p_TextIdentifier, int p_Id);

	void updateGraphicalCountdown(IEventData::Ptr p_Data);
	void updateGraphicalManabar(IEventData::Ptr p_Data);
	void updateCheckpointPosition(IEventData::Ptr p_Data);
	void updatePlayerTime(IEventData::Ptr p_Data);
	void updatePlayerRacePosition(IEventData::Ptr p_Data);
	void updateTakenCheckpoints(IEventData::Ptr p_Data);
	void activateHUD(IEventData::Ptr p_Data);
	void setNrOfCheckpoints(IEventData::Ptr p_Data);
	void onFinish(IEventData::Ptr p_Data);

	void preLoadModels();
	void releasePreLoadedModels();

	void getHUDSettings( std::string p_Id, Vector3 &p_Position, Vector3 &p_Scale );
	void adjustHUDPosition(Vector3 &p_Position);

public:
	/*########## TEST FUNCTIONS ##########*/
	int	getID() override;

	void createGUIElementTest(std::string p_GUIIdentifier, int p_Id);
	void createTextElementTest(std::string p_TextIdentifier, int p_Id);
};