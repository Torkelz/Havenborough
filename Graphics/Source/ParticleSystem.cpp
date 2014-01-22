#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(void)
{
	m_SysPosition			= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_SysType				= nullptr;

	m_SysName				= nullptr;
	m_TextureFilePath		= nullptr;
	m_TextureU				= 0.f;
	m_TextureV				= 0.f;
	m_ParticleLife			= 0.f;

	m_MaxParticles			= 0;
	m_ParticleDeviation		= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_VelocityDeviation		= 0.f;
	m_ParticlesPerSecound	= 0.f;

}


ParticleSystem::~ParticleSystem(void)
{
}

bool ParticleSystem::loadParticleSystemFromFile(const char* p_filename)
{
	//use std::istream!
	//#include<fstream>
	//#include<sstream>
	return false;
}

void ParticleSystem::init()
{

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

}

void ParticleSystem::killOldParticles()
{

}

void ParticleSystem::updateParticles(float p_DeltaTime)
{

}

void ParticleSystem::updateBuffers(ID3D11DeviceContext* p_DeviceContext)
{

}

void ParticleSystem::render()
{

}