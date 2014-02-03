#pragma once
#include "Buffer.h"
#include "ParticleEffectDefinition.h"

#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <random>

/**
 * A single instance of a particle effect object.
 */
class ParticleInstance
{
public:
	typedef std::shared_ptr<ParticleInstance> ptr;

private:
	std::shared_ptr<Buffer> m_ConstBuffer;
	std::shared_ptr<Buffer> m_ParticleBuffer;

	ParticleEffectDefinition::ptr m_ParticleEffectDef;
	
	std::vector<Particle>	m_ParticleList;
	DirectX::XMFLOAT4	m_SysPosition; //world pos, in cm
	
	float				m_AccumulatedTime;

	std::default_random_engine m_RandomEngine;
	
public:
	ParticleInstance();
	~ParticleInstance();
	
	/**
	 * Initialize the instance.
	 *
	 * @param p_ConstBuffer
	 * @param p_Particlebuffer 
	 * @param p_ParticleEffectDefinition pointer to what definition to use in this instance
	 */
	void init(std::shared_ptr<Buffer> p_ConstBuffer, std::shared_ptr<Buffer> p_ParticleBuffer, ParticleEffectDefinition::ptr p_ParticleEffectDefinition);
	
	/**
	 * 
	 */
	void update(float p_DeltaTime);
	
	/**
	 * 
	 */
	void updateBuffers(ID3D11DeviceContext *p_DeviceContext, DirectX::XMFLOAT3 *p_CameraPosition,
		DirectX::XMFLOAT4X4 *p_ViewMatrix, DirectX::XMFLOAT4X4 *p_ProjectionMatrix);
	
	/**
	 * 
	 */
	void render(ID3D11DeviceContext* p_Context, ID3D11BlendState* p_BlendState);
	
	/**
	 * 
	 */
	void setPosition(DirectX::XMFLOAT4 p_NewPosition);
	
	/**
	 * 
	 */
	DirectX::XMFLOAT4X4 getWorldMatrix() const;

private:
	void emitNewParticles(float p_DeltaTime);
	static bool isDying(Particle& p_Particle);
	void killOldParticles();
	void updateParticles(float p_DeltaTime);
};