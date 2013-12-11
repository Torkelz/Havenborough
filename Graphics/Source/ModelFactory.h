#pragma once
#include "Util.h"
#include "WrapperFactory.h"
#include "ModelDefinition.h"
//#include <d3d11.h>

//using namespace DirectX;

class ModelFactory
{
private:
	//struct Vertex
	//{
	//	XMFLOAT4 position;
	//	XMFLOAT3 normal;
	//	XMFLOAT2 uv;
	//	XMFLOAT3 tangent;
	//	XMFLOAT3 binormal;
	//	Vertex(){}
	//	Vertex(XMFLOAT3 _position, XMFLOAT3 _normal, XMFLOAT2 _uv, XMFLOAT3 _tangent)
	//	{
	//		position = XMFLOAT4(_position.x, _position.y, _position.z, 1.0f);
	//		normal = _normal;
	//		uv = _uv;
	//		tangent = _tangent;

	//		//might be wrong
	//		XMStoreFloat3(&binormal, XMVector3Cross(XMLoadFloat3(&tangent), XMLoadFloat3(&normal)));
	//	}
	//};

	static ModelFactory *m_Instance;

public:
	static ModelFactory *getInstance(void);

	virtual void shutdown(void);

	ModelDefinition createStaticModel(const char *p_Filename);

	ModelDefinition createAnimatedModel(const char *p_Filename);

private:
	ModelFactory(void);
	virtual ~ModelFactory(void);
};

