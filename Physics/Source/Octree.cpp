#include "Octree.h"

#include "Collision.h"
#include "Sphere.h"

using namespace DirectX;

Octree::Node::Node(const DirectX::XMFLOAT4& p_MinPos, const DirectX::XMFLOAT4& p_MaxPos) :
	m_IsLeaf(true),
	m_MinPos(p_MinPos),
	m_MaxPos(p_MaxPos),
	m_LargeBodies(0),
	m_NumBodies(0)
{
}

void Octree::Node::swapRoot(Node::uPtr& p_PrevRoot, size_t p_Id)
{
	createChildren();
	std::swap(m_Children[p_Id], p_PrevRoot);
}

const DirectX::XMFLOAT4& Octree::Node::getMinPos() const
{
	return m_MinPos;
}

const DirectX::XMFLOAT4& Octree::Node::getMaxPos() const
{
	return m_MaxPos;
}

size_t Octree::Node::getBodyCount() const
{
	size_t count = m_NumBodies + m_LargeBodies.size();

	if (!m_IsLeaf)
	{
		for (const auto& childNode : m_Children)
		{
			count += childNode->getBodyCount();
		}
	}

	return count;
}

void Octree::Node::addBody(const Volume& p_Body)
{
	if (isLarge(p_Body))
	{
		m_LargeBodies.push_back(p_Body);
		return;
	}

	if (m_IsLeaf)
	{
		if (m_NumBodies == bodiesPerLeaf)
		{
			expand();
			addToChildren(p_Body);
		}
		else
		{
			m_Bodies[m_NumBodies] = p_Body;
			++m_NumBodies;
		}
	}
	else
	{
		addToChildren(p_Body);
	}
}

void Octree::Node::addBodyIfIntersect(const Volume& p_Body)
{
	if (Collision::AABBvsSphereIntersect(m_MinPos, m_MaxPos, *p_Body.sphere))
	{
		addBody(p_Body);
	}
}

void Octree::Node::removeBody(BodyHandle p_Body, const Sphere* p_Sphere)
{
	if (!Collision::AABBvsSphereIntersect(m_MinPos, m_MaxPos, *p_Sphere))
		return;

	size_t i = 0;
	while (i < m_LargeBodies.size())
	{
		if (m_LargeBodies[i].handle == p_Body)
		{
			std::swap(m_LargeBodies[i], m_LargeBodies.back());
			m_LargeBodies.pop_back();
			continue;
		}

		++i;
	}

	if (m_IsLeaf)
	{
		i = 0;
		while (i < m_NumBodies)
		{
			if (m_Bodies[i].handle == p_Body)
			{
				std::swap(m_Bodies[i], m_Bodies[m_NumBodies - 1]);
				--m_NumBodies;
				continue;
			}

			++i;
		}
	}
	else
	{
		for (auto& childNode : m_Children)
		{
			childNode->removeBody(p_Body, p_Sphere);
		}
	}
}

void Octree::Node::expand()
{
	createChildren();

	for (auto& body : m_Bodies)
	{
		addToChildren(body);
	}
	m_NumBodies = 0;
}

void Octree::Node::createChildren()
{
	const XMFLOAT4 corners[3] =
	{
		m_MinPos,
		XMFLOAT4((m_MinPos.x + m_MaxPos.x) * 0.5f,
			(m_MinPos.y + m_MaxPos.y) * 0.5f,
			(m_MinPos.z + m_MaxPos.z) * 0.5f,
			1.f),
		m_MaxPos
	};
	static const size_t subDivides[8][3] =
	{
		{0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {0, 1, 1},
		{1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1}
	};

	size_t i = 0;
	for (const auto& divide : subDivides)
	{
		m_Children[i].reset(new Node(
			XMFLOAT4(corners[divide[0]    ].x, corners[divide[1]    ].y, corners[divide[2]    ].z, 1.f),
			XMFLOAT4(corners[divide[0] + 1].x, corners[divide[1] + 1].y, corners[divide[2] + 1].z, 1.f)));

		++i;
	}

	m_IsLeaf = false;
}

void Octree::Node::addToChildren(const Volume& p_Body)
{
	for (auto& childNode : m_Children)
	{
		childNode->addBodyIfIntersect(p_Body);
	}
}

bool Octree::Node::isLarge(const Volume& p_Body)
{
	if (p_Body.sphere->getRadius() >= (m_MaxPos.x - m_MinPos.x) * 8.f)
		return true;

	return Collision::AABBInsideSphere(m_MinPos, m_MaxPos, *p_Body.sphere);
}

Octree::Octree()
{
}

void Octree::reset()
{
	m_RootNode.reset();
}

void Octree::addBody(BodyHandle p_Body, const Sphere* p_Sphere)
{
	if (!m_RootNode)
	{
		const XMFLOAT4 center = p_Sphere->getPosition();
		const float radius = p_Sphere->getRadius();
		m_RootNode.reset(new Node(XMFLOAT4(center.x - radius, center.y - radius, center.z - radius, 1.f),
			XMFLOAT4(center.x + radius, center.y + radius, center.z + radius, 1.f)));
	}
	else
	{
		while (!Collision::SphereInsideAABB(getMinPos(), getMaxPos(), *p_Sphere))
		{
			increaseSize(p_Sphere->getPosition());
		}
	}

	m_RootNode->addBody(Volume(p_Body, p_Sphere));
}

void Octree::removeBody(BodyHandle p_Body, const Sphere* p_Sphere)
{
	if (!m_RootNode)
		return;

	m_RootNode->removeBody(p_Body, p_Sphere);
}

const DirectX::XMFLOAT4& Octree::getMinPos() const
{
	static const XMFLOAT4 origo(0.f, 0.f, 0.f, 1.f);
	if (!m_RootNode)
	{
		return origo;
	}

	return m_RootNode->getMinPos();
}

const DirectX::XMFLOAT4& Octree::getMaxPos() const
{
	static const XMFLOAT4 origo(0.f, 0.f, 0.f, 1.f);
	if (!m_RootNode)
	{
		return origo;
	}

	return m_RootNode->getMaxPos();
}

size_t Octree::getBodyCount() const
{
	if (!m_RootNode)
		return 0;

	return m_RootNode->getBodyCount();
}

void Octree::increaseSize(const DirectX::XMFLOAT4& p_Target)
{
	const XMFLOAT4& currentMin = getMinPos();
	const XMFLOAT4& currentMax = getMaxPos();
	XMFLOAT4 currentCenter(
		(currentMin.x + currentMax.x) * 0.5f,
		(currentMin.y + currentMax.y) * 0.5f,
		(currentMin.z + currentMax.z) * 0.5f,
		1.f);
	XMFLOAT3 currentSize(
		currentMax.x - currentMin.x,
		currentMax.y - currentMin.y,
		currentMax.z - currentMin.z);

	bool posX = p_Target.x > currentCenter.x;
	bool posY = p_Target.y > currentCenter.y;
	bool posZ = p_Target.z > currentCenter.z;

	size_t index = 0;

	XMFLOAT4 newMin = currentMin;
	XMFLOAT4 newMax = currentMax;
	if (posX)
	{
		newMax.x += currentSize.x;
	}
	else
	{
		newMin.x -= currentSize.x;
		index += 4;
	}

	if (posY)
	{
		newMax.y += currentSize.y;
	}
	else
	{
		newMin.y -= currentSize.y;
		index += 2;
	}

	if (posZ)
	{
		newMax.z += currentSize.z;
	}
	else
	{
		newMin.z -= currentSize.z;
		index += 1;
	}

	Node::uPtr newRoot(new Node(newMin, newMax));
	newRoot->swapRoot(m_RootNode, index);
	std::swap(m_RootNode, newRoot);
}
