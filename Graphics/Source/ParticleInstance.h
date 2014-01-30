#pragma once
#include "Buffer.h"
#include "ParticleEffectDefinition.h"

#include <DirectXMath.h>
#include <vector>
#include <memory>


class ParticleInstance
{
public:
	typedef std::shared_ptr<ParticleInstance> ptr;

private:
	Buffer				*m_Buffer;

	ParticleEffectDefinition::ptr m_ParticleEffectDef;
	
	std::vector<Particle>	m_ParticleList;
	DirectX::XMFLOAT4	m_SysPosition; //world pos, in cm
	
	unsigned int		m_CurrentParticleCount;
	float				m_AccumulatedTime;
	
public:
	ParticleInstance();
	~ParticleInstance();

	void init(Buffer* p_Buffer, ParticleEffectDefinition::ptr p_ParticleEffectDefinition);
	void update(float p_DeltaTime);
	void updateBuffers(ID3D11DeviceContext *p_DeviceContext, DirectX::XMFLOAT3 *p_CameraPosition,
		DirectX::XMFLOAT4X4 *p_ViewMatrix, DirectX::XMFLOAT4X4 *p_ProjectionMatrix);
	void render();

	void setPosition(DirectX::XMFLOAT4 p_NewPosition);

private:
	void emitNewParticles(float p_DeltaTime);
	static bool isDying(Particle& p_Particle);
	void killOldParticles();
	void updateParticles(float p_DeltaTime);
};