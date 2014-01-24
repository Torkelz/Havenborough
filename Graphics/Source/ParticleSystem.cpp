#include "ParticleSystem.h"


ParticleSystem::ParticleSystem()
{
	m_SysPosition			= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_SysType				= nullptr;

	m_SysName				= nullptr;
	m_TextureFilePath		= nullptr;
	m_TextureU				= 0.f;
	m_TextureV				= 0.f;
	m_ParticleMaxLife		= 0.f;
	m_MaxParticles			= 0;
	m_ParticlePositionDeviation		= DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	m_VelocityDeviation		= DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	m_ParticlesPerSecound	= 0.f;
	m_CurrentParticleCount	= 0.f;
	m_AccumulatedTime		= 0.f;
}


ParticleSystem::~ParticleSystem()
{
}

bool ParticleSystem::loadParticleSystemFromFile(const char* p_filename)
{
	//use std::istream!
	//#include<fstream>
	//#include<sstream>

	//set "m_" variables
	return false;
}

void ParticleSystem::init()
{
	//set "m_" before a new instance are used or reused
	m_CurrentParticleCount = 0.f;
}

void ParticleSystem::update(float p_DeltaTime, ID3D11DeviceContext* p_DeviceContext)
{
	killOldParticles();

	emitNewParticles(p_DeltaTime);

	updateParticles(p_DeltaTime);

	updateBuffers(p_DeviceContext);
}

void ParticleSystem::emitNewParticles(float p_DeltaTime)
{
	bool emitParticle = false;
	DirectX::XMFLOAT3 tempPos = DirectX::XMFLOAT3();
	DirectX::XMFLOAT3 tempVelocity = DirectX::XMFLOAT3();
	DirectX::XMFLOAT4 tempColor = DirectX::XMFLOAT4();
	


	m_AccumulatedTime += p_DeltaTime;

	//check if new particles are to be emitted or not
	if(m_AccumulatedTime > (1000.f/m_ParticlesPerSecound))
	{
		m_AccumulatedTime = 0.f;
		emitParticle = true;
	}

	if((emitParticle) && (m_CurrentParticleCount < (m_MaxParticles -1)))
	{
		m_CurrentParticleCount++;
		
		//Apply the random effects onto the new particle
		//A new position relative to the system position with the deviation, in cm
		tempPos = DirectX::XMFLOAT3((((float)rand()-(float)rand())/RAND_MAX) * m_ParticlePositionDeviation.x + m_SysPosition.x,
									(((float)rand()-(float)rand())/RAND_MAX) * m_ParticlePositionDeviation.y + m_SysPosition.y,
									(((float)rand()-(float)rand())/RAND_MAX) * m_ParticlePositionDeviation.z + m_SysPosition.z);

		//Add the new velocity to the particle with the deviation, in cm/s
		tempVelocity = DirectX::XMFLOAT3((((float)rand()-(float)rand())/RAND_MAX) * m_VelocityDeviation.x,
										 (((float)rand()-(float)rand())/RAND_MAX) * m_VelocityDeviation.y,
										 (((float)rand()-(float)rand())/RAND_MAX) * m_VelocityDeviation.z);

		//Add a base color on the particle with the deviation
		tempColor = DirectX::XMFLOAT4((((float)rand()-(float)rand())/RAND_MAX) + m_ParticleColorDeviation.x, 
									  (((float)rand()-(float)rand())/RAND_MAX) + m_ParticleColorDeviation.y, 
									  (((float)rand()-(float)rand())/RAND_MAX) + m_ParticleColorDeviation.z,
									  m_ParticleColorDeviation.w);

		//Put all the new data for the new particle into one container
		Particle tempParticle(tempPos, tempVelocity, tempColor, m_SizeX, m_SizeY, 0.f, m_ParticleMaxLife);

		//Add the new particle to the others in the same system
		m_ParticlesToSys.push_back(tempParticle);
	}



}

void ParticleSystem::killOldParticles()
{
	//might not work as intended

	//Will go thou the list of particles in the system and remove any particle that are to old
	//int i = 0;
	//for(auto& part : m_ParticlesToSys)
	//{
	//	if(part.life >= part.lifeMax)
	//	{
	//		m_ParticlesToSys.erase(i)
	//		i++;
	//	}
	//}

	//Example exists in the lobby.cpp and in the check if the player are in the lobby or not
	//Will go thou the list of particles in the system and remove any particle that are to old
	for(auto& part : m_ParticlesToSys)
	{
		//remove_if(m_ParticlesToSys.begin(),m_ParticlesToSys.end(),
		//	[] )

	}
}

void ParticleSystem::updateParticles(float p_DeltaTime)
{
	//Update the position of every particle in the system by its velocity and based on the delta time
	for(auto& part : m_ParticlesToSys)
	{
		part.Position = DirectX::XMFLOAT4(((part.Position.x + part.Velocity.x) * p_DeltaTime),
										  ((part.Position.y + part.Velocity.y) * p_DeltaTime),
										  ((part.Position.z + part.Velocity.z) * p_DeltaTime),
										  part.Position.w);
		part.life += p_DeltaTime;
	}
}

void ParticleSystem::updateBuffers(ID3D11DeviceContext* p_DeviceContext)
{

}

void ParticleSystem::render()
{

}