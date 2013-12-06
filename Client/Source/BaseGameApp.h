#pragma once

#include <IGraphics.h>
#include "Input\Input.h"
#include "IPhysics.h"
#include <INetwork.h>
#include "SceneManager.h"
#include "Window.h"
#include "RAMMemInfo.h"

#include <string>

#include "../../Graphics/Source/WrapperFactory.h" //Should be removed when merge to master

class BaseGameApp
{
private:
	static const std::string m_GameTitle;

	Window	m_Window;
	IGraphics* m_Graphics;
	Input	m_InputQueue;
	RAMMemInfo m_MemoryInfo;

	INetwork* m_Network;

	bool	m_ShouldQuit;
	bool	m_Connected;

	SceneManager m_SceneManager;

	BodyHandle m_Body, m_Object;
	IPhysics *m_Physics;

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
	UVec2 getWindowSize() const;

private:
	bool handleWindowClose(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result);

	static void connectedCallback(Result p_Res, void* p_UserData);

	void updateDebugInfo(float p_dt);
};
