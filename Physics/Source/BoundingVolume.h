#ifndef BOUNDINGVOLUME_H
#define BOUNDINGVOLUME_H

#include <DirectXMath.h>

class BoundingVolume
{
protected:
	DirectX::XMFLOAT4	m_position;
	DirectX::XMFLOAT4	m_prevPosition;
	int					m_index;
	int					m_type;

	enum TYPE {
		AABBOX,
		SPHERE
	};

public:
	BoundingVolume();
	/**
	* Get a unique index from the volume for comparison.
	* @return m_index
	*/
	int					getIndex();
	/**
	* Compare two bounding volumes to figure out if they are different.
	* @p_volume the volume to compare with
	* @return true if it's the same volume, else return false.
	*/
	bool				compare( BoundingVolume* p_volume );
	/**
	* Get the current position for the bounding volume.
	* @return m_position.
	*/
	DirectX::XMFLOAT4*	getPosition();
	/**
	* Get the bounding volume position in the last frame.
	* @return m_prevPosition.
	*/
	DirectX::XMFLOAT4*	getPrevPosition();
	/**
	* Return the bounding volume type tied to the TYPE enum.
	* @return m_type
	*/
	int					getType();
};

#endif