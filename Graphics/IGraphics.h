#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

__declspec(dllexport) class IGraphics
{
public:

public:
	/**
	* Initialize the graphics API.
	* @param p_Hwnd the handle to the window which the graphics should be connected to
	* @param p_ScreenWidth input of the window's width
	* @param p_ScreenHeight input of the window's height
	* @param p_Fullscreen input whether the program should run in fullscreen or not
	* @return true if successful, otherwise false
	*/
	bool initialize(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight,
		bool p_Fullscreen);

	/**
	* Release the resources allocated by the graphics API.
	*/
	void shutdown(void);

	/**
	*
	*/
	void renderModel(void);

	/**
	*
	*/
	void renderText(void);

	/**
	*
	*/
	void renderQuad(void);

	/**
	*
	*/
	void addStaticLight(void);

	/**
	*
	*/
	void removeStaticLight(void);

	/**
	*
	*/
	void useFrameLight(void);

	/**
	* Draw the current frame.
	*/
	void drawFrame(void);

};

