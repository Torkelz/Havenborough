#pragma once
#include "Util.h"
#include "WrapperFactory.h"
#include "ModelDefinition.h"
#include "ModelBinaryLoader.h"
#include "GraphicsExceptions.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

using namespace DirectX;
using std::vector;
using std::string;
using std::pair;

class ModelFactory
{
private:
	

public:
	typedef void (*loadModelTextureCallBack)(const char *p_ResourceName, const char *p_FilePath, void *p_Userdata);

private:
	static ModelFactory *m_Instance;
	vector<pair<string, ID3D11ShaderResourceView*>> *m_TextureList;

	loadModelTextureCallBack m_LoadModelTexture;
	void *m_LoadModelTextureUserdata;

public:

	/**
	* Gets an instance of the model factory.
	* @return a pointer to the instance
	*/
	static ModelFactory *getInstance(void);

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
	* Creates a static model with buffers and connects the textures to it.
	* @param p_Filename the model file to read
	* @return copy of the created model
	*/
	ModelDefinition createStaticModel(const char *p_Filename);

	/**
	* Creates an animated model with buffers and connects the textures to it.
	* @param p_Filename the model file to read
	* @return copy of the created model
	*/
	ModelDefinition createAnimatedModel(const char *p_Filename);

	/**
	* Callback function for loading texture to model.
	*/
	void setLoadModelTextureCallBack(loadModelTextureCallBack p_LoadModelTexture, void *p_Userdata);

private:
	ModelFactory(void);
	virtual ~ModelFactory(void);
	
	Buffer::Description createBufferDescription(vector<Vertex> p_VertexBuffer);

	void loadTextures(const char *p_Filename, unsigned int nrIndexBuffers, const vector<Material> &tempM, ModelDefinition &model);
	ID3D11ShaderResourceView *getTextureFromList(string p_Identifier);
};