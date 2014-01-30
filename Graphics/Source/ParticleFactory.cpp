#include "ParticleFactory.h"

#include <boost/filesystem.hpp>

void ParticleFactory::initialize(vector<pair<string, ID3D11ShaderResourceView*>> *p_TextureList)
{
	m_TextureList = p_TextureList;
}

ParticleEffectDefinition::ptr ParticleFactory::createParticleEffectDefinition(const char* p_Filename, const char* p_EffectName)
{
	//ParticleLoader particleLoader;
	//particleLoader.loadXMLFile(p_Filename);


	ParticleEffectDefinition::ptr particleSystem;

	particleSystem.reset(new ParticleEffectDefinition()); 

	particleSystem->diffuseTexture = loadTexture(p_Filename, "Particle1.dds");
	particleSystem->textureResourceName = "Particle1.dds";
	particleSystem->maxParticles = 50;
	particleSystem->particlesPerSec = 2;
	particleSystem->maxLife = 60.f;
	particleSystem->size = DirectX::XMFLOAT2(1.f, 1.f);
	particleSystem->particleSystemName = "fire";
	particleSystem->particlePositionDeviation = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	particleSystem->velocityDeviation = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	particleSystem->particleColorDeviation = DirectX::XMFLOAT4(0.5f, 0.7f, 0.3f, 0.95f);

	return particleSystem;
}

ParticleInstance::ptr ParticleFactory::createParticleInstance(ParticleEffectDefinition::ptr p_Effect)
{
	ParticleInstance::ptr instance(new ParticleInstance);
	instance->init(createParticleBuffer(p_Effect->maxParticles), p_Effect);

	return instance;
}

void ParticleFactory::setLoadParticleTextureCallBack(loadParticleTextureCallBack p_LoadParticleTexture, void* p_Userdata)
{
	m_LoadParticleTexture = p_LoadParticleTexture;
	m_LoadParticleTextureUserdata = p_Userdata;
}

Buffer* ParticleFactory::createParticleBuffer(unsigned int p_MaxParticles)
{
	Buffer *buffer;

	Buffer::Description cbDesc;
	cbDesc.initData = NULL; //can be needing a flag of some sort
	cbDesc.usage = Buffer::Usage::CPU_WRITE;
	cbDesc.numOfElements = p_MaxParticles;
	cbDesc.sizeOfElement = sizeof(particlecBuffer);
	cbDesc.type = Buffer::Type::CONSTANT_BUFFER_ALL;

	buffer = WrapperFactory::getInstance()->createBuffer(cbDesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(particlecBuffer));

	return buffer;
}

ID3D11ShaderResourceView *ParticleFactory::loadTexture(const char *p_Filename, const char *p_Identifier)
{
	boost::filesystem::path particlePath(p_Filename);
	boost::filesystem::path parentDir(particlePath.parent_path().parent_path() / "textures");
		
	boost::filesystem::path diff = (p_Identifier == "NONE" || p_Identifier == "Default_COLOR.dds") ?
		"assets/textures/Default_COLOR.dds" : parentDir / p_Identifier;

	m_LoadParticleTexture(p_Identifier, diff.string().c_str(), m_LoadParticleTextureUserdata);

	return getTextureFromList(p_Identifier);
}

ID3D11ShaderResourceView *ParticleFactory::getTextureFromList(string p_Identifier)
{
	for(auto it = m_TextureList->begin(); it != m_TextureList->end(); ++it)
	{
		if(it->first == p_Identifier)
		{
			return it->second;
		}
	}

	return nullptr;
}