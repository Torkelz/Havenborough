#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <IGraphics.h>
#include <vector>
#include <string>

#include "TextureLoader.h"
#include "DeferredRenderer.h"
#include "ForwardRendering.h"
#include "WrapperFactory.h"
#include "ModelFactory.h"
#include "ModelInstance.h"
#include "ModelDefinition.h"
#include "ParticleFactory.h"
#include "ParticleInstance.h"
#include "ScreenRenderer.h"
#include "TextFactory.h"

class Graphics : public IGraphics
{
private:
	ID3D11Device *m_Device;
	ID3D11DeviceContext *m_DeviceContext;

	IDXGISwapChain *m_SwapChain;
	ID3D11RenderTargetView *m_RenderTargetView;
	ID3D11SamplerState *m_Sampler;

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
	IGraphics::RenderTarget m_SelectedRenderTarget;

	float m_FOV;
	float m_FarZ;
	DirectX::XMFLOAT4X4 m_ViewMatrix;
	DirectX::XMFLOAT4X4 m_ProjectionMatrix;
	DirectX::XMFLOAT3 m_Eye;

	static const unsigned int m_MaxLightsPerLightInstance;
	TextureLoader m_TextureLoader;	
	WrapperFactory *m_WrapperFactory;
	ModelFactory *m_ModelFactory;

	std::map<std::string, Shader*> m_ShaderList;
	std::map<std::string, ModelDefinition> m_ModelList;
	std::map<std::string, ID3D11ShaderResourceView*> m_TextureList;
	std::map<InstanceId, ModelInstance> m_ModelInstances;
	std::map<Object2D_Id, Renderable2D> m_2D_Objects;
	InstanceId m_NextInstanceId;
	Object2D_Id m_Next2D_ObjectId;

	//Particles
	std::map<std::string, ParticleEffectDefinition::ptr> m_ParticleEffectDefinitionList;
	std::map<int, ParticleInstance::ptr> m_ParticleEffectInstanceList;
	int m_NextParticleInstanceId;
	std::unique_ptr<ParticleFactory> m_ParticleFactory;

	DeferredRenderer *m_DeferredRender;
	ForwardRendering *m_ForwardRenderer;
	ScreenRenderer *m_ScreenRenderer;
		
	//Lights
	std::vector<Light> m_SpotLights;
	std::vector<Light> m_PointLights;
	std::vector<Light> m_DirectionalLights;

	//Stuff needed for drawing bounding volumes
	std::vector<DirectX::XMFLOAT4> m_BVTriangles;
	Buffer *m_ConstantBuffer;
	Buffer *m_BVBuffer;
	unsigned int m_BVBufferNumOfElements;

	IGraphics::loadModelTextureCallBack m_LoadModelTexture;
	void *m_LoadModelTextureUserdata;
	
	IGraphics::releaseModelTextureCallBack m_ReleaseModelTexture;
	void *m_ReleaseModelTextureUserdata;

	TextFactory m_TextFactory;

	ID3D11ShaderResourceView *textSRV;
	IDXGIKeyedMutex *dxgiKeyedMutex10, *dxgiKeyedMutex11;

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
	bool releaseTexture(const char *p_TextureId) override;	

	//Particles
	bool createParticleEffectDefinition(const char *p_FileId, const char *p_filePath) override;
	bool releaseParticleEffectDefinition(const char *p_ParticleEffectId) override;

	InstanceId createParticleEffectInstance(const char *p_ParticleEffectId) override;
	void releaseParticleEffectInstance(InstanceId p_ParticleEffectId) override;
	void setParticleEffectPosition(InstanceId p_ParticleEffectId, Vector3 p_Position) override;

	void linkShaderToParticles(const char *p_ShaderId, const char *p_ParticlesId) override;
	void updateParticles(float p_DeltaTime) override;
	/////

	Object2D_Id create2D_Object(Vector3 p_Position, Vector2 p_HalfSize, Vector3 p_Scale, float p_Rotation,
		const char *p_TextureId) override;
	Object2D_Id create2D_Object(Vector3 p_Position, Vector3 p_Scale, float p_Rotation,
		const char *p_ModelDefinition) override;

	Text_Id createText(const char *p_Identifier, const wchar_t *p_Text, Vector2 p_TextureSize,
		const char *p_Font, float p_FontSize, Vector4 p_FontColor) override;
	Text_Id createText(const char *p_Identifier, const wchar_t *p_Text, Vector2 p_TextureSize,
		const char *p_Font, float p_FontSize, Vector4 p_FontColor, TEXT_ALIGNMENT p_TextAlignment,
		PARAGRAPH_ALIGNMENT p_ParagraphAlignment, WORD_WRAPPING p_WordWrapping) override;

	void useFramePointLight(Vector3 p_LightPosition, Vector3 p_LightColor, float p_LightRange) override;
	void useFrameSpotLight(Vector3 p_LightPosition, Vector3 p_LightColor, Vector3 p_LightDirection,
		Vector2 p_SpotLightAngles,	float p_LightRange) override;
	void useFrameDirectionalLight(Vector3 p_LightColor, Vector3 p_LightDirection) override;
	
	void setClearColor(Vector4 p_Color) override;

	void renderModel(InstanceId p_ModelId) override;
	virtual void renderSkydome(void) override;
	void renderText(void) override;
	void render2D_Object(Object2D_Id p_Id) override;
	void drawFrame(void) override;

	void setModelDefinitionTransparency(const char *p_ModelId, bool p_State) override;

	void animationPose(int p_Instance, const DirectX::XMFLOAT4X4* p_Pose, unsigned int p_Size) override;

	int getVRAMUsage(void) override;
	
	InstanceId createModelInstance(const char *p_ModelId) override;
	void createSkydome(const char *p_TextureResource, float p_Radius) override;
	void eraseModelInstance(InstanceId p_Instance) override;
	void setModelPosition(InstanceId p_Instance, Vector3 p_Position) override;
	void setModelRotation(InstanceId p_Instance, Vector3 p_YawPitchRoll) override;
	void setModelScale(InstanceId p_Instance, Vector3 p_Scale) override;
	void setModelColorTone(InstanceId p_Instance, Vector3 p_ColorTone) override;
	void set2D_ObjectPosition(Object2D_Id p_Instance, Vector3 p_Position) override;
	void set2D_ObjectScale(Object2D_Id p_Instance, Vector3 p_Scale) override;
	void set2D_ObjectRotationZ(Object2D_Id p_Instance, float p_Rotation) override;
	void set2D_ObjectLookAt(Object2D_Id p_Instance, Vector3 p_LookAt) override;

	void updateCamera(Vector3 p_Position, Vector3 p_Forward, Vector3 p_Up) override;

	void addBVTriangle(Vector3 p_Corner1, Vector3 p_Corner2, Vector3 p_Corner3) override;

	void setLogFunction(clientLogCallback_t p_LogCallback) override;
	void setTweaker(TweakSettings* p_Tweaker) override;
	void setRenderTarget(IGraphics::RenderTarget p_RenderTarget) override;

	void setLoadModelTextureCallBack(loadModelTextureCallBack p_LoadModelTexture, void* p_Userdata) override;
	void setReleaseModelTextureCallBack(releaseModelTextureCallBack p_ReleaseModelTexture, void* p_Userdata) override;
	void enableVsync(bool p_State) override;
	void renderJoint(DirectX::XMFLOAT4X4 p_World) override;
	void enableSSAO(bool p_State) override;

private:
	void createDefaultShaders(void) override;
	void shutdown(void) override;
	bool release2D_Model(Object2D_Id p_ObjectID);

	void setViewPort(int p_ScreenWidth, int p_ScreenHeight);
	HRESULT createDeviceAndSwapChain(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight, bool p_Fullscreen);
	HRESULT createRenderTargetView(void);
	HRESULT createDepthStencilBuffer(int p_ScreenWidth,	int p_ScreenHeight);
	HRESULT createDepthStencilState(void);
	HRESULT createDepthStencilView(void);
	HRESULT createRasterizerState(void);

	void initializeFactories(void);
	void initializeMatrices(int p_ScreenWidth, int p_ScreenHeight, float p_NearZ, float p_FarZ);
	
	Shader *getShaderFromList(std::string p_Identifier);
	ModelDefinition *getModelFromList(std::string p_Identifier);
	ParticleEffectDefinition::ptr getParticleFromList(std::string p_ParticleSystemId);
	ID3D11ShaderResourceView *getTextureFromList(std::string p_Identifier);
	
	int calculateTextureSize(ID3D11ShaderResourceView *p_Texture);
	void Begin(float color[4]);
	void End(void);

	void drawBoundingVolumes();
	void updateConstantBuffer();

	//TODO: Remove later
	void DebugDefferedDraw(void);
};