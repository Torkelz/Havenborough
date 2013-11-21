#pragma once

#include "Window.h"
#include "SceneManager.h"
#include "Sphere.h"
#include "IPhysics.h"

#include <string>

class BaseGameApp
{
private:
	static const std::string m_GameTitle;

	Window m_Window;
	bool m_ShouldQuit;
	SceneManager m_SceneManager;

	// TESTING SHIET
	Sphere sphere1, sphere2;
	AABB aabb, aabbb;

	IPhysics::ptr physics;

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
	bool handleKeyDown(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result);
};
