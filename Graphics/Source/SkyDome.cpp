#include "SkyDome.h"

using namespace DirectX;

SkyDome::SkyDome()
{
	m_NumIndices = 0;
}

SkyDome::~SkyDome()
{
}

bool SkyDome::init(float p_Radius)
{
	buildGeoSphere(1, p_Radius);

	std::vector<XMFLOAT3> initSkym_vertices(m_Vertices.size());
	for(unsigned int i = 0; i < m_Vertices.size(); i++)
	{
		initSkym_vertices[i].x = 0.5f * m_Vertices[i].x;
		initSkym_vertices[i].y = 0.5f * m_Vertices[i].y;
		initSkym_vertices[i].z = 0.5f * m_Vertices[i].z;
	}

	std::vector<XMFLOAT3> temp;

	for(unsigned int i = 0; i < m_InitIndices.size();i++)
	{
		temp.push_back(m_Vertices.at(m_InitIndices.at(i)));
	}
	m_Vertices = temp;
	m_Vertices.shrink_to_fit();
	m_InitIndices.clear();
	m_InitIndices.shrink_to_fit();
	temp.clear();
	temp.shrink_to_fit();

	return true;
}

void SkyDome::buildGeoSphere(unsigned int p_NumSubDivisions, float p_Radius)
{
	//Put a cap on the number of subdivisions
	p_NumSubDivisions = std::min(p_NumSubDivisions, unsigned int(5));

	//Approximate a sphere by tesselating an icosahedron
	const float X = 0.525731f;
	const float Z = 0.850651f;

	XMFLOAT3 pos[12] =
	{
		XMFLOAT3(-X, 0.0f, Z), XMFLOAT3(X, 0.0f, Z),
		XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
		XMFLOAT3(0.0f, Z, X),  XMFLOAT3(0.0f, Z, -X),
		XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
		XMFLOAT3(Z, X, 0.0f),  XMFLOAT3(-Z, X, 0.0f),
		XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
	};

	unsigned int k[60] =
	{
		1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4,
		1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
	};

	m_Vertices.resize(12);
	m_InitIndices.resize(60);

	for(unsigned int i = 0; i < m_Vertices.capacity(); i++)
	{
		m_Vertices[i] = pos[i];
	}
	for(unsigned int i = 0; i < m_InitIndices.capacity(); i++)
	{
		m_InitIndices[i] = k[i];
	}
	for(unsigned int i = 0; i < p_NumSubDivisions; i++)
	{
		subdivide();        
	}

	// Project m_vertices onto sphere and scale.
	for(size_t i = 0; i < m_Vertices.size(); ++i)
	{
		XMVECTOR tv = XMLoadFloat3(&m_Vertices[i]);
		tv = XMVector3Normalize(tv);
		XMStoreFloat3(&m_Vertices[i],tv);
		m_Vertices[i].x *= p_Radius;
		m_Vertices[i].y *= p_Radius;
		m_Vertices[i].z *= p_Radius;
	}
}

void SkyDome::subdivide()
{
	std::vector<XMFLOAT3> vin = m_Vertices;
	std::vector<unsigned int> iin = m_InitIndices;

	m_Vertices.resize(0);
	m_InitIndices.resize(0);

	// v1
	// *
	// / \
	// / \
	// m0*-----*m1
	// / \ / \
	// / \ / \
	// *-----*-----*
	// v0 m2 v2
	unsigned int numTris = (unsigned int)iin.size()/3;
	for(unsigned int i = 0; i < numTris; ++i)
	{
		XMFLOAT3 v0 = vin[ iin[i*3+0] ];
		XMFLOAT3 v1 = vin[ iin[i*3+1] ];
		XMFLOAT3 v2 = vin[ iin[i*3+2] ];

		XMVECTOR vv0 = XMLoadFloat3(&v0);
		XMVECTOR vv1 = XMLoadFloat3(&v1);
		XMVECTOR vv2 = XMLoadFloat3(&v2);

		XMFLOAT3 m0;
		XMStoreFloat3(&m0,0.5f*(vv0 + vv1));
		XMFLOAT3 m1;
		XMStoreFloat3(&m1,0.5f*(vv1 + vv2));
		XMFLOAT3 m2;
		XMStoreFloat3(&m2,0.5f*(vv0 + vv2));

		m_Vertices.push_back(v0); // 0
		m_Vertices.push_back(v1); // 1
		m_Vertices.push_back(v2); // 2
		m_Vertices.push_back(m0); // 3
		m_Vertices.push_back(m1); // 4
		m_Vertices.push_back(m2); // 5

		m_InitIndices.push_back(i*6+0);
		m_InitIndices.push_back(i*6+3);
		m_InitIndices.push_back(i*6+5);

		m_InitIndices.push_back(i*6+3);
		m_InitIndices.push_back(i*6+4);
		m_InitIndices.push_back(i*6+5);

		m_InitIndices.push_back(i*6+5);
		m_InitIndices.push_back(i*6+4);
		m_InitIndices.push_back(i*6+2);

		m_InitIndices.push_back(i*6+3);
		m_InitIndices.push_back(i*6+1);
		m_InitIndices.push_back(i*6+4);
	}
}

std::vector<XMFLOAT3> SkyDome::getVertices()
{
	return m_Vertices;
}