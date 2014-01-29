#pragma once

#include "ParticleEffectDefinition.h"
#include "ParticleInstance.h"
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
	vector<pair<string, ID3D11ShaderResourceView*>> *m_TextureList;

	loadParticleTextureCallBack m_LoadParticleTexture;
	void *m_LoadParticleTextureUserdata;

	DirectX::XMFLOAT4X4 m_ViewMatrix;
	DirectX::XMFLOAT4X4 m_ProjectionMatrix;
	DirectX::XMFLOAT3 m_CameraPosition;

public:

	/**
	* Initialize the factory.
	* p_TextureList pointer to the texture list pair 
	*/
	void initialize(vector<pair<string, ID3D11ShaderResourceView*>> *p_TextureList);

	/**
	* Creates a static particle system with buffers and connects the textures to it.
	* @param p_Filename the particle file to read
	* @return copy of the created particle system
	*/
	virtual ParticleEffectDefinition::ptr createParticleEffectDefinition(const char* p_Filename, const char* p_EffectName);

	virtual ParticleInstance::ptr createParticleInstance(ParticleEffectDefinition::ptr p_Effect);
	
	/**
	* Set the function to load a texture to a particle.
	* @param p_LoadParticleTexture the function to be called whenever a texture is to be loaded.
	* @param p_UserData user defined data
	*/
	void setLoadParticleTextureCallBack(loadParticleTextureCallBack p_LoadParticleTexture, void *p_Userdata);

private:
	Buffer* createParticleBuffer(unsigned int p_MaxParticles);

	ID3D11ShaderResourceView *loadTexture(const char *p_Filename, const char *p_Identifier);


	ID3D11ShaderResourceView *getTextureFromList(string p_Identifier);

};
