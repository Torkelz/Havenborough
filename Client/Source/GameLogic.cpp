#include "GameLogic.h"

GameLogic::GameLogic(void)
{
	m_Graphics = nullptr;
	m_Physics = nullptr;
	m_InputQueue = nullptr;
	m_ResourceManager = nullptr;
}


GameLogic::~GameLogic(void)
{
	m_Graphics = nullptr;
	m_Physics = nullptr;
	m_InputQueue = nullptr;
	m_ResourceManager = nullptr;
}

void GameLogic::initialize(IGraphics *p_Graphics, ResourceManager *p_ResourceManager,
	IPhysics *p_Physics, Input *p_InputQueue)
{
	m_Graphics = p_Graphics;
	m_Physics = p_Physics;
	m_InputQueue = p_InputQueue;
	m_ResourceManager = p_ResourceManager;

	m_Level = Level(m_Graphics, m_ResourceManager, m_Physics);
	m_Level.loadLevel("../Bin/assets/levels/Level3.btxl", "../Bin/assets/levels/CB_Level3.btxl");
	m_Level.setStartPosition(XMFLOAT3(0.0f, 200.0f, 1500.0f)); //TODO: Remove this line when level gets the position from file
	m_Level.setGoalPosition(XMFLOAT3(0.0f, 0.0f, 0.0f)); //TODO: Remove this line when level gets the position from file
	m_FinishLine = m_Physics->createSphere(0.0f, true, XMFLOAT3ToVector3(&(m_Level.getGoalPosition())), 200.0f);

	m_Player.initialize(m_Physics, m_Level.getStartPosition(), XMFLOAT3(0.f, 0.f, 1.f));

	m_Ground = m_Physics->createAABB(50.f, true, Vector3(0.f, 0.f, 0.f), Vector3(5000.f, 0.f, 5000.f), false);
	
	m_ChangeScene = GoToScene::NONE;

	//TODO: Remove later when we actually have a level to load.
	loadSandbox();
	currentDebugView = 3;
}

void GameLogic::shutdown(void)
{
	m_Graphics->eraseModelInstance(ground);
	m_Graphics->eraseModelInstance(skyBox);
	m_Level.releaseLevel();
	m_Physics->releaseAllBoundingVolumes();
	
	
	//TODO: Remove when we have a real level.
	shutdownSandbox();
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

	const InputState& state = m_InputQueue->getCurrentState();

	float forward = state.getValue("moveForward") - state.getValue("moveBackward");
	float right = state.getValue("moveRight") - state.getValue("moveLeft");

	if (forward != 0.f || right != 0.f)
	{
		float dir = atan2f(right, forward) + viewRot[0];

		m_Player.setDirectionX(sinf(dir));
		m_Player.setDirectionZ(cosf(dir));
	}
	if(!m_Player.getForceMove())		
		m_Physics->update(p_DeltaTime);

	Vector3 tempPos = m_Player.getEyePosition();
	Vector2 actualViewRot(viewRot[0], viewRot[1]);

	Actor::ptr strongPlayerActor = m_PlayerActor.lock();
	if (strongPlayerActor)
	{
		tempPos = strongPlayerActor->getPosition();
		Vector3 rot = strongPlayerActor->getRotation();
		actualViewRot.x += rot.x;
		actualViewRot.y += rot.y;
	}

	m_Graphics->updateCamera(tempPos, actualViewRot.x, actualViewRot.y);
	m_Graphics->setModelPosition(skyBox, tempPos);
	
	lookDir.x = -sinf(actualViewRot.x) * cosf(actualViewRot.y);
	lookDir.y = sinf(actualViewRot.y);
	lookDir.z = -cosf(actualViewRot.x) * cosf(actualViewRot.y);

	m_Graphics->updateAnimations(p_DeltaTime);
	
	
	updateSandbox(p_DeltaTime);
}

void GameLogic::render()
{
	m_Level.drawLevel();
	m_Graphics->renderModel(ground);
	m_Graphics->renderModel(skyBox);

	m_Graphics->useFrameDirectionalLight(Vector3(1.f,1.f,1.f),Vector3(0.1f,-0.99f,0.f));
	//m_Graphics->drawFrame(currView);

	//addDebugBVToDraw(1);
	/*addDebugBVToDraw(5);
	addDebugBVToDraw(6);
	addDebugBVToDraw(7);
	addDebugBVToDraw(8);
	addDebugBVToDraw(9);
	addDebugBVToDraw(10);
	addDebugBVToDraw(11);
	addDebugBVToDraw(12);
	addDebugBVToDraw(13);
	addDebugBVToDraw(14);
	addDebugBVToDraw(15);
	addDebugBVToDraw(16);*/

	for(int i = 0; i < 35; i++)
	{
		addDebugBVToDraw(i);
	}

	//m_Graphics->drawFrame(currView);

	renderSandbox();
	m_Graphics->drawFrame(currentDebugView);
}

void GameLogic::registeredInput(std::string p_Action, float p_Value)
{
	if(p_Action ==  "changeViewN" && p_Value == 1)
	{
		currentDebugView--;
		if(currentDebugView < 0)
			currentDebugView = 3;
		Logger::log(Logger::Level::DEBUG_L, "Selecting previous view");
	}
	else if(p_Action ==  "changeViewP" && p_Value == 1)
	{
		currentDebugView++;
		if(currentDebugView >= 4)
			currentDebugView = 0;
		Logger::log(Logger::Level::DEBUG_L, "Selecting next view");
	}
	else if (p_Action == "mouseMoveHori")
	{
		viewRot[0] += p_Value * sensitivity;
		if (viewRot[0] > PI)
		{
			viewRot[0] -= 2 * PI;
		}
		else if (viewRot[0] < -PI)
		{
			viewRot[0] += 2 * PI;
		}
	}
	else if (p_Action == "mouseMoveVert")
	{
		viewRot[1] += p_Value * sensitivity;
		if (viewRot[1] > PI * 0.45f)
		{
			viewRot[1] = PI * 0.45f;
		}
		else if (viewRot[1] < -PI * 0.45f)
		{
			viewRot[1] = -PI * 0.45f;
		}
	}
	else if( p_Action == "jump" && p_Value == 1)
	{
		m_Player.setJump();
	}
	else if (p_Action == "toggleIK" && p_Value == 1.f)
	{
		useIK_OnIK_Worm = !useIK_OnIK_Worm;
	}
}

void GameLogic::setPlayerActor(std::weak_ptr<Actor> p_Actor)
{
	m_PlayerActor = p_Actor;
}

void GameLogic::loadSandbox()
{
	m_ResourceIDs.push_back(m_ResourceManager->loadResource("volume", "House"));

	static const std::string preloadedModels[] =
	{
		"BOX",
		"SKYBOX",
		"House",
	};

	for (const std::string& model : preloadedModels)
	{
		m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", model));
		m_Graphics->linkShaderToModel("DefaultShader", model.c_str());
	}

	Logger::log(Logger::Level::DEBUG_L, "Adding IK test tube");
	m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", "IKTest"));
	m_Graphics->createShader("AnimatedShader", L"../../Graphics/Source/DeferredShaders/AnimatedGeometryPass.hlsl",
		"VS,PS","5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
	m_Graphics->linkShaderToModel("AnimatedShader", "IKTest");

	Logger::log(Logger::Level::DEBUG_L, "Adding debug animated Dzala");
	m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", "DZALA"));
	m_Graphics->linkShaderToModel("AnimatedShader", "DZALA");

	useIK_OnIK_Worm = false;


	Logger::log(Logger::Level::DEBUG_L, "Adding debug box model instances");

	for (int i = 0; i < NUM_BOXES; i++)
	{
		boxIds[i] = m_Graphics->createModelInstance("BOX");

		const float scale = 100.f + i * 300.f / NUM_BOXES;

		m_Graphics->setModelScale(boxIds[i], Vector3(scale, scale, scale));
		m_Graphics->setModelPosition(boxIds[i], Vector3((float)(i / 4) * 400.f, 100.f, (float)(i % 4) * 400.f + 4000.f));
	}

	static const Vector3 climbTestPos(0.f, 200.f, 3000.f);
	static const Vector3 climbTestHalfSize(100.f, 100.f, 100.f);

	BodyHandle boxTest = m_Physics->createAABB(50.f, true, climbTestPos, climbTestHalfSize, true );

	climbBox = m_Graphics->createModelInstance("BOX");

	m_Graphics->setModelPosition(climbBox, climbTestPos);
	m_Graphics->setModelScale(climbBox, Vector3(200.f, 200.f, 200.f));


	jointBox = m_Graphics->createModelInstance("BOX");
	m_Graphics->setModelScale(jointBox, Vector3(5.f, 5.f, 200.f));

	Logger::log(Logger::Level::DEBUG_L, "Adding debug skybox");
	skyBox = m_Graphics->createModelInstance("SKYBOX");
	m_Graphics->setModelScale(skyBox, Vector3(100.f, 100.f, 100.f));

	Logger::log(Logger::Level::DEBUG_L, "Adding debug ground");
	ground = m_Graphics->createModelInstance("BOX");
	m_Graphics->setModelScale(ground, Vector3(10000.f, 500.f, 10000.f));
	m_Graphics->setModelPosition(ground, Vector3(0.f, -250.f, 0.f));

	Logger::log(Logger::Level::DEBUG_L, "Adding debug character");
	circleWitch = m_Graphics->createModelInstance("DZALA");
	//m_Graphics->setModelScale(circleWitch, Vector3(0.01f, 0.01f, 0.01f));

	standingWitch = m_Graphics->createModelInstance("DZALA");
	m_Graphics->setModelPosition(standingWitch, Vector3(1600.f, 0.f, -500.f));
	//m_Graphics->setModelScale(standingWitch, Vector3(0.01f, 0.01f, 0.01f));

	wavingWitch = m_Graphics->createModelInstance("DZALA");
	m_Graphics->setModelPosition(wavingWitch, Vector3(1500.f, 0.f, -500.f));
	//m_Graphics->setModelScale(wavingWitch, Vector3(0.01f, 0.01f, 0.01f));

	ikTest = m_Graphics->createModelInstance("IKTest");
	m_Graphics->setModelPosition(ikTest, Vector3(800.f, 100.f, 200.f));
	m_Graphics->setModelScale(ikTest, Vector3(30.f, 30.f, 30.f));
	m_Graphics->setModelRotation(ikTest, Vector3(PI / 4.f, 0.f, 0.f));

	for(unsigned int i = 0; i < numTowerBoxes; i++)
	{
		towerBoxes[i] = m_Graphics->createModelInstance("BOX");
	}

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
		m_Graphics->setModelScale(towerBoxes[i], towerBoxSizes[i]);
		m_Graphics->setModelPosition(towerBoxes[i], towerBoxPositions[i]);
		m_Physics->createAABB(50.f, true, towerBoxPositions[i], towerBoxSizes[i] * 0.5f, false);
	}

	m_Physics->createAABB(0.f, true, Vector3(3000.f, 680.f, 3700.f), Vector3(280.f, 60.f, 20.f), true);
	m_Physics->createAABB(0.f, true, Vector3(3000.f, 680.f, 4300.f), Vector3(280.f, 60.f, 20.f), true);
	m_Physics->createAABB(0.f, true, Vector3(2700.f, 680.f, 4000.f), Vector3(20.f, 60.f, 280.f), true);
	m_Physics->createAABB(0.f, true, Vector3(3300.f, 680.f, 4000.f), Vector3(20.f, 60.f, 280.f), true);

	for(unsigned int i = 0; i < numRotatedTowerBoxes; i++)
	{
		rotatedTowerBoxes[i] = m_Graphics->createModelInstance("BOX");
	}

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

	BodyHandle rotatedTowerBodies[numRotatedTowerBoxes];

	for (unsigned int i = 0; i < numRotatedTowerBoxes; i++)
	{
		m_Graphics->setModelScale(rotatedTowerBoxes[i], rotatedTowerBoxSizes[i]);
		m_Graphics->setModelPosition(rotatedTowerBoxes[i], rotatedTowerBoxPositions[i]);
		m_Graphics->setModelRotation(rotatedTowerBoxes[i], Vector3(1.f, 0.f, 0.f));
		rotatedTowerBodies[i] = m_Physics->createOBB(50.f, true, rotatedTowerBoxPositions[i], rotatedTowerBoxSizes[i] * 0.5f, false);
		m_Physics->setBodyRotation(rotatedTowerBodies[i], Vector3(1.f, 0.f, 0.f));
	}

	static const Vector3 slantedPlanePosition(-4000.f, 300.f, 2000.f);
	static const Vector3 slantedPlaneSize(2000.f, 500.f, 3000.f);
	static const Vector3 slantedPlaneRotation(0.3f, 0.2f, -0.3f);
	slantedPlane = m_Graphics->createModelInstance("BOX");
	m_Graphics->setModelPosition(slantedPlane, slantedPlanePosition);
	m_Graphics->setModelScale(slantedPlane, slantedPlaneSize);
	m_Graphics->setModelRotation(slantedPlane, slantedPlaneRotation);

	BodyHandle slantedPlaneBody = m_Physics->createOBB(0.f, true, slantedPlanePosition, slantedPlaneSize * 0.5f, false);
	m_Physics->setBodyRotation(slantedPlaneBody, slantedPlaneRotation);

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

	sensitivity = 0.01f;

	yaw = 0.f;
	yawSpeed = 0.1f;
	pitch = 0.f;
	pitchSpeed = 0.05f;
	roll = 0.f;
	rollSpeed = 0.03f;

	witchCircleAngle = 0.0f;
}

void GameLogic::updateSandbox(float p_DeltaTime)
{
	static const Vector3 circleCenter(400.f, 0.f, 1500.f);
	static const float circleRadius = 800.f;


	static const float witchAngleSpeed = 0.3f;

	float witchWaveAngle = 0.f;
	static const float witchWavingAngleSpeed = 1.f;

	witchCircleAngle += witchAngleSpeed * p_DeltaTime;
	Vector3 witchCirclePosition(circleCenter);
	witchCirclePosition.x -= cosf(witchCircleAngle) * circleRadius;
	witchCirclePosition.z += sinf(witchCircleAngle) * circleRadius;
	m_Graphics->setModelPosition(circleWitch, witchCirclePosition);
	m_Graphics->setModelRotation(circleWitch, Vector3(witchCircleAngle, 0.f, 0.f));

	static const float waveRadius = 0.5f;
	Vector3 wavePos = m_Graphics->getJointPosition(wavingWitch, "Head");
	wavePos.x -= 1.f;
	witchWaveAngle += witchWavingAngleSpeed * p_DeltaTime;
	wavePos.y += sinf(witchWaveAngle) * waveRadius;
	wavePos.z += cosf(witchWaveAngle) * waveRadius;

	//m_Graphics->applyIK_ReachPoint(wavingWitch, "bn_l_wrist01", "bn_l_elbow_b01", "bn_l_arm01", wavePos);

	yaw += yawSpeed * p_DeltaTime;
	pitch += pitchSpeed * p_DeltaTime;
	roll += rollSpeed * p_DeltaTime;

	for (int i = 0; i < NUM_BOXES; i++)
	{
		m_Graphics->setModelRotation(boxIds[i], Vector3(yaw * i, pitch * i, roll * i));
		m_Graphics->renderModel(boxIds[i]);
	}

	static const float IK_Length = 500.f;

	//static const char* testTargetJoint = "bn_l_foot01";
	//static const char* testHingeJoint = "bn_l_Knee_a01";
	//static const char* testBaseJoint = "bn_l_Tigh01";

	Vector4 tempPos = m_Physics->getBodyPosition(m_Player.getBody());
	Vector3 IK_Target(tempPos.x + lookDir.x * IK_Length, tempPos.y + lookDir.y * IK_Length, tempPos.z + lookDir.z * IK_Length);
	if (useIK_OnIK_Worm)
	{
		//m_Graphics->applyIK_ReachPoint(circleWitch, testTargetJoint, testHingeJoint, testBaseJoint, IK_Target);
		m_Graphics->applyIK_ReachPoint(ikTest, "joint4", "joint3", "joint2", IK_Target);
	}

	//Vector3 jointPos = m_Graphics->getJointPosition(circleWitch, testTargetJoint);
	//m_Graphics->setModelPosition(jointBox, jointPos);
}

void GameLogic::renderSandbox()
{
	m_Graphics->renderModel(ikTest);
	m_Graphics->renderModel(circleWitch);
	m_Graphics->renderModel(standingWitch);
	m_Graphics->renderModel(wavingWitch);
	m_Graphics->renderModel(climbBox);
	for (int box : towerBoxes)
	{
		m_Graphics->renderModel(box);
	}
	for (int box : rotatedTowerBoxes)
	{
		m_Graphics->renderModel(box);
	}
	m_Graphics->renderModel(slantedPlane);

	m_Graphics->useFramePointLight(Vector3(0.f,0.f,0.f),Vector3(1.f,1.f,1.f),2000.f);
	m_Graphics->useFrameSpotLight(Vector3(-1000.f,500.f,0.f),Vector3(0.f,1.f,0.f),
		Vector3(0,0,-1),Vector2(cosf(3.14f/12),cosf(3.14f/4)), 2000.f );
	m_Graphics->useFramePointLight(Vector3(0.f, 3000.f, 3000.f), Vector3(0.5f, 0.5f, 0.5f), 2000000.f);
	m_Graphics->useFramePointLight(Vector3(0.f, 0.f, 3000.f), Vector3(0.5f, 0.5f, 0.5f), 2000000.f);
}

void GameLogic::shutdownSandbox()
{
	m_Graphics->eraseModelInstance(ikTest);
	m_Graphics->eraseModelInstance(circleWitch);
	m_Graphics->eraseModelInstance(standingWitch);
	m_Graphics->eraseModelInstance(wavingWitch);
	m_Graphics->eraseModelInstance(climbBox);
	m_Graphics->eraseModelInstance(jointBox);
	for(int box : towerBoxes)
	{
		m_Graphics->eraseModelInstance(box);
	}	
	for(int box : rotatedTowerBoxes)
	{
		m_Graphics->eraseModelInstance(box);
	}
	for (int box : boxIds)
	{
		m_Graphics->eraseModelInstance(box);
	}
	m_Graphics->eraseModelInstance(slantedPlane);
	
	for(int i : m_ResourceIDs)
	{
		m_ResourceManager->releaseResource(i);
	}

	m_Graphics->deleteShader("DefaultShader");
	m_Graphics->deleteShader("AnimatedShader");
}

void GameLogic::addDebugBVToDraw(BodyHandle p_BodyHandle)
{
	unsigned int size =  m_Physics->getNrOfTrianglesFromBody(p_BodyHandle);

	for(unsigned int i = 0; i < size; i++)
	{
		Triangle triangle;
		triangle = m_Physics->getTriangleFromBody(p_BodyHandle, i);
		m_Graphics->addBVTriangle(triangle.corners[0].xyz(), triangle.corners[1].xyz(), triangle.corners[2].xyz());
	}
}