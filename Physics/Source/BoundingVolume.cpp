#include "BoundingVolume.h"

using namespace DirectX;

BoundingVolume::BoundingVolume(){}

int BoundingVolume::getIndex()
{
	return m_index;
}

bool BoundingVolume::compair( BoundingVolume* p_volume )
{
	// Return true if it's the same volume, else return false.
	if(p_volume->getIndex() == m_index)
	{
		return true;
	}

	else
	{
		return false;
	}
}

int	BoundingVolume::getType()
{
	return m_type;
}

XMFLOAT4* BoundingVolume::getPosition()
{
	return &m_position;
}

XMFLOAT4* BoundingVolume::getPrevPosition()
{
	return &m_prevPosition;
}