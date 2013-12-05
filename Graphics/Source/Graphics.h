#pragma once
#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#include <windows.h>
#include <vector>
#include <string>
#include <d3d11.h>
#include <dxgi.h>

#include "Util.h"
#include "../include/IGraphics.h"
#include "MyGraphicsExceptions.h"
#include "TextureLoader.h"
#include "WrapperFactory.h"
#include "VRAMMemInfo.h"

using std::string;
using std::vector;
using std::pair;
using std::make_pair;

class Graphics : public IGraphics
{
private:
	struct Model
	{
		ID3D11Buffer *vertexBuffer;
		ID3D11Buffer *indexBuffer;
		Shader *shader;
		ID3D11ShaderResourceView *diffuseTexture;
		ID3D11ShaderResourceView *normalTexture;
		ID3D11ShaderResourceView *specularTexture;
		unsigned int numOfMaterials;
	};

	ID3D11Device *m_Device;
	ID3D11DeviceContext *m_DeviceContext;

	IDXGISwapChain *m_SwapChain;
	ID3D11RenderTargetView *m_RenderTargetView;
	
	ID3D11RasterizerState *m_RasterState;

	ID3D11Texture2D *m_DepthStencilBuffer;
	ID3D11DepthStencilState *m_DepthStencilState;
	ID3D11DepthStencilView *m_DepthStencilView;

	unsigned int m_Numerator;
	unsigned int m_Denominator;
	char m_GraphicsCard[128];
	int m_GraphicsMemory;
	bool m_VSyncEnabled;

	TextureLoader m_TextureLoader;	
	WrapperFactory *m_WrapperFactory;
	VRAMMemInfo *m_VRAMMemInfo;

	vector<pair<string, Shader*>> m_ShaderList;
	vector<pair<string, Model>> m_ModelList;
	vector<pair<string, string>> m_ShaderLinkList;
	vector<pair<string, ID3D11ShaderResourceView*>> m_TextureList;

public:
	Graphics(void);
	~Graphics(void);

	bool initialize(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight,	bool p_Fullscreen) override;
	bool reInitialize(HWND p_Hwnd, int p_ScreenWidht, int p_ScreenHeight, bool p_Fullscreen) override;
	
	void createModel(const char *p_ModelId, const char *p_Filename) override;
	void createShader(const char *p_shaderId, LPCWSTR p_Filename,
		const char *p_EntryPoint, const char *p_ShaderModel, ShaderType p_Type) override;
	void createShader(const char *p_shaderId, LPCWSTR p_Filename,
		const char *p_EntryPoint, const char *p_ShaderModel, ShaderType p_Type,
		ShaderInputElementDescription *p_VertexLayout, unsigned int p_NumOfInputElements) override;
	void linkShaderToModel(const char *p_ShaderId, const char *p_ModelId) override;
	
	void createTexture(const char *p_TextureId, const char *p_filename) override;
	
	void addStaticLight(void) override;
	void removeStaticLight(void) override;
	void useFrameLight(void) override;
	
	void renderModel(const char *p_ModelId) override;
	void renderText(void) override;
	void renderQuad(void) override;
	void drawFrame(void) override;

	int getVRAMMemUsage(void) override;

private:
	void shutdown(void) override;

	void setViewPort(int p_ScreenWidth, int p_ScreenHeight);
	HRESULT createDeviceAndSwapChain(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight, bool p_Fullscreen);
	HRESULT createRenderTargetView(void);
	HRESULT createDepthStencilBuffer(int p_ScreenWidth,	int p_ScreenHeight);
	HRESULT createDepthStencilState(void);
	HRESULT createDepthStencilView(void);
	HRESULT createRasterizerState(void);

	Buffer *createBuffer(Buffer::Description &p_Description);
	vector<string> createEntryPointList(const char *p_EntryPoint);
	void Begin(float color[4]);
	void End(void);
};