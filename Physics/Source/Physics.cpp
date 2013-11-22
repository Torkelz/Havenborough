#include "Physics.h"

_declspec(dllexport) IPhysics::ptr IPhysics::createPhysics()
{
	return IPhysics::ptr(new Physics());
}

void Physics::initialize()
{
	m_collision = Collision();
}

bool Physics::sphereVsSphere( Sphere* p_sphere1, Sphere* p_sphere2 )
{
	return m_collision.sphereVsSphere(p_sphere1, p_sphere2);
}

bool Physics::AABBvsAABB( AABB* p_aabb1, AABB* p_aabb2 )
{
	return m_collision.AABBvsAABB(p_aabb1, p_aabb2);
}

bool Physics::AABBvsSphere( AABB* p_aabb, Sphere* p_sphere )
{
	return m_collision.AABBvsSphere(p_aabb, p_sphere);
}