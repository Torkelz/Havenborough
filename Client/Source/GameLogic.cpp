#include "GameLogic.h"
#include "Components.h"
#include "EventData.h"

GameLogic::GameLogic(void)
{
	m_Physics = nullptr;
	m_ResourceManager = nullptr;
}


GameLogic::~GameLogic(void)
{
	m_Physics = nullptr;
	m_ResourceManager = nullptr;
}

void GameLogic::initialize(ResourceManager *p_ResourceManager, IPhysics *p_Physics, ActorFactory *p_ActorFactory, EventManager *p_EventManager)
{
	m_Physics = p_Physics;
	m_ResourceManager = p_ResourceManager;
	m_ActorFactory = p_ActorFactory;
	m_EventManager = p_EventManager;
	m_Level = Level(m_ResourceManager, m_Physics, m_ActorFactory);
	m_Level.loadLevel("../Bin/assets/levels/Level3.btxl", "../Bin/assets/levels/CB_Level3.btxl", m_Objects);
	m_Level.setStartPosition(XMFLOAT3(0.0f, 200.0f, 1500.0f)); //TODO: Remove this line when level gets the position from file
	m_Level.setGoalPosition(XMFLOAT3(0.0f, 0.0f, 0.0f)); //TODO: Remove this line when level gets the position from file
	m_FinishLine = m_Physics->createSphere(0.0f, true, XMFLOAT3ToVector3(&(m_Level.getGoalPosition())), 200.0f);

	m_Player.initialize(m_Physics, m_Level.getStartPosition(), XMFLOAT3(0.f, 0.f, 1.f));
	
	m_ChangeScene = GoToScene::NONE;

	//TODO: Remove later when we actually have a level to load.
	loadSandbox();
}

void GameLogic::shutdown(void)
{
	m_Level.releaseLevel();
	m_Physics->releaseAllBoundingVolumes();
}

std::vector<Actor::ptr> &GameLogic::getObjects()
{
	return m_Objects;
}

GameLogic::GoToScene GameLogic::getChangeScene(void) const
{
	return m_ChangeScene;
}

void GameLogic::onFrame(float p_DeltaTime)
{
	m_Player.update(p_DeltaTime);

	if(m_Physics->getHitDataSize() > 0)
	{
		for(int i = m_Physics->getHitDataSize() - 1; i >= 0; i--)
		{
			HitData hit = m_Physics->getHitDataAt(i);
			if(hit.intersect)
			{
				if(m_EdgeCollResponse.checkCollision(hit, m_Physics->getBodyPosition(hit.collisionVictim),
					m_Physics->getBodySize(hit.collisionVictim).y ,&m_Player))
				{
					m_Physics->removeHitDataAt(i);
				}
				if(m_FinishLine == hit.collisionVictim)
				{
					m_Player.setPosition(m_Level.getStartPosition());
					m_ChangeScene = GoToScene::POSTGAME;
					m_Physics->removeHitDataAt(i);
				}

				Logger::log(Logger::Level::TRACE, "Collision reported");
			}
		}
	}

	if (m_PlayerDirection.x != 0.f || m_PlayerDirection.y != 0.f)
	{
		float dir = atan2f(m_PlayerDirection.y, m_PlayerDirection.x) + m_PlayerViewRotation.x;

		m_Player.setDirectionX(sinf(dir));
		m_Player.setDirectionZ(cosf(dir));
	}
	if(!m_Player.getForceMove())		
		m_Physics->update(p_DeltaTime);

	Actor::ptr strongSkyBox = skyBox.lock();
	if (strongSkyBox)
	{
		strongSkyBox->setPosition(getPlayerEyePosition());
	}

	Vector3 actualViewRot = getPlayerViewRotation();
	lookDir.x = -sinf(actualViewRot.x) * cosf(actualViewRot.y);
	lookDir.y = sinf(actualViewRot.y);
	lookDir.z = -cosf(actualViewRot.x) * cosf(actualViewRot.y);
	
	for (auto& actor : m_Objects)
	{
		actor->onUpdate(p_DeltaTime);
	}

	updateSandbox(p_DeltaTime);
}

void GameLogic::setPlayerActor(std::weak_ptr<Actor> p_Actor)
{
	m_PlayerActor = p_Actor;
}
Vector3 GameLogic::getPlayerViewRotation()
{
	return m_PlayerViewRotation;
}

void GameLogic::setPlayerDirection(Vector2 p_Direction)
{
	m_PlayerDirection = p_Direction;
}

Vector2 GameLogic::getPlayerDirection() const
{
	return m_PlayerDirection;
}

Vector3 GameLogic::getPlayerEyePosition() const
{
	Vector3 tempPos;

	Actor::ptr strongPlayerActor = m_PlayerActor.lock();
	if (strongPlayerActor)
	{
		tempPos = strongPlayerActor->getPosition();
	}
	else
	{
		tempPos = m_Player.getEyePosition();
	}

	return tempPos;
}

Vector3 GameLogic::getPlayerViewRotation() const
{
	Vector3 actualViewRot = m_PlayerViewRotation;

	Actor::ptr strongPlayerActor = m_PlayerActor.lock();
	if (strongPlayerActor)
	{
		Vector3 rot = strongPlayerActor->getRotation();
		actualViewRot = actualViewRot + rot;
	}

	return actualViewRot;
}

void GameLogic::movePlayerView(float p_Yaw, float p_Pitch)
{
	m_PlayerViewRotation.x += p_Yaw;
	m_PlayerViewRotation.y += p_Pitch;

	if (m_PlayerViewRotation.x > PI)
	{
		m_PlayerViewRotation.x -= 2 * PI;
	}
	else if (m_PlayerViewRotation.x < -PI)
	{
		m_PlayerViewRotation.x += 2 * PI;
	}

	if (m_PlayerViewRotation.y > PI * 0.45f)
	{
		m_PlayerViewRotation.y = PI * 0.45f;
	}
	else if (m_PlayerViewRotation.y < -PI * 0.45f)
	{
		m_PlayerViewRotation.y = -PI * 0.45f;
	}
}

void GameLogic::playerJump()
{
	m_Player.setJump();
}

void GameLogic::toggleIK()
{
	useIK = !useIK;
}



void GameLogic::testBlendAnimation()
{
	playAnimation(wavingWitch.lock(), "Bomb");
	playAnimation(ikTest.lock(), "Spin");
	playAnimation(testWitch.lock(), "Idle");
}

void GameLogic::testResetAnimation()
{
	playAnimation(wavingWitch.lock(), "Kick");
	playAnimation(ikTest.lock(), "Wave");
	playAnimation(testWitch.lock(), "Run");
}

void GameLogic::testLayerAnimation()
{
	playAnimation(ikTest.lock(), "Wave");
	playAnimation(wavingWitch.lock(), "Bomb");
	playAnimation(testWitch.lock(), "Wave");
}

void GameLogic::testResetLayerAnimation()
{
	playAnimation(wavingWitch.lock(), "Kick");
	playAnimation(ikTest.lock(), "Wave");
	playAnimation(testWitch.lock(), "Run");
	playAnimation(testWitch.lock(), "DefLayer1");
}

void GameLogic::loadSandbox()
{
	useIK = false;

	Logger::log(Logger::Level::DEBUG_L, "Adding debug box model instances");

	for (int i = 0; i < NUM_BOXES; i++)
	{
		const float scale = 100.f + i * 300.f / NUM_BOXES;
		addRotatingBox(Vector3((float)(i / 4) * 400.f, 100.f, (float)(i % 4) * 400.f + 4000.f), Vector3(scale, scale, scale));
		rotBoxes[i] = m_Objects.back();
	}

	addBoxWithAABB(Vector3(0.f, -250.f, 0.f), Vector3(5000.f, 250.f, 5000.f));

	Logger::log(Logger::Level::DEBUG_L, "Adding debug animated Witch");
	testWitch = addBasicModel("WITCH", Vector3(1600.0f, 0.0f, 500.0f));
	playAnimation(testWitch.lock(), "Run");

	addClimbBox();
	skyBox = addSkybox(Vector3(100.f, 100.f, 100.f));

	circleWitch = addBasicModel("WITCH", Vector3(0.f, 0.f, 0.f));
	playAnimation(circleWitch.lock(), "Run");
	standingWitch = addBasicModel("DZALA", Vector3(1600.f, 0.f, -500.f));
	playAnimation(standingWitch.lock(), "Bomb");
	wavingWitch = addBasicModel("DZALA", Vector3(1500.f, 0.f, -500.f));
	playAnimation(wavingWitch.lock(), "Kick");

	ikTest = addIK_Worm();
	playAnimation(ikTest.lock(), "Wave");

	static const unsigned int numTowerBoxes = 5;
	Vector3 towerBoxSizes[numTowerBoxes] =
	{
		Vector3(2000.f, 160.f, 2000.f),
		Vector3(1200.f, 160.f, 1200.f),
		Vector3(10.f, 800.f, 10.f),
		Vector3(40.f, 40.f, 40.f),
	};

	Vector3 towerBoxPositions[numTowerBoxes] =
	{
		Vector3(3000.f, 80.f, 4000.f),
		Vector3(3000.f, 240.f, 4000.f),
		Vector3(3000.f, 1120.f, 4000.f),
		Vector3(3000.f, 1540.f, 4000.f),
	};

	for (unsigned int i = 0; i < numTowerBoxes; i++)
	{
		addBoxWithAABB(towerBoxPositions[i], towerBoxSizes[i] * 0.5f);
	}

	addClimbTowerBox(Vector3(3000.f, 520.f, 4000.f), Vector3(300.f, 200.f, 300.f));
	
	static const unsigned int numRotatedTowerBoxes = 5;
	Vector3 rotatedTowerBoxSizes[numRotatedTowerBoxes] =
	{
		Vector3(2000.f, 160.f, 2000.f),
		Vector3(1200.f, 160.f, 1200.f),
		Vector3(600.f, 400.f, 600.f),
		Vector3(10.f, 800.f, 10.f),
		Vector3(40.f, 40.f, 40.f),
	};

	Vector3 rotatedTowerBoxPositions[numRotatedTowerBoxes] =
	{
		Vector3(-3000.f, 80.f, 4000.f),
		Vector3(-3000.f, 240.f, 4000.f),
		Vector3(-3000.f, 520.f, 4000.f),
		Vector3(-3000.f, 1120.f, 4000.f),
		Vector3(-3000.f, 1540.f, 4000.f),
	};

	for (unsigned int i = 0; i < numRotatedTowerBoxes; i++)
	{
		addBoxWithOBB(rotatedTowerBoxPositions[i], rotatedTowerBoxSizes[i] * 0.5f, Vector3(1.f, 0.f, 0.f));
	}

	static const Vector3 slantedPlanePosition(-4000.f, 300.f, 2000.f);
	static const Vector3 slantedPlaneSize(2000.f, 500.f, 3000.f);
	static const Vector3 slantedPlaneRotation(0.3f, 0.2f, -0.3f);
	addBoxWithOBB(slantedPlanePosition, slantedPlaneSize * 0.5f, slantedPlaneRotation);

	witchCircleAngle = 0.0f;

	addLights();
}

void GameLogic::updateSandbox(float p_DeltaTime)
{
	static const Vector3 circleCenter(400.f, 0.f, 1500.f);
	static const float circleRadius = 800.f;
	static const float witchAngleSpeed = 0.3f;

	witchCircleAngle += witchAngleSpeed * p_DeltaTime;
	Vector3 witchCirclePosition(circleCenter);
	witchCirclePosition.x -= cosf(witchCircleAngle) * circleRadius;
	witchCirclePosition.z += sinf(witchCircleAngle) * circleRadius;
	Actor::ptr strongWitch = circleWitch.lock();
	if (strongWitch)
	{
		strongWitch->setPosition(witchCirclePosition);
		strongWitch->setRotation(Vector3(witchCircleAngle, 0.f, 0.f));
	}

	static const Vector3 blockRotationSpeed(0.1f, 0.05f, 0.03f);
	rotBlockRotation = rotBlockRotation + blockRotationSpeed * p_DeltaTime;
	
	for (size_t i = 0; i < NUM_BOXES; ++i)
	{
		auto& box = rotBoxes[i];

		Actor::ptr strongBox = box.lock();
		if (strongBox)
		{
			strongBox->setRotation(rotBlockRotation * (float)i);
		}
	}

	updateIK();
}

void GameLogic::playAnimation(Actor::ptr p_Actor, std::string p_AnimationName)
{
	if (!p_Actor)
	{
		return;
	}

	std::shared_ptr<ModelComponent> comp = p_Actor->getComponent<ModelComponent>(ModelInterface::m_ComponentId).lock();
	if (comp)
	{
		m_EventManager->queueEvent(IEventData::Ptr(new PlayAnimationEventData(comp->getId(), p_AnimationName)));
	}
}

void GameLogic::updateIK()
{
	static const char* testTargetJoint = "L_Hand";
	static const char* testHingeJoint = "L_LowerArm";
	static const char* testBaseJoint = "L_UpperArm";

	Vector3 IK_Target = Vector3(m_Player.getEyePosition()) + lookDir * 200.f;

	if (useIK)
	{
		std::shared_ptr<Actor> strIKTest = ikTest.lock();
		if (strIKTest)
		{
			std::shared_ptr<ModelComponent> comp = strIKTest->getComponent<ModelComponent>(ModelComponent::m_ComponentId).lock();
			if (comp)
			{
				m_EventManager->queueEvent(IEventData::Ptr(new AddReachIK_EventData(comp->getId(), "joint1", "joint2", "joint3", IK_Target)));
			}
		}
		std::shared_ptr<Actor> strWitch = circleWitch.lock();
		if (strWitch)
		{
			std::shared_ptr<ModelComponent> comp = strWitch->getComponent<ModelComponent>(ModelComponent::m_ComponentId).lock();
			if (comp)
			{
				m_EventManager->queueEvent(IEventData::Ptr(new AddReachIK_EventData(comp->getId(), testBaseJoint, testHingeJoint, testTargetJoint, IK_Target)));
			}
		}
	}
	else
	{
		std::shared_ptr<Actor> strIKTest = ikTest.lock();
		if (strIKTest)
		{
			std::shared_ptr<ModelComponent> comp = strIKTest->getComponent<ModelComponent>(ModelComponent::m_ComponentId).lock();
			if (comp)
			{
				m_EventManager->queueEvent(IEventData::Ptr(new RemoveReachIK_EventData(comp->getId(), "joint3")));
			}
		}
		std::shared_ptr<Actor> strWitch = circleWitch.lock();
		if (strWitch)
		{
			std::shared_ptr<ModelComponent> comp = strWitch->getComponent<ModelComponent>(ModelComponent::m_ComponentId).lock();
			if (comp)
			{
				m_EventManager->queueEvent(IEventData::Ptr(new RemoveReachIK_EventData(comp->getId(), testTargetJoint)));
			}
		}
	}
}

IPhysics *GameLogic::getPhysics() const
{
	return m_Physics;
}

void pushVector(tinyxml2::XMLPrinter& p_Printer, const std::string& p_ElementName, Vector3 p_Vec)
{
	p_Printer.OpenElement(p_ElementName.c_str());
	p_Printer.PushAttribute("x", p_Vec.x);
	p_Printer.PushAttribute("y", p_Vec.y);
	p_Printer.PushAttribute("z", p_Vec.z);
	p_Printer.CloseElement();
}

std::weak_ptr<Actor> GameLogic::addRotatingBox(Vector3 p_Position, Vector3 p_Scale)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", "BOX");
	pushVector(printer, "Scale", p_Scale);
	printer.CloseElement();
	printer.CloseElement();

	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr());

	Actor::ptr actor = m_ActorFactory->createActor(doc.FirstChildElement("Object"));
	actor->setPosition(p_Position);

	m_Objects.push_back(actor);

	return actor;
}

std::weak_ptr<Actor> GameLogic::addSkybox(Vector3 p_Scale)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", "SKYBOX");
	pushVector(printer, "Scale", p_Scale);
	printer.CloseElement();
	printer.CloseElement();

	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr());

	Actor::ptr actor = m_ActorFactory->createActor(doc.FirstChildElement("Object"));
	m_Objects.push_back(actor);

	return actor;
}

std::weak_ptr<Actor> GameLogic::addBasicModel(const std::string& p_Model, Vector3 p_Position)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", p_Model.c_str());
	printer.CloseElement();
	printer.CloseElement();

	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr());

	Actor::ptr actor = m_ActorFactory->createActor(doc.FirstChildElement("Object"));
	actor->setPosition(p_Position);
	m_Objects.push_back(actor);

	return actor;
}

std::weak_ptr<Actor> GameLogic::addIK_Worm()
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", "IKTest");
	printer.CloseElement();
	printer.CloseElement();

	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr());

	Actor::ptr actor = m_ActorFactory->createActor(doc.FirstChildElement("Object"));
	actor->setPosition(Vector3(800.f, 100.f, 200.f));

	m_Objects.push_back(actor);

	return actor;
}

std::weak_ptr<Actor> GameLogic::addBoxWithAABB(Vector3 p_Position, Vector3 p_Halfsize)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", "BOX");
	pushVector(printer, "Scale", p_Halfsize * 2.f);
	printer.CloseElement();
	printer.OpenElement("OBBPhysics");
	pushVector(printer, "Halfsize", p_Halfsize);
	pushVector(printer, "Position", p_Position);
	printer.CloseElement();
	printer.CloseElement();

	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr());

	Actor::ptr actor = m_ActorFactory->createActor(doc.FirstChildElement("Object"));
	actor->setPosition(p_Position);
	m_Objects.push_back(actor);

	return actor;
}

std::weak_ptr<Actor> GameLogic::addBoxWithOBB(Vector3 p_Position, Vector3 p_Halfsize, Vector3 p_Rotation)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", "BOX");
	pushVector(printer, "Scale", p_Halfsize * 2.f);
	printer.CloseElement();
	printer.OpenElement("OBBPhysics");
	pushVector(printer, "Halfsize", p_Halfsize);
	pushVector(printer, "Position", p_Position);
	printer.CloseElement();
	printer.CloseElement();

	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr());

	Actor::ptr actor = m_ActorFactory->createActor(doc.FirstChildElement("Object"));
	actor->setPosition(p_Position);
	actor->setRotation(p_Rotation);
	m_Objects.push_back(actor);

	return actor;
}

void addEdge(tinyxml2::XMLPrinter& p_Printer, Vector3 p_Position, Vector3 p_Halfsize)
{
	p_Printer.OpenElement("AABBPhysics");
	p_Printer.PushAttribute("Edge", true);
	pushVector(p_Printer, "Halfsize", p_Halfsize);
	pushVector(p_Printer, "RelativePosition", p_Position);
	p_Printer.CloseElement();
}

std::weak_ptr<Actor> GameLogic::addClimbBox()
{
	static const Vector3 climbTestPos(0.f, 200.f, 3000.f);
	static const Vector3 climbTestHalfSize(100.f, 100.f, 100.f);

	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", "BOX");
	pushVector(printer, "Scale", climbTestHalfSize * 2.f);
	printer.CloseElement();
	addEdge(printer, Vector3(0.f, 0.f, 0.f), climbTestHalfSize);
	printer.CloseElement();

	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr());

	Actor::ptr actor = m_ActorFactory->createActor(doc.FirstChildElement("Object"));
	actor->setPosition(climbTestPos);
	m_Objects.push_back(actor);

	return actor;
}

std::weak_ptr<Actor> GameLogic::addClimbTowerBox(Vector3 p_Position, Vector3 p_Halfsize)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", "BOX");
	pushVector(printer, "Scale", p_Halfsize * 2.f);
	printer.CloseElement();
	printer.OpenElement("AABBPhysics");
	pushVector(printer, "Halfsize", p_Halfsize);
	pushVector(printer, "Position", p_Position);
	printer.CloseElement();
	addEdge(printer, Vector3(0.f, p_Halfsize.y - 50.f, p_Halfsize.z), Vector3(p_Halfsize.x * 0.9f, 50.f, 10.f));
	addEdge(printer, Vector3(0.f, p_Halfsize.y - 50.f, -p_Halfsize.z), Vector3(p_Halfsize.x * 0.9f, 50.f, 10.f));
	addEdge(printer, Vector3(p_Halfsize.x, p_Halfsize.y - 50.f, 0.f), Vector3(10.f, 50.f, p_Halfsize.z * 0.9f));
	addEdge(printer, Vector3(-p_Halfsize.x, p_Halfsize.y - 50.f, 0.f), Vector3(10.f, 50.f, p_Halfsize.z * 0.9f));
	printer.CloseElement();
	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr());

	Actor::ptr actor = m_ActorFactory->createActor(doc.FirstChildElement("Object"));
	actor->setPosition(p_Position);
	m_Objects.push_back(actor);

	return actor;
}

void GameLogic::addLights()
{
	Light directional = Light::createDirectionalLight(Vector3(0.f, -1.f, 0.f), Vector3(1.0f, 1.0f, 1.0f));
	Light spot = Light::createSpotLight(Vector3(-1000.f,500.f,0.f), Vector3(0,0,-1),
		Vector2(cosf(3.14f/12),cosf(3.14f/4)), 2000.f, Vector3(0.f,1.f,0.f));

	m_EventManager->queueEvent(IEventData::Ptr(new LightEventData(directional)));
	m_EventManager->queueEvent(IEventData::Ptr(new LightEventData(spot)));

	Light point = Light::createPointLight(Vector3(0.f,0.f,0.f), 2000.f, Vector3(1.f,1.f,1.f));
	m_EventManager->queueEvent(IEventData::Ptr(new LightEventData(point)));
	point = Light::createPointLight(Vector3(0.f, 3000.f, 3000.f), 2000000.f, Vector3(0.5f, 0.5f, 0.5f));
	m_EventManager->queueEvent(IEventData::Ptr(new LightEventData(point)));
	Light::createPointLight(Vector3(0.f, 0.f, 3000.f), 2000000.f, Vector3(0.5f, 0.5f, 0.5f));
	m_EventManager->queueEvent(IEventData::Ptr(new LightEventData(point)));
}
