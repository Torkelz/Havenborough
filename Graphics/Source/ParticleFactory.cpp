#include "ParticleFactory.h"
#include "WrapperFactory.h"
#include "GraphicsExceptions.h"
#include "VRAMInfo.h"
#include "Utilities/MemoryUtil.h"
#include "../3rd party/tinyxml2/tinyxml2.h"
#include "GraphicsExceptions.h"

#include <boost/filesystem.hpp>

using std::string;
using std::vector;
using std::pair;

ParticleFactory::~ParticleFactory()
{
	SAFE_RELEASE(m_Sampler);
}

void ParticleFactory::initialize(std::map<std::string, ID3D11ShaderResourceView*> *p_TextureList, ID3D11Device* p_Device)
{
	m_TextureList = p_TextureList;
	createSampler(p_Device);
}

ParticleEffectDefinition::ptr ParticleFactory::createParticleEffectDefinition(const char* p_FilePath, const char* p_EffectName)
{	
	ParticleEffectDefinition::ptr particleSystem;
	particleSystem.reset(new ParticleEffectDefinition());

	std::vector<char> buffer;
	const char* name, *path;

	std::ifstream file(p_FilePath);
	if(!file)
	{
		throw GraphicsException("File failed to load", __LINE__, __FILE__);
	}
	file >> std::noskipws;

	std::copy(std::istream_iterator<char>(file), std::istream_iterator<char>(), std::back_inserter(buffer));
	buffer.push_back('\0');

	tinyxml2::XMLDocument particlesList;

	tinyxml2::XMLError error = particlesList.Parse(buffer.data());
	if(error)
	{
		throw GraphicsException("File not of type 'XML'",__LINE__, __FILE__);
	}
	tinyxml2::XMLElement* particlesFile = particlesList.FirstChildElement("Particle");
	if(particlesFile == nullptr)
	{
		throw GraphicsException("File not of type 'Particle'", __LINE__, __FILE__);
	}

	tinyxml2::XMLElement* Effect = particlesFile->FirstChildElement("Effect");
	if(Effect == nullptr)
	{
		throw GraphicsException("File not containing any effects", __LINE__, __FILE__);
	}

	if(Effect->Attribute("effectName", p_EffectName))
	{
		particleSystem->particleSystemName = p_EffectName;
	}


	
	particleSystem->textureResourceName = "Particle1.dds";
	particleSystem->maxParticles = 900;
	particleSystem->particlesPerSec = 900;
	particleSystem->maxLife = 0.6f;
	particleSystem->maxLifeDeviation = 0.2f;
	particleSystem->size = DirectX::XMFLOAT2(8.f, 8.f);
	particleSystem->particlePositionDeviation = 10.f;
	particleSystem->velocityDeviation = 40.f;
	particleSystem->particleColorDeviation = DirectX::XMFLOAT4(0.2f, 0.15f, 0.0f, 0.2f);

	particleSystem->diffuseTexture = loadTexture(p_FilePath, particleSystem->particleSystemName.c_str());
	particleSystem->sampler = m_Sampler;
	return particleSystem;
}

ParticleInstance::ptr ParticleFactory::createParticleInstance(ParticleEffectDefinition::ptr p_Effect)
{
	ParticleInstance::ptr instance(new ParticleInstance);
	instance->init(
		createConstBuffer(),
		createParticleBuffer(p_Effect->maxParticles),
		p_Effect);

	return instance;
}

void ParticleFactory::setLoadParticleTextureCallBack(loadParticleTextureCallBack p_LoadParticleTexture, void* p_Userdata)
{
	m_LoadParticleTexture = p_LoadParticleTexture;
	m_LoadParticleTextureUserdata = p_Userdata;
}

std::shared_ptr<Buffer> ParticleFactory::createParticleBuffer(unsigned int p_MaxParticles)
{
	Buffer::Description cbDesc;
	cbDesc.initData = nullptr;
	cbDesc.usage = Buffer::Usage::CPU_WRITE;
	cbDesc.numOfElements = p_MaxParticles;
	cbDesc.sizeOfElement = sizeof(ShaderParticle);
	cbDesc.type = Buffer::Type::VERTEX_BUFFER;

	std::shared_ptr<Buffer> buffer(WrapperFactory::getInstance()->createBuffer(cbDesc));
	VRAMInfo::getInstance()->updateUsage(sizeof(particlecBuffer));

	return buffer;
}

std::shared_ptr<Buffer> ParticleFactory::createConstBuffer()
{
	Buffer::Description cbDesc;
	cbDesc.initData = NULL;
	cbDesc.usage = Buffer::Usage::DEFAULT;
	cbDesc.numOfElements = 1;
	cbDesc.sizeOfElement = sizeof(particlecBuffer);
	cbDesc.type = Buffer::Type::CONSTANT_BUFFER_ALL;

	std::shared_ptr<Buffer> buffer(WrapperFactory::getInstance()->createBuffer(cbDesc));
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
	return m_TextureList->at(p_Identifier);
}

void ParticleFactory::createSampler(ID3D11Device* p_Device)
{
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU			= D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV			= D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW			= D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.ComparisonFunc	= D3D11_COMPARISON_NEVER;
	sd.MinLOD			= 0;
	sd.MaxLOD           = D3D11_FLOAT32_MAX;

	m_Sampler = nullptr;
	p_Device->CreateSamplerState( &sd, &m_Sampler );
}