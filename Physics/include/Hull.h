#pragma once

#include "BoundingVolume.h"
#include "Sphere.h"
#include "PhysicsTypes.h"
#include <DirectXMath.h>
#include <vector>

class Hull : public BoundingVolume
{
private:
	Sphere m_Sphere;
	std::vector<Triangle> m_Triangles;


public:
	/*
	 *
	 * DO NOT FORGET TO CHANGE THE RADIUS FOR THE SPHERE!!!!! Remove comment when fixed.
	 */
	Hull(DirectX::XMFLOAT4 p_CenterPos, std::vector<Triangle> p_Triangles)
	{
		m_Position = p_CenterPos;
		m_Triangles = p_Triangles;
		m_Type = Type::HULL;
		float radius = findFarthestDistanceOnTriangle();

		m_Sphere = Sphere( radius, p_CenterPos );
	}
	~Hull()
	{

	}

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

		calculateTriangles();
	}

	DirectX::XMFLOAT4 findClosestPointOnTriangle(const DirectX::XMFLOAT4 p_Position, int p_Index)
	{
		DirectX::XMVECTOR ab, ac, ap, a, b, c, pos;
		a = Vector4ToXMVECTOR(&getTriangleWorldCoordAt(p_Index).corners[0]);
		b = Vector4ToXMVECTOR(&getTriangleWorldCoordAt(p_Index).corners[1]);
		c = Vector4ToXMVECTOR(&getTriangleWorldCoordAt(p_Index).corners[2]);
		pos = DirectX::XMLoadFloat4(&p_Position);

		using DirectX::operator-;
		using DirectX::operator*;
		using DirectX::operator+;
		ab = b - a;
		ac = c - a;
		ap = pos - a;

		float d1 = DirectX::XMVector4Dot(ab, ap).m128_f32[0];
		float d2 = DirectX::XMVector4Dot(ac, ap).m128_f32[0];
		
		DirectX::XMFLOAT4 ret;
		if(d1 <= 0.f && d2 <= 0.f)
		{
			DirectX::XMStoreFloat4(&ret, a);
			return ret;
		}

		DirectX::XMVECTOR bp = pos - b;
		float d3 = DirectX::XMVector4Dot(ab, bp).m128_f32[0];
		float d4 = DirectX::XMVector4Dot(ac, bp).m128_f32[0];

		if(d3 >= 0.f && d4 <= d3)
		{
			DirectX::XMStoreFloat4(&ret, b);
			return ret;
		}

		float vc = d1*d4 - d3*d2;
		if(vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
		{
			float v = d1 / (d1 - d3);
			DirectX::XMStoreFloat4(&ret, a + v * ab);
			return ret;
		}

		DirectX::XMVECTOR cp = pos - c;
		float d5 = DirectX::XMVector3Dot(ab, cp).m128_f32[0];
		float d6 = DirectX::XMVector3Dot(ac, cp).m128_f32[0];

		if(d6 >= 0.f && d5 <= d6)
		{
			DirectX::XMStoreFloat4(&ret, c);
			return ret;
		}

		float vb = d5*d2 - d1*d6;
		if(vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
		{
			float w = d2 / (d2 - d6);
			DirectX::XMStoreFloat4(&ret, a + w * ac);
			return ret;
		}

		float va = d3*d6 - d5*d4;
		if(va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f)
		{
			float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
			DirectX::XMStoreFloat4(&ret, b + w * ( c - b ));
			return ret;
		}

		float denom = 1.f / ( va + vb + vc);

		float v = vb * denom;
		float w = vc * denom;
		DirectX::XMStoreFloat4(&ret, a + ab * v + ac * w);
		return ret;
		
	}

	float findFarthestDistanceOnTriangle()
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

	void setScale(DirectX::XMFLOAT4X4 p_scaleMatrix)
	{
		DirectX::XMVECTOR c1, c2, c3;
		DirectX::XMMATRIX m = XMLoadFloat4x4(&p_scaleMatrix);

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

	void setRotation(DirectX::XMFLOAT4X4 p_Rotation)
	{
		DirectX::XMMATRIX rotation = DirectX::XMLoadFloat4x4(&p_Rotation);
		DirectX::XMVECTOR c1, c2, c3;
		for(auto& tri : m_Triangles)
		{
			c1 = Vector4ToXMVECTOR(&tri.corners[0]);
			c2 = Vector4ToXMVECTOR(&tri.corners[1]);
			c3 = Vector4ToXMVECTOR(&tri.corners[2]);

			c1 = XMVector4Transform(c1, rotation);
			c2 = XMVector4Transform(c2, rotation);
			c3 = XMVector4Transform(c3, rotation);

			tri.corners[0] = XMVECTORToVector4(&c1);
			tri.corners[1] = XMVECTORToVector4(&c2);
			tri.corners[2] = XMVECTORToVector4(&c3);
		}
	}

	Sphere getSphere()
	{
		return m_Sphere;
	}
	/**
	 * Return a corner in world coordinates at the index specified.
	 * 
	 * @param p_Index index number in m_Bounds list
	 * @return a XMFLOAT4 corner.
	 */

	unsigned int getTriangleSize()
	{
		return m_Triangles.size();
	}

	const Triangle& getTriangleAt(int p_Index)
	{
		return m_Triangles[p_Index];
	}

	Triangle getTriangleWorldCoordAt(unsigned p_Index)
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
		void calculateTriangles()
	{
		for(auto& tri : m_Triangles)
		{
			tri.corners[0] = tri.corners[0] + XMFLOAT4ToVector4(&m_Position);
			tri.corners[1] = tri.corners[1] + XMFLOAT4ToVector4(&m_Position);
			tri.corners[2] = tri.corners[2] + XMFLOAT4ToVector4(&m_Position);

			tri.corners[0].w = 1.f;
			tri.corners[1].w = 1.f;
			tri.corners[2].w = 1.f;
		}
	}

};