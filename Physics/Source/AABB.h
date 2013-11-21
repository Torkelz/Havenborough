#ifndef AABB_H
#define AABB_H

#include "BoundingVolume.h"
#include "Sphere.h"
/*#include "Buffer.h"
#include "Shader.h"*/
#include <vector>
//#include <DirectXMath.h> included in BoundingVolume.h

//DEBUGGING
//struct CB
//{
//	XMMATRIX WVP;
//	XMVECTOR color;
//};

class AABB : public BoundingVolume
{
private:
	DirectX::XMFLOAT4	m_bottom;
	DirectX::XMFLOAT4	m_top;
	DirectX::XMFLOAT4	m_bounds[8];
	std::vector<int>	m_indices;
	Sphere				m_sphere;
	DirectX::XMFLOAT4	m_halfDiagonal;

	////DEBUGGING
	/*Buffer*				m_pBuffer;
	Buffer*				m_pCB;
	Buffer*				m_pIndexBuffer;
	Shader*				m_pShader;
	ID3D11Device*		m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	CB					m_cb;
	DirectX::XMFLOAT4X4	m_translate;
	DirectX::XMFLOAT4	m_color;*/
	
public:
	AABB(){};
	/**
	* ## MUST BE BUILT IN MODEL SPACE! ##
	* @p_top is the most positive corner in all axis.
	* @p_bot is the least positive corner in all axis.
	*/
	AABB( DirectX::XMFLOAT4 p_top, DirectX::XMFLOAT4 p_bot);
	~AABB();
	
	/**
	* Initialize the AABB
	*/
	void				initialize();
	/**
	* Calculate corners, half diagonal and create bounding sphere for AABB.
	*/
	void				calculateBounds();
	/**
	* Updates position for AABB with matrices.
	* @p_scale, scale the AABB. 
	* @p_rotation, rotate the AABB.
	* @p_translation, move the AABB.
	*/
	void				updatePosition( DirectX::XMFLOAT4X4& p_scale, DirectX::XMFLOAT4X4& p_rotation, DirectX::XMFLOAT4X4& p_translate );
	/**
	* @return m_top, return top corner
	*/
	DirectX::XMFLOAT4*	getMax();
	/**
	* @return m_bottom, return bottom corner
	*/
	DirectX::XMFLOAT4*	getMin();
	/**
	* @return m_halfDiagonal, return vector from center to top corner.
	*/
	DirectX::XMFLOAT4*	getHalfDiagonal();
	/**
	* @return m_sphere, AABB's sphere
	*/
	Sphere*				getSphere();

	//DEBUGGING
	/*void				buildCubeIndices( int offset );
	void				initDraw(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext, DirectX::XMFLOAT4 p_color);
	void				draw( DirectX::XMFLOAT4X4& p_world, DirectX::XMFLOAT4X4& p_view, DirectX::XMFLOAT4X4& p_proj );*/
};

#endif