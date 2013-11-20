#pragma once

#include "Window.h"
#include "IGraphics.h"

#include <string>

class BaseGameApp
{
private:
	static const std::string m_GameTitle;

	Window m_Window;
	IGraphics* m_Graphics;

	bool m_ShouldQuit;

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
