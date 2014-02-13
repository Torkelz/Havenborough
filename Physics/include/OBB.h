#pragma once
#include "Sphere.h"

class OBB : public BoundingVolume
{
private:
	DirectX::XMFLOAT4X4	m_Axes; // Local x-,y and z-axes
	DirectX::XMFLOAT4	m_Extents; //Positive half-width extents of OBB along each axis
	DirectX::XMFLOAT4	m_Corners[8];
	Sphere				m_Sphere;
	DirectX::XMFLOAT3	m_Orientation; // For edge climbing

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
		m_Sphere		= Sphere();
		m_Orientation	= DirectX::XMFLOAT3(0.f, 0.f, 0.f);
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

		m_Extents.x = fabs(p_Extents.x);
		m_Extents.y = fabs(p_Extents.y);
		m_Extents.z = fabs(p_Extents.z);
		m_Extents.w = 1.f;

		calculateCorners();

		//Sphere
		DirectX::XMVECTOR extentVector = DirectX::XMLoadFloat4(&m_Extents);
		DirectX::XMVECTOR length = DirectX::XMVector3Length(extentVector);
		float radius;
		DirectX::XMStoreFloat(&radius, length);
		m_Sphere = Sphere(radius, p_CenterPos);
		
		calculateOrientation();
	}
	/**
	* Destructor
	*/
	~OBB()
	{
	}

	/**
	* Updates position for OBB with translation matrix.
	* @param p_translation, move the OBB in relative coordinates.
	*/
	void updatePosition(DirectX::XMFLOAT4X4 const &p_Translation) override
	{
		m_PrevPosition = m_Position;

		DirectX::XMMATRIX mTrans;
		mTrans = DirectX::XMLoadFloat4x4(&p_Translation);

		DirectX::XMVECTOR vCenterPos;
		vCenterPos = DirectX::XMLoadFloat4(&m_Position);
		vCenterPos = DirectX::XMVector4Transform(vCenterPos, mTrans);

		DirectX::XMStoreFloat4(&m_Position, vCenterPos);
								  
		m_Sphere.updatePosition(m_Position);
	}
	/**
	* Changes ther center position for the OBB.
	* @param p_newPosition, new center pos for the OBB.
	*/
	void setPosition(DirectX::XMVECTOR const p_newPosition)
	{
		DirectX::XMStoreFloat4(&m_Position, p_newPosition);
		m_Sphere.updatePosition(m_Position);
	}
	/**
	* Gets the normalized local axes of the OBB
	* @return m_Axes the OBB Axes.
	*/
	DirectX::XMFLOAT4X4 getAxes() const
	{
		return m_Axes;
	}

	/**
	* Get the half lengths(extents) of the OBB
	* @return m_Extents, the extents of the OBB in m
	*/
	DirectX::XMFLOAT4 getExtents() const
	{
		return m_Extents;
	}

	/**
	* Get the sphere surrounding the OBB.
	* @return m_Sphere the surrounding sphere
	*/
	Sphere getSphere() const
	{
		return m_Sphere;
	}

	/**
	* Sets the half lengths(extents) of the OBB, which rezizes the box.
	* @param p_Extents, the new half length of the box.
	*/
	void setExtent(DirectX::XMVECTOR const &p_Extents)
	{
		DirectX::XMStoreFloat4(&m_Extents, p_Extents);
		calculateCorners();
	}
	/**
	* Scales the OBB.
	* @param p_scale vector to scale the box with..
	*/
	void scale(DirectX::XMVECTOR const &p_Scale) override
	{
		DirectX::XMMATRIX m = DirectX::XMMatrixScalingFromVector(p_Scale);
		DirectX::XMVECTOR e = XMLoadFloat4(&m_Extents);
		e = DirectX::XMVector4Transform(e, m);
		DirectX::XMStoreFloat4(&m_Extents, e);
		calculateCorners();
	}

	/**
	* Sets the rotation matrix of the OBB and rotates its Axes accordingly.
	* @param p_Rotation matrix to rotate the axes with.
	*/
	void setRotation(const DirectX::XMMATRIX &p_Rot)
	{
        XMStoreFloat4x4(&m_Axes, p_Rot);

        calculateCorners();
        DirectX::XMVECTOR tempCorners[8];
        for(int i = 0; i < 8; i++)
        {
                tempCorners[i] = DirectX::XMLoadFloat4(&m_Corners[i]);

                tempCorners[i] = DirectX::XMVector4Transform(tempCorners[i], p_Rot);

                DirectX::XMStoreFloat4(&m_Corners[i], tempCorners[i]);
        }

		calculateOrientation();
		DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&m_Orientation);
		n = DirectX::XMVector3Transform(n, p_Rot);
		DirectX::XMStoreFloat3(&m_Orientation, n);
	}
	 
	/**
	* Given point p, return point on(or in) OBB, closest to p
	* @param p_point the point you want to search from
	* @return closest point in the OBB in m
	*/
	DirectX::XMVECTOR findClosestPt(DirectX::XMVECTOR const &p_Point) const
	{
		DirectX::XMVECTOR tPos		= XMLoadFloat4(&m_Position);
		DirectX::XMMATRIX tAxes		= XMLoadFloat4x4(&m_Axes);
		DirectX::XMVECTOR tExtent	= XMLoadFloat4(&m_Extents);
		DirectX::XMVECTOR result;

		using DirectX::operator-;
		using DirectX::operator+=;
		using DirectX::operator*;
		DirectX::XMVECTOR d = p_Point - tPos;
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
		return result;
	}

	/**
	 * Return a corner at the index specified.
	 * 
	 * @param p_Index index number in m_Cornerslist
	 * @return a XMFLOAT4 corner.
	 */
	DirectX::XMFLOAT4 getCornerAt(unsigned int p_Index) const
	{
		return m_Corners[p_Index];
	}
	/**
	 * Return a corner in world coordinates at the index specified.
	 * 
	 * @param p_Index index number in m_Bounds list
	 * @return a XMFLOAT4 corner.
	 */
	DirectX::XMFLOAT4 getCornerWorldCoordAt(unsigned p_Index) const
	{
		return DirectX::XMFLOAT4(m_Corners[p_Index].x + m_Position.x, m_Corners[p_Index].y + m_Position.y, m_Corners[p_Index].z + m_Position.z, 1.f);
	}

	/**
	 * Return the line that goes through the box at it's tallest. It is not normalized.
	 */
	DirectX::XMFLOAT3 getOrientation() const
	{
		return m_Orientation;
	}

private:
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

	void calculateOrientation()
	{
		if(m_Extents.x > m_Extents.z)
			m_Orientation = DirectX::XMFLOAT3(m_Extents.x, m_Extents.y, 0.0f);
		else
			m_Orientation = DirectX::XMFLOAT3(0.0f, m_Extents.y, m_Extents.z);

		DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&m_Orientation);
		DirectX::XMStoreFloat3(&m_Orientation, n);
	}
};