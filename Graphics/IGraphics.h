#pragma once

class IGraphics
{
public:

public:
	/**
	* Initialize the graphics system.
	*/
	static __declspec(dllexport) bool initialize(int p_screenWidth, 
		int p_screenHeight, bool p_fullscreen);

	/**
	*
	*/
	static __declspec(dllexport) void shutdown(void);

	/**
	*
	*/
	static __declspec(dllexport) void renderModel(void);

	/**
	*
	*/
	static __declspec(dllexport) void renderText(void);

	/**
	*
	*/
	static __declspec(dllexport) void renderQuad(void);

	/**
	*
	*/
	static __declspec(dllexport) void addStaticLight(void);

	/**
	*
	*/
	static __declspec(dllexport) void removeStaticLight(void);

	/**
	*
	*/
	static __declspec(dllexport) void useFrameLight(void);

	/**
	* Draw the current frame.
	*/
	static __declspec(dllexport) void drawFrame(void);

};

