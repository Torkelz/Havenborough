#include "ParticleFactory.h"



ParticleFactory *ParticleFactory::m_Instance = nullptr;

ParticleFactory *ParticleFactory::getInstance(void)
{
	if(!m_Instance)
		m_Instance = new ParticleFactory();

	return m_Instance;
}

void ParticleFactory::initialize(vector<pair<string, ID3D11ShaderResourceView*>> *p_TextureList)
{
	if(!m_Instance)
		throw ParticleFactoryException("Error when initializing ModelFactory, no instance exists", __LINE__, __FILE__);

	m_TextureList = p_TextureList;
}

void ParticleFactory::shutdown(void)
{
	SAFE_DELETE(m_Instance);
}

ParticleDefinition ParticleFactory::createParticleSystem(const char* p_Filename)
{

}

ParticleFactory::ParticleFactory()
{

}

ParticleFactory::~ParticleFactory()
{

}
