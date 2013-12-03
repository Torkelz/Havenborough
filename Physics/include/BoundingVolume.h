#ifndef BOUNDINGVOLUME_H
#define BOUNDINGVOLUME_H

#include <DirectXMath.h>

class BoundingVolume
{
public:
	enum class Type {
		AABBOX,
		SPHERE
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
	 * @return m_index
	 */
	int	getIndex()
	{
		return m_Index;
	}
	/**
	 * Compare two bounding volumes to figure out if they are different.
	 * @p_volume the volume to compare with
	 * @return true if it's the same volume, else return false.
	 */
	bool compare( BoundingVolume* p_Volume )
	{
		// Return true if it's the same volume, else return false.
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
	 * @p_Translation, move the AABB in relative coordinates.
	 */
	virtual void updatePosition( DirectX::XMFLOAT4X4& p_Translation ) = 0;
	/**
	 * Get the current position for the bounding volume.
	 * @return m_position.
	 */
	virtual DirectX::XMFLOAT4* getPosition()
	{
		return &m_Position;
	}
	/**
	 * Get the bounding volume position in the last frame.
	 * @return m_prevPosition.
	 */
	DirectX::XMFLOAT4* getPrevPosition()
	{
		return &m_PrevPosition;
	}
	/**
	 * Return the bounding volume type tied to the TYPE enum.
	 * @return m_type
	 */
	Type getType()
	{
		return m_Type;
	}
};

#endif