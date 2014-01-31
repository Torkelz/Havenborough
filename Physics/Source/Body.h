#pragma once
#include <DirectXMath.h>
#include "BoundingVolume.h"

#include <memory>

class Body
{
protected:
	typedef unsigned int BodyHandle;

	static BodyHandle m_NextHandle;

	static BodyHandle getNextHandle();

	BodyHandle m_Handle;

	DirectX::XMFLOAT4	m_NetForce;			// kg*m/s^2
	DirectX::XMFLOAT4	m_Position;			// m
	DirectX::XMFLOAT4	m_Velocity;			// m/s
	DirectX::XMFLOAT4	m_Acceleration;		// m/s^2
	DirectX::XMFLOAT4	m_LastAcceleration;	// m/s^2
	DirectX::XMFLOAT4	m_AvgAcceleration;	// m/s^2
	DirectX::XMFLOAT4	m_NewAcceleration;	// m/s^2
	float				m_Mass;				// kg
	float				m_Gravity;			// m/s^2
	bool				m_InAir;

	bool				m_IsImmovable;
	bool				m_IsEdge;
	bool				m_CollisionResponse;

	std::unique_ptr<BoundingVolume>	m_Volume;
public:
	/**
	* Body Constructor, initialize all variables.
	* @p_Mass, object's mass.
	* @p_Volume, pointer to the body's BoundingVolume.
	* @p_IsImmovable, should a body be immovable(static)?
	*/
	Body(float p_Mass, std::unique_ptr<BoundingVolume> p_Volume, bool p_IsImmovable, bool p_IsEdge);
	/**
	* Move constructor, needed because c++11 is not fully integrated to VS2012.
	*/
	Body(Body &&p_Other);

	/**
	 * Move assignment
	 */
	Body& operator=(Body&& p_Other);
	
	Body(){};
	~Body();

	/**
	* Add force to the netForce(Total) for the object.
	* @p_Force, force to be added. Force in Newton (N = (kg*m)/s^2)
	*/
	void addForce(DirectX::XMFLOAT4 p_Force);

	/**
	* Update loop for a body. Updates acceleration, velocity and position.
	*/
	void update(float p_DeltaTime);
	/**
	* Updates the body's BoundingVolumes position with relative coordinates.
	* @p_Position, relative position in cm.
	*/
	virtual void updateBoundingVolumePosition(DirectX::XMFLOAT4 p_Position);
	/**
	* Set the gravity that will act on the object.
	* @p_Gravity, how much gravity shall the new gravity be.
	*/
	void setGravity(float p_Gravity);
	
	/**
	* Get the bool for if the body is in the air.
	* @return true if the body is in the air, otherwise false.
	*/
	bool getInAir();
	/**
	* Sets the bool for if the body is in the air.
	* @p_Bool, sets the bool to this parameter.
	*/
	void setInAir(bool p_Bool);
	/**
	* Is the body immovable(static)?
	* @return a bool, if the body is movable or immovable.
	*/
	bool getIsImmovable();
	/**
	* Is the body a Edge(static)?
	* @return a bool, if the body is an edge.
	*/
	bool getIsEdge();
	/**
	* Set if the body should have collision response.
	*/
	void setCollisionResponse(bool p_State);
	/**
	* Shall the body react to collision response?
	* @return a bool, if the body should have collision response.
	*/
	bool getCollisionResponse();
	/**
	* Get the volume that is connected to the body.
	* @return body's volume.
	*/
	BoundingVolume *getVolume();
	/**
	* Get the current velocity for the body.
	* @return m_Velocity in m/s.
	*/
	DirectX::XMFLOAT4 getVelocity();
	/**
	* Sets the velocity for the body.
	* @p_Velocity is the new velocity in m/s
	*/
	void setVelocity(DirectX::XMFLOAT4 p_Velocity);
	/**
	* Get the body's current position.
	* @return m_Position in m
	*/
	DirectX::XMFLOAT4 getPosition();
	/**
	* Sets the position(absolute) for the body.
	* @p_Positon is the new position in m
	*/
	void setPosition(DirectX::XMFLOAT4 p_Position);
	/**
	* Return the BodyHandle index of the body, its unique number.
	* @return m_Handle;
	*/
	BodyHandle getHandle() { return m_Handle; }
	/*
	* reset the BodyHandleCounter. Only use when clearing the body list in physics
	*/
	static void resetBodyHandleCounter();
		
private:
	/**
	 * Calculates the new acceleration in m/s^2.
	 */
	DirectX::XMFLOAT4 calculateAcceleration();
	void addGravity();
protected:
	Body(const Body&);
	Body& operator=(const Body&);

public:	
	DirectX::XMFLOAT4 getNetForce();
	DirectX::XMFLOAT4 getACC();
	DirectX::XMFLOAT4 getLastACC();
	float getGravity();
};
