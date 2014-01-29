#pragma once

#include <DirectXMath.h>
#include <vector>

class SkyDome
{
private:
	std::vector<DirectX::XMFLOAT3> m_Vertices;
	std::vector<unsigned int> m_InitIndices;
	unsigned int m_NumIndices;

public:
	/**
	* Constructor.
	*/
	SkyDome();
	
	/**
	* Destructor.
	*/
	~SkyDome();
	
	/**
	* Initialize the sky dome 
	* @param p_Radius the size of the sphere's radius in cm
	*/
	bool init(float p_Radius);
	
	/**
	*
	*/
	std::vector<DirectX::XMFLOAT3> getVertices();
	
private:

	void buildGeoSphere(unsigned int p_NumSubDivisions, float p_Radius);
	void subdivide();
};