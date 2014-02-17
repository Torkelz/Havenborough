#include "GameLogic.h"
#include "Components.h"
#include "EventData.h"
#include "ClientExceptions.h"
#include "HumanAnimationComponent.h"
#include "Logger.h"

#include <sstream>

using namespace DirectX;

GameLogic::GameLogic(void)
{
	m_Physics = nullptr;
	m_ResourceManager = nullptr;
	m_CurrentCheckPointPosition = Vector3(0.0f, 0.0f, 0.0f);
}


GameLogic::~GameLogic(void)
{
	m_Physics = nullptr;
	m_ResourceManager = nullptr;
}

void GameLogic::initialize(ResourceManager *p_ResourceManager, IPhysics *p_Physics, ActorFactory *p_ActorFactory,
	EventManager *p_EventManager, INetwork *p_Network)
{
	m_Physics = p_Physics;
	m_ResourceManager = p_ResourceManager;
	m_ActorFactory = p_ActorFactory;
	m_Network = p_Network;
	m_EventManager = p_EventManager;

	m_EventManager->addListener(EventListenerDelegate(this, &GameLogic::removeActorByEvent), RemoveActorEventData::sk_EventType);
	
	m_Actors.reset(new ActorList);
	m_ActorFactory->setActorList(m_Actors);

	m_ChangeScene = GoToScene::NONE;

	m_IsConnecting = false;
	m_Connected = false;
	m_InGame = false;
	m_PlayingLocal = true;
}

void GameLogic::shutdown(void)
{
	m_Level.releaseLevel();
	m_Physics->releaseAllBoundingVolumes();
}

ActorList::ptr GameLogic::getObjects()
{
	return m_Actors;
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
			if(m_EdgeCollResponse.checkCollision(hit, m_Physics->getBodyPosition(hit.collisionVictim),
				m_Physics->getBodyOrientation(hit.collisionVictim), &m_Player))
			{
				//m_Physics->removeHitDataAt(i);
			}

			Logger::log(Logger::Level::TRACE, "Collision reported");
		}
	}

	if (m_PlayerDirection.x != 0.f || m_PlayerDirection.y != 0.f)
	{
		XMVECTOR forward = XMLoadFloat3(&XMFLOAT3(getPlayerViewForward()));
		forward = XMVectorSetY(forward, 0.f);
		forward = XMVector3Normalize(forward);
		XMVECTOR right = XMLoadFloat3(&XMFLOAT3(getPlayerViewRight()));
		right = XMVectorSetY(right, 0.f);
		right = XMVector3Normalize(right);

		XMVECTOR rotDirV = forward * m_PlayerDirection.x + right * m_PlayerDirection.y;

		m_Player.setDirectionX(XMVectorGetX(rotDirV));
		m_Player.setDirectionZ(XMVectorGetZ(rotDirV));
	}
	if(!m_Player.getForceMove())
		m_Physics->update(p_DeltaTime, 50);

	Vector3 actualViewRot = getPlayerViewRotation();
	Actor::ptr playerActor = m_Player.getActor().lock();
	if (playerActor)
	{
		Vector3 playerRotation = actualViewRot;
		playerRotation.y = 0.f;
		playerActor->setRotation(playerRotation);
	}

	IConnectionController *conn = m_Network->getConnectionToServer();
	if (m_InGame && !m_PlayingLocal && conn && conn->isConnected())
	{
		PlayerControlData data;
		data.m_Rotation = getPlayerRotation();
		data.m_Position = m_Player.getPosition();
		data.m_Velocity = m_Player.getVelocity();
		data.m_Forward = getPlayerViewForward();
		data.m_Up = getPlayerViewUp();

		conn->sendPlayerControl(data);
	}
	
	m_Actors->onUpdate(p_DeltaTime);

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
	Actor::ptr player = m_Player.getActor().lock();
	if (!player)
	{
		return Vector3(0.f, 0.f, 0.f);
	}

	using namespace DirectX;

	Vector3 forward = getPlayerViewForward();
	Vector3 up = getPlayerViewUp();
	XMVECTOR rightV = XMVector3Cross(XMLoadFloat3(&XMFLOAT3(up)), XMLoadFloat3(&XMFLOAT3(forward)));
	Vector3 right;
	XMStoreFloat3((XMFLOAT3*)&right, rightV);
	Vector3 rotation;
	if (forward.y == 1.f)
	{
		rotation.x = atan2f(right.x, right.z);
		rotation.y = -PI;
		rotation.z = 0.f;
	}
	else if (forward.y == -1.f)
	{
		rotation.x = atan2f(right.x, right.z);
		rotation.y = PI;
		rotation.z = 0.f;
	}
	else
	{
		rotation.x = atan2f(forward.x, forward.z);
		rotation.y = asinf(-forward.y);
		rotation.z = atan2f(-right.y, up.y);
	}

	return rotation;
}

Vector3 GameLogic::getPlayerViewForward() const
{
	Actor::ptr actor = m_Player.getActor().lock();
	if (!actor)
	{
		return Vector3(0.f, 0.f, 1.f);
	}

	std::shared_ptr<LookInterface> look = actor->getComponent<LookInterface>(LookInterface::m_ComponentId).lock();
	if (!look)
	{
		return Vector3(0.f, 0.f, 1.f);
	}
	return look->getLookForward();
}

Vector3 GameLogic::getPlayerViewUp() const
{
	Actor::ptr actor = m_Player.getActor().lock();
	if (!actor)
	{
		return Vector3(0.f, 1.f, 0.f);
	}

	std::shared_ptr<LookInterface> look = actor->getComponent<LookInterface>(LookInterface::m_ComponentId).lock();
	if (!look)
	{
		return Vector3(0.f, 1.f, 0.f);
	}

	return look->getLookUp();
}

Vector3 GameLogic::getPlayerViewRight() const
{
	Actor::ptr actor = m_Player.getActor().lock();
	if (!actor)
	{
		return Vector3(1.f, 0.f, 0.f);
	}

	std::shared_ptr<LookInterface> look = actor->getComponent<LookInterface>(LookInterface::m_ComponentId).lock();
	if (!look)
	{
		return Vector3(1.f, 0.f, 0.f);
	}

	return look->getLookRight();
}

Vector3 GameLogic::getPlayerRotation() const
{
	Actor::ptr actor = m_Player.getActor().lock();
	if (!actor)
	{
		return Vector3(0.f, 0.f, 0.f);
	}

	return actor->getRotation();
}

DirectX::XMFLOAT4X4 GameLogic::getPlayerViewRotationMatrix() const
{
	Actor::ptr actor = m_Player.getActor().lock();
	if (!actor)
	{
		return XMFLOAT4X4();
	}
	
	std::shared_ptr<LookInterface> look = actor->getComponent<LookInterface>(LookInterface::m_ComponentId).lock();
	if (!look)
	{
		return XMFLOAT4X4();
	}

	return look->getRotationMatrix();
}

void GameLogic::movePlayerView(float p_Yaw, float p_Pitch)
{
	/*if(m_Player.getForceMove())
		return;*/

	Actor::ptr actor = m_Player.getActor().lock();
	if (!actor)
	{
		return;
	}

	std::shared_ptr<LookInterface> look = actor->getComponent<LookInterface>(LookInterface::m_ComponentId).lock();
	if (!look)
	{
		return;
	}

	XMFLOAT3 forward = look->getLookForward();
	XMFLOAT3 up = look->getLookUp();
	XMVECTOR vForward = XMVector3Normalize(XMLoadFloat3(&forward));
	XMVECTOR vUp = XMVector3Normalize(XMLoadFloat3(&up));
	XMVECTOR vRight = XMVector3Cross(vUp, vForward);

	XMVECTOR rotationYaw = XMQuaternionRotationRollPitchYaw(0.f, p_Yaw, 0.f);
	XMVECTOR rotationPitch = XMQuaternionRotationAxis(vRight, p_Pitch);
	XMVECTOR rotation = XMQuaternionMultiply(rotationPitch, rotationYaw);

	XMStoreFloat3(&forward, XMVector3Rotate(vForward, rotation));
	XMStoreFloat3(&up, XMVector3Rotate(vUp, rotation));

	if (forward.y > 0.9f || forward.y < -0.9f)
	{
		return;
	}

	look->setLookForward(forward);
	look->setLookUp(up);
}

Vector3 GameLogic::getCurrentCheckpointPosition(void) const
{
	return m_CurrentCheckPointPosition;
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
	//playAnimation(testWitch.lock(), "Idle", false);
}

void GameLogic::testResetAnimation()
{
	//playAnimation(testWitch.lock(), "Run", false);
}

void GameLogic::testLayerAnimation()
{
	//playAnimation(testWitch.lock(), "Wave", false);
	//playAnimation(m_Player.getActor().lock(), "LookAround", false);
}

void GameLogic::testResetLayerAnimation()
{
	//playAnimation(testWitch.lock(), "Run", false);
	//playAnimation(testWitch.lock(), "DefLayer1", false);
	//playAnimation(m_Player.getActor().lock(), "Idle2", false);
	changeAnimationWeight(m_Player.getActor().lock(), 4, 0.0f);
}

void GameLogic::playLocalLevel()
{
	m_Actors.reset();
	m_Actors.reset(new ActorList());
	m_ActorFactory->setActorList(m_Actors);

	m_Level = Level(m_ResourceManager, m_Physics, m_ActorFactory);
#ifdef _DEBUG
	std::ifstream input("../Bin/assets/levels/Level2.btxl", std::istream::in | std::istream::binary);
	if(!input)
	{
		throw InvalidArgument("File could not be found: LoadLevel", __LINE__, __FILE__);
	}
	m_Level.loadLevel(input, m_Actors);
	m_Level.setStartPosition(XMFLOAT3(0.f, 1000.0f, 1500.f)); //TODO: Remove this line when level gets the position from file
	m_Level.setGoalPosition(XMFLOAT3(4850.0f, 0.0f, -2528.0f)); //TODO: Remove this line when level gets the position from file
#else
	std::ifstream input("../Bin/assets/levels/Level1.2.1.btxl", std::istream::in | std::istream::binary);
	if(!input)
	{
		throw InvalidArgument("File could not be found: LoadLevel", __LINE__, __FILE__);
	}
	m_Level.loadLevel(input, m_Actors);
	m_Level.setStartPosition(XMFLOAT3(0.0f, 2000.0f, 1500.0f)); //TODO: Remove this line when level gets the position from file
	m_Level.setGoalPosition(XMFLOAT3(4850.0f, 0.0f, -2528.0f)); //TODO: Remove this line when level gets the position from file
#endif

	std::weak_ptr<Actor> playerActor = addActor(m_ActorFactory->createPlayerActor(m_Level.getStartPosition()));
	m_Player = Player();
	m_Player.initialize(m_Physics, playerActor);

	m_InGame = true;
	m_PlayingLocal = true;

	//TODO: Remove later when we actually have a level to load.
	loadSandbox();

	m_EventManager->queueEvent(IEventData::Ptr(new GameStartedEventData));
}

void GameLogic::connectToServer(const std::string& p_URL, unsigned short p_Port)
{
	if (!m_IsConnecting && !m_Connected)
	{
		m_IsConnecting = true;
		m_Network->connectToServer(p_URL.c_str(), p_Port, &connectedCallback, this);
	}
}

void GameLogic::leaveGame()
{
	if (m_InGame)
	{
		m_Level = Level();
		m_Actors.reset();
		m_Actors.reset(new ActorList);
		m_ActorFactory->setActorList(m_Actors);

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

void GameLogic::throwSpell(const char *p_SpellId)
{
	Actor::ptr playerActor = m_Player.getActor().lock();
	if (playerActor)
	{
		if(!m_Player.getForceMove())
		{
			m_Actors->addActor(m_ActorFactory->createSpell(p_SpellId, playerActor->getId(), getPlayerViewForward(), m_Player.getRightHandPosition()));
			playAnimation(playerActor, "CastSpell", false);

			IConnectionController *conn = m_Network->getConnectionToServer();
			if (m_InGame && !m_PlayingLocal && conn && conn->isConnected())
			{
				conn->sendThrowSpell(p_SpellId, m_Player.getRightHandPosition(), getPlayerViewForward());
			}
		}
		
	}
}

void GameLogic::setPlayerClimb(bool p_State)
{
	m_Player.setClimbing(p_State);
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
						m_Actors->addActor(actor);
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
						m_Level.loadLevel(stream, m_Actors);
					}
					else
					{
#ifdef _DEBUG
						std::string levelFileName("../Bin/assets/levels/Level2.btxl");
#else
						std::string levelFileName("../Bin/assets/levels/Level1.2.1.btxl");
#endif
						std::ifstream file(levelFileName, std::istream::binary);
						m_Level.loadLevel(file, m_Actors);
					}
					m_Level.setStartPosition(XMFLOAT3(0.f, 1000.0f, 1500.f)); //TODO: Remove this line when level gets the position from file
					m_Level.setGoalPosition(XMFLOAT3(4850.0f, 0.f, -2528.0f)); //TODO: Remove this line when level gets the position from file
				}
				break;
			case PackageType::RESULT_GAME:
				{
					int numberOfData = conn->getNumGameResultData(package);
					for(int i = 0; i < numberOfData; i++)
					{
						const char* result = conn->getGameResultData(package, i);
						tinyxml2::XMLDocument reader;
						reader.Parse(result);
						tinyxml2::XMLElement* object = reader.FirstChildElement("GameResult");
						if(object->Attribute("Type", "Result"))
						{
								m_Level = Level();
								m_Actors.reset();
								m_Actors.reset(new ActorList);
								m_ActorFactory->setActorList(m_Actors);

								m_InGame = false;

								IConnectionController* con = m_Network->getConnectionToServer();

								if (!m_PlayingLocal && con && con->isConnected())
								{
									con->sendLeaveGame();
								}
								
								object = object->FirstChildElement("ResultList");
								if(!object)
								Logger::log(Logger::Level::ERROR_L, "Could not find Object (ResultList)");
								int size = 0;
								object->QueryAttribute("VectorSize", &size);
								if(size == 0)
								{
									m_EventManager->queueEvent(IEventData::Ptr(new GameLeftEventData(false)));
								}
								else
								{
									std::vector<int> GoalList;
									int position;
									for(int i = 0; i < size; i++)
									{
										object->QueryAttribute("Place", &position);
										GoalList.push_back(position);
									}
									m_EventManager->queueEvent(IEventData::Ptr(new GameLeftEventData(false))); //DO SOMETHING HERE!!
								}
						}
						else if(object->Attribute("Type", "Position"))
						{
							//int b = 0; //DO SOMETHING HERE!!
						}
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
						const uint32_t actorId = data.m_Id;

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
						
						std::weak_ptr<MovementInterface> wMove = actor->getComponent<MovementInterface>(MovementInterface::m_ComponentId);
						std::shared_ptr<MovementInterface> shMove = wMove.lock();
						if (shMove)
						{
							shMove->setVelocity(data.m_Velocity);
							shMove->setRotationalVelocity(data.m_RotationVelocity);
						}

						std::shared_ptr<PhysicsInterface> physComp = actor->getComponent<PhysicsInterface>(PhysicsInterface::m_ComponentId).lock();
						if (physComp)
						{
							m_Physics->setBodyVelocity(physComp->getBodyHandle(), data.m_Velocity);
						}
					}

					unsigned int numberOfExtraData = conn->getNumUpdateObjectExtraData(package);
					for(unsigned int i = 0; i < numberOfExtraData; i++)
					{
						const char* updates = conn->getUpdateObjectExtraData(package, i);

						tinyxml2::XMLDocument reader;
						reader.Parse(updates);
						tinyxml2::XMLElement* object = reader.FirstChildElement("ObjectUpdate");
						Actor::Id actorId = -1;
						object->QueryAttribute("ActorId", &actorId);

						Actor::ptr actor = getActor(actorId);
						if (!actor)
						{
							Logger::log(Logger::Level::ERROR_L, "Could not find actor (" + std::to_string(actorId) + ")");
							continue;
						}

						if(object->Attribute("Type", "Color"))
						{
							object = object->FirstChildElement("SetColor");
							if(!object)
								Logger::log(Logger::Level::ERROR_L, "Could not find Object (" + std::to_string(actorId) + ")");
							Vector3 color;
							object->QueryAttribute("r", &color.x);
							object->QueryAttribute("g", &color.y);
							object->QueryAttribute("b", &color.z);
							actor->getComponent<ModelInterface>(ModelInterface::m_ComponentId).lock()->setColorTone(color);
							m_CurrentCheckPointPosition = actor->getPosition();
						}
						else if (object->Attribute("Type", "Look"))
						{
							if (actor == m_Player.getActor().lock())
							{
								continue;
							}

							const tinyxml2::XMLElement* look = object->FirstChildElement("Look");
							if (look)
							{
								std::shared_ptr<LookInterface> lookInt = actor->getComponent<LookInterface>(LookInterface::m_ComponentId).lock();
								if (lookInt)
								{
									Vector3 forward(0.f, 0.f, 1.f);
									Vector3 up(0.f, 1.f, 0.f);
									queryVector(look->FirstChildElement("Forward"), forward);
									queryVector(look->FirstChildElement("Up"), up);

									lookInt->setLookForward(forward);
									lookInt->setLookUp(up);
								}
							}
						}
					}
							
				}
				break;

			case PackageType::REMOVE_OBJECTS:
				{
					const unsigned int numObjects = conn->getNumRemoveObjectRefs(package);
					const uint32_t* removeObjects = conn->getRemoveObjectRefs(package);
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
						m_Player.initialize(m_Physics, actor);
					}

					conn->sendDoneLoading();
					m_InGame = true;
					m_PlayingLocal = false;

					m_EventManager->queueEvent(IEventData::Ptr(new GameStartedEventData));
				}
				break;

			case PackageType::SET_SPAWN:
				{
					m_Player.setSpawnPosition(conn->getSetSpawnPositionData(package));
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
	GameLogic* self = static_cast<GameLogic*>(p_UserData);
	self->m_IsConnecting = false;

	if (p_Res == Result::SUCCESS)
	{
		self->m_Connected = true;

		Logger::log(Logger::Level::INFO, "Connected successfully");
	}
	else
	{
		Logger::log(Logger::Level::WARNING, "Connection failed");
	}
}

Actor::ptr GameLogic::getActor(Actor::Id p_Actor)
{
	if (m_Actors)
	{
		return m_Actors->findActor(p_Actor);
	}
	else
	{
		return Actor::ptr();
	}
}

void GameLogic::removeActor(Actor::Id p_Actor)
{
	m_Actors->removeActor(p_Actor);
}

void GameLogic::removeActorByEvent(IEventData::Ptr p_Data)
{
	std::shared_ptr<RemoveActorEventData> data = std::static_pointer_cast<RemoveActorEventData>(p_Data);

	removeActor(data->getActorId());
}

void GameLogic::loadSandbox()
{
	useIK = false;

	Logger::log(Logger::Level::DEBUG_L, "Adding debug animated Witch");
	testWitch = addActor(m_ActorFactory->createPlayerActor(Vector3(1600.0f, 0.0f, 500.0f)));
	playAnimation(testWitch.lock(), "Run", false);

	circleWitch = addActor(m_ActorFactory->createPlayerActor(Vector3(0.f, 0.f, 0.f)));
	playAnimation(circleWitch.lock(), "Run", false);

	witchCircleAngle = 0.0f;

	//Event to create a particle effect on local test rounds

	addActor(m_ActorFactory->createParticles(Vector3(0.f, 80.f, 0.f), "TestParticle"));
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

	if (m_InGame)
	{
		updateIK();
	}
}

void GameLogic::playAnimation(Actor::ptr p_Actor, std::string p_AnimationName, bool p_Override)
{
	if (!p_Actor)
	{
		return;
	}

	std::shared_ptr<AnimationInterface> comp = p_Actor->getComponent<AnimationInterface>(AnimationInterface::m_ComponentId).lock();
	if (comp)
	{
		comp->playAnimation(p_AnimationName, p_Override);
	}
}

void GameLogic::queueAnimation(Actor::ptr p_Actor, std::string p_AnimationName)
{
	if (!p_Actor)
	{
		return;
	}

	std::shared_ptr<AnimationInterface> comp = p_Actor->getComponent<AnimationInterface>(AnimationInterface::m_ComponentId).lock();
	if (comp)
	{
		comp->queueAnimation(p_AnimationName);
	}
}

void GameLogic::changeAnimationWeight(Actor::ptr p_Actor, int p_Track, float p_Weight)
{
	if (!p_Actor)
	{
		return;
	}

	std::shared_ptr<AnimationInterface> comp = p_Actor->getComponent<AnimationInterface>(AnimationInterface::m_ComponentId).lock();
	if (comp)
	{
		comp->changeAnimationWeight(p_Track, p_Weight);
	}
}

void GameLogic::updateIK()
{
	Vector3 IK_Target = Vector3(m_Player.getEyePosition()) + getPlayerViewForward() * 200.f;

	if (useIK)
	{
		std::shared_ptr<Actor> strWitch = circleWitch.lock();
		if (strWitch)
		{
			std::shared_ptr<AnimationInterface> comp = strWitch->getComponent<AnimationInterface>(AnimationInterface::m_ComponentId).lock();
			if (comp)
			{
				comp->applyIK_ReachPoint("LeftArm", IK_Target);
			}
		}

		// Player
		strWitch = m_Player.getActor().lock();
		if (strWitch)
		{
			std::shared_ptr<AnimationInterface> comp = strWitch->getComponent<AnimationInterface>(AnimationInterface::m_ComponentId).lock();
			if (comp)
			{
				comp->applyIK_ReachPoint("LeftArm", IK_Target);
			}
		}
	}

	m_Player.updateIKJoints();
}

IPhysics *GameLogic::getPhysics() const
{
	return m_Physics;
}

std::weak_ptr<Actor> GameLogic::addActor(Actor::ptr p_Actor)
{
	m_Actors->addActor(p_Actor);
	return p_Actor;
}
