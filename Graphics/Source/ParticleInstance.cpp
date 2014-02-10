#include "ParticleInstance.h"
#include <functional>
#include <algorithm>

ParticleInstance::ParticleInstance()
{
	m_SysPosition = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f); // change pos in the "GameLogic.cpp - LoadSandBox()" createParticles for local play

	m_AccumulatedTime = 0.f;
}

ParticleInstance::~ParticleInstance()
{
}

void ParticleInstance::init(std::shared_ptr<Buffer> p_ConstBuffer, std::shared_ptr<Buffer> p_ParticleBuffer, ParticleEffectDefinition::ptr p_ParticleEffectDefinition)
{
	m_ConstBuffer = p_ConstBuffer;
	m_ParticleBuffer = p_ParticleBuffer;
	m_ParticleEffectDef = p_ParticleEffectDefinition;
	m_ParticleList.reserve(m_ParticleEffectDef->maxParticles);
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
		return p.life >= p.maxLife;
	});
	m_ParticleList.erase(removeIt, m_ParticleList.end());

}

void ParticleInstance::updateParticles(float p_DeltaTime)
{
	//Update the position of every particle in the system by its velocity and based on the delta time
	for(auto& part : m_ParticleList)
	{
		part.velocity.y += 50.f * p_DeltaTime;

		part.shaderData.position = DirectX::XMFLOAT3(
			(part.shaderData.position.x + part.velocity.x * p_DeltaTime),
			(part.shaderData.position.y + part.velocity.y * p_DeltaTime),
			(part.shaderData.position.z + part.velocity.z * p_DeltaTime));
		part.life += p_DeltaTime;
	}
}

void ParticleInstance::emitNewParticles(float p_DeltaTime)
{
	DirectX::XMFLOAT3 tempPos = DirectX::XMFLOAT3(m_SysPosition.x, m_SysPosition.y, m_SysPosition.z);
	DirectX::XMFLOAT4 tempColor = DirectX::XMFLOAT4(0.8f, 0.15f, 0.0f, 0.3f);

	m_AccumulatedTime += p_DeltaTime;

	const float timePerParticle = 1.f / m_ParticleEffectDef->particlesPerSec;
	//check if new particles are to be emitted or not
	while (m_AccumulatedTime > timePerParticle)
	{
		m_AccumulatedTime -= timePerParticle;

		if (m_ParticleList.size() >= m_ParticleEffectDef->maxParticles)
		{
			break;
		}

		std::uniform_real_distribution<float> velDistribution(-m_ParticleEffectDef->velocityDeviation, m_ParticleEffectDef->velocityDeviation);
		DirectX::XMFLOAT3 randVel(
			velDistribution(m_RandomEngine),
			velDistribution(m_RandomEngine),
			velDistribution(m_RandomEngine));

		std::uniform_real_distribution<float> posDistribution(-m_ParticleEffectDef->particlePositionDeviation, m_ParticleEffectDef->particlePositionDeviation);
		DirectX::XMFLOAT3 randPos(
			tempPos.x + posDistribution(m_RandomEngine),
			tempPos.y + posDistribution(m_RandomEngine),
			tempPos.z + posDistribution(m_RandomEngine));

		std::uniform_real_distribution<float> lifeDistribution(-m_ParticleEffectDef->maxLifeDeviation, m_ParticleEffectDef->maxLifeDeviation);
		float randMaxLife = m_ParticleEffectDef->maxLife + lifeDistribution(m_RandomEngine);

		std::uniform_real_distribution<float> oneToOneDistribution(-1.f, 1.f);
		DirectX::XMFLOAT4 randColorOffset(
			tempColor.x + oneToOneDistribution(m_RandomEngine) * m_ParticleEffectDef->particleColorDeviation.x,
			tempColor.y + oneToOneDistribution(m_RandomEngine) * m_ParticleEffectDef->particleColorDeviation.y,
			tempColor.z + oneToOneDistribution(m_RandomEngine) * m_ParticleEffectDef->particleColorDeviation.z,
			tempColor.w + oneToOneDistribution(m_RandomEngine) * m_ParticleEffectDef->particleColorDeviation.w);

		//Put all the new data for the new particle into one container
		Particle tempParticle(randPos, randVel, randColorOffset, m_ParticleEffectDef->size, 0.f, randMaxLife);

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
	pcb.cameraPos = DirectX::XMFLOAT4(p_CameraPosition->x, p_CameraPosition->y, p_CameraPosition->z, 1.f);
	pcb.size = m_ParticleEffectDef->size;
	p_DeviceContext->UpdateSubresource(m_ConstBuffer->getBufferPointer(),NULL,NULL, &pcb, NULL,NULL);

	D3D11_MAPPED_SUBRESOURCE resource = {};
	p_DeviceContext->Map(m_ParticleBuffer->getBufferPointer(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &resource);
	ShaderParticle* mappedShaderParticle = (ShaderParticle*)resource.pData;
	for (const auto& part : m_ParticleList)
	{
		*mappedShaderParticle = part.shaderData;
		mappedShaderParticle++;
	}
	p_DeviceContext->Unmap(m_ParticleBuffer->getBufferPointer(), 0);
}

void ParticleInstance::render(ID3D11DeviceContext* p_Context, ID3D11BlendState* p_BlendState)
{
	p_Context->PSSetSamplers(0,1,&m_ParticleEffectDef->sampler);
	m_ParticleBuffer->setBuffer(0);
	m_ConstBuffer->setBuffer(1);

	m_ParticleEffectDef->shader->setShader();
	float data[] = { 1.0f, 1.0f, 1.f, 1.0f};
	m_ParticleEffectDef->shader->setBlendState(p_BlendState, data);

	ID3D11ShaderResourceView *srvs[] =  {
		m_ParticleEffectDef->diffuseTexture
	};
	p_Context->PSSetShaderResources(0, 1, srvs);

	p_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	p_Context->Draw(m_ParticleList.size(), 0);

	static ID3D11ShaderResourceView * const nullsrvs[] = {NULL};
	p_Context->PSSetShaderResources(0, 1, nullsrvs);

	m_ParticleEffectDef->shader->setBlendState(0, data);
	m_ParticleEffectDef->shader->unSetShader();
	m_ParticleBuffer->unsetBuffer(0);
	m_ConstBuffer->unsetBuffer(1);
	p_Context->PSSetSamplers(0,0,0);
}

void ParticleInstance::setPosition(DirectX::XMFLOAT4 p_NewPosition)
{
	m_SysPosition = p_NewPosition;
}

DirectX::XMFLOAT4X4 ParticleInstance::getWorldMatrix() const
{
	DirectX::XMMATRIX world = DirectX::XMMatrixTranslation(m_SysPosition.x, m_SysPosition.y, m_SysPosition.z);
	DirectX::XMFLOAT4X4 worldF;
	DirectX::XMStoreFloat4x4(&worldF, world);
	return worldF;
}

