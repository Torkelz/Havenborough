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
	float					m_SysLife;
	float					m_SysMaxLife;
	bool					m_Seppuku;

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

	/**
	 * Called to get the current rotation of the system
	 * 
	 * @return the current systems rotation, (yaw, pitch, roll)
	 */
	DirectX::XMFLOAT3 getSysRotation() const;

	/**
	 * Called to set an new rotation for the system, based from the default rotation (no rotation)
	 * 
	 * @param p_NewSysRotation a new rotation for the system, (yaw, pitch, roll)
	 */
	void setSysRotation(DirectX::XMFLOAT3 p_NewSysRotation);

	/**
	 * Called to get what the systems base color are
	 * 
	 * @return the base color for this system, [0,1]
	 */
	DirectX::XMFLOAT4 getSysBaseColor() const;

	/**
	 * Called to set a new base color for the system
	 * 
	 * @param p_NewSysBaseColor a new base color for the system, [0,1]
	 */
	void setSysBaseColor(DirectX::XMFLOAT4 p_NewSysBaseColor);

	/**
	 * Called to see if it's time for the system to tell if it should be removed
	 * 
	 * @return true if it's time to remove it else false
	 */
	bool getSeppuku() const;

	/**
	 * Called to change the bool if it should be removed or not
	 * 
	 * @param p_DoSeppuku the new value of the bool, true if it's time to remove the system
	 */
	void setSeppuku(bool p_DoSeppuku);

private:
	void emitNewParticles(float p_DeltaTime);
	static bool isDying(Particle& p_Particle);
	void killOldParticles();
	void updateParticles(float p_DeltaTime);
};