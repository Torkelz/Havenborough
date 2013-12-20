#pragma once

#include <IGraphics.h>
#include <INetwork.h>
#include <IPhysics.h>

#include "Input\Input.h"
#include "SceneManager.h"
#include "Window.h"
#include "RAMInfo.h"
#include "Level.h"
#include "Player.h"
#include "EdgeCollisionResponse.h"

#include "ResourceManager.h"

#include <string>

class BaseGameApp
{
private:
	static const std::string m_GameTitle;

	Window	m_Window;
	IGraphics *m_Graphics;
	Level m_Level;
	Input	m_InputQueue;
	RAMInfo m_MemoryInfo;
	float m_MemUpdateDelay;
	float m_TimeToNextMemUpdate;

	INetwork* m_Network;

	bool	m_ShouldQuit;
	bool	m_Connected;

	SceneManager m_SceneManager;

	BodyHandle m_Ground;
	IPhysics *m_Physics;
	ResourceManager* m_ResourceManager;

	std::vector<int> m_ResourceIDs;

	Player m_Player;
	EdgeCollisionResponse m_EdgeCollResponse;

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

	void updateDebugInfo(float p_dt);
};
