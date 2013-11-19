#pragma once
#define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#include "igraphics.h"

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#define SAFE_RELEASE(x) { if(x) { x->Release(); x = nullptr; } }
#define SAFE_DELETE(x) { if(x) { delete x; x = nullptr; } }
#define SAFE_DELETE_ARRAY(x) { if(x) { delete[] x; x = nullptr; } }

class Graphics :
	public IGraphics
{
public:
	ID3D11Device *m_Device;
	ID3D11DeviceContext *m_DeviceContext;

	IDXGISwapChain *m_SwapChain;
	ID3D11RenderTargetView *m_RenderTargetView;
	
	ID3D11RasterizerState *m_RasterState;

	ID3D11Texture2D *m_DepthStencilBuffer;
	ID3D11DepthStencilState *m_DepthStencilState;
	ID3D11DepthStencilView *m_DepthStencilView;

	char m_GraphicsCard[128];
	int m_GraphicsMemory;
	int m_VSyncEnabled;


public:
	Graphics(void);
	~Graphics(void);

	bool initialize(HWND p_hwnd, int p_screenWidth, int p_screenHeight,
		bool p_fullscreen);
	void shutdown(void);
	
	void renderModel(void);
	void renderText(void);
	void renderQuad(void);

	void addStaticLight(void);
	void removeStaticLight(void);
	void useFrameLight(void);
	
	void drawFrame(void);

private:
	bool reInitialize(HWND p_hwnd, int p_screenWidht, int p_screenHeight,
		bool p_fullscreen);

};

