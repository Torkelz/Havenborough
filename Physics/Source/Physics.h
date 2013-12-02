#pragma once

#include "IPhysics.h"

class Physics : public IPhysics
{
private:
	Collision m_Collision;
	float m_GlobalGravity;
	std::vector<Body> m_Bodies;
	std::vector<Collision::HitData> m_HitDatas;

private:
	Body* findBody(BodyHandle p_Body);
	
public:
	Physics();
	~Physics();

	void initialize();

	void update(float p_DeltaTime);
	void applyForce(DirectX::XMFLOAT4 p_Force, BodyHandle p_Body);

	BodyHandle createBody(float p_Mass, BoundingVolume* p_BoundingVolume, bool p_IsImmovable);

	void setGlobalGravity(float p_Gravity);

	BoundingVolume* getVolume(BodyHandle p_Body);
	DirectX::XMFLOAT4 getVelocity(BodyHandle p_Body);

	Collision::HitData getHitDataAt(unsigned int p_Index);
	unsigned int getHitDataSize();

	//Collision::HitData boundingVolumeVsBoundingVolume(BoundingVolume* p_Volume1, BoundingVolume* p_Volume2);
	//Collision::HitData boundingVolumeVsSphere(BoundingVolume* p_Volume, Sphere* p_Sphere);
	//Collision::HitData boundingVolumeVsAABB(BoundingVolume* p_Volume, AABB* p_AABB);
	//Collision::HitData sphereVsSphere(Sphere* p_Sphere1, Sphere* p_Sphere2);
	//Collision::HitData AABBvsAABB( AABB* p_AABB1, AABB* p_AABB2 );
	//Collision::HitData AABBvsSphere( AABB* p_AABB, Sphere* p_Sphere );

	//DEBUGGING
	void moveBodyPosition(DirectX::XMFLOAT4 p_Position, BodyHandle p_Body);
		 
	
};

