#include "Physics.h"

#include "PhysicsLogger.h"

using namespace DirectX;

Physics::Physics(void)
	: m_GlobalGravity(9.82f)
{

}

Physics::~Physics()
{
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

		for (unsigned j = i + 1; j < m_Bodies.size(); j++)
		{
			HitData hit = m_Collision.boundingVolumeVsBoundingVolume(b.getVolume(), m_Bodies[j].getVolume());
			
			if(hit.intersect)
			{
				m_HitDatas.push_back(hit);
				XMVECTOR temp;
				XMFLOAT4 tempPos;

				temp = XMLoadFloat4(&b.getPosition()) + Vector4ToXMVECTOR(&hit.colNorm) * hit.colLength;
				XMStoreFloat4(&tempPos, temp);

				b.setPosition(tempPos);

				if (hit.colNorm.y > 0.88f)
				{
					onSomething = true;

					XMFLOAT4 velocity = b.getVelocity();
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

void Physics::applyForce(Vector4 p_Force, BodyHandle p_Body)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return;

	XMFLOAT4 tempForce = XMFLOAT4(0.f, 0.f, 0.f, 0.f);

	tempForce.x = p_Force.x;
	tempForce.y = p_Force.y;
	tempForce.z = p_Force.z;
	tempForce.w = p_Force.w;

	body->addForce(tempForce);
}

BodyHandle Physics::createSphere(float p_Mass, bool p_IsImmovable, Vector3 p_Position, float p_Radius)
{
	XMFLOAT4 tempPosition = XMFLOAT4(0.f, 0.f, 0.f, 0.f);;
	tempPosition.x = p_Position.x;
	tempPosition.y = p_Position.y;
	tempPosition.z = p_Position.z;
	tempPosition.w = 1.f;

	Sphere* sphere = new Sphere(p_Radius, tempPosition);

	return createBody(p_Mass, sphere, p_IsImmovable);
}

BodyHandle Physics::createAABB(float p_Mass, bool p_IsImmovable, Vector3 p_Bot, Vector3 p_Top)
{
	XMFLOAT4 tempBot = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMFLOAT4 tempTop = XMFLOAT4(0.f, 0.f, 0.f, 0.f);;
	tempBot.x = p_Bot.x;
	tempBot.y = p_Bot.y;
	tempBot.z = p_Bot.z;
	tempBot.w = 1.f;

	tempTop.x = p_Top.x;
	tempTop.y = p_Top.y;
	tempTop.z = p_Top.z;
	tempTop.w = 1.f;

	AABB* aabb = new AABB(tempBot, tempTop);

	return createBody(p_Mass, aabb, p_IsImmovable);
}

bool Physics::createLevelBV(const char* p_ModelID, const char* p_FilePath)
{
	m_BVLoader.loadBinaryFile(p_FilePath);


	return true;
}

bool Physics::releaseLevelBV(const char* p_ModelID)
{
	return true;
}

void Physics::setBVPosition(int p_Instance, float p_x, float p_y, float p_z)
{

}

void Physics::setBVRotation(int p_Instance, float p_x, float p_y, float p_z)
{

}

void Physics::setBVScale(int p_Instance, float p_x, float p_y, float p_z)
{

}

BodyHandle Physics::createBody(float p_Mass, BoundingVolume* p_BoundingVolume, bool p_IsImmovable)
{
	m_Bodies.emplace_back(p_Mass, std::unique_ptr<BoundingVolume>(p_BoundingVolume), p_IsImmovable);
	return m_Bodies.back().getHandle();
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

	return Vector4(tempVel.x, tempVel.y, tempVel.z, tempVel.w);
}

HitData Physics::getHitDataAt(unsigned int p_Index)
{
	return m_HitDatas.at(p_Index);
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

	XMFLOAT4 temp = body->getPosition();
	Vector4 tempvec4;

	tempvec4 = XMFLOAT4ToVector4(&temp);

	return tempvec4;
}

void Physics::setBodyPosition(Vector3 p_Position, BodyHandle p_Body)
{
	Body* body = findBody(p_Body);
	if(body == nullptr)
		return;

	XMFLOAT4 tempPosition = XMFLOAT4(0.f, 0.f, 0.f, 0.f);

	tempPosition.x = p_Position.x;
	tempPosition.y = p_Position.y;
	tempPosition.z = p_Position.z;
	tempPosition.w = 1.f;

	body->setPosition(tempPosition);
}

void Physics::setLogFunction(clientLogCallback_t p_LogCallback)
{
	PhysicsLogger::setLogFunction(p_LogCallback);
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