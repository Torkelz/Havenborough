#pragma once
#include <DirectXMath.h>
#include "BoundingVolume.h"

#include <vector>

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
	bool				m_OnSomething;

	bool				m_IsImmovable;
	bool				m_IsEdge;
	bool				m_Landed;

	std::vector<BoundingVolume::ptr> m_Volumes;
public:
	/**
	* Body Constructor, initialize all variables.
	* @p_Mass, object's mass.
	* @p_Volume, pointer to the body's BoundingVolume.
	* @p_IsImmovable, should a body be immovable(static)?
	*/
	Body(float p_Mass, BoundingVolume::ptr p_Volume, bool p_IsImmovable, bool p_IsEdge);
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
	 * Add an impulse to the object.
	 *
	 * @param p_Impulse the impulse to add, in Newton-seconds (N*s = (kg*m)/s)
	 */
	void addImpulse(DirectX::XMFLOAT4 p_Impulse);

	void addVolume(BoundingVolume::ptr p_Volume);
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
	* Get the bool for if the body is on a surface.
	* @return true if the body is on a surface, otherwise false.
	*/
	bool getOnSomething();
	/**
	* Sets the bool for if the body is on a surface.
	* @p_Bool, sets the bool to this parameter.
	*/
	void setOnSomething(bool p_Bool);

	void setRotation(DirectX::XMMATRIX const &p_Rotation);
	/**
	* Get the bool for if the body is on a has landed on a surface.
	* @return true if the body is on a surface, otherwise false.
	*/
	bool getLanded();
	/**
	* Sets the bool for if the body has landed on a surface.
	* @p_Bool, sets the bool to this parameter.
	*/
	void setLanded(bool p_Bool);

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
	* Set if the all volumes in the body should have collision response
	* @param p_State, true if the volumes should react to collision otherwise false.
	*/
	void setCollisionResponse(bool p_State);
	/**
	* Set if a volume in the body should react to collision response.
	* @param p_Volume, which volume in the body.
	* @param p_State, true if the volume should react to collision
	*/
	void setCollisionResponse(unsigned int p_Volume, bool p_State);
	/**
	* Shall a volume in the body react to collision response?
	* @param p_Volume, which volume in the body.
	* @return a bool, if the volume should have collision response.
	*/
	bool getCollisionResponse(unsigned int p_Volume);

	/**
	* set collision victim
	* @param p_Body which body that this collided against..
	*/
	void setLastCollision(BodyHandle p_Body);
	/**
	* get last collision body that happend
	* @return return the body.
	*/
	BodyHandle getLastCollision();

	/**
	* Get the first volume that is connected to the body.
	* @return body's volume.
	*/
	BoundingVolume *getVolume();		
	/**
	* Get the chosen volume that is connected to the body.
	* @param p_Volume, what volume to get.
	* @return body's volume.
	*/
	BoundingVolume *getVolume(unsigned int p_Volume);
	/**
	* Get how many bounding volumes this body has
	* @return the size of the list containing volumes.
	*/
	unsigned int getVolumeListSize();
	/**
	* Set the chosen volume's position that is connected to the body.
	* @param p_Volume, which volume to move 
	* @param p_Position, the new position for the chosen volume.
	*/
	void setVolumePosition(unsigned int p_Volume, DirectX::XMVECTOR const &p_Position);
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
	/**
	* Get the current orientation for the body
	* @return m_Orientation.
	*/
	DirectX::XMFLOAT4X4 getOrientation();
	/**
	* Sets the orientation for the body
	* @p_Orientation, the new orientation (frame of reference/local coordinate system)
	*/
	void setOrientation(DirectX::XMMATRIX const &p_Orientation);

		
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
