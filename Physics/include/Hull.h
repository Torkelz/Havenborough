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

		//THe idea is that to find the furthest point away from the center
		//get the length and set that to the spheres radius.
		m_Sphere = Sphere(1.f, p_CenterPos);
		calculateTriangles();
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

	unsigned int getTriangleSize()
	{
		return m_Triangles.size();
	}

	const Triangle& getTriangleAt(int p_Index)
	{
		return m_Triangles[p_Index];
	}

	DirectX::XMFLOAT4 findClosestPointOnTriangle(const DirectX::XMFLOAT4 p_Position, int p_Index)
	{
		DirectX::XMVECTOR ab, ac, bc, a, b, c, pos;
		a = Vector4ToXMVECTOR(&m_Triangles[p_Index].corners[0]);
		b = Vector4ToXMVECTOR(&m_Triangles[p_Index].corners[1]);
		c = Vector4ToXMVECTOR(&m_Triangles[p_Index].corners[2]);
		pos = DirectX::XMLoadFloat4(&p_Position);

		using DirectX::operator-;
		ab = b - a;
		ac = c - a;
		bc = c - b;

		float snom = DirectX::XMVector4Dot(pos - a, ab).m128_f32[0];
		float sdenom = DirectX::XMVector4Dot(pos - b, a - b).m128_f32[0];

		float tnom = DirectX::XMVector4Dot(pos - a, ac).m128_f32[0];
		float tdenom = DirectX::XMVector4Dot(pos - c, a - c).m128_f32[0];
		
		DirectX::XMFLOAT4 ret;
		if(snom <= 0.f && tnom <= 0.f)
		{
			DirectX::XMStoreFloat4(&ret, a);
			return ret;
		}

		float unom = DirectX::XMVector4Dot(pos - b, bc).m128_f32[0];
		float udenom = DirectX::XMVector4Dot(pos - c, b - c).m128_f32[0];

		if(sdenom <= 0.f && unom <= 0.f)
		{
			DirectX::XMStoreFloat4(&ret, b);
			return ret;
		}

		if(tdenom <= 0.f && udenom <= 0.f)
		{
			DirectX::XMStoreFloat4(&ret, c);
			return ret;
		}

		DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(b - a, c - a));
		float vc = DirectX::XMVector4Dot(normal, DirectX::XMVector3Cross(a - pos, b - pos)).m128_f32[0];

		using DirectX::operator*;
		using DirectX::operator+;
		using DirectX::operator/;
		if ( vc <= 0.f && snom >= 0.f && sdenom >= 0.f)
		{
			DirectX::XMStoreFloat4(&ret, a + snom / (snom + sdenom) * ab);
			return ret;
		}

		float va = DirectX::XMVector4Dot(normal, DirectX::XMVector3Cross(b - pos, c - pos)).m128_f32[0];
		if(va <= 0.f && unom >= 0.f && udenom >= 0.f)
		{
			DirectX::XMStoreFloat4(&ret, b + unom / ( unom + udenom ) * bc);
			return ret;
		}

		float vb = DirectX::XMVector4Dot(normal, DirectX::XMVector3Cross(c - pos, a - pos)).m128_f32[0];
		if(vb <= 0.f && tnom >= 0.f && tdenom >= 0.f)
		{
			DirectX::XMStoreFloat4(&ret, a + tnom / (tnom + tdenom) * ac);
			return ret;
		}

		float u = va / ( va + vb + vc);
		float v = vb / ( va + vb + vc);
		float w = 1.f - u - v; // vc / (va + vb + vc);
		DirectX::XMStoreFloat4(&ret, u * a + v * b + w * c);
		return ret;
		
	}

	Sphere getSphere()
	{
		return m_Sphere;
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