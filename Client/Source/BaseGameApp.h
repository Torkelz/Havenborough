#pragma once

#include <IGraphics.h>
#include "Input\Input.h"
#include "IPhysics.h"
#include "NetworkClient.h"
#include "SceneManager.h"
#include "Window.h"

#include "ResourceManager.h"

#include <string>

class BaseGameApp
{
private:
	static const std::string m_GameTitle;

	Window	m_Window;
	IGraphics* m_Graphics;
	Input	m_InputQueue;

	NetworkClient m_Network;

	bool	m_ShouldQuit;
	SceneManager m_SceneManager;

	std::string m_ProjectDirectory;

	BodyHandle m_Body, m_Object;
	IPhysics *m_Physics;

	ResourceManager* m_ResourceManager;

	float dt;
public:
	/**
	 * Initialize the game and create a window.
	 */
	void init(std::string p_ProjectDirectory);
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
};
