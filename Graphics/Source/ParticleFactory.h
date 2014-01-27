#pragma once

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
	typedef void (*loadModelTextureCallBack)(const char *p_ResourceName, const char *p_FilePath, void *p_Userdata);

private:
	vector<pair<string, ID3D11ShaderResourceView*>> *m_TextureList;

public:


protected:
	ParticleFactory();
	~ParticleFactory();

private:


};
