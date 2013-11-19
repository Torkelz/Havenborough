#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

__declspec(dllexport) class IGraphics
{
public:

public:
	/**
	* Initialize the graphics system.
	*/
	bool initialize(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight,
		bool p_Fullscreen);

	/**
	*
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

