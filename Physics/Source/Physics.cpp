#include "Physics.h"

#include "PhysicsLogger.h"

using namespace DirectX;

Physics::Physics(void)
	: m_GlobalGravity(30.f)
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
		p_Physics->releaseAllBoundingVolumes();
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

	fillTriangleIndexList();
	m_LoadBVSphereTemplateOnce = true;
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
			if(i == j)
				continue;

			HitData hit = Collision::boundingVolumeVsBoundingVolume(*b.getVolume(), *m_Bodies[j].getVolume());
			
			if(hit.intersect)
			{
				hit.collider = m_Bodies.at(i).getHandle();
				hit.collisionVictim = m_Bodies.at(j).getHandle();
				hit.isEdge = m_Bodies.at(j).getIsEdge();
				m_HitDatas.push_back(hit);

				if(m_Bodies.at(i).getCollisionResponse() && m_Bodies.at(j).getCollisionResponse())
				{
					XMVECTOR temp;		// m
					XMFLOAT4 tempPos;	// m

					temp = XMLoadFloat4(&b.getPosition()) + Vector4ToXMVECTOR(&hit.colNorm) * hit.colLength / 100.f;	// m
					XMStoreFloat4(&tempPos, temp);

					b.setPosition(tempPos);

					if (hit.colNorm.y > 0.68f)
					{
						onSomething = true;

						XMFLOAT4 velocity = b.getVelocity();	// m/s
						velocity.y = 0.f;
						b.setVelocity(velocity);
					}
				}
			}
		}

		b.setInAir(!onSomething);
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

BodyHandle Physics::createBVInstance(const char* p_VolumeID)
{
	std::vector<BVLoader::BoundingVolume> tempBV;
	for(auto& bv : m_TemplateBVList)
	{
		if(strcmp(bv.first.c_str(), p_VolumeID) == 0)
		{
			tempBV = bv.second;
			break;
		}
	}

	if(tempBV.empty())
	{	
		PhysicsLogger::log(PhysicsLogger::Level::ERROR_L, "Bounding Volume from template is empty");
		return (BodyHandle)0;
	}

	std::vector<Triangle> triangles;
	Triangle triangle;

	for(unsigned i = 0; i < tempBV.size() / 3; i++)
	{
		triangle.corners[0] = XMFLOAT4ToVector4(&tempBV[i * 3].m_Postition);
		triangle.corners[1] = XMFLOAT4ToVector4(&tempBV[i * 3 + 1].m_Postition);
		triangle.corners[2] = XMFLOAT4ToVector4(&tempBV[i * 3 + 2].m_Postition);

		triangles.push_back(triangle);
	}

	Hull *hull = new Hull(triangles);

	return createBody(1.f, hull, true, false);

}

void Physics::releaseBody(BodyHandle p_Body)
{
	for (auto& body : m_Bodies)
	{
		if (body.getHandle() == p_Body)
		{
			std::swap(body, m_Bodies.back());
			m_Bodies.pop_back();
			return;
		}
	}
}

bool Physics::createBV(const char* p_VolumeID, const char* p_FilePath)
{
	if(!m_BVLoader.loadBinaryFile(p_FilePath))
	{
		PhysicsLogger::log(PhysicsLogger::Level::ERROR_L, "Loading Bounding Volume file error");
		return false;
	}
	std::vector<BVLoader::BoundingVolume> tempBV;
	tempBV = m_BVLoader.getBoundingVolumes();

	if(tempBV.empty())
	{
		PhysicsLogger::log(PhysicsLogger::Level::ERROR_L, "Bounding Volume from BVLoader is empty");
		return false;
	}

	for(unsigned i = 0; i < tempBV.size(); i++)
	{
		tempBV[i].m_Postition.x *= 0.01f;
		tempBV[i].m_Postition.y *= 0.01f;
		tempBV[i].m_Postition.z *= 0.01f; 
	}

	m_TemplateBVList.push_back(std::pair<std::string, std::vector<BVLoader::BoundingVolume>>(p_VolumeID, tempBV));
	m_BVLoader.clear();
	PhysicsLogger::log(PhysicsLogger::Level::INFO, "CreateBV success");
	return true;
}

bool Physics::releaseBV(const char* p_VolumeID)
{
	for(auto bv = m_TemplateBVList.begin(); bv != m_TemplateBVList.end(); bv++)
	{
		const char* temp = bv->first.c_str();
		if(strcmp(temp, p_VolumeID) == 0)
		{
			m_TemplateBVList.erase(bv);
			return true;
		}
	}

	return false;
}

void Physics::releaseAllBoundingVolumes(void)
{
	m_Bodies.clear();
	m_Bodies.shrink_to_fit();
	Body b;
	b.resetBodyHandleCounter();
	m_sphereBoundingVolume.clear();
}

void Physics::setBodyScale(BodyHandle p_BodyHandle, Vector3 p_Scale)
{
	Body* body = findBody(p_BodyHandle);
	if(body == nullptr)
		return;
	XMVECTOR scale = Vector3ToXMVECTOR(&p_Scale, 0.f);

	switch (body->getVolume()->getType())
	{
	case BoundingVolume::Type::AABBOX:
		((AABB*)body->getVolume())->scale(scale);
		break;

	case BoundingVolume::Type::HULL:
		((Hull*)body->getVolume())->scale(scale);
		break;

	case BoundingVolume::Type::OBB:
		((OBB*)body->getVolume())->scale(scale);
		break;

	case BoundingVolume::Type::SPHERE:
		((Sphere*)body->getVolume())->scale(scale);
		break;
	default:
		break;
	}
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

bool Physics::getBodyInAir(BodyHandle p_Body)
{
    Body* body = findBody(p_Body);
    if(body == nullptr)
            return false;

    return body->getInAir();
}

BoundingVolume* Physics::getVolume(BodyHandle p_Body)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return nullptr;

	return body->getVolume();
}

HitData Physics::getHitDataAt(unsigned int p_Index)
{
	return m_HitDatas.at(p_Index);
}

void Physics::removeHitDataAt(unsigned int p_index)
{
	m_HitDatas.erase(m_HitDatas.begin() + p_index);
}

unsigned int Physics::getHitDataSize()
{
	return m_HitDatas.size();
}

void Physics::setBodyCollisionResponse(BodyHandle p_Body, bool p_State)
{
	Body *body = findBody(p_Body);
	if(!body)
		return;

	body->setCollisionResponse(p_State);
}

Vector3 Physics::getBodyPosition(BodyHandle p_Body)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return Vector3(0.f, 0.f, 0.f);

	XMFLOAT4 temp = body->getPosition();	// m

	Vector3 tempvec3(temp.x * 100.f, temp.y * 100.f, temp.z * 100.f);	// cm

	return tempvec3;
}

Vector3 Physics::getBodySize(BodyHandle p_Body)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return Vector3(0.f, 0.f, 0.f);

	Vector3 temp;
	float r;
	switch (body->getVolume()->getType())
	{
	case BoundingVolume::Type::AABBOX:
		temp = XMFLOAT4ToVector3(((AABB*)body->getVolume())->getHalfDiagonal());
		break;
	case BoundingVolume::Type::SPHERE:
		r = ((Sphere*)body->getVolume())->getRadius();
		temp = Vector3(r,r,r);
		break;
	case BoundingVolume::Type::OBB:
		temp = XMFLOAT4ToVector3(&((OBB*)body->getVolume())->getExtents());
		break;
	case BoundingVolume::Type::HULL:
		temp = XMFLOAT4ToVector3(&((Hull*)body->getVolume())->getScale());
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

Vector3 Physics::getBodyVelocity(BodyHandle p_Body)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return Vector3(0.f, 0.f, 0.f);

	XMFLOAT4 tempVel = body->getVelocity();

	return Vector3(tempVel.x, tempVel.y, tempVel.z) * 100.f;
}

void Physics::setBodyRotation( BodyHandle p_Body, Vector3 p_Rotation)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return;

	XMFLOAT4X4 temp;
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(p_Rotation.y, p_Rotation.x, p_Rotation.z);

	XMStoreFloat4x4(&temp, rotation);

	switch (body->getVolume()->getType())
	{
	case BoundingVolume::Type::OBB:
		{
			((OBB*)body->getVolume())->setRotation(rotation);
			break;
		}
	case BoundingVolume::Type::HULL:
		{
			((Hull*)body->getVolume())->setRotation(rotation);
			break;
		}
	default:
		break;
	}

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
			Triangle triangle = Triangle(XMFLOAT4ToVector4(&((AABB*)volume)->getBoundWorldCoordAt((int)triangleIndex.x)) * 100.f,
										 XMFLOAT4ToVector4(&((AABB*)volume)->getBoundWorldCoordAt((int)triangleIndex.y)) * 100.f,
										 XMFLOAT4ToVector4(&((AABB*)volume)->getBoundWorldCoordAt((int)triangleIndex.z)) * 100.f);

			return triangle;
		}
	case BoundingVolume::Type::HULL:
		{
			Triangle triangle = ((Hull*)volume)->getTriangleInWorldCoord(p_TriangleIndex);
			triangle.uniformScale(100.f);
			return triangle;
		}
		
	case BoundingVolume::Type::OBB:
		{
			XMFLOAT3 triangleIndex = m_BoxTriangleIndex.at(p_TriangleIndex);
			Triangle triangle = Triangle(XMFLOAT4ToVector4(&((OBB*)volume)->getCornerWorldCoordAt((int)triangleIndex.x)) * 100.f,
										 XMFLOAT4ToVector4(&((OBB*)volume)->getCornerWorldCoordAt((int)triangleIndex.y)) * 100.f,
										 XMFLOAT4ToVector4(&((OBB*)volume)->getCornerWorldCoordAt((int)triangleIndex.z)) * 100.f);
			return triangle;
		}
	case BoundingVolume::Type::SPHERE:
		{
			Triangle triangle = Triangle(XMFLOAT4ToVector4(&m_sphereBoundingVolume.at(p_TriangleIndex * 3).m_Postition    ),
										 XMFLOAT4ToVector4(&m_sphereBoundingVolume.at(p_TriangleIndex * 3 + 1).m_Postition),
										 XMFLOAT4ToVector4(&m_sphereBoundingVolume.at(p_TriangleIndex * 3 + 2).m_Postition));
			triangle.uniformScale(((Sphere*)volume)->getRadius());
			triangle.translate(XMFLOAT4ToVector4(&body->getPosition()));
			triangle.uniformScale(100.f);
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
		{
			if(m_LoadBVSphereTemplateOnce)
			{
				m_LoadBVSphereTemplateOnce = false;
				m_BVLoader.loadBinaryFile("assets/LightModels/CB_Sphere.txc");

				m_sphereBoundingVolume = m_BVLoader.getBoundingVolumes();
				m_BVLoader.clear();
			}
			return m_sphereBoundingVolume.size() / 3;
		}
	default:
		break;
	}

	return 0;
}
