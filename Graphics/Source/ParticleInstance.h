#pragma once

#include <algorithm>

#include "ParticleFactory.h"

class ParticleInstance
{
private:

	ID3D11Device		*m_Device;
	ID3D11DeviceContext	*m_DeviceContext;

	ID3D11DepthStencilView	*m_DepthStencilView;
	ID3D11RenderTargetView	*m_RenderTarget;
	ID3D11SamplerState		*m_Sampler;
	ID3D11RasterizerState	*m_RasterState;

	DirectX::XMFLOAT3	*m_CameraPosition;
	DirectX::XMFLOAT4X4	*m_ViewMatrix;
	DirectX::XMFLOAT4X4	*m_ProjectionMatrix;

	Buffer				*m_Buffer;
	
	std::vector<Particle>	m_ParticleList;
	DirectX::XMFLOAT4	m_SysPosition; //world pos, in cm
	std::string			m_SysType;

	float				m_CurrentParticleCount;
	float				m_AccumulatedTime;

	//Header
	std::string			m_SysName;
	std::string			m_TextureFilePath;
	float				m_TextureU; 
	float				m_TextureV;	
	float				m_ParticleMaxLife;

	unsigned int		m_MaxParticles;
	DirectX::XMFLOAT3	m_ParticlePositionDeviation;
	DirectX::XMFLOAT3	m_VelocityDeviation;
	DirectX::XMFLOAT4	m_ParticleColorDeviation;
	float				m_SizeX;
	float				m_SizeY;
	float				m_ParticlesPerSecound;

public:
	ParticleInstance();
	~ParticleInstance();

	void init(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext, DirectX::XMFLOAT3 *p_CameraPosition,
		DirectX::XMFLOAT4X4 *p_ViewMatrix, DirectX::XMFLOAT4X4 *p_ProjectionMatrix,
		ID3D11DepthStencilView* p_DepthStencilView,	ID3D11RenderTargetView *p_RenderTarget);
	void update(float p_DeltaTime);

	void render();

private:

	void emitNewParticles(float p_DeltaTime);
	static bool isDying(Particle& p_Particle);
	void killOldParticles();
	void updateParticles(float p_DeltaTime);
	void updateBuffers();
	void render();
};