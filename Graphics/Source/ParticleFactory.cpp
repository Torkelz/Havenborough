#include "ParticleFactory.h"

#include <boost/filesystem.hpp>


ParticleFactory::ParticleFactory()
{

}

ParticleFactory::~ParticleFactory()
{

}

ParticleFactory *ParticleFactory::m_Instance = nullptr;

ParticleFactory *ParticleFactory::getInstance(void)
{
	if(!m_Instance)
		m_Instance = new ParticleFactory();

	return m_Instance;
}


void ParticleFactory::initialize(ID3D11ShaderResourceView *p_DiffuseTexture)
{
	if(!m_Instance)
		throw ParticleFactoryException("Error when initializing ModelFactory, no instance exists", __LINE__, __FILE__);

	m_DiffuseTexture = p_DiffuseTexture;
}

void ParticleFactory::shutdown(void)
{
	SAFE_DELETE(m_Instance);
}

ParticleEffectDefinition* ParticleFactory::createParticleSystem(const char* p_Filename, const char* p_EffectName, DirectX::XMFLOAT4X4 *p_ViewMatrix, 
										   DirectX::XMFLOAT4X4 *p_ProjectionMatrix, DirectX::XMFLOAT3 *p_CameraPos)
{
	m_ViewMatrix = *p_ViewMatrix;
	m_ProjectionMatrix = *p_ProjectionMatrix;
	m_CameraPosition = *p_CameraPos;

	//ParticleLoader particleLoader;
	//particleLoader.loadXMLFile(p_Filename);

	//TEMP VARIABLES! (until file is loaded in)
	/*ID3D11ShaderResourceView* diffuseTexture;

	unsigned int maxParticles = 50;
	unsigned int particlesPerSec = 2;
	float maxLife = 60.f;
	string particleSystemName = "fire";
	DirectX::XMFLOAT3	particlePositionDeviation(0.f, 0.f, 0.f);
	DirectX::XMFLOAT3	velocityDeviation(0.f, 0.f, 0.f);
	DirectX::XMFLOAT4	particleColorDeviation(0.5f, 0.7f, 0.3f, 0.95f);*/



	ParticleEffectDefinition* particleSystem;

	particleSystem = new ParticleEffectDefinition();


	

	Buffer buffer;
	buffer = createParticleBuffer(50);

	

}

void ParticleFactory::setLoadParticleTextureCallBack(loadParticleTextureCallBack p_LoadParticleTexture, void* p_Userdata)
{
	m_LoadParticleTexture = p_LoadParticleTexture;
	m_LoadParticleTextureUserdata = p_Userdata;
}

Buffer ParticleFactory::createParticleBuffer(unsigned int p_MaxParticles)
{
	Buffer *buffer;

	particlecBuffer pcb;
	pcb.viewM = m_ViewMatrix;
	pcb.projM = m_ProjectionMatrix;
	pcb.cameraPos = m_CameraPosition;

	Buffer::Description cbDesc;
	cbDesc.initData = &pcb;
	cbDesc.usage = Buffer::Usage::CPU_WRITE;
	cbDesc.numOfElements = p_MaxParticles;
	cbDesc.sizeOfElement = sizeof(particlecBuffer);
	cbDesc.type = Buffer::Type::CONSTANT_BUFFER_ALL;

	buffer = WrapperFactory::getInstance()->createBuffer(cbDesc);
	VRAMInfo::getInstance()->updateUsage(sizeof(particlecBuffer));

	return *buffer;
}

void ParticleFactory::loadTexture(ParticleEffectDefinition &p_Particle, const char *p_Filename)
{
	boost::filesystem::path particlePath(p_Filename);
	boost::filesystem::path parentDir(particlePath.parent_path().parent_path() / "textures");

	ID3D11ShaderResourceView *diffuse;
	
	boost::filesystem::path diff = (p_Filename == "NONE" || p_Filename == "Default_COLOR.dds") ?
		"assets/textures/Default_COLOR.dds" : parentDir / p_Filename;

	m_LoadParticleTexture(p_Filename, diff.string().c_str(), m_LoadParticleTextureUserdata);
	
	diffuse = getDiffuseTexture();

	p_Particle.diffuseTexture = diffuse;
}

ID3D11ShaderResourceView *ParticleFactory::getDiffuseTexture()
{
	return m_DiffuseTexture;
}
