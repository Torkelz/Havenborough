#pragma once

#include "Collision.h"
#include "Sphere.h"

#include <array>
#include <vector>

class Octree
{
public:
	typedef unsigned int BodyHandle;

private:
	struct Volume
	{
		BodyHandle handle;
		const Sphere* sphere;

		Volume() :
			handle(0),
			sphere(nullptr)
		{
		}

		Volume(BodyHandle p_Handle, const Sphere* p_Sphere) :
			handle(p_Handle),
			sphere(p_Sphere)
		{
		}
	};

	class Node
	{
	public:
		typedef std::unique_ptr<Node> uPtr;

	private:
		DirectX::XMFLOAT4 m_MinPos;
		DirectX::XMFLOAT4 m_MaxPos;

		bool m_IsLeaf;
		size_t m_NumBodies;

		std::array<Node::uPtr, 8> m_Children;
		
		static const size_t bodiesPerLeaf = 16;
		std::array<Volume, bodiesPerLeaf> m_Bodies;
		std::vector<Volume> m_LargeBodies;

	public:
		Node(const DirectX::XMFLOAT4& p_MinPos, const DirectX::XMFLOAT4& p_MaxPos);

		void swapRoot(Node::uPtr& p_PrevRoot, size_t p_Id);

		const DirectX::XMFLOAT4& getMinPos() const;
		const DirectX::XMFLOAT4& getMaxPos() const;

		size_t getBodyCount() const;

		void addBody(const Volume& p_Body);
		void addBodyIfIntersect(const Volume& p_Body);
		void removeBody(BodyHandle p_Body, const Sphere* p_Sphere);

		template <typename OutIt>
		void findPotentialIntersections(const Sphere* p_Sphere, OutIt p_Output) const
		{
			if (!Collision::AABBvsSphereIntersect(m_MinPos, m_MaxPos, *p_Sphere))
				return;

			for (const auto& largeBody : m_LargeBodies)
			{
				*p_Output++ = largeBody.handle;
			}

			if (m_IsLeaf)
			{
				for (size_t i = 0; i < m_NumBodies; ++i)
				{
					*p_Output++ = m_Bodies[i].handle;
				}
			}
			else
			{
				for (const auto& childNode : m_Children)
				{
					childNode->findPotentialIntersections(p_Sphere, p_Output);
				}
			}
		}

	private:
		void expand();
		void createChildren();
		void addToChildren(const Volume& p_Body);
		bool isLarge(const Volume& p_Body);
	};

	Node::uPtr m_RootNode;

public:
	Octree();

	void reset();

	void addBody(BodyHandle p_Body, const Sphere* p_Sphere);
	void removeBody(BodyHandle p_Body, const Sphere* p_Sphere);

	const DirectX::XMFLOAT4& getMinPos() const;
	const DirectX::XMFLOAT4& getMaxPos() const;

	size_t getBodyCount() const;

	template <typename OutIt>
	void findPotentialIntersections(const Sphere* p_Sphere, OutIt p_Output) const
	{
		if (!m_RootNode)
			return;

		m_RootNode->findPotentialIntersections(p_Sphere, p_Output);
	}

private:
	void increaseSize(const DirectX::XMFLOAT4& p_Target);
};
