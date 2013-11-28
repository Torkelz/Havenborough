#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class IGraphics
{
public:
	enum class ShaderType
	{
		VERTEX_SHADER = 0,
		PIXEL_SHADER = 1,
		GEOMETRY_SHADER = 2,
		HULL_SHADER = 3,
		DOMAIN_SHADER = 4
	};

	virtual ~IGraphics(void)
	{}
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
	virtual bool initialize(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight,	bool p_Fullscreen) = 0;

	/**
	* Reinitialize parts of the graphics API when switching fullscreen on/off or changing resoluton.
	* @param p_Hwnd the handle to the window which the graphics should be connected to
	* @param p_ScreenWidth input of the window's width
	* @param p_ScreenHeight input of the window's height
	* @param p_Fullscreen input whether the program should run in fullscreen or not
	* @return true if successful, otherwise false
	*/
	virtual bool reInitialize(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight, bool p_Fullscreen) = 0;

	
	/**
	* Clear sub resources allocated by the graphics API and delete the pointer. 
	*/
	__declspec(dllexport) static void deleteGraphics(IGraphics *p_Graphics);

	/**
	* 
	*/
	virtual void createModel(const char *p_ModelId, const char *p_Filename) = 0;

	/**
	* 
	*/
	virtual void createShader(const char *p_shaderId, LPCWSTR p_Filename,
		const char *p_EntryPoint, const char *p_ShaderModel, ShaderType p_Type) = 0;

	/**
	* Establish a map of shader name to a model name.
	* @param p_ShaderId name of the shader
	* @param p_ModelId name of the model
	*/
	virtual void linkShaderToModel(const char *p_ShaderId, const char *p_ModelId) = 0;

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
	* 
	*/
	virtual void renderModel(char *p_ModelId) = 0;

	/**
	* 
	*/
	virtual void renderText(void) = 0;

	/**
	* 
	*/
	virtual void renderQuad(void) = 0;
	
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

