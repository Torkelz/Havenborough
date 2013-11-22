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
	BoundingVolume(){};
	/**
	* Get a unique index from the volume for comparison.
	* @return m_index
	*/
	int	getIndex(){
		return m_index;
	};
	/**
	* Compare two bounding volumes to figure out if they are different.
	* @p_volume the volume to compare with
	* @return true if it's the same volume, else return false.
	*/
	bool compare( BoundingVolume* p_volume ){
		// Return true if it's the same volume, else return false.
		if(p_volume->getIndex() == m_index)
		{
			return true;
		}

		else
		{
			return false;
		}
	};
	/**
	* Get the current position for the bounding volume.
	* @return m_position.
	*/
	DirectX::XMFLOAT4*	getPosition(){
		return &m_position;
	};
	/**
	* Get the bounding volume position in the last frame.
	* @return m_prevPosition.
	*/
	DirectX::XMFLOAT4*	getPrevPosition(){
		return &m_prevPosition;
	};
	/**
	* Return the bounding volume type tied to the TYPE enum.
	* @return m_type
	*/
	int	getType(){
		return m_type;
	};
};

#endif