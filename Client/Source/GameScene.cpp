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

	m_Player.initialize(m_Physics, XMFLOAT3(0.f, 2.f, 10.f), XMFLOAT3(0.f, 0.f, 1.f));

	m_Ground = m_Physics->createAABB(50.f, true, Vector3(0.f, 0.f, 0.f), Vector3(50.f, 0.f, 50.f), false);

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

	m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", "WITCH"));
	m_Graphics->linkShaderToModel("AnimatedShader", "WITCH");

	currView = 3;

	InitTemporaryStuff();

	// Test.
	m_Graphics->playAnimation(circleWitch, "Run", false, false, 0, 1.0f, 0);
	m_Graphics->playAnimation(wavingWitch, "Kick", false, false, 0, 1.0f, 0);
	m_Graphics->playAnimation(standingWitch, "Bomb", false, false, 0, 1.0f, 0);
	m_Graphics->playAnimation(testWitch, "Run", false, false, 0, 1.0f, 0);
	m_Graphics->playAnimation(ikTest, "Wave", false, false, 0, 1.0f, 0);

	return true;
}

void GameScene::destroy()
{
	
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

				Logger::log(Logger::Level::DEBUG, "Collision reported");
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

	Vector4 tempPos = m_Physics->getBodyPosition(m_Player.getBody());

	m_Graphics->updateCamera(Vector3(tempPos.x, tempPos.y + m_Player.getHeight() * 0.305f, tempPos.z), viewRot[0], viewRot[1]);
	m_Graphics->setModelPosition(skyBox, Vector3(tempPos.x, tempPos.y, tempPos.z));

	static const Vector3 circleCenter(4.f, 0.f, 15.f);
	static const float circleRadius = 8.f;

	
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

	static const float IK_Length = 5.f;

	static const char* testTargetJoint = "L_Hand";
	static const char* testHingeJoint = "L_LowerArm";
	static const char* testBaseJoint = "L_UpperArm";

	Vector3 IK_Target(tempPos.x + lookDir.x * IK_Length, tempPos.y + lookDir.y * IK_Length, tempPos.z + lookDir.z * IK_Length);
	if (useIK_OnIK_Worm)
	{
		m_Graphics->applyIK_ReachPoint(circleWitch, testTargetJoint, testHingeJoint, testBaseJoint, IK_Target);
		m_Graphics->applyIK_ReachPoint(ikTest, "joint3", "joint2", "joint1", IK_Target);
	}

	//Vector3 jointPos = m_Graphics->getJointPosition(circleWitch, testTargetJoint);
	//m_Graphics->setModelPosition(jointBox, jointPos);
	//m_Graphics->renderModel(jointBox);

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

	
	m_Graphics->renderModel(testWitch);
	//m_Level.drawLevel();

	m_Graphics->useFrameDirectionalLight(Vector3(1.f,1.f,1.f),Vector3(0.1f,-0.99f,0.f));
	m_Graphics->useFramePointLight(Vector3(0.f,0.f,0.f),Vector3(1.f,1.f,1.f),20.f);
	m_Graphics->useFrameSpotLight(Vector3(-10.f,5.f,0.f),Vector3(0.f,1.f,0.f),
		Vector3(0,0,-1),Vector2(cosf(3.14f/12),cosf(3.14f/4)), 20.f );
	m_Graphics->useFramePointLight(Vector3(0.f, 30.f, 30.f), Vector3(0.5f, 0.5f, 0.5f), 20000.f);
	m_Graphics->useFramePointLight(Vector3(0.f, 0.f, 30.f), Vector3(0.5f, 0.5f, 0.5f), 20000.f);

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
		Logger::log(Logger::Level::DEBUG, "Selecting previous view");
	}
	else if(p_Action ==  "changeViewP" && p_Value == 1)
	{
		currView++;
		if(currView >= 4)
		currView = 0;
		Logger::log(Logger::Level::DEBUG, "Selecting next view");
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
	else if( p_Action == "blendAnimation" && p_Value == 1.0f )
	{
		m_Graphics->playAnimation(wavingWitch, "Bomb", false, true, 2, 1.0f, 2);
		m_Graphics->playAnimation(ikTest, "Spin", false, true, 12, 1.0f, 2);
		m_Graphics->playAnimation(testWitch, "Idle", false, true, 12, 1.0f, 2);
	}
	else if( p_Action == "resetAnimation" && p_Value == 1.0f )
	{
		m_Graphics->playAnimation(wavingWitch, "Kick", false, true, 2, 1.0f, 2);
		m_Graphics->playAnimation(ikTest, "Wave", false, true, 12, 1.0f, 2);
		m_Graphics->playAnimation(testWitch, "Run", false, true, 12, 1.0f, 2);
	}
	else if( p_Action == "layerAnimation" && p_Value == 1.0f )
	{
		m_Graphics->playAnimation(ikTest, "Wave", true, false, 0, 0.7f, 1);
		m_Graphics->playAnimation(wavingWitch, "Bomb", true, false, 0, 0.5f, 1);
		m_Graphics->playAnimation(testWitch, "Idle", true, true, 12, 0.5f, 1);
	}
	else if( p_Action == "resetLayerAnimation" && p_Value == 1.0f )
	{
		m_Graphics->playAnimation(ikTest, "Wave", false, false, 0, 1.0f, 0);
		m_Graphics->playAnimation(wavingWitch, "Kick", false, false, 0, 1.0f, 0);
		m_Graphics->playAnimation(testWitch, "Run", false, false, 0, 1.0f, 0);
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
	

	Logger::log(Logger::Level::DEBUG, "Adding debug box model instances");
	
	for (int i = 0; i < NUM_BOXES; i++)
	{
		boxIds[i] = m_Graphics->createModelInstance("BOX");

		const float scale = 1.f + i * 3.f / NUM_BOXES;

		m_Graphics->setModelScale(boxIds[i], Vector3(scale, scale, scale));
		m_Graphics->setModelPosition(boxIds[i], Vector3((float)(i / 4) * 4.f, 1.f, (float)(i % 4) * 4.f + 40.f));
	}

	static const Vector3 climbTestPos(0.f, 2.f, 30.f);
	static const Vector3 climbTestHalfSize(1.f, 1.f, 1.f);

	BodyHandle boxTest = m_Physics->createAABB(50.f, true, climbTestPos, climbTestHalfSize, true );

	climbBox = m_Graphics->createModelInstance("BOX");

	m_Graphics->setModelPosition(climbBox, climbTestPos);
	m_Graphics->setModelScale(climbBox, Vector3(2.f, 2.f, 2.f));


	jointBox = m_Graphics->createModelInstance("BOX");
	m_Graphics->setModelScale(jointBox, Vector3(0.05f, 0.05f, 2.f));

	Logger::log(Logger::Level::DEBUG, "Adding debug skybox");
	skyBox = m_Graphics->createModelInstance("SKYBOX");
	m_Graphics->setModelScale(skyBox, Vector3(1.f, 1.f, 1.f));

	Logger::log(Logger::Level::DEBUG, "Adding debug ground");
	ground = m_Graphics->createModelInstance("BOX");
	m_Graphics->setModelScale(ground, Vector3(100.f, 5.f, 100.f));
	m_Graphics->setModelPosition(ground, Vector3(0.f, -2.5f, 0.f));

	Logger::log(Logger::Level::DEBUG, "Adding debug character");
	circleWitch = m_Graphics->createModelInstance("WITCH");
	m_Graphics->setModelScale(circleWitch, Vector3(0.01f, 0.01f, 0.01f));

	Logger::log(Logger::Level::DEBUG, "Adding debug witch");
	testWitch = m_Graphics->createModelInstance("WITCH");
	m_Graphics->setModelScale(testWitch, Vector3(0.01f, 0.01f, 0.01f));
	m_Graphics->setModelPosition(testWitch, Vector3(16.0f, 0.0f, 5.0f));

	standingWitch = m_Graphics->createModelInstance("DZALA");
	m_Graphics->setModelScale(standingWitch, Vector3(0.01f, 0.01f, 0.01f));
	m_Graphics->setModelPosition(standingWitch, Vector3(16.f, 0.f, -5.f));

	wavingWitch = m_Graphics->createModelInstance("DZALA");
	m_Graphics->setModelScale(wavingWitch, Vector3(0.01f, 0.01f, 0.01f));
	m_Graphics->setModelPosition(wavingWitch, Vector3(15.f, 0.f, -5.f));

	ikTest = m_Graphics->createModelInstance("IKTest");
	m_Graphics->setModelPosition(ikTest, Vector3(8.f, 1.f, 2.f));
	m_Graphics->setModelScale(ikTest, Vector3(0.01f, 0.01f, 0.01f));
	m_Graphics->setModelRotation(ikTest, Vector3(PI / 4.f, 0.f, 0.f));

	for(unsigned int i = 0; i < numTowerBoxes; i++)
	{
		towerBoxes[i] = m_Graphics->createModelInstance("BOX");
	}

	Vector3 towerBoxSizes[numTowerBoxes] =
	{
		Vector3(20.f, 1.6f, 20.f),
		Vector3(12.f, 1.6f, 12.f),
		Vector3(6.f, 4.f, 6.f),
		Vector3(0.1f, 8.f, 0.1f),
		Vector3(0.4f, 0.4f, 0.4f),
	};

	Vector3 towerBoxPositions[numTowerBoxes] =
	{
		Vector3(30.f, 0.8f, 40.f),
		Vector3(30.f, 2.4f, 40.f),
		Vector3(30.f, 5.2f, 40.f),
		Vector3(30.f, 11.2f, 40.f),
		Vector3(30.f, 15.4f, 40.f),
	};

	for (unsigned int i = 0; i < numTowerBoxes; i++)
	{
		m_Graphics->setModelScale(towerBoxes[i], towerBoxSizes[i]);
		m_Graphics->setModelPosition(towerBoxes[i], towerBoxPositions[i]);
		m_Physics->createAABB(50.f, true, towerBoxPositions[i], towerBoxSizes[i] * 0.5f, false);
	}

	m_Physics->createAABB(0.f, true, Vector3(30.f, 6.8f, 37.f), Vector3(2.8f, 0.6f, 0.2f), true);
	m_Physics->createAABB(0.f, true, Vector3(30.f, 6.8f, 43.f), Vector3(2.8f, 0.6f, 0.2f), true);
	m_Physics->createAABB(0.f, true, Vector3(27.f, 6.8f, 40.f), Vector3(0.2f, 0.6f, 2.8f), true);
	m_Physics->createAABB(0.f, true, Vector3(33.f, 6.8f, 40.f), Vector3(0.2f, 0.6f, 2.8f), true);

	for(unsigned int i = 0; i < numRotatedTowerBoxes; i++)
	{
		rotatedTowerBoxes[i] = m_Graphics->createModelInstance("BOX");
	}

	Vector3 rotatedTowerBoxSizes[numRotatedTowerBoxes] =
	{
		Vector3(20.f, 1.6f, 20.f),
		Vector3(12.f, 1.6f, 12.f),
		Vector3(6.f, 4.f, 6.f),
		Vector3(0.1f, 8.f, 0.1f),
		Vector3(0.4f, 0.4f, 0.4f),
	};

	Vector3 rotatedTowerBoxPositions[numRotatedTowerBoxes] =
	{
		Vector3(-30.f, 0.8f, 40.f),
		Vector3(-30.f, 2.4f, 40.f),
		Vector3(-30.f, 5.2f, 40.f),
		Vector3(-30.f, 11.2f, 40.f),
		Vector3(-30.f, 15.4f, 40.f),
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

	static const Vector3 slantedPlanePosition(-40.f, 3.f, 20.f);
	static const Vector3 slantedPlaneSize(20.f, 5.f, 30.f);
	static const Vector3 slantedPlaneRotation(0.3f, 0.2f, -0.3f);
	slantedPlane = m_Graphics->createModelInstance("BOX");
	m_Graphics->setModelPosition(slantedPlane, slantedPlanePosition);
	m_Graphics->setModelScale(slantedPlane, slantedPlaneSize);
	m_Graphics->setModelRotation(slantedPlane, slantedPlaneRotation);

	BodyHandle slantedPlaneBody = m_Physics->createOBB(0.f, true, slantedPlanePosition, slantedPlaneSize * 0.5f, false);
	m_Physics->setBodyRotation(slantedPlaneBody, slantedPlaneRotation);

	OBBhouse1 = m_Physics->createOBB(1.f, true, Vector3(), Vector3(5.f, 0.5f, 7.f/2.f), false);
	m_Physics->setBodyRotation(OBBhouse1, Vector3(0.f, 0.f, 3.14f/6.5f));
	m_Physics->setBodyPosition(OBBhouse1, Vector3(14.f, 4.5f, -10.f));

	OBBhouse2 = m_Physics->createOBB(1.f, true, Vector3(), Vector3(5.f, 0.5f, 7.f/2.f), false);
	m_Physics->setBodyRotation(OBBhouse2, Vector3(0.f, 0.f, 3.14f/6.5f));
	m_Physics->setBodyPosition(OBBhouse2, Vector3(3.5f, 5.0f, -10.f));

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