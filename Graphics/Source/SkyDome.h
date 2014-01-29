#pragma once

#include <DirectXMath.h>
#include <vector>

class SkyDome
{
public:
        SkyDome();
        ~SkyDome();

        bool init(float p_Radius);
        std::vector<DirectX::XMFLOAT3> getVertices();
        std::vector<unsigned int> getIndices();
        DirectX::XMFLOAT3* getVertexData();

        unsigned int* getIndicesData();
private:
        
        void BuildGeoSphere( unsigned int p_NumSubDivisions, float p_Radius);
        void Subdivide();

        std::vector<DirectX::XMFLOAT3> m_vertices;
        std::vector<unsigned int> m_initIndices;
        unsigned int m_NumIndices;
};