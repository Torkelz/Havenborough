#pragma once
#include <DirectXMath.h>
#include <memory>

class Sphere;

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
	Type				m_Type;
	bool				m_CollisionResponse;
	int					m_IDInBody;
	unsigned int		m_BodyHandle;
	Sphere*				m_SurroundingSphere;

public:
	typedef std::unique_ptr<BoundingVolume> ptr;

	explicit BoundingVolume(Sphere* p_SurroundingSphere) :
		m_SurroundingSphere(p_SurroundingSphere)
	{}
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
	virtual const DirectX::XMFLOAT4& getPosition() const 
	{
		return m_Position;
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
	/**
	 * Which body handle is this volume connected to
	 * @return bodyhandle to the body containing this volume. Has bodyhandle 0 if its not assigned to a body
	 */
	unsigned int getBodyHandle() const
	{
		return m_BodyHandle;
	}
	/**
	 * Sets which body this volume should be connected to.
	 * @param p_BodyHandle, which body to connect the volume to
	 */
	void setBodyHandle(unsigned int p_BodyHandle)
	{
		m_BodyHandle = p_BodyHandle;
	}
	/**
	 * Set which id this volume has in a body.
	 * @param p_IDInBody, which id the volume should have in the body
	 * Note! Spheres and AABB can not be rotated!
	 */
	void setIDInBody(unsigned int p_IDInBody)
	{
		m_IDInBody = p_IDInBody;
	}
	/**
	 * Destructor
	 */
	virtual ~BoundingVolume(){};

	const Sphere* getSurroundingSphere() const
	{
		return m_SurroundingSphere;
	};
};