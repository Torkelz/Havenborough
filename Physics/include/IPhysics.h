#pragma once
#include <memory>

#include "..\Source\Collision.h"
class IPhysics
{
public:
	typedef std::shared_ptr<IPhysics> ptr;
public:
	_declspec(dllexport) static IPhysics::ptr createPhysics();

	virtual void initialize() = 0;

	/**
	* Sphere versus Sphere collision
	* @return true if collision happens, otherwise false.
	*/
	virtual bool sphereVsSphere( Sphere* p_sphere1, Sphere* p_sphere2 ) = 0;
	/**
	* AABB versus AABB collision
	* ## Tip: You can use the AABBs' spheres and do SphereVsSphere before calling this 
	* if you are doing a lot of collision tests and expect very few of them to return 
	* true for better performance. ##
	* @return true if collision happens, otherwise false
	*/
	virtual bool AABBvsAABB( AABB* p_aabb1, AABB* p_aabb2 ) = 0;
	/**
	* AABB versus Sphere collision
	* ## Tip: You can use the AABB's sphere and do SphereVsSphere before calling this 
	* if you are doing a lot of collision tests and expect very few of them to return 
	* true for better performance. ##
	* @return true if collision happens, otherwise false
	*/
	virtual bool AABBvsSphere( AABB* p_aabb, Sphere* p_sphere ) = 0;
};