#pragma once
#include <DirectXMath.h>
#include <memory>

class BoundingVolume
{
public:
	enum class Type {
		AABBOX,
		SPHERE,
		OBB,
		HULL,
		NONE
	};

protected:
	DirectX::XMFLOAT4	m_Position;
	DirectX::XMFLOAT4	m_PrevPosition;
	Type				m_Type;
	bool				m_CollisionResponse;
	int					m_IDInBody;
	unsigned int		m_BodyHandle;

public:
	typedef std::unique_ptr<BoundingVolume> ptr;

	BoundingVolume(){};
	/* Updates position for BoundingVolume with translation matrix.
	 * @param p_Translation, move the BV in relative coordinates.
	 */
	virtual void updatePosition(DirectX::XMFLOAT4X4 const &p_Translation) = 0;	
	
	/* sets position for BoundingVolume with a new position.
	 * @param p_Position, move the BV in to this position.
	 */
	virtual void setPosition(DirectX::XMVECTOR const &p_Position) = 0;
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
	/**
	 * Set if the boundingVolume should have collision response or not.
	 * @param p_ShouldRespond, true if the volume should respond to collisions otherwise false
	 */
	void setCollisionResponse(bool p_ShouldRespond)
	{
		m_CollisionResponse = p_ShouldRespond;
	}
	/**
	 * Should the volume respond to collision or not.
	 * @return p_ShouldRespond, true if the volume should respond to collisions otherwise false
	 */
	bool getCollisionResponse() const
	{
		return m_CollisionResponse;
	}
	/**
	 * Scales the volume.
	 * @param p_Scale, Scale vector to use when scaling.
	 */
	virtual void scale(DirectX::XMVECTOR const &p_Scale) = 0;

	/**
	 * Rotates the volume.
	 * @param p_Rotation, Rotation vector containing RollPitchYaw
	 * Note! Spheres and AABB can not be rotated!
	 */
	virtual void setRotation(DirectX::XMMATRIX const &p_Rotation) = 0;
	
	unsigned int getBodyHandle() const
	{
		return m_BodyHandle;
	}

	void setBodyHandle(unsigned int p_BodyHandle)
	{
		m_BodyHandle = p_BodyHandle;
	}

	void setIDInBody(int p_IDInBody)
	{
		m_IDInBody = p_IDInBody;
	}

	int getIDInBody() const
	{
		return m_IDInBody;
	}

	virtual ~BoundingVolume(){};
};