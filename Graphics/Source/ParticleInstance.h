#pragma once

#include <algorithm>

#include "ParticleFactory.h"

class ParticleInstance
{
private:
	Buffer				*m_Buffer;

	ParticleEffectDefinition m_ParticleEffectDef;
	
	std::vector<Particle>	m_ParticleList;
	DirectX::XMFLOAT4	m_SysPosition; //world pos, in cm
	std::string			m_SysType;

	float				m_CurrentParticleCount;
	float				m_AccumulatedTime;

	float				m_SizeX;
	float				m_SizeY;

public:
	ParticleInstance();
	~ParticleInstance();

	void init();
	void update(float p_DeltaTime, ID3D11DeviceContext *p_DeviceContext, DirectX::XMFLOAT3 *p_CameraPosition,
		DirectX::XMFLOAT4X4 *p_ViewMatrix, DirectX::XMFLOAT4X4 *p_ProjectionMatrix);

	void render();

private:
	void emitNewParticles(float p_DeltaTime);
	static bool isDying(Particle& p_Particle);
	void killOldParticles();
	void updateParticles(float p_DeltaTime);
	void updateBuffers(ID3D11DeviceContext *p_DeviceContext, DirectX::XMFLOAT3 *p_CameraPosition,
		DirectX::XMFLOAT4X4 *p_ViewMatrix, DirectX::XMFLOAT4X4 *p_ProjectionMatrix);
	void render();
};