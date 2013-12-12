#pragma once
#include "Util.h"
#include "WrapperFactory.h"
#include "ModelDefinition.h"
#include "ModelLoader.h"

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class ModelFactory
{
private:
	struct Vertex
	{
		XMFLOAT4 position;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
		Vertex(){}
		Vertex(XMFLOAT3 _position, XMFLOAT3 _normal, XMFLOAT2 _uv, XMFLOAT3 _tangent)
		{
			position = XMFLOAT4(_position.x, _position.y, _position.z, 1.0f);
			normal = _normal;
			uv = _uv;
			tangent = _tangent;

			//might be wrong
			XMStoreFloat3(&binormal, XMVector3Cross(XMLoadFloat3(&tangent), XMLoadFloat3(&normal)));
		}
	};

public:
	typedef void (*loadModelTextureCallBack)(const char *p_ResourceName, const char *p_FilePath, void* p_Userdata);

private:
	static ModelFactory *m_Instance;
	std::vector<std::pair<std::string, ID3D11ShaderResourceView*>> *m_TextureList;

	loadModelTextureCallBack m_LoadModelTexture;
	void* m_LoadModelTextureUserdata;

public:
	static ModelFactory *getInstance(void);
	void initialize(std::vector<std::pair<std::string, ID3D11ShaderResourceView*>> *p_TextureList);
	virtual void shutdown(void);

	ModelDefinition createStaticModel(const char *p_Filename);

	

	ModelDefinition createAnimatedModel(const char *p_Filename);

	void setLoadModelTextureCallBack(loadModelTextureCallBack p_LoadModelTexture, void* p_Userdata);

private:
	ModelFactory(void);
	virtual ~ModelFactory(void);
	
	void loadTextures( const char * p_Filename, unsigned int nrIndexBuffers, vector<ModelLoader::Material> &tempM, ModelDefinition &model);
	ID3D11ShaderResourceView *getTextureFromList(string p_Identifier);
};

