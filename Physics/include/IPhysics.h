#pragma once
#include <memory>

#include "..\Source\Collision.h"
#include "..\Source\Body.h"
class IPhysics
{
public:
	typedef unsigned int BodyHandle;
	typedef std::unique_ptr<IPhysics> ptr;
public:
	_declspec(dllexport) static IPhysics::ptr createPhysics();

	virtual void initialize() = 0;

	/**
	* Create a body to apply physics on.
	* @p_Mass, define the mass for the body.
	* @p_BoundingVolume, which boundingvolume the body should have.
	* @p_IsImmovable, tells if the body are movible.
	* @return a BodyHandle so it can be mapped outside of Physics.
	*/
	virtual BodyHandle createBody(float p_Mass, BoundingVolume* p_BoundingVolume, bool p_IsImmovable) = 0;
	/**
	* Keeps physics updated.
	* @p_DeltaTime, timestep.
	*/
	virtual void update(float p_DeltaTime) = 0;
	/**
	* Apply a force on an object.
	* @p_Force, force to be added.
	* @p_Body, what body to apply the force.
	*/
	virtual void applyForce(DirectX::XMFLOAT4 p_Force, BodyHandle p_Body) = 0;
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
	virtual BoundingVolume* getVolume(BodyHandle p_Body) = 0;
	/**
	*Get the velocity for the chosen body.
	*@p_Body, what body to get the data from.
	* @return objects velocity.
	*/
	virtual DirectX::XMFLOAT4 getVelocity(BodyHandle p_Body) = 0;
	/**
	* Get the hitdata from the vector containing all the collision hitdata for the last frame.
	* @p_Index, index number in the vector.
	* @return the hitdata on that index.
	*/
	virtual Collision::HitData getHitDataAt(unsigned int p_Index) = 0;

	virtual unsigned int getHitDataSize() = 0;

	///**
	//* Redirect to the appropriate check, when neither BoundingVolumes' type is known.
	//* @return HitData, see HitData definition.
	//*/
	//virtual Collision::HitData boundingVolumeVsBoundingVolume(BoundingVolume* p_Volume1, BoundingVolume* p_Volume2) = 0;
	///**
	//* Check for the appropriate collision, a BoundingVolume versus a sphere.
	//* @return HitData, see HitData definition.
	//*/
	//virtual Collision::HitData boundingVolumeVsSphere(BoundingVolume* p_Volume, Sphere* p_Sphere) = 0;
	///**
	//* Check for the appropriate collision, a BoundingVolume versus an AABB.
	//* @return HitData, see HitData definition.
	//*/
	//virtual Collision::HitData boundingVolumeVsAABB(BoundingVolume* p_Volume, AABB* p_AABB) = 0;
	///**
	//* Sphere versus Sphere collision
	//* @return HitData, see HitData definition.
	//*/
	//virtual Collision::HitData sphereVsSphere(Sphere* p_Sphere1, Sphere* p_Sphere2) = 0;
	///**
	//* AABB versus AABB collision
	//* ## SphereVsSphere check happens before 
	//* the actual AABBvsAABB collision check. ##
	//* @return HitData, see HitData definition.
	//*/
	//virtual Collision::HitData AABBvsAABB( AABB* p_AABB1, AABB* p_AABB2 ) = 0;
	///**
	//* AABB versus Sphere collision
	//* ## SphereVsSphere check happens before 
	//* the actual AABBvsAABB collision check. ##
	//* @return HitData, see HitData definition.
	//*/
	//virtual Collision::HitData AABBvsSphere( AABB* p_AABB, Sphere* p_Sphere ) = 0;

	//DEBUGGING
	/**
	* A function used for debugging purpose.
	*/
	virtual void moveBodyPosition(DirectX::XMFLOAT4 p_Position, BodyHandle p_Body) = 0;
};