#pragma once

#include <DirectXMath.h>
#include "BoundingVolume.h"
#include "VolumeIncludeAll.h"

#include <memory>

class Body
{
protected:
	typedef unsigned int BodyHandle;

	static BodyHandle m_NextHandle;

	static BodyHandle getNextHandle();

	BodyHandle m_Handle;

	DirectX::XMFLOAT4	m_NetForce;
	DirectX::XMFLOAT4	m_Position;
	DirectX::XMFLOAT4	m_Velocity;
	DirectX::XMFLOAT4	m_Acceleration;
	DirectX::XMFLOAT4	m_LastAcceleration;
	DirectX::XMFLOAT4	m_AvgAcceleration;
	DirectX::XMFLOAT4	m_NewAcceleration;
	float				m_Mass;
	float				m_Gravity;
	bool				m_InAir;

	bool				m_IsImmovable;

	std::unique_ptr<BoundingVolume>	m_Volume;
public:
	/**
	* Body Constructor, initialize all variables.
	* @p_Mass, object's mass.
	* @p_Volume, pointer to the body's BoundingVolume.
	* @p_IsImmovable, should a body be immovable(static)?
	*/
	Body(float p_Mass, std::unique_ptr<BoundingVolume> p_Volume, bool p_IsImmovable);
	/**
	* Move constructor, needed because c++11 is not fully integrated to VS2012.
	*/
	Body(Body &&p_Other);
	
	Body(){};
	~Body();

	/**
	* Add force to the netForce(Total) for the object.
	* @p_Force, force to be added.
	*/
	void addForce(DirectX::XMFLOAT4 p_Force);

	/**
	* Update loop for a body. Updates acceleration, velocity and position.
	*/
	void update(float p_DeltaTime);
	/**
	* Updates the body's BoundingVolumes position with relative coordinates.
	* @p_Position, relative position.
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
	* Get the volume that is connected to the body.
	* @return body's volume.
	*/
	BoundingVolume* getVolume();
	/**
	* Get the current velocity for the body.
	* @return m_Velocity.
	*/
	DirectX::XMFLOAT4 getVelocity();
	/**
	* Sets the velocity for the body.
	* @p_Velocity is the new velocity.
	*/
	void setVelocity(DirectX::XMFLOAT4 p_Velocity);
	/**
	* Get the body's current position.
	* @return m_Position.
	*/
	DirectX::XMFLOAT4 getPosition();
	/**
	* Sets the position(absolute) for the body.
	* @p_Positon is the new position.
	*/
	void setPosition(DirectX::XMFLOAT4 p_Position);
	/**
	* Return the BodyHandle index of the body, its unique number.
	* @return m_Handle;
	*/
	BodyHandle getHandle() { return m_Handle; }
		
private:
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
