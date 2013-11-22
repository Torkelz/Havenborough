#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class IGraphics
{
public:
	/**
	* Create a pointer from which the graphics library can be accessed.
	* @return the graphics pointer
	*/
	__declspec(dllexport) static IGraphics *createGraphics(void);
	
	/**
	* Initialize the graphics API.
	* @param p_Hwnd the handle to the window which the graphics should be connected to
	* @param p_ScreenWidth input of the window's width
	* @param p_ScreenHeight input of the window's height
	* @param p_Fullscreen input whether the program should run in fullscreen or not
	* @return true if successful, otherwise false
	*/
	virtual bool initialize(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight,
		bool p_Fullscreen) =0;

	/**
	* Reinitialize parts of the graphics API when switching fullscreen on/off or changing resoluton.
	* @param p_Hwnd the handle to the window which the graphics should be connected to
	* @param p_ScreenWidth input of the window's width
	* @param p_ScreenHeight input of the window's height
	* @param p_Fullscreen input whether the program should run in fullscreen or not
	* @return true if successful, otherwise false
	*/
	virtual bool reInitialize(HWND p_Hwnd, int p_ScreenWidth,
		int p_ScreenHeight, bool p_Fullscreen) = 0;

	
	/**
	* Clear sub resources allocated by the graphics API and delete the pointer. 
	*/
	__declspec(dllexport) static void deleteGraphics(IGraphics *p_Graphics);

	/**
	*
	*/
	virtual void renderModel(void) = 0;

	/**
	*
	*/
	virtual void renderText(void) = 0;

	/**
	*
	*/
	virtual void renderQuad(void) = 0;

	/**
	*
	*/
	virtual void addStaticLight(void) = 0;

	/**
	*
	*/
	virtual void removeStaticLight(void) = 0;

	/**
	*
	*/
	virtual void useFrameLight(void) = 0;
	
	/**
	* Draw the current frame.
	*/
	virtual void drawFrame(void) = 0;
private:
	/**
	* Release the sub resources allocated by the graphics API.
	*/
	virtual void shutdown(void) = 0;

};

