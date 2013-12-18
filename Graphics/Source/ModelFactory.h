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
public:
	/**
	* Callback for loading a texture to a model.
	* @param p_ResourceName the resource name of the texture
	* @param p_FilePath path to where the texture is located
	* @param p_UserData user defined data
	*/
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
	* @param p_IsAnimated check whether the model should be animated or not, true = animated, false = static
	* @return copy of the created model
	*/
	virtual ModelDefinition createModel(const char *p_Filename);

	/**
	* Set the function to load a texture to a model.
	* @param p_LoadModelTexture the function to be called whenever a texture is to be loaded.
	* @param p_UserData user defined data
	*/
	void setLoadModelTextureCallBack(loadModelTextureCallBack p_LoadModelTexture, void *p_Userdata);

protected:
	ModelFactory(void);
	~ModelFactory(void);
	
private:
	Buffer::Description createBufferDescription(const vector<StaticVertex> &p_VertexData, Buffer::Usage p_Usage);
	Buffer::Description createBufferDescription(const vector<AnimatedVertex> &p_VertexData, Buffer::Usage p_Usage);

	void loadTextures(ModelDefinition &model, const char *p_Filename, unsigned int p_NumOfMaterials,
		const vector<Material> &p_Materials);
	ID3D11ShaderResourceView *getTextureFromList(string p_Identifier);
};