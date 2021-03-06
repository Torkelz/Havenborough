#pragma once
#include "Sphere.h"
#include "PhysicsTypes.h"
#include <DirectXMath.h>
#include <vector>

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
	Hull(std::vector<Triangle> p_Triangles) :
		BoundingVolume(&m_Sphere)
	{
		m_BodyHandle = 0;
		m_Position = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);
		m_Triangles = p_Triangles;
		m_Type = Type::HULL;
		float radius = findFarthestDistanceOnTriangle();
		m_Scale = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f);
		m_Sphere = Sphere( radius, m_Position );
		m_CollisionResponse = true;
		m_IDInBody = 0;
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
	void updatePosition(DirectX::XMFLOAT4X4 const &p_Translation) override
	{
		DirectX::XMMATRIX tempTrans;
		tempTrans = DirectX::XMLoadFloat4x4(&p_Translation);

		DirectX::XMVECTOR centerPos;
		centerPos = DirectX::XMLoadFloat4(&m_Position);
		centerPos = DirectX::XMVector4Transform(centerPos, tempTrans);

		DirectX::XMStoreFloat4(&m_Position, centerPos);
								  
		m_Sphere.setPosition(centerPos);
	}

	void setPosition(DirectX::XMVECTOR const &p_newPosition) override
	{
		DirectX::XMStoreFloat4(&m_Position, p_newPosition);
		m_Sphere.setPosition(p_newPosition);
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

			tri.corners[0] = c1; 
			tri.corners[1] = c2; 
			tri.corners[2] = c3;
		}
		float radius = findFarthestDistanceOnTriangle();
		m_Sphere.setRadius(radius);

	}
	/**
	 * Rotates all the trangles in the hull
	 * @param p_Rotation matrix to rotate the triangles with.
	 */
	void setRotation(DirectX::XMMATRIX const &p_Rotation) override
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

			tri.corners[0] = c1;
			tri.corners[1] = c2;
			tri.corners[2] = c3;
		}
	}
	/**
	 * Get the sphere surrounding the hull.
	 * @return m_Sphere the surrounding sphere
	 */
	const Sphere& getSphere() const
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
	Triangle getTriangleInWorldCoord(unsigned int p_Index) const
	{
		//Triangle triangle;
		//triangle = m_Triangles[p_Index];

		//triangle.corners[0] = triangle.corners[0] + m_Position;
		//triangle.corners[1] = triangle.corners[1] + m_Position;
		//triangle.corners[2] = triangle.corners[2] + m_Position;

		//triangle.corners[0].w = 1.f;
		//triangle.corners[1].w = 1.f;
		//triangle.corners[2].w = 1.f;

		//return triangle;

		return Triangle(Vector4(m_Triangles[p_Index].corners[0].x + m_Position.x, m_Triangles[p_Index].corners[0].y + m_Position.y, m_Triangles[p_Index].corners[0].z + m_Position.z, 1.0f),
			Vector4(m_Triangles[p_Index].corners[1].x + m_Position.x, m_Triangles[p_Index].corners[1].y + m_Position.y, m_Triangles[p_Index].corners[1].z + m_Position.z, 1.0f),
			Vector4(m_Triangles[p_Index].corners[2].x + m_Position.x, m_Triangles[p_Index].corners[2].y + m_Position.y, m_Triangles[p_Index].corners[2].z + m_Position.z, 1.0f));
	}

		/**
	* Given point p and the triangle corners in world coordinates, return point in triangle, closest to p
	* @param p_point the point you want to search from
	* @param p_A, corner[0] of triangle in world coordinates
	* @param p_B, corner[1] of triangle in world coordinates
	* @param p_C, corner[2] of triangle in world coordinates
	* @return closest point in the triangle
	*/
	DirectX::XMVECTOR findClosestPointOnTriangle(DirectX::XMFLOAT4 const &p_Point, int p_TriangleIndex) const
	{
		DirectX::XMVECTOR p = XMLoadFloat4(&m_Position);

		using DirectX::operator-;
		using DirectX::operator*;
		using DirectX::operator+;

		DirectX::XMVECTOR a = XMLoadFloat3(&m_Triangles[p_TriangleIndex].corners[0].xyz()) + p;
		//a.m128_f32[3] = 1.f;
		DirectX::XMVECTOR b = XMLoadFloat3(&m_Triangles[p_TriangleIndex].corners[1].xyz()) + p;
		//b.m128_f32[3] = 1.f;
		DirectX::XMVECTOR c = XMLoadFloat3(&m_Triangles[p_TriangleIndex].corners[2].xyz()) + p;
		//c.m128_f32[3] = 1.f;
		DirectX::XMVECTOR pos = DirectX::XMLoadFloat4(&p_Point);

		DirectX::XMVECTOR ab = b - a;
		DirectX::XMVECTOR ac = c - a;
		DirectX::XMVECTOR ap = pos - a;

		float d1 = DirectX::XMVector3Dot(ab, ap).m128_f32[0];
		float d2 = DirectX::XMVector3Dot(ac, ap).m128_f32[0];

		//DirectX::XMFLOAT4 ret;
		if(d1 <= 0.f && d2 <= 0.f)
		{
			//DirectX::XMStoreFloat4(&ret, a);
			return a;
		}

		DirectX::XMVECTOR bp = pos - b;
		float d3 = DirectX::XMVector3Dot(ab, bp).m128_f32[0];
		float d4 = DirectX::XMVector3Dot(ac, bp).m128_f32[0];

		if(d3 >= 0.f && d4 <= d3)
		{
			//DirectX::XMStoreFloat4(&ret, b);
			return b;
		}

		float vc = d1*d4 - d3*d2;
		using DirectX::operator*;
		using DirectX::operator+;

		if(vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
		{
			float v = d1 / (d1 - d3);	
			//DirectX::XMStoreFloat4(&ret, a + v * ab);
			return a + v * ab;
		}

		DirectX::XMVECTOR cp = pos - c;
		float d5 = DirectX::XMVector3Dot(ab, cp).m128_f32[0];
		float d6 = DirectX::XMVector3Dot(ac, cp).m128_f32[0];

		if(d6 >= 0.f && d5 <= d6)
		{
			//DirectX::XMStoreFloat4(&ret, c);
			return c;
		}

		float vb = d5*d2 - d1*d6;
		if(vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
		{
			float w = d2 / (d2 - d6);
			//DirectX::XMStoreFloat4(&ret, a + w * ac);
			return a + w * ac;
		}

		float va = d3*d6 - d5*d4;
		if(va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f)
		{
			float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
			//DirectX::XMStoreFloat4(&ret, b + w * ( c - b ));
			return b + w * ( c - b );
		}

		float denom = 1.f / ( va + vb + vc);

		float v = vb * denom;
		float w = vc * denom;
		//DirectX::XMStoreFloat4(&ret, a + ab * v + ac * w);
		return a + ab * v + ac * w;
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