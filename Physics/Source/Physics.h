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
	float m_GlobalGravity;
	std::vector<Body> m_Bodies;
	std::vector<HitData> m_HitDatas;
	BVLoader m_BVLoader;
	bool m_LoadBVSphereTemplateOnce;
	std::vector<std::pair<std::string, std::vector<BVLoader::BoundingVolume>>> m_TemplateBVList;
	std::vector<BVLoader::BoundingVolume> m_sphereBoundingVolume ;

	std::vector<DirectX::XMFLOAT3> m_BoxTriangleIndex;
public:
	Physics();
	~Physics();

	void initialize() override;

	void update(float p_DeltaTime) override;
	void applyForce(BodyHandle p_Body, Vector3 p_Force) override;

	BodyHandle createSphere(float p_Mass, bool p_IsImmovable, Vector3 p_Position, float p_Radius) override;
	BodyHandle createAABB(float p_Mass, bool p_IsImmovable, Vector3 p_CenterPos, Vector3 p_Extents, bool p_IsEdge);
	BodyHandle createOBB(float p_Mass, bool p_IsImmovable, Vector3 p_CenterPos, Vector3 p_Extents, bool p_IsEdge) override;

	BodyHandle createBVInstance(const char* p_VolumeID) override;
	bool createBV(const char* m_ModelID, const char* m_FilePath) override;

	bool releaseBV(const char* p_ModelID) override; 
	void releaseBody(BodyHandle p_Body) override;
	void releaseAllBoundingVolumes(void) override;

	void setGlobalGravity(float p_Gravity) override;
	bool getBodyInAir(BodyHandle p_Body) override;

	HitData getHitDataAt(unsigned int p_Index) override;
	void removeHitDataAt(unsigned int p_index) override;
	unsigned int getHitDataSize() override;

	void setBodyCollisionResponse(BodyHandle p_Body, bool p_State) override;
	Vector3 getBodyPosition(BodyHandle p_Body) override;

	Vector3 getBodySize(BodyHandle p_Body) override;

	void setBodyPosition(BodyHandle p_Body, Vector3 p_Position) override;

	void setBodyVelocity(BodyHandle p_Body, Vector3 p_Velocity) override;
	Vector3 getBodyVelocity(BodyHandle p_Body) override;

	void setBodyRotation(BodyHandle p_Body, Vector3 p_Rotation) override;
	void setBodyScale(BodyHandle p_BodyHandle, Vector3 p_Scale) override;

	void setLogFunction(clientLogCallback_t p_LogCallback) override;

	Triangle getTriangleFromBody(unsigned int p_BodyHandle, unsigned int p_TriangleIndex) override;
	unsigned int getNrOfTrianglesFromBody(unsigned int p_BodyHandle) override;
		 
private:
	Body* findBody(BodyHandle p_Body);
	
	BodyHandle createBody(float p_Mass, BoundingVolume* p_BoundingVolume, bool p_IsImmovable, bool p_IsEdge);

	BoundingVolume* getVolume(BodyHandle p_Body);

	void fillTriangleIndexList();
};

