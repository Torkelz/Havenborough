#pragma once
#include "IScene.h"
#include "../Settings.h"

class HUDScene : public IScene
{
private:
	int m_SceneID;
	bool m_Visible;
	int  m_NewSceneID;
	bool m_ChangeScene;
	bool m_ChangeList;

	IGraphics *m_Graphics;
	GameLogic *m_GameLogic;
	EventManager *m_EventManager;
	ResourceManager *m_ResourceManager;

	std::vector<int> m_ResourceIDs;

	std::map<std::string, int> m_GUI;
	std::map<std::string, int> m_TextHandle;
	std::map<std::string, Settings::HUDSettings> m_HUDSettings;

	bool m_RenderCountdown;
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

	void setHUDSettings(std::map<std::string, Settings::HUDSettings> p_Settings);

private:
	void createGUIElement(std::string p_GUIIdentifier, int p_Id);
	void createTextElement(std::string p_TextIdentifier, int p_Id);

	void updateGraphicalCountdown(IEventData::Ptr p_Data);
	void updateGraphicalManabar(IEventData::Ptr p_Data);

	void preLoadModels();
	void releasePreLoadedModels();
public:
	/*########## TEST FUNCTIONS ##########*/
	int	getID() override;
};