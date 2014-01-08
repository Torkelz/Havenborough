#pragma once

#include "BoundingVolume.h"
#include "Sphere.h"

class OBB : public BoundingVolume
{
private:
	DirectX::XMFLOAT4X4	m_Axes; // Local x-,y and z-axes
	DirectX::XMFLOAT4	m_Extents; //Positive half-width extents of OBB along each axis
	DirectX::XMFLOAT4	m_Corners[8];
	DirectX::XMFLOAT4X4 m_Rotation;
	Sphere				m_Sphere;

public:
	/**
	* Default Constructor
	*/
	OBB()
	{
		m_Position		= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f); //OBB Center Point
		m_PrevPosition	= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_Extents		= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_Type			= Type::OBB;
		DirectX::XMStoreFloat4x4(&m_Axes, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&m_Rotation, DirectX::XMMatrixIdentity());
		m_Sphere		= Sphere();
	}

	/**
	* Constructor
	* @param p_CenterPos the center position of the box
	* @param p_Extents the box half lengths
	*/
	OBB(DirectX::XMFLOAT4 p_CenterPos, DirectX::XMFLOAT4 p_Extents)
	{
		m_Position		= p_CenterPos; //OBB Center Point.
		m_PrevPosition	= DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		m_Type			= Type::OBB;
		DirectX::XMStoreFloat4x4(&m_Axes, DirectX::XMMatrixIdentity());
		
		DirectX::XMStoreFloat4x4(&m_Rotation, DirectX::XMMatrixIdentity());

		m_Extents.x = fabs(p_Extents.x);
		m_Extents.y = fabs(p_Extents.y);
		m_Extents.z = fabs(p_Extents.z);
		m_Extents.w = 0.f;

		calculateCorners();

		//Sphere
		DirectX::XMVECTOR extentVector = DirectX::XMLoadFloat4(&m_Extents);
		DirectX::XMVECTOR length = DirectX::XMVector3Length(extentVector);
		float radius;
		DirectX::XMStoreFloat(&radius, length);
		m_Sphere		= Sphere(radius, p_CenterPos);

	}
	~OBB()
	{
	}

	/**
	* Updates position for OBB with translation matrix.
	* @param p_translation, move the OBB in relative coordinates.
	*/
	void updatePosition(DirectX::XMFLOAT4X4& p_Translation) override
	{
		m_PrevPosition = m_Position;

		DirectX::XMMATRIX mTrans;
		mTrans = DirectX::XMLoadFloat4x4(&p_Translation);

		DirectX::XMVECTOR vCenterPos;
		vCenterPos = DirectX::XMLoadFloat4(&m_Position);
		vCenterPos = DirectX::XMVector4Transform(vCenterPos, mTrans);

		DirectX::XMStoreFloat4(&m_Position, vCenterPos);
								  
		m_Sphere.updatePosition(m_Position);

		calculateCorners();
	}

	/**
	* Gets the normalized local axes of the OBB
	* @return m_Axes the OBB Axes.
	*/
	DirectX::XMFLOAT4X4 getAxes()
	{
		return m_Axes;
	}

	/**
	* Get the half lengths(extents) of the OBB
	* @return m_Extents, the extents of the OBB
	*/
	DirectX::XMFLOAT4 getExtents()
	{
		return m_Extents;
	}

	/**
	* Get the rotation matrix of the OBB. Note not the OBB's axes.
	* @return m_Rotation 
	*/
	DirectX::XMFLOAT4X4 getRotationMatrix()
	{
		return m_Rotation;
	}

	/**
	* Get the sphere surrounding the OBB.
	* @return m_Sphere the surrounding sphere
	*/
	Sphere getSphere()
	{
		return m_Sphere;
	}

	/**
	* Sets the half lengths(extents) of the OBB, which rezizes the box.
	* @param p_Extents, the new half length of the box.
	*/
	void setExtent(const DirectX::XMFLOAT4 &p_Extents)
	{
		m_Extents = p_Extents;
		calculateCorners();
	}

	/**
	* Sets the rotation matrix of the OBB and rotates its Axes accordingly.
	* @param p_Rotation matrix to rotate the axes with.
	*/
	void setRotationMatrix(const DirectX::XMFLOAT4X4 &p_Rot)
	{
		m_Rotation = p_Rot;
		updateRotation();
	}
	 
	/**
	* Given point p, return point on(or in) OBB, closest to p
	* @param p_point the point you want to search from
	* @return closest point in the OBB
	*/
	DirectX::XMFLOAT4 findClosestPt(const DirectX::XMFLOAT4 &p_point)
	{
		DirectX::XMVECTOR tPoint	= XMLoadFloat4(&p_point);
		DirectX::XMVECTOR tPos		= XMLoadFloat4(&m_Position);
		DirectX::XMMATRIX tAxes		= XMLoadFloat4x4(&m_Axes);
		DirectX::XMVECTOR tExtent	= XMLoadFloat4(&m_Extents);
		DirectX::XMVECTOR result;
		DirectX::XMFLOAT4 fResult;

		using DirectX::operator-;
		using DirectX::operator+=;
		using DirectX::operator*;
		DirectX::XMVECTOR d = tPoint - tPos;
		result = tPos;
		// For each OBB axis.
		for(int i = 0; i < 3; i++)
		{
			//project d onto that axis to get the distance
			//along the axis of d from the box center
			DirectX::XMVECTOR dotResult = DirectX::XMVector4Dot(d, tAxes.r[i]);
			float dist = dotResult.m128_f32[0];
			
			if(dist > tExtent.m128_f32[i])
			{
				dist = tExtent.m128_f32[i];
			}

			if(dist < -tExtent.m128_f32[i])
			{
				dist = -tExtent.m128_f32[i];
			}

			result += dist * tAxes.r[i];
		}

		XMStoreFloat4(&fResult, result);
		return  fResult;
	}
private:
	void updateRotation()
	{
		DirectX::XMMATRIX tempRot = DirectX::XMLoadFloat4x4(&m_Rotation);
		DirectX::XMMATRIX tAxes = XMLoadFloat4x4(&m_Axes);
		tAxes = tempRot * tAxes;

		DirectX::XMStoreFloat4x4(&m_Axes, tAxes);


		DirectX::XMVECTOR tempCorners[8];
		for(int i = 0; i < 8; i++)
		{
			tempCorners[i] = DirectX::XMLoadFloat4(&m_Corners[i]);

			tempCorners[i] = DirectX::XMVector4Transform(tempCorners[i], tAxes);

			DirectX::XMStoreFloat4(&m_Corners[i], tempCorners[i]);
		}
	}

	void calculateCorners()
	{
		m_Corners[0] = DirectX::XMFLOAT4(- m_Extents.x, - m_Extents.y, - m_Extents.z, 1.f); 
		m_Corners[1] = DirectX::XMFLOAT4(+ m_Extents.x, - m_Extents.y, - m_Extents.z, 1.f); 
		m_Corners[2] = DirectX::XMFLOAT4(- m_Extents.x, + m_Extents.y, - m_Extents.z, 1.f); 
		m_Corners[3] = DirectX::XMFLOAT4(+ m_Extents.x, + m_Extents.y, - m_Extents.z, 1.f); 
		m_Corners[4] = DirectX::XMFLOAT4(- m_Extents.x, - m_Extents.y, + m_Extents.z, 1.f); 																						 
		m_Corners[5] = DirectX::XMFLOAT4(+ m_Extents.x, - m_Extents.y, + m_Extents.z, 1.f); 																						 
		m_Corners[6] = DirectX::XMFLOAT4(- m_Extents.x, + m_Extents.y, + m_Extents.z, 1.f); 																								 
		m_Corners[7] = DirectX::XMFLOAT4(+ m_Extents.x, + m_Extents.y, + m_Extents.z, 1.f); 
	}

};