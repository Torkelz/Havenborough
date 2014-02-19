#pragma once
#include "ParticleInstance.h"
#include "ResourceTranslator.h"

#include <map>
class ParticleFactory
{
public:
	/**
	* Callback for loading a texture to a particle.
	*
	* @param p_ResourceName the resource name of the texture
	* @param p_FilePath path to where the texture is located
	* @param p_UserData user defined data
	*/
	typedef void (*loadParticleTextureCallBack)(const char *p_ResourceName, const char *p_FilePath, void *p_Userdata);

private:
	std::map<std::string, ID3D11ShaderResourceView*> *m_TextureList;
	ID3D11SamplerState* m_Sampler;

	loadParticleTextureCallBack m_LoadParticleTexture;
	void *m_LoadParticleTextureUserdata;

public:
	~ParticleFactory();

	/**
	* Initialize the factory.
	*
	* @param p_TextureList pointer to the texture list pair 
	* @param p_Device pointer to the device
	*/
	void initialize(std::map<std::string, ID3D11ShaderResourceView*> *p_TextureList, ID3D11Device* p_Device);

	/**
	* Creates a list of static particle systems with buffers and connects the textures to it.
	*
	* @param p_FilePath the name of the file where the definitions are stored
	* @return list of definitions
	*/
	virtual std::vector<ParticleEffectDefinition::ptr> createParticleEffectDefinition(const char* p_FilePath);

	/**
	 * Create an instance of a particle effect from an already loaded definition.
	 *
	 * @param p_Effect a pointer to what definition to use
	 * @return a pointer to the new particle instance created
	 */
	virtual ParticleInstance::ptr createParticleInstance(ParticleEffectDefinition::ptr p_Effect);
	
	/**
	* Set the function to load a texture to a particle.
	*
	* @param p_LoadParticleTexture the function to be called whenever a texture is to be loaded.
	* @param p_UserData user defined data
	*/
	void setLoadParticleTextureCallBack(loadParticleTextureCallBack p_LoadParticleTexture, void *p_Userdata);

private:
	std::shared_ptr<Buffer> createParticleBuffer(unsigned int p_MaxParticles);
	std::shared_ptr<Buffer> createConstBuffer();

	ID3D11ShaderResourceView *loadTexture(const char *p_Filepath, const char *p_Identifier);

	ID3D11ShaderResourceView *getTextureFromList(std::string p_Identifier);
	void createSampler(ID3D11Device* p_Device);
};