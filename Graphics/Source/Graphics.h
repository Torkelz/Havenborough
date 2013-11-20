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
private:
	unsigned int m_Numerator;
	unsigned int m_Denominator;

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
	bool m_VSyncEnabled;

public:
	Graphics(void);
	~Graphics(void);

	bool initialize(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight,
		bool p_Fullscreen);
	bool reInitialize(HWND p_Hwnd, int p_ScreenWidht, int p_ScreenHeight,
		bool p_Fullscreen);
	void shutdown(void);
	
	void renderModel(void);
	void renderText(void);
	void renderQuad(void);

	void addStaticLight(void);
	void removeStaticLight(void);
	void useFrameLight(void);
	
	void drawFrame(void);

private:
	
	void setViewPort(int p_ScreenWidth, int p_ScreenHeight);
	HRESULT createDeviceAndSwapChain(HWND p_Hwnd, int p_ScreenWidth,
		int p_ScreenHeight, bool p_Fullscreen);
	HRESULT createRenderTargetView(void);
	HRESULT createDepthStencilBuffer(int p_ScreenWidth,
		int p_ScreenHeight);
	HRESULT createDepthStencilState(void);
	HRESULT createDepthStencilView(void);
	HRESULT createRasterizerState(void);

	void Begin(float color[4]);
	void End(void);
};

