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

	void initialize();

	void update(float p_DeltaTime);
	void applyForce(Vector4 p_Force, BodyHandle p_Body);

	BodyHandle createSphere(float p_Mass, bool p_IsImmovable, Vector3 p_Position, float p_Radius);
	BodyHandle createAABB(float p_Mass, bool p_IsImmovable, Vector3 p_Bot, Vector3 p_Top, bool p_IsEdge);

	bool createLevelBV(const char* m_ModelID, const char* m_FilePath) override;

	bool releaseLevelBV(const char* p_ModelID) override; 

	void setBVPosition(int p_Instance, Vector3 p_Position) override;
	void setBVRotation(int p_Instance, Vector3 p_Rotation) override;
	void setBVScale(int p_Instance, Vector3 p_Scale) override;

	void setGlobalGravity(float p_Gravity);
	Vector4 getVelocity(BodyHandle p_Body);

	HitData getHitDataAt(unsigned int p_Index);
	void removedHitDataAt(unsigned int p_index) override;
	unsigned int getHitDataSize();

	Vector4 getBodyPosition(BodyHandle p_Body);
	Vector3 getBodySize(BodyHandle p_Body) override;

	void setBodyPosition(Vector3 p_Position, BodyHandle p_Body);
	void setBodyVelocity(Vector3 p_Velocity, BodyHandle p_Body) override;

	void setLogFunction(clientLogCallback_t p_LogCallback) override;
		 
private:
	Body* findBody(BodyHandle p_Body);
	
	BodyHandle createBody(float p_Mass, BoundingVolume* p_BoundingVolume, bool p_IsImmovable, bool p_IsEdge);

	BoundingVolume* getVolume(BodyHandle p_Body);
};

