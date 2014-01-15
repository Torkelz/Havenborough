#include "GameLogic.h"

GameLogic::GameLogic(void)
{
	//m_Graphics = nullptr;
	m_Physics = nullptr;
	//m_InputQueue = nullptr;
	m_ResourceManager = nullptr;
}


GameLogic::~GameLogic(void)
{
	//m_Graphics = nullptr;
	m_Physics = nullptr;
	//m_InputQueue = nullptr;
	m_ResourceManager = nullptr;
}

void GameLogic::initialize(ResourceManager *p_ResourceManager, IPhysics *p_Physics, ActorFactory *p_ActorFactory)
{
	m_Physics = p_Physics;
	m_ResourceManager = p_ResourceManager;
	m_ActorFactory = p_ActorFactory;

	m_Level = Level(m_ResourceManager, m_Physics, m_ActorFactory);
	m_Level.loadLevel("../Bin/assets/levels/Level3.btxl", "../Bin/assets/levels/CB_Level3.btxl", m_Objects);
	m_Level.setStartPosition(XMFLOAT3(0.0f, 200.0f, 1500.0f)); //TODO: Remove this line when level gets the position from file
	m_Level.setGoalPosition(XMFLOAT3(0.0f, 0.0f, 0.0f)); //TODO: Remove this line when level gets the position from file
	m_FinishLine = m_Physics->createSphere(0.0f, true, XMFLOAT3ToVector3(&(m_Level.getGoalPosition())), 200.0f);

	m_Player.initialize(m_Physics, m_Level.getStartPosition(), XMFLOAT3(0.f, 0.f, 1.f));

	m_Ground = m_Physics->createAABB(50.f, true, Vector3(0.f, 0.f, 0.f), Vector3(5000.f, 0.f, 5000.f), false);
	
	m_ChangeScene = GoToScene::NONE;

	//TODO: Remove later when we actually have a level to load.
	loadSandbox();
}

void GameLogic::shutdown(void)
{
	m_Level.releaseLevel();
	m_Physics->releaseAllBoundingVolumes();
	
	//TODO: Remove when we have a real level.
	shutdownSandbox();
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
		float dir = atan2f(m_PlayerDirection.y, m_PlayerDirection.x) + viewRot[0];

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

//void GameLogic::render()
//{
//	m_Graphics->useFrameDirectionalLight(Vector3(1.f,1.f,1.f),Vector3(0.1f,-0.99f,0.f));
//	//m_Graphics->drawFrame(currView);
//
//	//addDebugBVToDraw(1);
//	/*addDebugBVToDraw(5);
//	addDebugBVToDraw(6);
//	addDebugBVToDraw(7);
//	addDebugBVToDraw(8);
//	addDebugBVToDraw(9);
//	addDebugBVToDraw(10);
//	addDebugBVToDraw(11);
//	addDebugBVToDraw(12);
//	addDebugBVToDraw(13);
//	addDebugBVToDraw(14);
//	addDebugBVToDraw(15);
//	addDebugBVToDraw(16);*/
//
//	for(int i = 0; i < 35; i++)
//	{
//		addDebugBVToDraw(i);
//	}
//
//	//m_Graphics->drawFrame(currView);
//
//	renderSandbox();
//	m_Graphics->drawFrame(currentDebugView);
//}

void GameLogic::setPlayerActor(std::weak_ptr<Actor> p_Actor)
{
	m_PlayerActor = p_Actor;
}

Vector3 GameLogic::getPlayerViewRotation()
{
	return Vector3(viewRot[0], viewRot[1], 0.f);
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
	Vector3 actualViewRot(viewRot[0], viewRot[1], 0.f);

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
	viewRot[0] += p_Yaw;
	viewRot[1] += p_Pitch;

	if (viewRot[0] > PI)
	{
		viewRot[0] -= 2 * PI;
	}
	else if (viewRot[0] < -PI)
	{
		viewRot[0] += 2 * PI;
	}

	if (viewRot[1] > PI * 0.45f)
	{
		viewRot[1] = PI * 0.45f;
	}
	else if (viewRot[1] < -PI * 0.45f)
	{
		viewRot[1] = -PI * 0.45f;
	}
}

void GameLogic::playerJump()
{
	m_Player.setJump();
}

void GameLogic::toggleIK()
{
	useIK_OnIK_Worm = !useIK_OnIK_Worm;
}

void GameLogic::loadSandbox()
{
	useIK_OnIK_Worm = false;

	Logger::log(Logger::Level::DEBUG_L, "Adding debug box model instances");

	for (int i = 0; i < NUM_BOXES; i++)
	{
		const float scale = 100.f + i * 300.f / NUM_BOXES;
		addRotatingBox(Vector3((float)(i / 4) * 400.f, 100.f, (float)(i % 4) * 400.f + 4000.f), Vector3(scale, scale, scale));
		rotBoxes[i] = m_Objects.back();
	}

	static const Vector3 climbTestPos(0.f, 200.f, 3000.f);
	static const Vector3 climbTestHalfSize(100.f, 100.f, 100.f);

	//BodyHandle boxTest = m_Physics->createAABB(50.f, true, climbTestPos, climbTestHalfSize, true );

	//climbBox = m_Graphics->createModelInstance("BOX");

	//m_Graphics->setModelPosition(climbBox, climbTestPos);
	//m_Graphics->setModelScale(climbBox, Vector3(200.f, 200.f, 200.f));


	//jointBox = m_Graphics->createModelInstance("BOX");
	//m_Graphics->setModelScale(jointBox, Vector3(5.f, 5.f, 200.f));

	skyBox = addSkybox(Vector3(100.f, 100.f, 100.f));
	addRotatingBox(Vector3(0.f, -250.f, 0.f), Vector3(10000.f, 500.f, 10000.f));

	circleWitch = addDzala(Vector3(0.f, 0.f, 0.f));
	addDzala(Vector3(1600.f, 0.f, -500.f));
	addDzala(Vector3(1500.f, 0.f, -500.f));

	addIK_Worm();

	Vector3 towerBoxSizes[numTowerBoxes] =
	{
		Vector3(2000.f, 160.f, 2000.f),
		Vector3(1200.f, 160.f, 1200.f),
		Vector3(600.f, 400.f, 600.f),
		Vector3(10.f, 800.f, 10.f),
		Vector3(40.f, 40.f, 40.f),
	};

	Vector3 towerBoxPositions[numTowerBoxes] =
	{
		Vector3(3000.f, 80.f, 4000.f),
		Vector3(3000.f, 240.f, 4000.f),
		Vector3(3000.f, 520.f, 4000.f),
		Vector3(3000.f, 1120.f, 4000.f),
		Vector3(3000.f, 1540.f, 4000.f),
	};

	for (unsigned int i = 0; i < numTowerBoxes; i++)
	{
		addBoxWithAABB(towerBoxPositions[i], towerBoxSizes[i] * 0.5f);
	}

	//m_Physics->createAABB(0.f, true, Vector3(3000.f, 680.f, 3700.f), Vector3(280.f, 60.f, 20.f), true);
	//m_Physics->createAABB(0.f, true, Vector3(3000.f, 680.f, 4300.f), Vector3(280.f, 60.f, 20.f), true);
	//m_Physics->createAABB(0.f, true, Vector3(2700.f, 680.f, 4000.f), Vector3(20.f, 60.f, 280.f), true);
	//m_Physics->createAABB(0.f, true, Vector3(3300.f, 680.f, 4000.f), Vector3(20.f, 60.f, 280.f), true);

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

	//static const Vector3 slantedPlanePosition(-4000.f, 300.f, 2000.f);
	//static const Vector3 slantedPlaneSize(2000.f, 500.f, 3000.f);
	//static const Vector3 slantedPlaneRotation(0.3f, 0.2f, -0.3f);
	//slantedPlane = m_Graphics->createModelInstance("BOX");
	//m_Graphics->setModelPosition(slantedPlane, slantedPlanePosition);
	//m_Graphics->setModelScale(slantedPlane, slantedPlaneSize);
	//m_Graphics->setModelRotation(slantedPlane, slantedPlaneRotation);

	//BodyHandle slantedPlaneBody = m_Physics->createOBB(0.f, true, slantedPlanePosition, slantedPlaneSize * 0.5f, false);
	//m_Physics->setBodyRotation(slantedPlaneBody, slantedPlaneRotation);

	/*hej = m_Graphics->createModelInstance("BOX");
	m_Graphics->setModelPosition(hej,  Vector3(14.f, 4.5f, -10.f));
	m_Graphics->setModelScale(hej,  Vector3(5.f, 0.5f, 7.f));
	m_Graphics->setModelRotation(hej, Vector3(0.f, 0.f, 3.14f/6.5f));*/


	//OBBhouse1 = m_Physics->createOBB(1.f, true, Vector3(), Vector3(500.f, 50.f, 350.f), false);
	//m_Physics->setBodyRotation(OBBhouse1, Vector3(0.f, 0.f, 3.14f/6.5f));
	//m_Physics->setBodyPosition(OBBhouse1, Vector3(1400.f, 450.f, -1000.f));
	////m_Physics->setBodyPosition(OBBhouse1, Vector3(0.f, 2.5f, 0.f));

	//OBBhouse2 = m_Physics->createOBB(1.f, true, Vector3(), Vector3(500.f, 50.f, 350.f), false);
	//m_Physics->setBodyRotation(OBBhouse2, Vector3(0.f, 0.f, 3.14f/6.5f));
	//m_Physics->setBodyPosition(OBBhouse2, Vector3(350.f, 500.0f, -1000.f));

	viewRot[0] = 0.f;
	viewRot[1] = 0.f;

	witchCircleAngle = 0.0f;
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

	//float witchWaveAngle = 0.f;
	//static const float witchWavingAngleSpeed = 1.f;
	//static const float waveRadius = 0.5f;
	//Vector3 wavePos = m_Graphics->getJointPosition(wavingWitch, "Head");
	//wavePos.x -= 1.f;
	//witchWaveAngle += witchWavingAngleSpeed * p_DeltaTime;
	//wavePos.y += sinf(witchWaveAngle) * waveRadius;
	//wavePos.z += cosf(witchWaveAngle) * waveRadius;

	//m_Graphics->applyIK_ReachPoint(wavingWitch, "bn_l_wrist01", "bn_l_elbow_b01", "bn_l_arm01", wavePos);

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

	//static const float IK_Length = 500.f;

	//static const char* testTargetJoint = "bn_l_foot01";
	//static const char* testHingeJoint = "bn_l_Knee_a01";
	//static const char* testBaseJoint = "bn_l_Tigh01";

	//Vector4 tempPos = m_Physics->getBodyPosition(m_Player.getBody());
	//Vector3 IK_Target(tempPos.x + lookDir.x * IK_Length, tempPos.y + lookDir.y * IK_Length, tempPos.z + lookDir.z * IK_Length);
	//if (useIK_OnIK_Worm)
	//{
	//	//m_Graphics->applyIK_ReachPoint(circleWitch, testTargetJoint, testHingeJoint, testBaseJoint, IK_Target);
	//	m_Graphics->applyIK_ReachPoint(ikTest, "joint4", "joint3", "joint2", IK_Target);
	//}

	//Vector3 jointPos = m_Graphics->getJointPosition(circleWitch, testTargetJoint);
	//m_Graphics->setModelPosition(jointBox, jointPos);
}

void GameLogic::renderSandbox()
{
	//m_Graphics->useFramePointLight(Vector3(0.f,0.f,0.f),Vector3(1.f,1.f,1.f),2000.f);
	//m_Graphics->useFrameSpotLight(Vector3(-1000.f,500.f,0.f),Vector3(0.f,1.f,0.f),
	//	Vector3(0,0,-1),Vector2(cosf(3.14f/12),cosf(3.14f/4)), 2000.f );
	//m_Graphics->useFramePointLight(Vector3(0.f, 3000.f, 3000.f), Vector3(0.5f, 0.5f, 0.5f), 2000000.f);
	//m_Graphics->useFramePointLight(Vector3(0.f, 0.f, 3000.f), Vector3(0.5f, 0.5f, 0.5f), 2000000.f);
}

void GameLogic::shutdownSandbox()
{
}

void GameLogic::addDebugBVToDraw(BodyHandle p_BodyHandle)
{
	//unsigned int size =  m_Physics->getNrOfTrianglesFromBody(p_BodyHandle);

	//for(unsigned int i = 0; i < size; i++)
	//{
	//	Triangle triangle;
	//	triangle = m_Physics->getTriangleFromBody(p_BodyHandle, i);
	//	m_Graphics->addBVTriangle(triangle.corners[0].xyz(), triangle.corners[1].xyz(), triangle.corners[2].xyz());
	//}
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

std::weak_ptr<Actor> GameLogic::addDzala(Vector3 p_Position)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Model");
	printer.PushAttribute("Mesh", "DZALA");
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
	pushVector(printer, "Scale", Vector3(30.f, 30.f, 30.f));
	printer.CloseElement();
	printer.CloseElement();

	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr());

	Actor::ptr actor = m_ActorFactory->createActor(doc.FirstChildElement("Object"));
	actor->setPosition(Vector3(800.f, 100.f, 200.f));
	actor->setRotation(Vector3(PI / 4.f, 0.f, 0.f));

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
