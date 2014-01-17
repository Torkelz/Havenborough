#include "SkyDome.h"


SkyDome::SkyDome()
{
        m_NumIndices = 0;
}

SkyDome::~SkyDome()
{
}

bool SkyDome::init(float p_Radius)
{
        

        BuildGeoSphere( 2, p_Radius);

        std::vector<DirectX::XMFLOAT3> initSkym_vertices(m_vertices.size());
        for(unsigned int i = 0; i < m_vertices.size(); i++)
        {
                initSkym_vertices[i].x = 0.5f * m_vertices[i].x;
				initSkym_vertices[i].y = 0.5f * m_vertices[i].y;
				initSkym_vertices[i].z = 0.5f * m_vertices[i].z;
        }

		std::vector<DirectX::XMFLOAT3> temp;

		/*for( auto &i : m_initIndices )
		{
			temp.push_back(m_vertices.at(i));
		}*/
		for(unsigned int i = 0; i < m_initIndices.size();i++)
		{
			temp.push_back(m_vertices.at(m_initIndices.at(i)));
		}
		m_vertices = temp;
		m_vertices.shrink_to_fit();
		m_initIndices.clear();
		m_initIndices.shrink_to_fit();
		temp.clear();
		temp.shrink_to_fit();

        return true;
}

void SkyDome::BuildGeoSphere(unsigned int p_NumSubDivisions, float p_Radius)
{
        //Put a cap on the number of subdivisions
        p_NumSubDivisions = std::min(p_NumSubDivisions, unsigned int(5));

        //Aprox a asphere by tesselating an icosahedron
        const float X = 0.525731f;
        const float Z = 0.850651f;

        DirectX::XMFLOAT3 pos[12] =
        {
                DirectX::XMFLOAT3(-X, 0.0f, Z),  DirectX::XMFLOAT3(X, 0.0f, Z),
                DirectX::XMFLOAT3(-X, 0.0f, -Z),  DirectX::XMFLOAT3(X, 0.0f, -Z),
                DirectX::XMFLOAT3(0.0f, Z, X),  DirectX::XMFLOAT3(0.0f, Z, -X),
                DirectX::XMFLOAT3(0.0f, -Z, X),  DirectX::XMFLOAT3(0.0f, -Z, -X),
                DirectX::XMFLOAT3(Z, X, 0.0f),  DirectX::XMFLOAT3(-Z, X, 0.0f),
                DirectX::XMFLOAT3(Z, -X, 0.0f),  DirectX::XMFLOAT3(-Z, -X, 0.0f)
        };

        unsigned int k[60] =
        {
                1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4,
                1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2,
                3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
                10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
        };

        m_vertices.resize(12);
        m_initIndices.resize(60);

        for(unsigned int i = 0; i < m_vertices.capacity(); i++)
        {
                m_vertices[i] = pos[i];
        }
        for(unsigned int i = 0; i < m_initIndices.capacity(); i++)
        {
                m_initIndices[i] = k[i];
        }
        for(unsigned int i = 0; i < p_NumSubDivisions; i++)
        {
                Subdivide();        
        }

        // Project m_vertices onto sphere and scale.
        for(size_t i = 0; i < m_vertices.size(); ++i)
        {
				DirectX::XMVECTOR tv = DirectX::XMLoadFloat3(&m_vertices[i]);
				tv = DirectX::XMVector3Normalize(tv);
                DirectX::XMStoreFloat3(&m_vertices[i],tv);
				m_vertices[i].x *= p_Radius;
				m_vertices[i].y *= p_Radius;
				m_vertices[i].z *= p_Radius;
        }
}

void SkyDome::Subdivide()
{
        std::vector<DirectX::XMFLOAT3> vin = m_vertices;
        std::vector<unsigned int> iin = m_initIndices;

        m_vertices.resize(0);
        m_initIndices.resize(0);

        // v1
        // *
        // / \
        // / \
        // m0*-----*m1
        // / \ / \
        // / \ / \
        // *-----*-----*
        // v0 m2 v2
		using namespace DirectX;
        unsigned int numTris = (unsigned int)iin.size()/3;
        for(unsigned int i = 0; i < numTris; ++i)
        {
                DirectX::XMFLOAT3 v0 = vin[ iin[i*3+0] ];
                DirectX::XMFLOAT3 v1 = vin[ iin[i*3+1] ];
                DirectX::XMFLOAT3 v2 = vin[ iin[i*3+2] ];

				DirectX::XMVECTOR vv0 = DirectX::XMLoadFloat3(&v0);
				DirectX::XMVECTOR vv1 = DirectX::XMLoadFloat3(&v1);
				DirectX::XMVECTOR vv2 = DirectX::XMLoadFloat3(&v2);

                DirectX::XMFLOAT3 m0;
				XMStoreFloat3(&m0,0.5f*(vv0 + vv1));
                DirectX::XMFLOAT3 m1;
				XMStoreFloat3(&m1,0.5f*(vv1 + vv2));
                DirectX::XMFLOAT3 m2;
				XMStoreFloat3(&m2,0.5f*(vv0 + vv2));

                m_vertices.push_back(v0); // 0
                m_vertices.push_back(v1); // 1
                m_vertices.push_back(v2); // 2
                m_vertices.push_back(m0); // 3
                m_vertices.push_back(m1); // 4
                m_vertices.push_back(m2); // 5
 
                m_initIndices.push_back(i*6+0);
                m_initIndices.push_back(i*6+3);
                m_initIndices.push_back(i*6+5);

                m_initIndices.push_back(i*6+3);
                m_initIndices.push_back(i*6+4);
                m_initIndices.push_back(i*6+5);

                m_initIndices.push_back(i*6+5);
                m_initIndices.push_back(i*6+4);
                m_initIndices.push_back(i*6+2);

                m_initIndices.push_back(i*6+3);
                m_initIndices.push_back(i*6+1);
                m_initIndices.push_back(i*6+4);
        }
}

std::vector<DirectX::XMFLOAT3> SkyDome::getVertices()
{
        return m_vertices;
}
std::vector<unsigned int> SkyDome::getIndices()
{
        return m_initIndices;
}
DirectX::XMFLOAT3* SkyDome::getVertexData()
{
  DirectX::XMFLOAT3* temp = NULL;
  unsigned int size = m_vertices.size();
  temp                                = new DirectX::XMFLOAT3[size];
  for(unsigned int i = 0; i < size; i++)
  {
         temp[i] = m_vertices.at(i);
  }
  return temp;
}
unsigned int* SkyDome::getIndicesData()
{
  unsigned int* temp = NULL;
  unsigned int size = m_initIndices.size();
  temp                                = new unsigned int[size];
  for(unsigned int i = 0; i < size; i++)
  {
         temp[i] = m_initIndices.at(i);
  }
  return temp;
}