#pragma once

#include "IPhysics.h"
#include <memory>

#include "Collision.h"
#include "Body.h"
#include "BVLoader.h"

class Physics : public IPhysics
{
public:
private:
	Collision m_Collision;
	float m_GlobalGravity;
	std::vector<Body> m_Bodies;
	std::vector<HitData> m_HitDatas;
	BVLoader m_BVLoader;
public:
	Physics();
	~Physics();

	void initialize() override;

	void update(float p_DeltaTime) override;
	void applyForce(Vector4 p_Force, BodyHandle p_Body) override;

	BodyHandle createSphere(float p_Mass, bool p_IsImmovable, Vector3 p_Position, float p_Radius) override;
	BodyHandle createAABB(float p_Mass, bool p_IsImmovable, Vector3 p_Bot, Vector3 p_Top, bool p_IsEdge);
	BodyHandle createOBB(float p_Mass, bool p_IsImmovable, Vector3 p_CenterPos, Vector3 p_Extents, bool p_IsEdge) override;

	bool createLevelBV(const char* m_ModelID, const char* m_FilePath) override;

	bool releaseLevelBV(const char* p_ModelID) override; 

	void setBVPosition(int p_Instance, float p_x, float p_y, float p_z) override;
	void setBVRotation(int p_Instance, float p_x, float p_y, float p_z) override;
	void setBVScale(int p_Instance, float p_x, float p_y, float p_z) override;

	void setGlobalGravity(float p_Gravity) override;
	Vector4 getVelocity(BodyHandle p_Body) override;

	HitData getHitDataAt(unsigned int p_Index) override;
	void removedHitDataAt(unsigned int p_index) override;
	unsigned int getHitDataSize() override;

	Vector4 getBodyPosition(BodyHandle p_Body) override;
	Vector3 getBodySize(BodyHandle p_Body) override;

	void setBodyPosition(Vector3 p_Position, BodyHandle p_Body) override;
	void setBodyVelocity(Vector3 p_Velocity, BodyHandle p_Body) override;

	void setBodyRotation(BodyHandle p_Body, float p_Yaw, float p_Pitch, float p_Roll) override;

	void setLogFunction(clientLogCallback_t p_LogCallback) override;
		 
private:
	Body* findBody(BodyHandle p_Body);
	
	BodyHandle createBody(float p_Mass, BoundingVolume* p_BoundingVolume, bool p_IsImmovable, bool p_IsEdge);

	BoundingVolume* getVolume(BodyHandle p_Body);
};

