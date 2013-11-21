#pragma once
#include <memory>

#include "..\Source\Collision.h"
class IPhysics
{
public:
	typedef std::shared_ptr<IPhysics> ptr;

	_declspec(dllexport) static IPhysics::ptr createPhysics();

	virtual void	initialize() = 0;

	virtual bool	sphereVsSphere( Sphere* p_sphere1, Sphere* p_sphere2 ) = 0;
	virtual int		AABBvsAABB( AABB* p_aabb1, AABB* p_aabb2 ) = 0;
	virtual bool	AABBvsSphere( AABB* p_aabb, Sphere* p_sphere ) = 0;
};