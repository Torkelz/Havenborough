#pragma once

#include "PhysicsTypes.h"

class IPhysics
{	
public:
	__declspec(dllexport) static IPhysics *createPhysics(void);

	virtual void initialize() = 0;
	/**
	* Create a boundingVolume sphere with a body.
	* @p_Mass, define the mass for the body.
	* @p_IsImmovable, tells if the body are movible.
	* @p_Position, position for the boundingvolume(sphere). Body get same position.
	* @p_Radius, the radius for the sphere.
	* @return a BodyHandle so it can be mapped outside of Physics.
	*/
	virtual BodyHandle createSphere(float p_Mass, bool p_IsImmovable, Vector3 p_Position, float p_Radius) = 0;
	/**
	* Create a boundingVolume AABB with a body, the center position is calculated from the bot and top corner.
	* @p_Mass, define the mass for the body.
	* @p_IsImmovable, tells if the body are movible.
	* @p_Bot, bottom corner for box.
	* @p_Top, top corner for box.
	* @return a BodyHandle so it can be mapped outside of Physics.
	*/
	virtual BodyHandle createAABB(float p_Mass, bool p_IsImmovable, Vector3 p_Bot, Vector3 p_Top) = 0;
	/**
	* Keeps physics updated, collision checks etc.
	* @p_DeltaTime, timestep.
	*/
	virtual void update(float p_DeltaTime) = 0;
	/**
	* Apply a force on an object.
	* @p_Force, force to be added.
	* @p_Body, what body to apply the force.
	*/
	virtual void applyForce(Vector3 p_Force, BodyHandle p_Body) = 0;
	/**
	* Used to change the gravity constant.
	* @p_Gravity, the new gravity.
	*/
	virtual void setGlobalGravity(float p_Gravity) = 0;
	/**
	* Return the boundingvolume from the body.
	* @p_Body, what body to get the data from.
	* @return a BoundingVolum pointer.
	*/
	//virtual BoundingVolume* getVolume(BodyHandle p_Body) = 0;
	/**
	*Get the velocity for the chosen body.
	*@p_Body, what body to get the data from.
	* @return objects velocity.
	*/
	virtual Vector4 getVelocity(BodyHandle p_Body) = 0;
	/**
	* Get the hitdata from the vector containing all the collision hitdata for the last frame.
	* @p_Index, index number in the vector.
	* @return the hitdata on that index.
	*/
	//virtual Collision::HitData getHitDataAt(unsigned int p_Index) = 0;
	virtual unsigned int getHitDataSize() = 0;

	//DEBUGGING
	/**
	* A function used for debugging purpose.
	*/
	virtual void moveBodyPosition(Vector3 p_Position, BodyHandle p_Body) = 0;
};