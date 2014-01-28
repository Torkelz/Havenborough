#pragma once

#include <DirectXMath.h>

class BoundingVolume
{
public:
	enum class Type {
		AABBOX,
		SPHERE,
		OBB,
		HULL
	};

protected:
	DirectX::XMFLOAT4	m_Position;
	DirectX::XMFLOAT4	m_PrevPosition;
	int					m_Index;
	Type				m_Type;

public:
	BoundingVolume(){};
	/**
	 * Get a unique index from the volume for comparison.
	 * @return index of the bounding volume.
	 */
	int	getIndex()
	{
		return m_Index;
	}
	/**
	 * Compare two bounding volumes to figure out if they are different.
	 * @param p_volume the volume to compare with.
	 * @return true if it's the same volume, else return false.
	 */
	bool compare(BoundingVolume* p_Volume)
	{
		if(p_Volume->getIndex() == m_Index)
		{
			return true;
		}

		else
		{
			return false;
		}
	}
	/* Updates position for BoundingVolume with translation matrix.
	 * @param p_Translation, move the AABB in relative coordinates.
	 */
	virtual void updatePosition(DirectX::XMFLOAT4X4& p_Translation) = 0;
	/**
	 * Get the current position for the bounding volume.
	 * @return the position of the bounding volume in m
	 */
	virtual DirectX::XMFLOAT4* getPosition()
	{
		return &m_Position;
	}
	/**
	 * Get the bounding volume position in the last frame.
	 * @return the previus position of the bounding volume in m
	 */
	DirectX::XMFLOAT4* getPrevPosition()
	{
		return &m_PrevPosition;
	}
	/**
	 * Return the bounding volume type tied to the TYPE enum.
	 * @return type of the bounding volume.
	 */
	BoundingVolume::Type getType()
	{
		return m_Type;
	}

	virtual void scale(const DirectX::XMVECTOR &p_Scale) = 0;
	
};