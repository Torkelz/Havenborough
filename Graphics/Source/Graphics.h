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
#include "DeferredRenderer.h"
#include "WrapperFactory.h"
#include "ModelLoader.h"
#include "VRAMMemInfo.h"

using std::string;
using std::vector;
using std::pair;
using std::make_pair;

class Graphics : public IGraphics
{
private:
	struct ModelInstance
	{
		std::string m_ModelName;
		DirectX::XMFLOAT3 m_Position;
		DirectX::XMFLOAT3 m_Rotation;
		DirectX::XMFLOAT3 m_Scale;

		mutable bool m_IsCalculated;
		mutable DirectX::XMFLOAT4X4 m_World;

		const DirectX::XMFLOAT4X4& getWorldMatrix() const
		{
			if (!m_IsCalculated)
			{
				calculateWorldMatrix();
			}
			return m_World;
		}
		void setPosition(const DirectX::XMFLOAT3& p_Position)
		{
			m_Position = p_Position;
			m_IsCalculated = false;
		}
		void setRotation(const DirectX::XMFLOAT3& p_Rotation)
		{
			m_Rotation = p_Rotation;
			m_IsCalculated = false;
		}
		void setScale(const DirectX::XMFLOAT3& p_Scale)
		{
			m_Scale = p_Scale;
			m_IsCalculated = false;
		}
		void calculateWorldMatrix() const
		{
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYawFromVector( DirectX::XMLoadFloat3(&m_Rotation) );
			DirectX::XMMATRIX translation = DirectX::XMMatrixTranslationFromVector( DirectX::XMLoadFloat3(&m_Position) );
			DirectX::XMMATRIX scale = DirectX::XMMatrixScalingFromVector( XMLoadFloat3(&m_Scale) );

			DirectX::XMStoreFloat4x4( &m_World, DirectX::XMMatrixTranspose(scale * rotation * translation) );

			m_IsCalculated = true;
		}
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
	vector<pair<string, ID3D11ShaderResourceView*>> m_TextureList;
	vector<pair<int, ModelInstance>> m_ModelInstances;
	int m_NextInstanceId;
	
	DeferredRenderer	*m_DeferredRender;

	Shader				*m_Shader; //DEBUG
	ID3D11SamplerState	*m_Sampler;

	IGraphics::loadModelTextureCallBack m_LoadModelTexture;
	void* m_LoadModelTextureUserdata;

public:
	Graphics(void);
	~Graphics(void);

	bool initialize(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight,	bool p_Fullscreen) override;
	bool reInitialize(HWND p_Hwnd, int p_ScreenWidht, int p_ScreenHeight, bool p_Fullscreen) override;
	
	bool createModel(const char *p_ModelId, const char *p_Filename) override;
	bool releaseModel(const char *p_ModelID) override;

	void createShader(const char *p_shaderId, LPCWSTR p_Filename,
		const char *p_EntryPoint, const char *p_ShaderModel, ShaderType p_Type) override;
	void createShader(const char *p_shaderId, LPCWSTR p_Filename,
		const char *p_EntryPoint, const char *p_ShaderModel, ShaderType p_Type,
		ShaderInputElementDescription *p_VertexLayout, unsigned int p_NumOfInputElements) override;
	void linkShaderToModel(const char *p_ShaderId, const char *p_ModelId) override;
	
	bool createTexture(const char *p_TextureId, const char *p_filename) override;
	bool releaseTexture(const char *p_TextureID) override;
	

	void addStaticLight(void) override;
	void removeStaticLight(void) override;
	void useFrameLight(void) override;
	
	void renderModel(int p_ModelId) override;
	void renderText(void) override;
	void renderQuad(void) override;
	void drawFrame(int i) override;

	int getVRAMMemUsage(void) override;
	
	int createModelInstance(const char *p_ModelId) override;
	void eraseModelInstance(int p_Instance) override;
	void setModelPosition(int p_Instance, float p_X, float p_Y, float p_Z) override;
	void setModelRotation(int p_Instance, float p_Yaw, float p_Pitch, float p_Roll) override;
	void setModelScale(int p_Instance, float p_X, float p_Y, float p_Z) override;

	void updateCamera(float p_PosX, float p_PosY, float p_PosZ, float p_Yaw, float p_Pitch) override;

	void setLoadModelTextureCallBack(loadModelTextureCallBack p_LoadModelTexture, void* p_Userdata) override;

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
	Shader *getShaderFromList(string p_Identifier);
	Model *getModelFromList(string p_Identifier);
	ID3D11ShaderResourceView* getTextureFromList(string p_Identifier);

	struct vertex
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 tangent;
		DirectX::XMFLOAT3 binormal;
		vertex(){}
		vertex(DirectX::XMFLOAT3 _position,
			DirectX::XMFLOAT3 _normal,
			DirectX::XMFLOAT2 _uv,
			DirectX::XMFLOAT3 _tangent)
		{
			position = DirectX::XMFLOAT4(_position.x,_position.y,_position.z,1.0f);
			normal = _normal;
			uv = _uv;
			tangent = _tangent;

			//might be wrong
			DirectX::XMStoreFloat3(&binormal, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&tangent),DirectX::XMLoadFloat3(&normal)));
		}
	};
};