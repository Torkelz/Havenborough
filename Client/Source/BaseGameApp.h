#pragma once

#include "Input\Input.h"
#include "Window.h"
#include "IGraphics.h"
#include "SceneManager.h"
#include "IPhysics.h"

#include <string>

class BaseGameApp
{
private:
	static const std::string m_GameTitle;

	Window	m_Window;
	IGraphics* m_Graphics;
	Input	m_InputQueue;

	bool	m_ShouldQuit;
	SceneManager m_SceneManager;

	//TEMP
	Buffer	*m_Buffer, *m_CBuffer;
	Shader	*m_Shader;

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



	//TEMP --------------------------------------------------
	#define PI (3.14159265358979323846f)
	struct vertex
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 normal;
		DirectX::XMFLOAT4 color;
		vertex(){}
		vertex(DirectX::XMFLOAT4 _position,
				DirectX::XMFLOAT4 _normal,
				DirectX::XMFLOAT4 _color)
		{
			position = _position;
			normal = _normal;
			color = _color;
		}
	};
	struct cBuffer
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 proj;
	};

	vertex* createBOX(unsigned int size, float x, float y, float z);

	//TEMP --------------------------------------------------
};
