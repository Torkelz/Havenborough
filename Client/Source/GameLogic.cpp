#include "GameLogic.h"
#include "Components.h"
#include "EventData.h"
#include "ClientExceptions.h"

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
	
	m_ChangeScene = GoToScene::NONE;

	m_Connected = false;
	m_InGame = false;
	m_PlayingLocal = true;
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

	if (!m_InGame)
	{
		return;
	}

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
				if(!m_Level.reachedFinishLine() && m_Level.getCurrentCheckpointBodyHandle() == hit.collisionVictim)
				{
					m_Level.changeCheckpoint(m_Objects);
					if(m_Level.reachedFinishLine())
					{
						m_Level = Level();
						m_Objects.clear();

						m_InGame = false;

						IConnectionController* con = m_Network->getConnectionToServer();

						if (!m_PlayingLocal && con && con->isConnected())
						{
							con->sendLeaveGame();
						}

						m_EventManager->queueEvent(IEventData::Ptr(new GameLeftEventData(false)));
					}
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


	Vector3 actualViewRot = getPlayerViewRotation();
	lookDir.x = -sinf(actualViewRot.x) * cosf(actualViewRot.y);
	lookDir.y = sinf(actualViewRot.y);
	lookDir.z = -cosf(actualViewRot.x) * cosf(actualViewRot.y);

	IConnectionController *conn = m_Network->getConnectionToServer();
	if (m_InGame && !m_PlayingLocal && conn && conn->isConnected())
	{
		PlayerControlData data;
		data.m_Rotation = actualViewRot;
		data.m_Position = m_Player.getPosition();
		data.m_Velocity = m_Player.getVelocity();
		data.m_Rotation.x += 3.1415f;
		data.m_Rotation.y = 0.f;

		conn->sendPlayerControl(data);
	}
	
	for (auto& actor : m_Objects)
	{
		actor->onUpdate(p_DeltaTime);
	}

	updateSandbox(p_DeltaTime);
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
	return m_Player.getEyePosition();
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

	Vector3 playerRotation = Vector3(m_PlayerViewRotation.x + PI, 0.f, 0.f);
	Actor::ptr actor = m_Player.getActor().lock();
	if (actor)
	{
		actor->setRotation(playerRotation);
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

void GameLogic::playLocalLevel()
{
	m_Objects.clear();

	m_Level = Level(m_ResourceManager, m_Physics, m_ActorFactory);
#ifdef _DEBUG
	std::ifstream input("../Bin/assets/levels/Level2.btxl", std::istream::in | std::istream::binary);
	if(!input)
	{
		throw InvalidArgument("File could not be found: LoadLevel", __LINE__, __FILE__);
	}
	m_Level.loadLevel(input, m_Objects);
	m_Level.setStartPosition(XMFLOAT3(0.f, 1000.0f, 1500.f)); //TODO: Remove this line when level gets the position from file
	m_Level.setGoalPosition(XMFLOAT3(4850.0f, 0.0f, -2528.0f)); //TODO: Remove this line when level gets the position from file
#else
	std::ifstream input("../Bin/assets/levels/Level1.2.2.btxl", std::istream::in | std::istream::binary);
	if(!input)
	{
		throw InvalidArgument("File could not be found: LoadLevel", __LINE__, __FILE__);
	}
	m_Level.loadLevel(input, m_Objects);
	m_Level.setStartPosition(XMFLOAT3(0.0f, 2000.0f, 1500.0f)); //TODO: Remove this line when level gets the position from file
	m_Level.setGoalPosition(XMFLOAT3(4850.0f, 0.0f, -2528.0f)); //TODO: Remove this line when level gets the position from file
#endif

	std::weak_ptr<Actor> playerActor = addActor(m_ActorFactory->createPlayerActor(m_Level.getStartPosition()));
	m_Player = Player();
	m_Player.initialize(m_Physics, XMFLOAT3(0.f, 0.f, 1.f), playerActor);

	m_InGame = true;
	m_PlayingLocal = true;

	//TODO: Remove later when we actually have a level to load.
	loadSandbox();

	m_EventManager->queueEvent(IEventData::Ptr(new GameStartedEventData));
}

void GameLogic::connectToServer(const std::string& p_URL, unsigned short p_Port)
{
	m_Network->connectToServer(p_URL.c_str(), p_Port, &connectedCallback, this);
}

void GameLogic::leaveGame()
{
	if (m_InGame)
	{
		m_Level = Level();
		m_Objects.clear();

		m_InGame = false;

		IConnectionController* con = m_Network->getConnectionToServer();

		if (!m_PlayingLocal && con && con->isConnected())
		{
			con->sendLeaveGame();
		}
		
		m_EventManager->queueEvent(IEventData::Ptr(new GameLeftEventData(true)));
	}
}

void GameLogic::joinGame(const std::string& p_LevelName)
{
	IConnectionController* con = m_Network->getConnectionToServer();
	if (!m_InGame && con && con->isConnected())
	{
		con->sendJoinGame(p_LevelName.c_str());
	}
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
					unsigned int numInstances = conn->getNumCreateObjects(package);
					for (unsigned int i = 0; i < numInstances; ++i)
					{
						using tinyxml2::XMLAttribute;
						using tinyxml2::XMLDocument;
						using tinyxml2::XMLElement;

						const ObjectInstance data = conn->getCreateObjectDescription(package, i);

						XMLDocument description;
						description.Parse(data.m_Description);

						const XMLElement* obj = description.FirstChildElement("Object");

						Actor::ptr actor = m_ActorFactory->createActor(obj, data.m_Id);
						m_Objects.push_back(actor);
					}
				}
				break;

			case PackageType::LEVEL_DATA:
				{
					m_Level = Level(m_ResourceManager, m_Physics, m_ActorFactory);
					size_t size = conn->getLevelDataSize(package);
					if (size > 0)
					{
						std::string buffer(conn->getLevelData(package),size);
						std::istringstream stream(buffer);
						m_Level.loadLevel(stream, m_Objects);
					}
					else
					{
#ifdef _DEBUG
						std::string levelFileName("../Bin/assets/levels/Level2.btxl");
#else
						std::string levelFileName("../Bin/assets/levels/Level1.2.2.btxl");
#endif
						std::ifstream file(levelFileName, std::istream::binary);
						m_Level.loadLevel(file, m_Objects);
					}
					m_Level.setStartPosition(XMFLOAT3(0.f, 1000.0f, 1500.f)); //TODO: Remove this line when level gets the position from file
					m_Level.setGoalPosition(XMFLOAT3(4850.0f, 0.f, -2528.0f)); //TODO: Remove this line when level gets the position from file
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

						if (actor == m_Player.getActor().lock())
						{
							continue;
						}

						actor->setPosition(data.m_Position);
						actor->setRotation(data.m_Rotation);
						
						std::weak_ptr<MovementInterface> wMove = actor->getComponent<MovementInterface>(3);
						std::shared_ptr<MovementInterface> shMove = wMove.lock();
						if (shMove)
						{
							shMove->setVelocity(data.m_Velocity);
							shMove->setRotationalVelocity(data.m_RotationVelocity);
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
						m_Player = Player();
						m_Player.initialize(m_Physics, XMFLOAT3(0.f, 0.f, 1.f), actor);
					}

					conn->sendDoneLoading();
					m_InGame = true;
					m_PlayingLocal = false;

					m_EventManager->queueEvent(IEventData::Ptr(new GameStartedEventData));
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
		GameLogic* self = static_cast<GameLogic*>(p_UserData);

		self->m_Connected = true;
		//self->m_Network->getConnectionToServer()->sendJoinGame("test");

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
		rotBoxes[i] = addActor(m_ActorFactory->createRotatingBox(Vector3((float)(i / 4) * 400.f, 100.f, (float)(i % 4) * 400.f + 4000.f),
			Vector3(scale, scale, scale)));
	}

	//addBoxWithAABB(Vector3(0.f, -250.f, 0.f), Vector3(5000.f, 250.f, 5000.f));

	Logger::log(Logger::Level::DEBUG_L, "Adding debug animated Witch");
	addActor(m_ActorFactory->createBasicModel("WITCH", Vector3(1600.0f, 0.0f, 500.0f)));
	playAnimation(testWitch.lock(), "Run");

	m_Objects.push_back(m_ActorFactory->createClimbBox());

	circleWitch = addActor(m_ActorFactory->createBasicModel("WITCH", Vector3(0.f, 0.f, 0.f)));
	playAnimation(circleWitch.lock(), "Run");
	standingWitch = addActor(m_ActorFactory->createBasicModel("DZALA", Vector3(1600.f, 0.f, -500.f)));
	playAnimation(standingWitch.lock(), "Bomb");
	wavingWitch = addActor(m_ActorFactory->createBasicModel("DZALA", Vector3(1500.f, 0.f, -500.f)));
	playAnimation(wavingWitch.lock(), "Kick");
	
	//m_CheckpointSystem = CheckpointSystem();
	//m_CheckpointSystem.addCheckpoint(addActor(m_ActorFactory->createCheckPointActor(m_Level.getGoalPosition(), Vector3(1.0f, 10.0f, 1.0f))));
	//m_CheckpointSystem.addCheckpoint(addActor(m_ActorFactory->createCheckPointActor(Vector3(-1000.0f, 0.0f, -1000.0f), Vector3(1.0f, 10.0f, 1.0f))));
	//m_CheckpointSystem.addCheckpoint(addActor(m_ActorFactory->createCheckPointActor(Vector3(-1000.0f, 0.0f, 1000.0f), Vector3(1.0f, 10.0f, 1.0f))));
	//m_CheckpointSystem.addCheckpoint(addActor(m_ActorFactory->createCheckPointActor(Vector3(1000.0f, 0.0f, 1000.0f), Vector3(1.0f, 10.0f, 1.0f))));
	//m_CheckpointSystem.addCheckpoint(addActor(m_ActorFactory->createCheckPointActor(Vector3(1000.0f, 0.0f, -1000.0f), Vector3(1.0f, 10.0f, 1.0f))));

	ikTest = addActor(m_ActorFactory->createIK_Worm());
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
		addActor(m_ActorFactory->createBoxWithAABB(towerBoxPositions[i], towerBoxSizes[i] * 0.5f));
	}

	addActor(m_ActorFactory->createClimbTowerBox(Vector3(3000.f, 520.f, 4000.f), Vector3(300.f, 200.f, 300.f)));
	
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
		addActor(m_ActorFactory->createBoxWithOBB(rotatedTowerBoxPositions[i], rotatedTowerBoxSizes[i] * 0.5f, Vector3(1.f, 0.f, 0.f)));
	}

	//static const Vector3 slantedPlanePosition(-4000.f, 300.f, 2000.f);
	//static const Vector3 slantedPlaneSize(2000.f, 500.f, 3000.f);
	//static const Vector3 slantedPlaneRotation(0.3f, 0.2f, -0.3f);
	//addBoxWithOBB(slantedPlanePosition, slantedPlaneSize * 0.5f, slantedPlaneRotation);
	
	addActor(m_ActorFactory->createBoxWithOBB(Vector3(0.f, 100.0f, 4000.0f), Vector3(200.0f, 100.0f, 200.0f), Vector3(0.0f, 0.0f, 0.0f)));
	addActor(m_ActorFactory->createBoxWithOBB(Vector3(-1000.0f, 100.0f, 4000.0f), Vector3(200.0f, 100.0f, 200.0f), Vector3(1.0f, 0.0f, 0.0f)));
	addActor(m_ActorFactory->createBoxWithOBB(Vector3(1000.0f, 100.0f, 4000.0f), Vector3(200.0f, 100.0f, 200.0f), Vector3(1.0f, 0.0f, 0.0f)));
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

	if (m_InGame)
	{
		updateIK();
	}
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

std::weak_ptr<Actor> GameLogic::addActor(Actor::ptr p_Actor)
{
	m_Objects.push_back(p_Actor);
	return p_Actor;
}

void GameLogic::addLights()
{
	addActor(m_ActorFactory->createDirectionalLight(Vector3(0.f, -1.f, 0.f), Vector3(1.0f, 1.0f, 1.0f)));
	addActor(m_ActorFactory->createSpotLight(Vector3(-1000.f,500.f,0.f), Vector3(0,0,-1),
		Vector2(cosf(3.14f/12),cosf(3.14f/4)), 2000.f, Vector3(0.f,1.f,0.f)));
	addActor(m_ActorFactory->createPointLight(Vector3(0.f,0.f,0.f), 2000.f, Vector3(1.f,1.f,1.f)));
	addActor(m_ActorFactory->createPointLight(Vector3(0.f, 3000.f, 3000.f), 2000000.f, Vector3(0.5f, 0.5f, 0.5f)));
	addActor(m_ActorFactory->createPointLight(Vector3(0.f, 0.f, 3000.f), 2000000.f, Vector3(0.5f, 0.5f, 0.5f)));
}
