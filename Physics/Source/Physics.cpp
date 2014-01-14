#include "Physics.h"

#include "PhysicsLogger.h"

using namespace DirectX;

Physics::Physics(void)
	: m_GlobalGravity(9.82f)
{}

Physics::~Physics()
{
	m_Bodies.clear();
	m_sphereBoundingVolume.clear();
}

IPhysics *IPhysics::createPhysics()
{
	return new Physics();
}

void IPhysics::deletePhysics(IPhysics* p_Physics)
{
	if (p_Physics)
	{
		PhysicsLogger::log(PhysicsLogger::Level::INFO, "Shutting down physics");
		delete p_Physics;
	}
}

Body* Physics::findBody(BodyHandle p_Body)
{
	for (Body& b : m_Bodies)
	{
		if (b.getHandle() == p_Body)
		{
			return &b;
		}
	}

	return nullptr;
}

void Physics::initialize()
{
	PhysicsLogger::log(PhysicsLogger::Level::INFO, "Initializing physics");

	m_Collision = Collision();

	fillTriangleIndexList();

	std::vector<Triangle> triangles;

	float size = 1.f;
	//Back
	triangles.push_back(Triangle(Vector4( -size,  -size, -size, 1.f), Vector4(-size, size, -size, 1.f), Vector4(size,	size, -size, 1.f)));
	triangles.push_back(Triangle(Vector4( -size,  -size, -size, 1.f), Vector4( size, size, -size, 1.f), Vector4(size, -size, -size, 1.f)));
	
	//Top
	triangles.push_back(Triangle(Vector4( -size,  size,  -size, 1.f), Vector4(-size,  size, size, 1.f), Vector4( size,	size,  size, 1.f)));
	triangles.push_back(Triangle(Vector4( -size,  size,  -size, 1.f), Vector4( size,  size, size, 1.f), Vector4( size,	size, -size, 1.f)));
	
	//Front
	triangles.push_back(Triangle(Vector4( -size, -size,  size, 1.f), Vector4(  size,  size,  size, 1.f), Vector4(-size,	 size,  size, 1.f)));
	triangles.push_back(Triangle(Vector4( -size, -size,  size, 1.f), Vector4(  size,  -size,  size, 1.f), Vector4(size,  size,  size, 1.f)));

	//right	 																  
	triangles.push_back(Triangle(Vector4(-size,  -size,  size, 1.f), Vector4( -size, size, size, 1.f), Vector4(-size,	size,  -size, 1.f)));
	triangles.push_back(Triangle(Vector4(-size, -size, size, 1.f), Vector4( -size, size, -size, 1.f), Vector4(-size,	-size,  -size, 1.f)));

	//left
	triangles.push_back(Triangle(Vector4(size, -size, -size, 1.f), Vector4(size,  size, -size, 1.f), Vector4( size, size, size, 1.f)));
	triangles.push_back(Triangle(Vector4( size,  -size, -size, 1.f), Vector4( size, size, size, 1.f), Vector4(size, -size, size, 1.f)));

	//bottom
	triangles.push_back(Triangle(Vector4( size, -size, size, 1.f), Vector4( -size,  -size, size, 1.f), Vector4( -size,	-size,  -size, 1.f)));
	triangles.push_back(Triangle(Vector4( size,  -size,  -size, 1.f), Vector4( size,  -size, size, 1.f), Vector4( -size,	-size, -size, 1.f)));

	Hull *hull = new Hull(triangles);

	float scale = 1.5f;
	XMMATRIX m = XMMatrixScaling(scale, scale, scale);
	XMFLOAT4X4 fm;
	XMStoreFloat4x4(&fm, m);  
	hull->setScale(fm);

	float rot = 0.0f;
	XMMATRIX m_rot = XMMatrixRotationRollPitchYaw(rot, rot, rot);
	XMStoreFloat4x4(&fm, m_rot);  

	hull->setRotation(fm);


	DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(0.f, 1.f, 0.f);
	DirectX::XMFLOAT4X4 mtrans;
	DirectX::XMStoreFloat4x4(&mtrans, trans);
	hull->updatePosition(mtrans);

	createBody(1.f, hull, true, false);

	m_BVLoader.loadBinaryFile("assets/LightModels/CB_Sphere.txc");
	
	m_sphereBoundingVolume = m_BVLoader.getBoundingVolumes();

	m_BVLoader.clear();
}

void Physics::update(float p_DeltaTime)
{
	m_HitDatas.clear();
	for(unsigned i = 0; i < m_Bodies.size(); i++)
	{
		Body& b = m_Bodies[i];

		if(b.getIsImmovable())
			continue;

		if(b.getInAir())
		{
			b.setGravity(m_GlobalGravity);
		}
		else
		{
			b.setGravity(0.f);
		}

		b.update(p_DeltaTime);

		bool onSomething = false;

		for (unsigned j = 0; j < m_Bodies.size(); j++)
		{
			unsigned int hh = m_Bodies.at(j).getHandle();
			HitData hit = m_Collision.boundingVolumeVsBoundingVolume(b.getVolume(), m_Bodies[j].getVolume());
			
			if(hit.intersect)
			{
				hit.collider = m_Bodies.at(i).getHandle();
				hit.collisionVictim = m_Bodies.at(j).getHandle();
				hit.isEdge = m_Bodies.at(j).getIsEdge();
				m_HitDatas.push_back(hit);
				XMVECTOR temp;		// m
				XMFLOAT4 tempPos;	// m

				temp = XMLoadFloat4(&b.getPosition()) + Vector4ToXMVECTOR(&hit.colNorm) * hit.colLength / 100.f;	// m
				XMStoreFloat4(&tempPos, temp);

				b.setPosition(tempPos);

				if (hit.colNorm.y > 0.88f)
				{
					onSomething = true;

					XMFLOAT4 velocity = b.getVelocity();	// m/s
					velocity.y = 0.f;
					b.setVelocity(velocity);
				}
			}
		}

		b.setInAir(!onSomething);

		/*if(b.m_IsImmovable())
		{
			Collision col;
			for(SPD &s : m_spatialList)
				col.collision(b.getVolume, s.getVolume)
		}*/
	}
}

void Physics::applyForce(BodyHandle p_Body, Vector3 p_Force)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return;

	XMFLOAT4 tempForce = Vector3ToXMFLOAT4(&p_Force, 0.f); // kg*m/s^2

	body->addForce(tempForce);
}

BodyHandle Physics::createSphere(float p_Mass, bool p_IsImmovable, Vector3 p_Position, float p_Radius)
{
	Vector3 convPosition = p_Position * 0.01f;	// m
	XMFLOAT4 tempPosition = Vector3ToXMFLOAT4(&convPosition, 1.f); // m

	Sphere* sphere = new Sphere(p_Radius / 100.f, tempPosition);

	return createBody(p_Mass, sphere, p_IsImmovable, false);
}

BodyHandle Physics::createAABB(float p_Mass, bool p_IsImmovable, Vector3 p_CenterPos, Vector3 p_Extents, bool p_IsEdge)
{
	Vector3 convPosition = p_CenterPos * 0.01f;	// m
	Vector3 convExtents = p_Extents * 0.01f;	// m

	XMFLOAT4 tempPos = Vector3ToXMFLOAT4(&convPosition, 1.f);	// m
	XMFLOAT4 tempExt = Vector3ToXMFLOAT4(&convExtents , 0.f);	// m

	AABB* aabb = new AABB(tempPos, tempExt);

	return createBody(p_Mass, aabb, p_IsImmovable, p_IsEdge);
}

BodyHandle Physics::createOBB(float p_Mass, bool p_IsImmovable, Vector3 p_CenterPos, Vector3 p_Extent, bool p_IsEdge)
{
	Vector3 convPosition = p_CenterPos * 0.01f;	// m
	Vector3 convExtents = p_Extent * 0.01f;	// m

	XMFLOAT4 tempPos	= Vector3ToXMFLOAT4(&convPosition, 1.f);	// m
	XMFLOAT4 tempExt	= Vector3ToXMFLOAT4(&convExtents, 0.f);	// m

	OBB *obb = new OBB(tempPos, tempExt);

	return createBody(p_Mass, obb, p_IsImmovable, p_IsEdge);
}

bool Physics::createLevelBV(const char* p_VolumeID, const char* p_FilePath)
{
	m_BVLoader.loadBinaryFile(p_FilePath);

	return true;
}

bool Physics::releaseLevelBV(const char* p_VolumeID)
{
	throw std::exception("Unimplemented function");
}

void Physics::releaseAllBoundingVolumes(void)
{
	m_Bodies.clear();
	m_Bodies.shrink_to_fit();

	int i=0;
}

void Physics::setBVPosition(int p_Instance, Vector3 p_Position)
{
	throw std::exception("Unimplemented function");
}

void Physics::setBVRotation(int p_Instance, Vector3 p_Rotation)
{
	throw std::exception("Unimplemented function");
}

void Physics::setBVScale(int p_Instance, Vector3 p_Scale)
{
	Body* body = findBody(p_Instance);
	if(body == nullptr)
		return;

	if(body->getVolume()->getType() == BoundingVolume::Type::OBB)
	{
		((OBB*)body->getVolume())->setExtent(XMFLOAT4(p_Scale.x, p_Scale.y, p_Scale.z, 0.f));
		return;
	}

	return;
}

BodyHandle Physics::createBody(float p_Mass, BoundingVolume* p_BoundingVolume, bool p_IsImmovable, bool p_IsEdge)
{
	m_Bodies.emplace_back(p_Mass, std::unique_ptr<BoundingVolume>(p_BoundingVolume), p_IsImmovable, p_IsEdge);
	return m_Bodies.back().getHandle();
}

void Physics::fillTriangleIndexList()
{
	m_BoxTriangleIndex.push_back(XMFLOAT3(1, 0, 2));
	m_BoxTriangleIndex.push_back(XMFLOAT3(2, 3, 1));
	m_BoxTriangleIndex.push_back(XMFLOAT3(5, 1, 3));
	m_BoxTriangleIndex.push_back(XMFLOAT3(3, 7, 5));
	m_BoxTriangleIndex.push_back(XMFLOAT3(4, 5, 7));
	m_BoxTriangleIndex.push_back(XMFLOAT3(7, 6, 4));
	m_BoxTriangleIndex.push_back(XMFLOAT3(2, 0, 4));
	m_BoxTriangleIndex.push_back(XMFLOAT3(4, 6, 2));
	m_BoxTriangleIndex.push_back(XMFLOAT3(6, 7, 3));
	m_BoxTriangleIndex.push_back(XMFLOAT3(3, 2, 6));
	m_BoxTriangleIndex.push_back(XMFLOAT3(1, 5, 4));
	m_BoxTriangleIndex.push_back(XMFLOAT3(4, 0, 1));
}

void Physics::setGlobalGravity(float p_Gravity)
{
	m_GlobalGravity = p_Gravity;
}

BoundingVolume* Physics::getVolume(BodyHandle p_Body)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return nullptr;

	return body->getVolume();
}

Vector4 Physics::getVelocity(BodyHandle p_Body)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return Vector4(0.f, 0.f, 0.f, 0.f);

	XMFLOAT4 tempVel = body->getVelocity();

	return Vector4(tempVel.x, tempVel.y, tempVel.z, tempVel.w) * 100.f;
}

HitData Physics::getHitDataAt(unsigned int p_Index)
{
	return m_HitDatas.at(p_Index);
}

void Physics::removedHitDataAt(unsigned int p_index)
{
	m_HitDatas.erase(m_HitDatas.begin() + p_index);
}

unsigned int Physics::getHitDataSize()
{
	return m_HitDatas.size();
}

Vector4 Physics::getBodyPosition(BodyHandle p_Body)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return Vector4(0.f, 0.f, 0.f, 1.f);

	XMFLOAT4 temp = body->getPosition();	// m
	Vector4 tempvec4;	// cm

	tempvec4 = XMFLOAT4ToVector4(&temp) * 100.f;

	return tempvec4;
}

Vector3 Physics::getBodySize(BodyHandle p_Body)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return Vector3(0.f, 0.f, 0.f);

	Vector3 temp;
	float r;
	XMFLOAT4 bSize;
	switch (body->getVolume()->getType())
	{
	case BoundingVolume::Type::AABBOX:
		bSize = *((AABB*)body->getVolume())->getHalfDiagonal();
		temp = Vector3(bSize.x,bSize.y,bSize.z);
		break;
	case BoundingVolume::Type::SPHERE:
		r = ((Sphere*)body->getVolume())->getRadius();
		temp = Vector3(r,r,r);
		break;
	default:
		temp = Vector3(0,0,0);
		break;
	}
	
	return temp * 100.f;
}

void Physics::setBodyPosition( BodyHandle p_Body, Vector3 p_Position)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return;

	Vector3 convPosition = p_Position * 0.01f;	// m
	XMFLOAT4 tempPosition = Vector3ToXMFLOAT4(&convPosition, 1.f);	// m

	body->setPosition(tempPosition);
}
void Physics::setBodyVelocity( BodyHandle p_Body, Vector3 p_Velocity)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return;

	Vector3 convVelocity = p_Velocity * 0.01f;	// m
	XMFLOAT4 tempPosition = Vector3ToXMFLOAT4(&convVelocity, 0.f);	// m

	body->setVelocity(tempPosition);
}

void Physics::setBodyRotation( BodyHandle p_Body, Vector3 p_Rotation)
{
	Body* body = findBody(p_Body);
	if(body == nullptr || body->getVolume()->getType() != BoundingVolume::Type::OBB)
		return;

	OBB *obb = (OBB*)(body->getVolume());
	XMFLOAT4X4 temp;
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(p_Rotation.y, p_Rotation.x, p_Rotation.z);

	XMStoreFloat4x4(&temp, rotation);
	obb->setRotationMatrix(temp);
}

void Physics::setLogFunction(clientLogCallback_t p_LogCallback)
{
	PhysicsLogger::setLogFunction(p_LogCallback);
}

Triangle Physics::getTriangleFromBody(unsigned int p_BodyHandle, unsigned int p_TriangleIndex)
{
	Body* body = findBody(p_BodyHandle);
	Triangle trig;
	if(body == nullptr)
		return trig;

	BoundingVolume *volume = body->getVolume();

	switch (volume->getType())
	{
	case BoundingVolume::Type::AABBOX:
		{
			XMFLOAT3 triangleIndex = m_BoxTriangleIndex.at(p_TriangleIndex);
			Triangle triangle = Triangle(XMFLOAT4ToVector4(&((AABB*)volume)->getBoundWorldCoordAt((int)triangleIndex.x)) * 100,
										 XMFLOAT4ToVector4(&((AABB*)volume)->getBoundWorldCoordAt((int)triangleIndex.y)) * 100,
										 XMFLOAT4ToVector4(&((AABB*)volume)->getBoundWorldCoordAt((int)triangleIndex.z)) * 100);

			return triangle;
		}
	case BoundingVolume::Type::HULL:
		{
			Triangle temp = ((Hull*)volume)->getTriangleInWorldCoord(p_TriangleIndex);
			temp.corners[0] = temp.corners[0] * 100;
			temp.corners[1] = temp.corners[1] * 100;
			temp.corners[2] = temp.corners[2] * 100;
			return temp;
		}
		
	case BoundingVolume::Type::OBB:
		{
			XMFLOAT3 triangleIndex = m_BoxTriangleIndex.at(p_TriangleIndex);
			Triangle triangle = Triangle(XMFLOAT4ToVector4(&((OBB*)volume)->getCornerWorldCoordAt((int)triangleIndex.x)) * 100,
										 XMFLOAT4ToVector4(&((OBB*)volume)->getCornerWorldCoordAt((int)triangleIndex.y)) * 100,
										 XMFLOAT4ToVector4(&((OBB*)volume)->getCornerWorldCoordAt((int)triangleIndex.z)) * 100);
			return triangle;
		}
	case BoundingVolume::Type::SPHERE:
		{
			Triangle triangle = Triangle(XMFLOAT4ToVector4(&m_sphereBoundingVolume.at(p_TriangleIndex * 3).m_Postition    ) * 100,
										 XMFLOAT4ToVector4(&m_sphereBoundingVolume.at(p_TriangleIndex * 3 + 1).m_Postition) * 100,
										 XMFLOAT4ToVector4(&m_sphereBoundingVolume.at(p_TriangleIndex * 3 + 2).m_Postition) * 100);
			triangle.uniformScale(((Sphere*)volume)->getRadius());
			triangle.translate(XMFLOAT4ToVector4(&body->getPosition()));

			return triangle;
		}
	default:
		break;
	}

	return trig;
}
unsigned int Physics::getNrOfTrianglesFromBody(unsigned int p_BodyHandle)
{
	Body* body = findBody(p_BodyHandle);
	if(body == nullptr)
		return 0;

	BoundingVolume *volume = body->getVolume();

	switch (volume->getType())
	{
	case BoundingVolume::Type::AABBOX:
	case BoundingVolume::Type::OBB:
		return m_BoxTriangleIndex.size();
	case BoundingVolume::Type::HULL:
		return ((Hull*)volume)->getTriangleListSize();
	case BoundingVolume::Type::SPHERE:
		return m_sphereBoundingVolume.size() / 3;
	default:
		break;
	}

	return 0;
}

//
//Collision::HitData Physics::sphereVsSphere( Sphere* p_sphere1, Sphere* p_sphere2 )
//{
//	return m_Collision.sphereVsSphere(p_sphere1, p_sphere2);
//}
//
//Collision::HitData Physics::AABBvsAABB( AABB* p_aabb1, AABB* p_aabb2 )
//{
//	return m_Collision.AABBvsAABB(p_aabb1, p_aabb2);
//}
//
//Collision::HitData Physics::AABBvsSphere( AABB* p_aabb, Sphere* p_sphere )
//{
//	return m_Collision.AABBvsSphere(p_aabb, p_sphere);
//}