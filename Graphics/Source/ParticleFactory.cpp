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

void ParticleFactory::setLoadParticleTextureCallBack(loadParticleTextureCallBack p_LoadParticleTexture, void* p_Userdata)
{
	m_LoadParticleTexture = p_LoadParticleTexture;
	m_LoadParticleTextureUserdata = p_Userdata;
}

Buffer ParticleFactory::createParticleBuffer(unsigned int p_MaxParticles, DirectX::XMFLOAT4X4 *p_ViewMatrix, 
										   DirectX::XMFLOAT4X4 *p_ProjectionMatrix, DirectX::XMFLOAT3 *p_CameraPos)
{
	Buffer *buffer;

	particlecBuffer pcb;
	pcb.viewM = *p_ViewMatrix;
	pcb.projM = *p_ProjectionMatrix;
	pcb.cameraPos = *p_CameraPos;

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

void ParticleFactory::loadTextures(ParticleDefinition &p_Particle, const char *p_Filename, unsigned int p_NumOfMaterials,  const vector<Material> &p_Materials)
{
	using std::pair;

	boost::filesystem::path particlePath(p_Filename);
	boost::filesystem::path parentDir(particlePath.parent_path().parent_path() / "textures");

	vector<pair<string, ID3D11ShaderResourceView*>> diffuse;

	for(unsigned int i = 0; i < p_NumOfMaterials; i++)
	{
		const Material &material = p_Materials.at(i);
		boost::filesystem::path diff = (material.m_DiffuseMap == "NONE" || material.m_DiffuseMap == "Default_COLOR.dds") ?
			"assets/textures/Default_COLOR.dds" : parentDir / material.m_DiffuseMap;

		m_LoadParticleTexture(material.m_DiffuseMap.c_str(), diff.string().c_str(), m_LoadParticleTextureUserdata);

		diffuse.push_back(std::make_pair(material.m_DiffuseMap, getTextureFromList(material.m_DiffuseMap.c_str())));

	}

	p_Particle.diffuseTexture = diffuse;
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
