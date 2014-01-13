#pragma once

#include "Actor.h"
#include "ActorFactory.h"
#include <IGraphics.h>
#include "Input\Input.h"
#include "IPhysics.h"
#include <INetwork.h>
#include "SceneManager.h"
#include "Window.h"
#include "RAMMemInfo.h"
#include "Subject.h"

#include "ResourceManager.h"

#include <string>

class BaseGameApp
{
private:
	static const std::string m_GameTitle;

	Window	m_Window;
	IGraphics* m_Graphics;
	Input	m_InputQueue;
	RAMMemInfo m_MemoryInfo;
	float m_MemUpdateDelay;
	float m_TimeToNextMemUpdate;

	INetwork* m_Network;

	bool m_ShouldQuit;
	bool m_Connected;

	SceneManager m_SceneManager;
	Subject<std::string> m_Observer;

	IPhysics *m_Physics;
	ResourceManager* m_ResourceManager;

	ActorFactory m_ActorFactory;
	std::vector<Actor::ptr> m_ServerActors;

	__int64 m_PrevTimeStamp;
	__int64 m_CurrTimeStamp;
	float m_SecsPerCnt;
	float m_DeltaTime;

public:
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
	XMFLOAT2 getWindowSize() const;

private:
	bool handleWindowClose(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result);

	static void connectedCallback(Result p_Res, void* p_UserData);

	void updateDebugInfo();

	void resetTimer();
	void updateTimer();
	void handleInput();
	void handleNetwork();
	void updateLogic();
	void render();

	void removeActor(Actor::Id m_Actor);
};
