#include "GameScene.h"

GameScene::GameScene()
{
	m_SceneID = 0;
	m_Visible = false;
	m_ChangeScene = false;
	m_ChangeList = false;
	m_Graphics = nullptr;
	m_InputQueue = nullptr;
}

GameScene::~GameScene()
{
	m_Graphics = nullptr;
	m_InputQueue = nullptr;
}

bool GameScene::init(IGraphics *p_Graphics, ResourceManager *p_ResourceManager, IPhysics *p_Physics, Input *p_InputQueue, unsigned int p_SceneID)
{
	m_SceneID = p_SceneID;

	m_Graphics = p_Graphics;
	m_ResourceManager = p_ResourceManager;
	m_Physics = p_Physics;
	m_InputQueue = p_InputQueue;

	m_Level = Level(m_Graphics, m_ResourceManager, m_Physics);
	m_Level.loadLevel("../Bin/assets/levels/Level3.btxl", "../Bin/assets/levels/Level3.btxl");

	m_Player.initialize(m_Physics, XMFLOAT3(0.f, 20.f, 10.f), XMFLOAT3(0.f, 0.f, 1.f));

	m_Ground = m_Physics->createAABB(50.f, true, Vector3(0.f, 0.f, 0.f), Vector3(5000.f, 0.f, 5000.f), false);

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

	m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", "IKTest"));
	m_Graphics->createShader("AnimatedShader", L"../../Graphics/Source/DeferredShaders/AnimatedGeometryPass.hlsl",
		"VS,PS","5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
	m_Graphics->linkShaderToModel("AnimatedShader", "IKTest");

	m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", "DZALA"));
	m_Graphics->linkShaderToModel("AnimatedShader", "DZALA");

	currView = 3;

	InitTemporaryStuff();


	return true;
}

void GameScene::destroy()
{
	m_Graphics->eraseModelInstance(testScaleBox);


	m_Graphics->eraseModelInstance(ground);
	for (int box : boxIds)
	{
		m_Graphics->eraseModelInstance(box);
	}
	for(int i : m_ResourceIDs)
	{
		m_ResourceManager->releaseResource(i);
	}
	m_Level.releaseLevel();
}

void GameScene::onFrame(float p_DeltaTime, int* p_IsCurrentScene)
{
	if(m_ChangeScene)
	{
		*p_IsCurrentScene = m_NewSceneID;
		m_Visible = false;
		m_ChangeScene = false;
	}
	else if(m_ChangeList)
	{
		*p_IsCurrentScene = -1;
		m_ChangeList = false;
	}

	m_Player.update(p_DeltaTime);

	if(m_Physics->getHitDataSize() > 0)
	{
		for(int i = m_Physics->getHitDataSize() - 1; i >= 0; i--)
		{
			HitData hit = m_Physics->getHitDataAt(i);
			if(hit.intersect)
			{
				if(m_EdgeCollResponse.checkCollision(hit, m_Physics->getBodyPosition(hit.collisionVictim),m_Physics->getBodySize(hit.collisionVictim).y ,&m_Player))
				m_Physics->removedHitDataAt(i);

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

	m_Graphics->updateCamera(tempPos, viewRot[0], viewRot[1]);
	m_Graphics->setModelPosition(skyBox, m_Player.getGroundPosition());

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

	m_Graphics->updateAnimations(p_DeltaTime);

	//m_Graphics->applyIK_ReachPoint(wavingWitch, "bn_l_wrist01", "bn_l_elbow_b01", "bn_l_arm01", wavePos);

	yaw += yawSpeed * p_DeltaTime;
	pitch += pitchSpeed * p_DeltaTime;
	roll += rollSpeed * p_DeltaTime;

	for (int i = 0; i < NUM_BOXES; i++)
	{
		m_Graphics->setModelRotation(boxIds[i], Vector3(yaw * i, pitch * i, roll * i));
		m_Graphics->renderModel(boxIds[i]);
	}
	Vector3 lookDir;
	lookDir.x = -sinf(viewRot[0]) * cosf(viewRot[1]);
	lookDir.y = sinf(viewRot[1]);
	lookDir.z = -cosf(viewRot[0]) * cosf(viewRot[1]);

	static const float IK_Length = 500.f;

	//static const char* testTargetJoint = "bn_l_foot01";
	//static const char* testHingeJoint = "bn_l_Knee_a01";
	//static const char* testBaseJoint = "bn_l_Tigh01";

	Vector3 IK_Target(tempPos.x + lookDir.x * IK_Length, tempPos.y + lookDir.y * IK_Length, tempPos.z + lookDir.z * IK_Length);
	if (useIK_OnIK_Worm)
	{
		//m_Graphics->applyIK_ReachPoint(circleWitch, testTargetJoint, testHingeJoint, testBaseJoint, IK_Target);
		m_Graphics->applyIK_ReachPoint(ikTest, "joint4", "joint3", "joint2", IK_Target);
	}

	//Vector3 jointPos = m_Graphics->getJointPosition(circleWitch, testTargetJoint);
	//m_Graphics->setModelPosition(jointBox, jointPos);
	//m_Graphics->renderModel(jointBox);


	m_Graphics->renderModel(testScaleBox);


	m_Graphics->renderModel(ground);
	m_Graphics->renderModel(skyBox);
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
	//m_Level.drawLevel();

	m_Graphics->useFrameDirectionalLight(Vector3(1.f,1.f,1.f),Vector3(0.1f,-0.99f,0.f));
	m_Graphics->useFramePointLight(Vector3(0.f,0.f,0.f),Vector3(1.f,1.f,1.f),2000.f);
	m_Graphics->useFrameSpotLight(Vector3(-1000.f,500.f,0.f),Vector3(0.f,1.f,0.f),
		Vector3(0,0,-1),Vector2(cosf(3.14f/12),cosf(3.14f/4)), 2000.f );
	m_Graphics->useFramePointLight(Vector3(0.f, 30.f, 30.f), Vector3(0.5f, 0.5f, 0.5f), 2000000.f);
	m_Graphics->useFramePointLight(Vector3(0.f, 0.f, 30.f), Vector3(0.5f, 0.5f, 0.5f), 2000000.f);

	m_Graphics->drawFrame(currView);


}

void GameScene::render()
{
	m_Level.drawLevel();
}

bool GameScene::getIsVisible()
{
	return m_Visible;
}

void GameScene::setIsVisible(bool p_SetVisible)
{
	m_Visible = p_SetVisible;
}

void GameScene::registeredKeyStroke(std::string p_Action, float p_Value)
{
	if(p_Action == "changeSceneN" && p_Value == 1)
	{
		m_NewSceneID = GAMEPAUSE;
		m_ChangeScene = true;
	}
	else if(p_Action == "changeSceneP" && p_Value == 1)
	{
		m_ChangeList = true;
	}
	else if(p_Action ==  "changeViewN" && p_Value == 1)
	{
		currView--;
		if(currView < 0)
		currView = 3;
		Logger::log(Logger::Level::DEBUG_L, "Selecting previous view");
	}
	else if(p_Action ==  "changeViewP" && p_Value == 1)
	{
		currView++;
		if(currView >= 4)
		currView = 0;
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

/*########## TEST FUNCTIONS ##########*/

int GameScene::getID()
{
	return m_SceneID;
}

void GameScene::InitTemporaryStuff()
{
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

	

	Logger::log(Logger::Level::DEBUG, "Adding debug testScaleBox");
	testScaleBox = m_Graphics->createModelInstance("BOX");
	//m_Graphics->setModelScale(testScaleBox, Vector3(100.f, 100.f, 100.f));
	m_Graphics->setModelPosition(testScaleBox, Vector3(0.f, 0.f, 0.f));



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

	OBBhouse1 = m_Physics->createOBB(1.f, true, Vector3(), Vector3(500.f, 50.f, 350.f), false);
	m_Physics->setBodyRotation(OBBhouse1, Vector3(0.f, 0.f, 3.14f/6.5f));
	m_Physics->setBodyPosition(OBBhouse1, Vector3(0.f, 450.f, 0.f));

	OBBhouse2 = m_Physics->createOBB(1.f, true, Vector3(), Vector3(500.f, 50.f, 350.f), false);
	m_Physics->setBodyRotation(OBBhouse2, Vector3(0.f, 0.f, 3.14f/6.5f));
	m_Physics->setBodyPosition(OBBhouse2, Vector3(350.f, 500.0f, -1000.f));

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