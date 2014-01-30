#pragma once
#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#include <windows.h>
#include <vector>
#include <string>
#include <d3d11.h>
#include <dxgi.h>

#include "../include/IGraphics.h"
#include "GraphicsExceptions.h"
#include "TextureLoader.h"
#include "DeferredRenderer.h"
#include "ForwardRendering.h"
#include "WrapperFactory.h"
#include "ModelFactory.h"
#include "ModelInstance.h"
#include "ModelDefinition.h"
#include "VRAMInfo.h"

#include "ParticleFactory.h"
#include "ParticleInstance.h"

#include "ShaderStructs.h"

using std::string;
using std::vector;
using std::pair;
using std::make_pair;

class Graphics : public IGraphics
{
private:
	ID3D11Device *m_Device;
	ID3D11DeviceContext *m_DeviceContext;

	IDXGISwapChain *m_SwapChain;
	ID3D11RenderTargetView *m_RenderTargetView;
	
	ID3D11RasterizerState *m_RasterState;
	ID3D11RasterizerState *m_RasterStateBV;

	ID3D11Texture2D *m_DepthStencilBuffer;
	ID3D11DepthStencilState	*m_DepthStencilState;
	ID3D11DepthStencilView *m_DepthStencilView;

	unsigned int m_Numerator;
	unsigned int m_Denominator;
	char m_GraphicsCard[128];
	int m_GraphicsMemory;
	bool m_VSyncEnabled;
	float m_ClearColor[4];
	int m_SelectedRenderTarget;

	XMFLOAT4X4 m_ViewMatrix;
	XMFLOAT4X4 m_ProjectionMatrix;
	XMFLOAT3 m_Eye;

	static const unsigned int m_MaxLightsPerLightInstance;
	TextureLoader m_TextureLoader;	
	WrapperFactory *m_WrapperFactory;
	ModelFactory *m_ModelFactory;
	VRAMInfo *m_VRAMInfo;

	vector<pair<string, Shader*>> m_ShaderList;
	vector<pair<string, ModelDefinition>> m_ModelList;
	vector<pair<string, ID3D11ShaderResourceView*>> m_TextureList;
	vector<pair<InstanceId, ModelInstance>> m_ModelInstances;
	InstanceId m_NextInstanceId;
	
	//Particles
	vector<pair<string, ParticleEffectDefinition::ptr>>  m_ParticleEffectDefinitionList;
	vector<pair<int, ParticleInstance::ptr>>  m_ParticleEffectInstanceList;
	int m_NextParticleInstanceId;
	std::unique_ptr<ParticleFactory> m_ParticleFactory;

	DeferredRenderer *m_DeferredRender;
	ForwardRendering *m_ForwardRenderer;
		
	//Lights
	std::vector<Light> m_SpotLights;
	std::vector<Light> m_PointLights;
	std::vector<Light> m_DirectionalLights;

	//Stuff needed for drawing bounding volumes
	std::vector<XMFLOAT4> m_BVTriangles;
	Buffer *m_BVBuffer;
	unsigned int m_BVBufferNumOfElements;
	Shader *m_BVShader;

	Shader *m_Shader; //DEBUG
	ID3D11SamplerState *m_Sampler;

	IGraphics::loadModelTextureCallBack m_LoadModelTexture;
	void *m_LoadModelTextureUserdata;
	
	IGraphics::releaseModelTextureCallBack m_ReleaseModelTexture;
	void *m_ReleaseModelTextureUserdata;

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
	
	void deleteShader(const char *p_ShaderId) override;

	bool createTexture(const char *p_TextureId, const char *p_filename) override;
	bool releaseTexture(const char *p_TextureID) override;	

	//Particles
	bool createParticleEffectDefinition(const char *p_ParticleEffectId, const char *p_filename) override;
	bool releaseParticleEffectDefinition(const char *p_ParticleEffectId) override;

	InstanceId createParticleEffectInstance(const char *p_ParticleEffectId) override;
	void releaseParticleEffectInstance(InstanceId p_ParticleEffectId) override;

	void linkShaderToParticles(const char *p_ShaderId, const char *p_ParticlesId) override;
	void updateParticles(float p_DeltaTime) override;


	void addStaticLight(void) override;
	void removeStaticLight(void) override;
	
	void useFramePointLight(Vector3 p_LightPosition, Vector3 p_LightColor, float p_LightRange) override;
	void useFrameSpotLight(Vector3 p_LightPosition, Vector3 p_LightColor, Vector3 p_LightDirection,
		Vector2 p_SpotLightAngles,	float p_LightRange) override;
	void useFrameDirectionalLight(Vector3 p_LightColor, Vector3 p_LightDirection) override;
	
	void setClearColor(Vector4 p_Color) override;

	void renderModel(InstanceId p_ModelId) override;
	virtual void renderSkydome() override;
	void renderText(void) override;
	void renderQuad(void) override;
	void drawFrame() override;

	void setModelDefinitionTransparency(const char *p_ModelId, bool p_State) override;

	void updateAnimations(float p_DeltaTime) override;
	void playAnimation(int p_Instance, const char* p_ClipName, bool p_Override) override;
	void queueAnimation(int p_Instance, const char* p_ClipName) override;
	void changeAnimationWeight(int p_Instance, int p_Track, float p_Weight) override;

	int getVRAMUsage(void) override;
	
	InstanceId createModelInstance(const char *p_ModelId) override;
	void createSkydome(const char *p_TextureResource, float p_Radius) override;
	void eraseModelInstance(InstanceId p_Instance) override;
	void setModelPosition(InstanceId p_Instance, Vector3 p_Position) override;
	void setModelRotation(InstanceId p_Instance, Vector3 p_YawPitchRoll) override;
	void setModelScale(InstanceId p_Instance, Vector3 p_Scale) override;
	void setModelColorTone(InstanceId p_Instance, Vector3 p_ColorTone) override;
	void applyIK_ReachPoint(InstanceId p_Instance, const char* p_TargetJoint, const char* p_HingeJoint, const char* p_BaseJoint, Vector3 p_Target) override;
	Vector3 getJointPosition(InstanceId p_Instance, const char* p_Joint) override;

	void updateCamera(Vector3 p_Position, float p_Yaw, float p_Pitch) override;

	void addBVTriangle(Vector3 p_Corner1, Vector3 p_Corner2, Vector3 p_Corner3) override;

	void setLogFunction(clientLogCallback_t p_LogCallback) override;
	void setRenderTarget(int p_RenderTarget) override;

	void setLoadModelTextureCallBack(loadModelTextureCallBack p_LoadModelTexture, void* p_Userdata) override;
	void setReleaseModelTextureCallBack(releaseModelTextureCallBack p_ReleaseModelTexture, void* p_Userdata) override;

private:
	void shutdown(void) override;

	void setViewPort(int p_ScreenWidth, int p_ScreenHeight);
	HRESULT createDeviceAndSwapChain(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight, bool p_Fullscreen);
	HRESULT createRenderTargetView(void);
	HRESULT createDepthStencilBuffer(int p_ScreenWidth,	int p_ScreenHeight);
	HRESULT createDepthStencilState(void);
	HRESULT createDepthStencilView(void);
	HRESULT createRasterizerState(void);
	void initializeMatrices(int p_ScreenWidth, int p_ScreenHeight);
	
	Shader *getShaderFromList(string p_Identifier);
	ModelDefinition *getModelFromList(string p_Identifier);

	ParticleEffectDefinition::ptr getParticleFromList(string p_ParticleSystemId);

	ID3D11ShaderResourceView *getTextureFromList(string p_Identifier);
	int calculateTextureSize(ID3D11ShaderResourceView *p_Texture);
	void Begin(float color[4]);
	void End(void);

	void drawBoundingVolumes();

	//TODO: Remove later
	void DebugDefferedDraw(void);
};