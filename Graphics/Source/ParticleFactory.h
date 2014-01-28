#pragma once

#include "ParticleEffectDefinition.h"
#include "GraphicsExceptions.h"
#include "VRAMInfo.h"
#include "Buffer.h"

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
	ID3D11ShaderResourceView *m_DiffuseTexture;

	loadParticleTextureCallBack m_LoadParticleTexture;
	void *m_LoadParticleTextureUserdata;

	DirectX::XMFLOAT4X4 m_ViewMatrix;
	DirectX::XMFLOAT4X4 m_ProjectionMatrix;
	DirectX::XMFLOAT3 m_CameraPosition;

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
	void initialize(ID3D11ShaderResourceView *p_DiffuseTexture);

	/**
	* Shuts down the factory and releases the memory allocated. Nulls all pointers.
	*/
	virtual void shutdown(void);
		
	/**
	* Creates a static particle system with buffers and connects the textures to it.
	* @param p_Filename the particle file to read
	* @return copy of the created particle system
	*/
	virtual ParticleEffectDefinition* createParticleSystem(const char* p_Filename, const char* p_EffectName, DirectX::XMFLOAT4X4 *p_ViewMatrix, 
		DirectX::XMFLOAT4X4 *p_ProjectionMatrix, DirectX::XMFLOAT3 *p_CameraPos);
	
	/**
	* Set the function to load a texture to a particle.
	* @param p_LoadParticleTexture the function to be called whenever a texture is to be loaded.
	* @param p_UserData user defined data
	*/
	void setLoadParticleTextureCallBack(loadParticleTextureCallBack p_LoadParticleTexture, void *p_Userdata);

	ID3D11ShaderResourceView *ParticleFactory::getDiffuseTexture();

protected:
	ParticleFactory();
	~ParticleFactory();

private:
	Buffer createParticleBuffer(unsigned int p_MaxParticles);

	void loadTexture(ParticleEffectDefinition &particle, const char *p_Filename);


};
