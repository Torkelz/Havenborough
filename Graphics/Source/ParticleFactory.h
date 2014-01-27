#pragma once

#include "ParticleDefinition.h"
#include "GraphicsExceptions.h"


#include <DirectXMath.h>
#include <vector>
#include <string>

using std::vector;
using std::string;
using std::pair;

class ParticleFactory
{
public:
	/**
	* Callback for loading a texture to a particle.
	* @param p_ResourceName the resource name of the texture
	* @param p_FilePath path to where the texture is located
	* @param p_UserData user defined data
	*/
	typedef void (*loadParticleTextureCallBack)(const char *p_ResourceName, const char *p_FilePath, void *p_Userdata);

private:
	static ParticleFactory *m_Instance;
	vector<pair<string, ID3D11ShaderResourceView*>> *m_TextureList;

	loadParticleTextureCallBack m_LoadParticleTexture;
	void *m_LoadParticleTextureUserdata;

public:
	
	/**
	* Gets an instance of the particle factory.
	* @return a pointer to the instance
	*/
	static ParticleFactory *getInstance(void);
	
	/**
	* Initialize the factory.
	* p_TextureList pointer to the texture list pair 
	*/
	void initialize(vector<pair<string, ID3D11ShaderResourceView*>> *p_TextureList);

	/**
	* Shuts down the factory and releases the memory allocated. Nulls all pointers.
	*/
	virtual void shutdown(void);
		
	/**
	* Creates a static particle system with buffers and connects the textures to it.
	* @param p_Filename the particle file to read
	* @return copy of the created particle system
	*/
	virtual ParticleDefinition createParticleSystem(const char *p_Filename);
	
	/**
	* Set the function to load a texture to a particle.
	* @param p_LoadParticleTexture the function to be called whenever a texture is to be loaded.
	* @param p_UserData user defined data
	*/
	void setLoadParticleTextureCallBack(loadParticleTextureCallBack p_LoadParticleTexture, void *p_Userdata);


protected:
	ParticleFactory();
	~ParticleFactory();

private:
	Buffer::Description createBufferDescription(const vector<particlecBuffer> &p_Element);

	void loadTextures(ParticleDefinition &particle, const char *p_Filename, unsigned int p_NumOfMaterials, const vector<Material> &p_Materials);
	ID3D11ShaderResourceView *getTextureFromList(string p_Identifier);

};
