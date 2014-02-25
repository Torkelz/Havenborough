#pragma once
#include "Buffer.h"
#include "ParticleEffectDefinition.h"

#include <vector>
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
	DirectX::XMFLOAT4		m_SysPosition; //world pos, in cm
	DirectX::XMFLOAT3		m_SysRotation;
	DirectX::XMFLOAT4		m_SysBaseColor;

	float m_AccumulatedTime;

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
	 * Main update function for a particle instance
	 * 
	 * @param p_DeltaTime time between frames
	 */
	void update(float p_DeltaTime);
	
	/**
	 * Function for updating buffers
	 * 
	 * @param p_DeviceContext pointer to device context
	 * @param p_CameraPosition pointer to the cameras position
	 * @param p_ViewMatrix pointer to the view matrix
	 * @param p_ProjectionMatrix to the projection matrix
	 */
	void updateBuffers(ID3D11DeviceContext *p_DeviceContext, DirectX::XMFLOAT3 *p_CameraPosition,
		DirectX::XMFLOAT4X4 *p_ViewMatrix, DirectX::XMFLOAT4X4 *p_ProjectionMatrix);
	
	/**
	 * Main function for rendering the particle effect
	 * 
	 * @param p_Context pointer to device context
	 * @param p_BlendState pointer to the blend state in use
	 */
	void render(ID3D11DeviceContext* p_Context, ID3D11BlendState* p_BlendState);
	
	/**
	 * Set the position of the particle instance
	 * 
	 * @param p_NewPosition give the particle effect instance a new position, in cm
	 */
	void setPosition(DirectX::XMFLOAT4 p_NewPosition);
	
	/**
	 * Function to fetch the world matrix
	 * 
	 * @return a XMFLOAT4X4 containing the world matrix
	 */
	DirectX::XMFLOAT4X4 getWorldMatrix() const;

	DirectX::XMFLOAT3 getSysRotation() const;

	void setSysRotation(DirectX::XMFLOAT3 p_NewSysRotation);

	DirectX::XMFLOAT4 getSysBaseColor() const;

	void setSysBaseColor(DirectX::XMFLOAT4 p_NewSysBaseColor);

private:
	void emitNewParticles(float p_DeltaTime);
	static bool isDying(Particle& p_Particle);
	void killOldParticles();
	void updateParticles(float p_DeltaTime);
};