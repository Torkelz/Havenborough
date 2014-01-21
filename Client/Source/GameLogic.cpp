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

void GameLogic::initialize(ResourceManager *p_ResourceManager, IPhysics *p_Physics, ActorFactory *p_ActorFactory, EventManager *p_EventManager, INetwork *p_Network)
{
	m_Physics = p_Physics;
	m_ResourceManager = p_ResourceManager;
	m_ActorFactory = p_ActorFactory;
	m_Network = p_Network;
	m_EventManager = p_EventManager;
	m_Level = Level(m_ResourceManager, m_Physics, m_ActorFactory);
#ifdef _DEBUG
	m_Level.loadLevel("../Bin/assets/levels/Level2.btxl", "../Bin/assets/levels/Level2.btxl", m_Objects);
	m_Level.setStartPosition(XMFLOAT3(-1000.f, 2400.0f, 4000.f)); //TODO: Remove this line when level gets the position from file
	m_Level.setGoalPosition(XMFLOAT3(4850.0f, 679.0f, -2528.0f)); //TODO: Remove this line when level gets the position from file
#else
	m_Level.loadLevel("../Bin/assets/levels/Level1.2.btxl", "../Bin/assets/levels/Level1.2.btxl", m_Objects);
	m_Level.setStartPosition(XMFLOAT3(0.0f, 2400.0f, 1500.0f)); //TODO: Remove this line when level gets the position from file
	m_Level.setGoalPosition(XMFLOAT3(4850.0f, 679.0f, -2528.0f)); //TODO: Remove this line when level gets the position from file
#endif
	//m_Physics->createSphere(0.0f, true, XMFLOAT3ToVector3(&(m_Level.getGoalPosition())), 200.0f);
	m_FinishLine = addCollisionSphere(m_Level.getGoalPosition(), 200.f);

	m_Player.initialize(m_Physics, m_Level.getStartPosition(), XMFLOAT3(0.f, 0.f, 1.f));
	
	m_ChangeScene = GoToScene::NONE;

	m_Connected = false;
	m_Network->connectToServer("localhost", 31415, &connectedCallback, this); //Note: IP to server if running: 194.47.150.5


	addBoxWithOBB(Vector3(0.f, 100.0f, 4000.0f), Vector3(200.0f, 100.0f, 200.0f), Vector3(0.0f, 0.0f, 0.0f));
	addBoxWithOBB(Vector3(-1000.0f, 100.0f, 4000.0f), Vector3(200.0f, 100.0f, 200.0f), Vector3(1.0f, 0.0f, 0.0f));
	addBoxWithOBB(Vector3(1000.0f, 100.0f, 4000.0f), Vector3(200.0f, 100.0f, 200.0f), Vector3(1.0f, 0.0f, 0.0f));

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
	handleNetwork();

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
				if(m_FinishLine.lock()->getBodyHandles()[0] == hit.collisionVictim)
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
	{
		m_Physics->update(p_DeltaTime*0.5f);
		m_Physics->update(p_DeltaTime*0.5f);
	}

	Actor::ptr strongSkyBox = skyBox.lock();
	if (strongSkyBox)
	{
		strongSkyBox->setPosition(getPlayerEyePosition());
	}

	Vector3 actualViewRot = getPlayerViewRotation();
	lookDir.x = -sinf(actualViewRot.x) * cosf(actualViewRot.y);
	lookDir.y = sinf(actualViewRot.y);
	lookDir.z = -cosf(actualViewRot.x) * cosf(actualViewRot.y);

	IConnectionController *conn = m_Network->getConnectionToServer();
	if (conn)
	{
		PlayerControlData data;
		data.m_Rotation[0] = actualViewRot.x;
		data.m_Rotation[1] = actualViewRot.y;
		data.m_Rotation[2] = actualViewRot.z;
		Vector3 playerVel = m_Player.getVelocity();

		playerVel = m_Player.getPosition();
		data.m_Velocity[0] = playerVel.x;
		data.m_Velocity[1] = playerVel.y;
		data.m_Velocity[2] = playerVel.z;
		//data.m_Velocity[0] = playerVel.x;
		//data.m_Velocity[1] = playerVel.y;
		//data.m_Velocity[2] = playerVel.z;

		conn->sendPlayerControl(data);
	}
	
	for (auto& actor : m_Objects)
	{
		actor->onUpdate(p_DeltaTime);
	}

	updateSandbox(p_DeltaTime);
}

void GameLogic::setPlayerActor(std::weak_ptr<Actor> p_Actor)
{
	m_Player.setActor(p_Actor);
}

void GameLogic::setPlayerDirection(Vector2 p_Direction)
{
	m_PlayerDirection = p_Direction;
}

Vector2 GameLogic::getPlayerDirection() const
{
	return m_PlayerDirection;
}

BodyHandle GameLogic::getPlayerBodyHandle() const
{
	return m_Player.getBody();
}

Vector3 GameLogic::getPlayerEyePosition() const
{
	Vector3 tempPos;

	//Actor::ptr strongPlayerActor = m_Player.getActor().lock();
	//if (strongPlayerActor)
	//{
	//	tempPos = strongPlayerActor->getPosition();
	//}
	//else
	{
		tempPos = m_Player.getEyePosition();
	}

	return tempPos;
}

Vector3 GameLogic::getPlayerViewRotation() const
{
	return m_PlayerViewRotation;
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

	m_Physics->setBodyRotation(m_Player.getBody(), Vector3(m_PlayerViewRotation.x , 0.f, 0.f));
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

void GameLogic::handleNetwork()
{
	if (m_Connected)
	{
		IConnectionController* conn = m_Network->getConnectionToServer();
		unsigned int numPackages = conn->getNumPackages();
		for (unsigned int i = 0; i < numPackages; i++)
		{
			Package package = conn->getPackage(i);
			PackageType type = conn->getPackageType(package);

			std::string msg("Received package of type: " + std::to_string((uint16_t)type));
			Logger::log(Logger::Level::TRACE, msg);

			switch (type)
			{
			case PackageType::CREATE_OBJECTS:
				{
					unsigned int numInstances = conn->getNumCreateObjectInstances(package);
					const ObjectInstance* instances = conn->getCreateObjectInstances(package);
					for (unsigned int i = 0; i < numInstances; ++i)
					{
						using tinyxml2::XMLAttribute;
						using tinyxml2::XMLDocument;
						using tinyxml2::XMLElement;

						const ObjectInstance& data = instances[i];
						std::ostringstream msg;
						msg << "Adding object at (" 
							<< data.m_Position[0] << ", "
							<< data.m_Position[1] << ", " 
							<< data.m_Position[2] << ")";
						Logger::log(Logger::Level::INFO, msg.str());

						XMLDocument description;
						description.Parse(conn->getCreateObjectDescription(package, data.m_DescriptionIdx));

						const XMLElement* obj = description.FirstChildElement("Object");

						Actor::ptr actor = m_ActorFactory->createActor(obj, data.m_Id);
						actor->setPosition(Vector3(data.m_Position[0], data.m_Position[1], data.m_Position[2]));
						actor->setRotation(Vector3(data.m_Rotation[0], data.m_Rotation[1], data.m_Rotation[2]));
						m_Objects.push_back(actor);
					}
				}
				break;

			case PackageType::UPDATE_OBJECTS:
				{
					const unsigned int numUpdates = conn->getNumUpdateObjectData(package);
					const UpdateObjectData* const updates = conn->getUpdateObjectData(package);
					for (unsigned int i = 0; i < numUpdates; ++i)
					{
						const UpdateObjectData& data = updates[i];
						const uint16_t actorId = data.m_Id;

						Actor::ptr actor = getActor(actorId);
						if (!actor)
						{
							Logger::log(Logger::Level::ERROR_L, "Could not find actor (" + std::to_string(actorId) + ")");
							continue;
						}

						//if (actor == m_Player.getActor().lock())
						//{
						//	continue;
						//}

						actor->setPosition(Vector3(data.m_Position[0], data.m_Position[1], data.m_Position[2]));
						actor->setRotation(Vector3(data.m_Rotation[0], data.m_Rotation[1], data.m_Rotation[2]));
						
						std::weak_ptr<MovementInterface> wMove = actor->getComponent<MovementInterface>(3);
						std::shared_ptr<MovementInterface> shMove = wMove.lock();
						if (shMove)
						{
							shMove->setVelocity(Vector3(data.m_Velocity[0], data.m_Velocity[1], data.m_Velocity[2]));
							shMove->setRotationalVelocity(Vector3(data.m_RotationVelocity[0], data.m_RotationVelocity[1], data.m_RotationVelocity[2]));
						}
					}

					// TODO: Handle extra data
				}
				break;

			case PackageType::REMOVE_OBJECTS:
				{
					const unsigned int numObjects = conn->getNumRemoveObjectRefs(package);
					const uint16_t* removeObjects = conn->getRemoveObjectRefs(package);
					for (unsigned int i = 0; i < numObjects; ++i)
					{
						removeActor(removeObjects[i]);
					}
				}
				break;

			case PackageType::OBJECT_ACTION:
				{
					const Actor::Id actorId = conn->getObjectActionId(package);
					const char* xmlAction = conn->getObjectActionAction(package);
					tinyxml2::XMLDocument actionDoc;
					actionDoc.Parse(xmlAction);
					const tinyxml2::XMLElement* root = actionDoc.FirstChildElement("Action");
					const tinyxml2::XMLElement* action = root->FirstChildElement();

					if (std::string(action->Value()) == "Pulse")
					{
						Actor::ptr actor = getActor(actorId);

						if (actor->getId())
						{
							std::shared_ptr<PulseInterface> pulseComp = actor->getComponent<PulseInterface>(PulseComponent::m_ComponentId).lock();
							if (pulseComp)
							{
								pulseComp->pulseOnce();
							}
							break;
						}
					}
				}
				break;

			case PackageType::ASSIGN_PLAYER:
				{
					const Actor::Id actorId = conn->getAssignPlayerObject(package);
					Actor::ptr actor = getActor(actorId);
					if (actor)
					{
						m_Player.setActor(actor);
					}
				}
				break;

			default:
				std::string msg("Received unhandled package of type " + std::to_string((uint16_t)type));
				Logger::log(Logger::Level::WARNING, msg);
				break;
			}
		}

		conn->clearPackages(numPackages);
	}
}

void GameLogic::connectedCallback(Result p_Res, void* p_UserData)
{
	if (p_Res == Result::SUCCESS)
	{
		((GameLogic*)p_UserData)->m_Connected = true;
		Logger::log(Logger::Level::INFO, "Connected successfully");
	}
	else
	{
		Logger::log(Logger::Level::WARNING, "Connection failed");
	}
}

Actor::ptr GameLogic::getActor(Actor::Id p_Actor)
{
	for (auto actor : m_Objects)
	{
		if (actor->getId() == p_Actor)
		{
			return actor;
		}
	}

	return Actor::ptr();
}

void GameLogic::removeActor(Actor::Id p_Actor)
{
	for (size_t i = 0; i < m_Objects.size(); ++i)
	{
		if (m_Objects[i]->getId() == p_Actor)
		{
			std::swap(m_Objects[i], m_Objects.back());
			m_Objects.pop_back();
			return;
		}
	}
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

	//addBoxWithAABB(Vector3(0.f, -250.f, 0.f), Vector3(5000.f, 250.f, 5000.f));

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
	printer.OpenElement("AABBPhysics");
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

std::weak_ptr<Actor> GameLogic::addCollisionSphere(Vector3 p_Position, float p_Radius)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("SpherePhysics");
	printer.PushAttribute("Radius", p_Radius);
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

std::weak_ptr<Actor> GameLogic::addPlayerActor(Vector3 p_Position)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Movement");
	pushVector(printer, "Velocity", Vector3(0.f, 0.f, 0.f));
	pushVector(printer, "RotationalVelocity", Vector3(0.f, 0.f, 0.f));
	printer.CloseElement();
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", "BOX");
	static const Vector3 scale(50.f, 50.f, 50.f);
	pushVector(printer, "Scale", scale);
	printer.CloseElement();
	printer.OpenElement("SpherePhysics");
	printer.PushAttribute("Immovable", true);
	printer.PushAttribute("Radius", 50.f);
	printer.CloseElement();
	printer.OpenElement("Pulse");
	printer.PushAttribute("Length", 0.5f);
	printer.PushAttribute("Strength", 0.5f);
	printer.CloseElement();
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
