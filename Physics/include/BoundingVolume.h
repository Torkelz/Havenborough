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
	Type				m_Type;

public:
	BoundingVolume(){};
	/* Updates position for BoundingVolume with translation matrix.
	 * @param p_Translation, move the BV in relative coordinates.
	 */
	virtual void updatePosition(DirectX::XMFLOAT4X4 const &p_Translation) = 0;
	/**
	 * Get the current position for the bounding volume.
	 * @return the position of the bounding volume in m
	 */
	virtual DirectX::XMFLOAT4 getPosition() const 
	{
		return m_Position;
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
	BoundingVolume::Type getType() const
	{
		return m_Type;
	}

	virtual void scale(DirectX::XMVECTOR const &p_Scale) = 0;
	
	virtual ~BoundingVolume(){};
};