#include "ParticleInstance.h"
#include <functional>
#include <algorithm>

ParticleInstance::ParticleInstance()
{
	m_SysPosition			= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);

	m_CurrentParticleCount	= 0;
	m_AccumulatedTime		= 0.f;
}

ParticleInstance::~ParticleInstance()
{
	m_Buffer = nullptr;
}

void ParticleInstance::init(Buffer* p_Buffer, ParticleEffectDefinition::ptr p_ParticleEffectDefinition)
{
	m_Buffer = p_Buffer;
	m_ParticleEffectDef = p_ParticleEffectDefinition;
	m_ParticleList.resize(m_ParticleEffectDef->maxParticles);
}


void ParticleInstance::update(float p_DeltaTime) 
{
	killOldParticles();

	emitNewParticles(p_DeltaTime);

	updateParticles(p_DeltaTime);
}

void ParticleInstance::killOldParticles()
{
	//Will go thou the list of particles in the system and remove any particle that are to old

	auto removeIt = std::remove_if(m_ParticleList.begin(),m_ParticleList.end(), [&] (Particle &p) 
	{
		return p.life >= m_ParticleEffectDef->maxLife;
	});
	m_ParticleList.erase(removeIt, m_ParticleList.end());

}

void ParticleInstance::updateParticles(float p_DeltaTime)
{
	//Update the position of every particle in the system by its velocity and based on the delta time
	for(auto& part : m_ParticleList)
	{
		part.position = DirectX::XMFLOAT4(
			(part.position.x + part.velocity.x * p_DeltaTime),
			(part.position.y + part.velocity.y * p_DeltaTime),
			(part.position.z + part.velocity.z * p_DeltaTime),
			part.position.w);
		part.life += p_DeltaTime;
	}
}

void ParticleInstance::emitNewParticles(float p_DeltaTime)
{
	DirectX::XMFLOAT3 tempPos = DirectX::XMFLOAT3(m_SysPosition.x, m_SysPosition.y, m_SysPosition.z);
	DirectX::XMFLOAT3 tempVelocity = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	DirectX::XMFLOAT4 tempColor = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);

	bool emitParticle = false;
	m_AccumulatedTime += p_DeltaTime;

	//check if new particles are to be emitted or not
	if(m_AccumulatedTime > (1000.f/m_ParticleEffectDef->particlesPerSec))
	{
		m_AccumulatedTime = 0.f;
		emitParticle = true;
	}

	if((emitParticle) && (m_CurrentParticleCount < (m_ParticleEffectDef->maxParticles -1)))
	{
		m_CurrentParticleCount++;

		//Put all the new data for the new particle into one container
		Particle tempParticle(tempPos, tempVelocity, tempColor, m_ParticleEffectDef->size, 0.f);

		//Add the new particle to the others in the same system
		m_ParticleList.push_back(tempParticle);
	}
}

void ParticleInstance::updateBuffers(ID3D11DeviceContext *p_DeviceContext, DirectX::XMFLOAT3 *p_CameraPosition,
									 DirectX::XMFLOAT4X4 *p_ViewMatrix, DirectX::XMFLOAT4X4 *p_ProjectionMatrix)
{
	particlecBuffer pcb;
	pcb.viewM = *p_ViewMatrix;
	pcb.projM = *p_ProjectionMatrix;
	pcb.cameraPos = *p_CameraPosition;
	p_DeviceContext->UpdateSubresource(m_Buffer->getBufferPointer(),NULL,NULL, &pcb, NULL,NULL);
}

void ParticleInstance::render()
{

}

void ParticleInstance::setPosition(DirectX::XMFLOAT4 p_NewPosition)
{
	m_SysPosition = p_NewPosition;
}

