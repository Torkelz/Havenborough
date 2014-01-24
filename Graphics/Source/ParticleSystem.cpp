#include "ParticleSystem.h"

#include<fstream>
#include<sstream>

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
	m_Device = nullptr;
	m_DeviceContext = nullptr;

	m_DepthStencilView = nullptr;
	m_RenderTarget = nullptr;
	SAFE_RELEASE(m_Sampler);
	SAFE_RELEASE(m_RasterState);

	m_CameraPosition = nullptr;
	m_ViewMatrix = nullptr;
	m_ProjectionMatrix = nullptr;

	SAFE_DELETE(m_Buffer);
}

ParticleSystem ParticleSystem::loadParticleSystemFromFile(const char* p_filename)
{
	//use std::istream!
	return;// false;
}

void ParticleSystem::init(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext,
						  DirectX::XMFLOAT3 *p_CameraPosition, DirectX::XMFLOAT4X4 *p_ViewMatrix,
						  DirectX::XMFLOAT4X4 *p_ProjectionMatrix, ID3D11DepthStencilView* p_DepthStencilView,
						  ID3D11RenderTargetView *p_RenderTarget)
{
	m_Device = p_Device;
	m_DeviceContext = p_DeviceContext;

	m_DepthStencilView = p_DepthStencilView;
	m_RenderTarget = p_RenderTarget;

	m_CameraPosition = p_CameraPosition;
	m_ViewMatrix = p_ViewMatrix;
	m_ProjectionMatrix = p_ProjectionMatrix;

	createParticleBuffer();
}

void ParticleSystem::createParticleBuffer()
{
	particlecBuffer pcb;
	pcb.viewM = *m_ViewMatrix;
	pcb.projM = *m_ProjectionMatrix;
	pcb.cameraPos = *m_CameraPosition;
	
	Buffer::Description cbDesc;
	cbDesc.initData = &pcb;
	cbDesc.usage = Buffer::Usage::CPU_WRITE;
	cbDesc.numOfElements = m_MaxParticles;
	cbDesc.sizeOfElement = sizeof(particlecBuffer);
	cbDesc.type = Buffer::Type::CONSTANT_BUFFER_ALL;

	m_Buffer = WrapperFactory::getInstance()->createBuffer(cbDesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(particlecBuffer));
}

void ParticleSystem::update(float p_DeltaTime)
{
	killOldParticles();

	emitNewParticles(p_DeltaTime);

	updateParticles(p_DeltaTime);

	updateBuffers();
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

static bool isDying(Particle& p_Particle)
{
	return p_Particle.life >= p_Particle.lifeMax;
}

void ParticleSystem::killOldParticles()
{
	//Will go thou the list of particles in the system and remove any particle that are to old

	auto removeIt = std::remove_if(m_ParticlesToSys.begin(),m_ParticlesToSys.end(), isDying);
	m_ParticlesToSys.erase(removeIt, m_ParticlesToSys.end());

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

void ParticleSystem::updateBuffers()
{
	particlecBuffer pcb;
	pcb.viewM = *m_ViewMatrix;
	pcb.projM = *m_ProjectionMatrix;
	pcb.cameraPos = *m_CameraPosition;
	m_DeviceContext->UpdateSubresource(m_Buffer->getBufferPointer(),NULL,NULL, &pcb, NULL,NULL);
}

void ParticleSystem::render()
{

}