#include "ParticleInstance.h"

ParticleInstance::ParticleInstance()
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

ParticleInstance::~ParticleInstance()
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;

	m_DepthStencilView = nullptr;
	m_RenderTarget = nullptr;
	SAFE_RELEASE(m_Sampler);
	SAFE_RELEASE(m_RasterState);

}

void ParticleInstance::init(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext, DirectX::XMFLOAT3 *p_CameraPosition,
							DirectX::XMFLOAT4X4 *p_ViewMatrix, DirectX::XMFLOAT4X4 *p_ProjectionMatrix,
						  ID3D11DepthStencilView* p_DepthStencilView, ID3D11RenderTargetView *p_RenderTarget)
{
	m_Device		= p_Device;
	m_DeviceContext = p_DeviceContext;

	m_DepthStencilView	= p_DepthStencilView;
	m_RenderTarget		= p_RenderTarget;

	m_CameraPosition	= p_CameraPosition;
	m_ViewMatrix		= p_ViewMatrix;
	m_ProjectionMatrix	= p_ProjectionMatrix;

	m_Buffer ;
}


void ParticleInstance::update(float p_DeltaTime) //particle instance
{
	killOldParticles();

	emitNewParticles(p_DeltaTime);

	updateParticles(p_DeltaTime);

	updateBuffers();
}

static bool isDying(Particle& p_Particle) //instance
{
	return p_Particle.life >= p_Particle.lifeMax;
}

void ParticleInstance::killOldParticles() //instance
{
	//Will go thou the list of particles in the system and remove any particle that are to old

	auto removeIt = std::remove_if(m_ParticleList.begin(),m_ParticleList.end(), isDying);
	m_ParticleList.erase(removeIt, m_ParticleList.end());

}

void ParticleInstance::updateParticles(float p_DeltaTime) //instance
{
	//Update the position of every particle in the system by its velocity and based on the delta time
	for(auto& part : m_ParticleList)
	{
		part.Position = DirectX::XMFLOAT4(((part.Position.x + part.Velocity.x) * p_DeltaTime),
			((part.Position.y + part.Velocity.y) * p_DeltaTime),
			((part.Position.z + part.Velocity.z) * p_DeltaTime),
			part.Position.w);
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
	if(m_AccumulatedTime > (1000.f/m_ParticlesPerSecound)) //update i particle instance
	{
		m_AccumulatedTime = 0.f;
		emitParticle = true;
	}

	if((emitParticle) && (m_CurrentParticleCount < (m_MaxParticles -1)))
	{
		m_CurrentParticleCount++;

		//Put all the new data for the new particle into one container
		Particle tempParticle(tempPos, tempVelocity, tempColor, m_SizeX, m_SizeY, 0.f, m_ParticleMaxLife);

		//Add the new particle to the others in the same system
		m_ParticleList.push_back(tempParticle);
	}
}

void ParticleInstance::updateBuffers() //instance
{
	particlecBuffer pcb;
	pcb.viewM = *m_ViewMatrix;
	pcb.projM = *m_ProjectionMatrix;
	pcb.cameraPos = *m_CameraPosition;
	m_DeviceContext->UpdateSubresource(m_Buffer->getBufferPointer(),NULL,NULL, &pcb, NULL,NULL);
}

void ParticleInstance::render() //instance
{

}

