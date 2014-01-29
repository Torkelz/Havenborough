#pragma once

#include "BoundingVolume.h"
#include "Sphere.h"
#include "PhysicsTypes.h"
#include <DirectXMath.h>
#include <vector>
#include <utility> 

class Hull : public BoundingVolume
{
private:
	Sphere m_Sphere; //Sphere surrounding the hull
	std::vector<Triangle> m_Triangles; //Triangles that make up the hull
	DirectX::XMFLOAT4	m_Scale;

public:
	/**
	 * Constructor.
	 * The hull is always created with origo as center position, call updatePosition to move the hull to its desired place.
	 * @param p_Triangles, a list of triangles that make up the hull
	 */
	Hull(std::vector<Triangle> p_Triangles)
	{
		
		m_Position = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);
		m_PrevPosition = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);
		m_Triangles = p_Triangles;
		m_Type = Type::HULL;
		float radius = findFarthestDistanceOnTriangle();
		m_Scale = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f);
		m_Sphere = Sphere( radius, m_Position );
	}

	/**
	 * Destructor
	 */
	~Hull()
	{
	}


	/**
	 * Updates position for the hull with a translation matrix.
	 * @param p_translation, move the hull in relative coordinates.
	 */
	void updatePosition(DirectX::XMFLOAT4X4& p_Translation) override
	{
		m_PrevPosition = m_Position;

		DirectX::XMMATRIX tempTrans;
		tempTrans = DirectX::XMLoadFloat4x4(&p_Translation);

		DirectX::XMVECTOR centerPos;
		centerPos = DirectX::XMLoadFloat4(&m_Position);
		centerPos = DirectX::XMVector4Transform(centerPos, tempTrans);

		DirectX::XMStoreFloat4(&m_Position, centerPos);
								  
		m_Sphere.updatePosition(m_Position);
	}
	
	/**
	 * Scales all the triangles in the hull
	 * @param p_Scale is a vector3 with all the scale coordinates 
	 */
	void scale(DirectX::XMVECTOR const &p_Scale) override
	{
		DirectX::XMVECTOR c1, c2, c3;
		DirectX::XMMATRIX m = DirectX::XMMatrixScalingFromVector(p_Scale);
		DirectX::XMStoreFloat4(&m_Scale, p_Scale);
		for(auto &tri : m_Triangles)
		{
			c1 = Vector4ToXMVECTOR(&tri.corners[0]);
			c2 = Vector4ToXMVECTOR(&tri.corners[1]);
			c3 = Vector4ToXMVECTOR(&tri.corners[2]);

			c1 = DirectX::XMVector4Transform(c1, m);
			c2 = DirectX::XMVector4Transform(c2, m);
			c3 = DirectX::XMVector4Transform(c3, m);

			tri.corners[0] = XMVECTORToVector4(&c1); 
			tri.corners[1] = XMVECTORToVector4(&c2); 
			tri.corners[2] = XMVECTORToVector4(&c3);
		}
		float radius = findFarthestDistanceOnTriangle();
		m_Sphere.setRadius(radius);

	}
	/**
	 * Rotates all the trangles in the hull
	 * @param p_Rotation matrix to rotate the triangles with.
	 */
	void setRotation(DirectX::XMMATRIX const &p_Rotation)
	{
		DirectX::XMVECTOR c1, c2, c3;
		for(auto& tri : m_Triangles)
		{
			c1 = Vector4ToXMVECTOR(&tri.corners[0]);
			c2 = Vector4ToXMVECTOR(&tri.corners[1]);
			c3 = Vector4ToXMVECTOR(&tri.corners[2]);

			c1 = XMVector4Transform(c1, p_Rotation);
			c2 = XMVector4Transform(c2, p_Rotation);
			c3 = XMVector4Transform(c3, p_Rotation);

			tri.corners[0] = XMVECTORToVector4(&c1);
			tri.corners[1] = XMVECTORToVector4(&c2);
			tri.corners[2] = XMVECTORToVector4(&c3);
		}
	}
	/**
	 * Get the sphere surrounding the hull.
	 * @return m_Sphere the surrounding sphere
	 */
	Sphere getSphere() const
	{
		return m_Sphere;
	}

	/**
	 * Gets the number of triangles in the hull
	 * @return size of the triangle list
	 */
	const unsigned int getTriangleListSize() const
	{
		return m_Triangles.size();
	}
	/**
	 * Gets a triangle from the hull
	 * @param p_Index index of the triangle int the hulls triangle list
	 * @return a triangle with local coordinates from the hulls triangle list at the specified index
	 */
	const Triangle& getTriangleAt(int p_Index) const
	{
		return m_Triangles.at(p_Index);
	}
	/**
	 * Gets the current scale of the Hull based on it's orginial scale, the default value of scale is XMFLOAT4(1.f, 1.f, 1.f, 0.f).
	 * @return the hulls current scale.
	 */
	const DirectX::XMFLOAT4 getScale() const
	{
		return m_Scale;
	}

	/**
	 * Return a triangle from the hull in world coordinates.
	 * 
	 * @param p_Index index number in triangle list
	 * @return a triangle in world coordinates.
	 */
	Triangle getTriangleInWorldCoord(unsigned p_Index) const
	{
		Triangle triangle;
		triangle = m_Triangles[p_Index];

		triangle.corners[0] = triangle.corners[0] + XMFLOAT4ToVector4(&m_Position);
		triangle.corners[1] = triangle.corners[1] + XMFLOAT4ToVector4(&m_Position);
		triangle.corners[2] = triangle.corners[2] + XMFLOAT4ToVector4(&m_Position);

		triangle.corners[0].w = 1.f;
		triangle.corners[1].w = 1.f;
		triangle.corners[2].w = 1.f;

		return triangle;
	}
private:
	float findFarthestDistanceOnTriangle() const
	{
		//The idea is that to find the furthest point away from the center
		//get the length and set that to the spheres radius.
		DirectX::XMVECTOR v, farthest, centerPos, corner;
		centerPos = DirectX::XMVectorSet(0.f, 0.f, 0.f, 1.f);

		float farthestDistance = 0.f;

		for(auto& tri : m_Triangles)
		{
			using DirectX::operator-;
			corner = Vector4ToXMVECTOR(&tri.corners[0]);
			v = corner - centerPos;
			float c1 = DirectX::XMVector3Dot(v, v).m128_f32[0];
			
			if(c1 > farthestDistance)
			{
				farthestDistance = c1;
				farthest = v;
			}

			corner = Vector4ToXMVECTOR(&tri.corners[1]);
			v = corner - centerPos;
			float c2 = DirectX::XMVector3Dot(v, v).m128_f32[0];

			if(c2 > farthestDistance)
			{
				farthestDistance = c1;
				farthest = v;
			}

			corner = Vector4ToXMVECTOR(&tri.corners[2]);
			v = corner - centerPos;
			float c3 = DirectX::XMVector3Dot(v, v).m128_f32[0];

			if(c3 > farthestDistance)
			{
				farthestDistance = c3;
				farthest = v;
			}

		}
	
		farthest.m128_f32[3] = 1.f;
		return sqrtf(farthestDistance);
	}
};