#pragma once
#include <IGraphics.h>
#include <IPhysics.h>
#include <INetwork.h>
#include <ISound.h>
#include "ActorFactory.h"
#include "Input\Input.h"
#include "SceneManager.h"
#include "Window.h"
#include "RAMInfo.h"
#include "EventManager.h"
#include "ResourceManager.h"
#include "GameLogic.h"

#include <DirectXMath.h>

class BaseGameApp
{
private:
	static const std::string m_GameTitle;

	Window	m_Window;
	IGraphics* m_Graphics;
	Input	m_InputQueue;
	RAMInfo m_MemoryInfo;
	float m_MemUpdateDelay;
	float m_TimeToNextMemUpdate;

	INetwork* m_Network;

	bool m_ShouldQuit;

	SceneManager m_SceneManager;
	std::unique_ptr<EventManager> m_EventManager;

	IPhysics *m_Physics;
	std::unique_ptr<ResourceManager> m_ResourceManager;

	ActorFactory m_ActorFactory;
	DirectX::XMFLOAT2 m_NewWindowSize;

	ISound *m_Sound;
	__int64 m_PrevTimeStamp;
	__int64 m_CurrTimeStamp;
	float m_SecsPerCnt;
	float m_DeltaTime;
	
	std::unique_ptr<GameLogic> m_GameLogic;
public:
	/**
	 * constructor.
	 */
	BaseGameApp();

	/**
	 * Initialize the game and create a window.
	 */
	void init();
	/**
	 * Run the game. Doesn't return until the game is exited.
	 */
	void run();


	/**
	 * Shutdown the game and cleanup any resources.
	 */
	void shutdown();

	/**
	 * Get the title of the game.
	 *
	 * @return Name of the game.
	 */
	std::string getGameTitle() const;
	/**
	 * Get the size of the window.
	 *
	 * @return The size (width, height) of the client area of the window.
	 */
	DirectX::XMFLOAT2 getWindowSize() const;

private:
	bool handleWindowClose(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result);
	
	bool handleWindowExitSizeMove(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result);

	bool handleWindowSize(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result);

	void updateDebugInfo();

	void resetTimer();
	void updateTimer();
	void handleInput();
	void updateLogic();
	void render();
	
	void startGame(IEventData::Ptr p_Data);
	void gameLeft(IEventData::Ptr p_Data);
	void quitGame(IEventData::Ptr p_Data);
	void showMouse(IEventData::Ptr p_Data);
};
