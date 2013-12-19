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

	void initialize() override;

	void update(float p_DeltaTime) override;
	void applyForce(Vector4 p_Force, BodyHandle p_Body) override;

	BodyHandle createSphere(float p_Mass, bool p_IsImmovable, Vector3 p_Position, float p_Radius) override;
	BodyHandle createAABB(float p_Mass, bool p_IsImmovable, Vector3 p_Bot, Vector3 p_Top) override;
	BodyHandle createOBB(float p_Mass, bool p_IsImmovable, Vector3 p_CenterPos, Vector3 p_Corner) override;

	void setGlobalGravity(float p_Gravity) override;
	Vector4 getVelocity(BodyHandle p_Body) override;

	HitData getHitDataAt(unsigned int p_Index) override;
	unsigned int getHitDataSize() override;

	Vector4 getBodyPosition(BodyHandle p_Body) override;

	void setBodyPosition(Vector3 p_Position, BodyHandle p_Body) override;

	void setBodyRotation(BodyHandle p_Body, float p_Yaw, float p_Pitch, float p_Roll) override;

	void setLogFunction(clientLogCallback_t p_LogCallback) override;
		 
private:
	Body* findBody(BodyHandle p_Body);
	
	BodyHandle createBody(float p_Mass, BoundingVolume* p_BoundingVolume, bool p_IsImmovable);

	BoundingVolume* getVolume(BodyHandle p_Body);
};

