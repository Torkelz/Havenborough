#pragma once

#include "IPhysics.h"
#include <memory>

#include "Collision.h"
#include "Body.h"

class Physics : public IPhysics
{
public:
private:
	Collision m_Collision;
	float m_GlobalGravity;
	std::vector<Body> m_Bodies;
	std::vector<HitData> m_HitDatas;
	
public:
	Physics();
	~Physics();

	void initialize();

	void update(float p_DeltaTime);
	void applyForce(Vector4 p_Force, BodyHandle p_Body);

	BodyHandle createSphere(float p_Mass, bool p_IsImmovable, Vector3 p_Position, float p_Radius);
	BodyHandle createAABB(float p_Mass, bool p_IsImmovable, Vector3 p_Bot, Vector3 p_Top);

	void setGlobalGravity(float p_Gravity);
	Vector4 getVelocity(BodyHandle p_Body);

	HitData getHitDataAt(unsigned int p_Index);
	unsigned int getHitDataSize();

	Vector4 getBodyPosition(BodyHandle p_Body);

	void setBodyPosition(Vector3 p_Position, BodyHandle p_Body);

	void setLogFunction(clientLogCallback_t p_LogCallback) override;
		 
private:
	Body* findBody(BodyHandle p_Body);
	
	BodyHandle createBody(float p_Mass, BoundingVolume* p_BoundingVolume, bool p_IsImmovable);

	BoundingVolume* getVolume(BodyHandle p_Body);
};

