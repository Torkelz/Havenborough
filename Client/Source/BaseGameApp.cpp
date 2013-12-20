#include "BaseGameApp.h"
#include "Input\InputTranslator.h"
#include "Logger.h"

#include <iomanip>
#include <sstream>

const double pi = 3.14159265358979323846264338;

const std::string BaseGameApp::m_GameTitle = "The Apprentice of Havenborough";


void BaseGameApp::init()
{
	Logger::log(Logger::Level::INFO, "Initializing game app");
	
	m_MemUpdateDelay = 0.1f;
	m_TimeToNextMemUpdate = 0.f;

	m_SceneManager.init();
	m_Window.init(getGameTitle(), getWindowSize());

	m_Graphics = IGraphics::createGraphics();
	m_Graphics->setLogFunction(&Logger::logRaw);
	
	//TODO: Need some input setting variable to handle fullscreen.
	bool fullscreen = false;
	m_Graphics->initialize(m_Window.getHandle(), (int)m_Window.getSize().x, (int)m_Window.getSize().y, fullscreen);
	m_Window.registerCallback(WM_CLOSE, std::bind(&BaseGameApp::handleWindowClose, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	m_ResourceManager = new ResourceManager();
	using namespace std::placeholders;
	m_Graphics->setLoadModelTextureCallBack(&ResourceManager::loadModelTexture, m_ResourceManager);
	m_Graphics->setReleaseModelTextureCallBack(&ResourceManager::releaseModelTexture, m_ResourceManager);
	m_ResourceManager->registerFunction( "model", std::bind(&IGraphics::createModel, m_Graphics, _1, _2), std::bind(&IGraphics::releaseModel, m_Graphics, _1) );
	m_ResourceManager->registerFunction( "texture", std::bind(&IGraphics::createTexture, m_Graphics, _1, _2), std::bind(&IGraphics::releaseTexture, m_Graphics, _1));
	


	InputTranslator::ptr translator(new InputTranslator);
	translator->init(&m_Window);
	
	Logger::log(Logger::Level::DEBUG, "Adding input mappings");
	translator->addKeyboardMapping(VK_ESCAPE, "exit");
	translator->addKeyboardMapping('W', "moveForward");
	translator->addKeyboardMapping('S', "moveBackward");
	translator->addKeyboardMapping('A', "moveLeft");
	translator->addKeyboardMapping('D', "moveRight");
	//translator->addKeyboardMapping('C', "connect");
	translator->addKeyboardMapping('Z', "changeViewN");
	translator->addKeyboardMapping('X', "changeViewP");
	translator->addKeyboardMapping('I', "toggleIK");
	translator->addKeyboardMapping(VK_SPACE, "jump");
	
	translator->addMouseMapping(InputTranslator::Axis::HORIZONTAL, "mousePosHori", "mouseMoveHori");
	translator->addMouseMapping(InputTranslator::Axis::VERTICAL, "mousePosVert", "mouseMoveVert");

	translator->addMouseButtonMapping(InputTranslator::MouseButton::LEFT, "gogogogo");
	translator->addMouseButtonMapping(InputTranslator::MouseButton::MIDDLE, "rollMe!");

	m_InputQueue.init(std::move(translator));

	m_Network = INetwork::createNetwork();
	m_Network->setLogFunction(&Logger::logRaw);
	m_Network->initialize();
	m_Connected = false;
	
	m_Physics = IPhysics::createPhysics();
	m_Physics->setLogFunction(&Logger::logRaw);
	m_Physics->initialize();
	m_ResourceManager->registerFunction( "volume", std::bind(&IPhysics::createLevelBV, m_Physics, _1, _2), std::bind(&IPhysics::releaseLevelBV, m_Physics, _1));
		
	m_Level = Level(m_Graphics, m_ResourceManager, m_Physics);
	m_Level.loadLevel("../Bin/assets/levels/Level3.btxl", "../Bin/assets/levels/Level3.btxl");


	m_Player.initialize(m_Physics, XMFLOAT3(0.f, 2.f, 10.f), XMFLOAT3(0.f, 0.f, 1.f));
		
	Logger::log(Logger::Level::DEBUG, "Adding debug bodies");
	m_Ground = m_Physics->createAABB(50.f, true, Vector3(0.f, 0.f, 0.f), Vector3(50.f, 0.f, 50.f), false);


	//Logger::log(Logger::Level::DEBUG, "Adding debug models");
	//m_Graphics->createShader("DefaultShader", L"../../Graphics/Source/DeferredShaders/GeometryPass.hlsl",
	//						"VS,PS","5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
	
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

	//m_ResourceIDs.push_back(m_ResourceManager->loadResource("texture", "TEXTURE_NOT_FOUND"));
	m_MemoryInfo.update();

	m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", "IKTest"));
	m_Graphics->createShader("AnimatedShader", L"../../Graphics/Source/DeferredShaders/AnimatedGeometryPass.hlsl",
							"VS,PS","5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
	m_Graphics->linkShaderToModel("AnimatedShader", "IKTest");

	m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", "DZALA"));
	m_Graphics->linkShaderToModel("AnimatedShader", "DZALA");
}

void BaseGameApp::run()
{
	Logger::log(Logger::Level::INFO, "Running the game");

	m_ShouldQuit = false;
	int currView = 3; // FOR DEBUGGING
	
	bool useIK_OnIK_Worm = false;
	float witchCircleAngle = 0.f;
	static const float witchAngleSpeed = 0.3f;

	float witchWaveAngle = 0.f;
	static const float witchWavingAngleSpeed = 1.f;

	Logger::log(Logger::Level::DEBUG, "Adding debug box model instances");
	const static int NUM_BOXES = 16;
	int boxIds[NUM_BOXES];
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
	
	int climbBox = m_Graphics->createModelInstance("BOX");
	
	m_Graphics->setModelPosition(climbBox, climbTestPos);
	m_Graphics->setModelScale(climbBox, Vector3(2.f, 2.f, 2.f));


	int jointBox = m_Graphics->createModelInstance("BOX");
	m_Graphics->setModelScale(jointBox, Vector3(0.05f, 0.05f, 2.f));
	
	Logger::log(Logger::Level::DEBUG, "Adding debug skybox");
	int skyBox = m_Graphics->createModelInstance("SKYBOX");
	m_Graphics->setModelScale(skyBox, Vector3(1.f, 1.f, 1.f));

	Logger::log(Logger::Level::DEBUG, "Adding debug ground");
	int ground = m_Graphics->createModelInstance("BOX");
	m_Graphics->setModelScale(ground, Vector3(100.f, 0.0001f, 100.f));
	m_Graphics->setModelPosition(ground, Vector3(0.f, 0.f, 0.f));

	Logger::log(Logger::Level::DEBUG, "Adding debug character");
	int circleWitch = m_Graphics->createModelInstance("DZALA");
	m_Graphics->setModelScale(circleWitch, Vector3(0.1f, 0.1f, 0.1f));

	int standingWitch = m_Graphics->createModelInstance("DZALA");
	m_Graphics->setModelScale(standingWitch, Vector3(0.1f, 0.1f, 0.1f));
	m_Graphics->setModelPosition(standingWitch, Vector3(16.f, 0.f, -5.f));

	int wavingWitch = m_Graphics->createModelInstance("DZALA");
	m_Graphics->setModelScale(wavingWitch, Vector3(0.1f, 0.1f, 0.1f));
	m_Graphics->setModelPosition(wavingWitch, Vector3(15.f, 0.f, -5.f));
	
	int ikTest = m_Graphics->createModelInstance("IKTest");
	m_Graphics->setModelPosition(ikTest, Vector3(8.f, 1.f, 2.f));
	m_Graphics->setModelScale(ikTest, Vector3(0.3f, 0.3f, 0.3f));
	m_Graphics->setModelRotation(ikTest, Vector3((float)pi / 4.f, 0.f, 0.f));
	
	static const unsigned int numTowerBoxes = 5;
	int towerBoxes[numTowerBoxes] =
	{
		m_Graphics->createModelInstance("BOX"),
		m_Graphics->createModelInstance("BOX"),
		m_Graphics->createModelInstance("BOX"),
		m_Graphics->createModelInstance("BOX"),
		m_Graphics->createModelInstance("BOX"),
	};

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

	m_Physics->createAABB(0.f, true, Vector3(30.f, 7.f, 37.f), Vector3(2.8f, 0.2f, 0.2f), true);
	m_Physics->createAABB(0.f, true, Vector3(30.f, 7.f, 43.f), Vector3(2.8f, 0.2f, 0.2f), true);
	m_Physics->createAABB(0.f, true, Vector3(27.f, 7.f, 40.f), Vector3(0.2f, 0.2f, 2.8f), true);
	m_Physics->createAABB(0.f, true, Vector3(33.f, 7.f, 40.f), Vector3(0.2f, 0.2f, 2.8f), true);

	static const int numRotatedTowerBoxes = 5;
	int rotatedTowerBoxes[numRotatedTowerBoxes] =
	{
		m_Graphics->createModelInstance("BOX"),
		m_Graphics->createModelInstance("BOX"),
		m_Graphics->createModelInstance("BOX"),
		m_Graphics->createModelInstance("BOX"),
		m_Graphics->createModelInstance("BOX"),
	};

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
		m_Physics->setBodyRotation(rotatedTowerBodies[i], 1.f, 0.f, 0.f);
	}

	float viewRot[] = {0.f, 0.f};

	float sensitivity = 0.01f;

	float yaw = 0.f;
	float yawSpeed = 0.1f;
	float pitch = 0.f;
	float pitchSpeed = 0.05f;
	float roll = 0.f;
	float rollSpeed = 0.03f;

	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;
	__int64 currTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
	currTimeStamp--;

	while (!m_ShouldQuit)
	{
		Logger::log(Logger::Level::TRACE, "New frame");

		prevTimeStamp = currTimeStamp;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
		float dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;
		const static float maxDeltaTime = 1.f / 5.f;
		if (dt > maxDeltaTime)
		{
			Logger::log(Logger::Level::WARNING, "Computer to slow or something");
			dt = maxDeltaTime;
		}

		m_Player.update(dt);
		
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
		
		const InputState& state = m_InputQueue.getCurrentState();
		
		float forward = state.getValue("moveForward") - state.getValue("moveBackward");
		float right = state.getValue("moveRight") - state.getValue("moveLeft");
		
		if (forward != 0.f || right != 0.f)
		{
			float dir = atan2f(right, forward) + viewRot[0];

			m_Player.setDirectionX(sinf(dir));
			m_Player.setDirectionZ(cosf(dir));
		}
		if(!m_Player.getForceMove())		
			m_Physics->update(dt);

		Vector4 tempPos = m_Physics->getBodyPosition(m_Player.getBody());

 		m_Graphics->updateCamera(Vector3(tempPos.x, tempPos.y, tempPos.z), viewRot[0], viewRot[1]);
		m_Graphics->setModelPosition(skyBox, Vector3(tempPos.x, tempPos.y, tempPos.z));

		static const Vector3 circleCenter(4.f, 0.f, 15.f);
		static const float circleRadius = 8.f;
		witchCircleAngle += witchAngleSpeed * dt;
		Vector3 witchCirclePosition(circleCenter);
		witchCirclePosition.x -= cosf(witchCircleAngle) * circleRadius;
		witchCirclePosition.z += sinf(witchCircleAngle) * circleRadius;
		m_Graphics->setModelPosition(circleWitch, witchCirclePosition);
		m_Graphics->setModelRotation(circleWitch, Vector3(witchCircleAngle, 0.f, 0.f));

		static const float waveRadius = 0.5f;
		Vector3 wavePos = m_Graphics->getJointPosition(wavingWitch, "bn_head01");
		wavePos.x -= 1.f;
		witchWaveAngle += witchWavingAngleSpeed * dt;
		wavePos.y += sinf(witchWaveAngle) * waveRadius;
		wavePos.z += cosf(witchWaveAngle) * waveRadius;

		m_Graphics->updateAnimations(dt);

		m_Graphics->applyIK_ReachPoint(wavingWitch, "bn_l_wrist01", "bn_l_elbow_b01", "bn_l_arm01", wavePos);

		yaw += yawSpeed * dt;
		pitch += pitchSpeed * dt;
		roll += rollSpeed * dt;

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

		static const char* testTargetJoint = "bn_l_foot01";
		static const char* testHingeJoint = "bn_l_Knee_a01";
		static const char* testBaseJoint = "bn_l_Tigh01";

		Vector3 IK_Target(tempPos.x + lookDir.x * IK_Length, tempPos.y + lookDir.y * IK_Length, tempPos.z + lookDir.z * IK_Length);
		if (useIK_OnIK_Worm)
		{
			m_Graphics->applyIK_ReachPoint(circleWitch, testTargetJoint, testHingeJoint, testBaseJoint, IK_Target);
			m_Graphics->applyIK_ReachPoint(ikTest, "joint4", "joint3", "joint2", IK_Target);
		}

		Vector3 jointPos = m_Graphics->getJointPosition(circleWitch, testTargetJoint);
		m_Graphics->setModelPosition(jointBox, jointPos);
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

		m_Level.drawLevel();

		m_Graphics->useFrameDirectionalLight(Vector3(1.f,1.f,1.f),Vector3(0.1f,-0.99f,0.f));
		m_Graphics->useFramePointLight(Vector3(0.f,0.f,0.f),Vector3(1.f,1.f,1.f),20.f);
		m_Graphics->useFrameSpotLight(Vector3(-10.f,5.f,0.f),Vector3(0.f,1.f,0.f),
			Vector3(0,0,-1),Vector2(cosf(3.14f/12),cosf(3.14f/4)), 20.f );
		m_Graphics->useFramePointLight(Vector3(0.f, 30.f, 30.f), Vector3(0.5f, 0.5f, 0.5f), 20000.f);
		m_Graphics->useFramePointLight(Vector3(0.f, 0.f, 30.f), Vector3(0.5f, 0.5f, 0.5f), 20000.f);

		m_Graphics->drawFrame(currView);
		
		m_MemoryInfo.update();
		updateDebugInfo(dt);

		m_InputQueue.onFrame();
		m_Window.pollMessages();

		for (auto& in : m_InputQueue.getFrameInputs())
		{
			std::ostringstream msg;
			msg << "Received input action: " << in.m_Action << " (" << std::setprecision(2) << std::fixed << in.m_Value << ")";
			Logger::log(Logger::Level::TRACE, msg.str());

			if (in.m_Action == "exit")
			{
				m_ShouldQuit = true;
			}
			else if (in.m_Action == "connect" && in.m_Value == 1.0f)
			{
				m_Connected = false;
				m_Network->connectToServer("localhost", 31415, &connectedCallback, this);
			}
			else if(in.m_Action ==  "changeViewN" && in.m_Value == 1)
			{
				currView--;
				if(currView < 0)
					currView = 3;
				Logger::log(Logger::Level::DEBUG, "Selecting previous view");
			}
			else if(in.m_Action ==  "changeViewP" && in.m_Value == 1)
			{
				currView++;
				if(currView >= 4)
					currView = 0;
				Logger::log(Logger::Level::DEBUG, "Selecting next view");
			}
			else if (in.m_Action == "mouseMoveHori")
			{
				viewRot[0] += in.m_Value * sensitivity;
				if (viewRot[0] > pi)
				{
					viewRot[0] -= 2 * (float)pi;
				}
				else if (viewRot[0] < -pi)
				{
					viewRot[0] += 2 * (float)pi;
				}
			}
			else if (in.m_Action == "mouseMoveVert")
			{
				viewRot[1] += in.m_Value * sensitivity;
				if (viewRot[1] > pi * 0.45f)
				{
					viewRot[1] = (float)pi * 0.45f;
				}
				else if (viewRot[1] < -pi * 0.45f)
				{
					viewRot[1] = -(float)pi * 0.45f;
				}
			}
			else if( in.m_Action == "jump" && in.m_Value == 1)
			{
				m_Player.setJump();
			}
			else if (in.m_Action == "toggleIK" && in.m_Value == 1.f)
			{
				useIK_OnIK_Worm = !useIK_OnIK_Worm;
			}
		}
		
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
				case PackageType::ADD_OBJECT:
					{
						AddObjectData data = conn->getAddObjectData(package);
						std::ostringstream msg;
						msg << "Adding object at (" 
							<< data.m_Position[0] << ", "
							<< data.m_Position[1] << ", " 
							<< data.m_Position[2] << ")";
						Logger::log(Logger::Level::INFO, msg.str());
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

	m_Graphics->eraseModelInstance(ground);
	for (int box : boxIds)
	{
		m_Graphics->eraseModelInstance(box);
	}
}

void BaseGameApp::shutdown()
{
	Logger::log(Logger::Level::INFO, "Shutting down the game app");
	
	m_Level.releaseLevel();

	IPhysics::deletePhysics(m_Physics);
	m_Physics = nullptr;

	INetwork::deleteNetwork(m_Network);	
	m_Network = nullptr;
	
	for (int i : m_ResourceIDs)
	{
		m_ResourceManager->releaseResource(i);
	}



	m_ResourceIDs.clear();
	delete m_ResourceManager;

	m_InputQueue.destroy();
	
	IGraphics::deleteGraphics(m_Graphics);
	m_Graphics = nullptr;

	m_Window.destroy();
}

std::string BaseGameApp::getGameTitle() const
{
	return m_GameTitle;
}

XMFLOAT2 BaseGameApp::getWindowSize() const
{
	// TODO: Read from user option
	
	const static XMFLOAT2 size = XMFLOAT2(1280, 720);
	return size;
}

bool BaseGameApp::handleWindowClose(WPARAM /*p_WParam*/, LPARAM /*p_LParam*/, LRESULT& p_Result)
{
	Logger::log(Logger::Level::DEBUG, "Handling window close");

	m_ShouldQuit = true;
	p_Result = 0;
	return true;
}

void BaseGameApp::connectedCallback(Result p_Res, void* p_UserData)
{
	if (p_Res == Result::SUCCESS)
	{
		((BaseGameApp*)p_UserData)->m_Connected = true;
		Logger::log(Logger::Level::INFO, "Connected successfully");
	}
	else
	{
		Logger::log(Logger::Level::WARNING, "Connection failed");
	}
}

void BaseGameApp::updateDebugInfo(float p_dt)
{
	m_TimeToNextMemUpdate -= p_dt;
	if (m_TimeToNextMemUpdate > 0.f)
	{
		return;
	}

	m_TimeToNextMemUpdate = m_MemUpdateDelay;

	std::string vMemUsage = "Virtual MemUsage: " + std::to_string(m_MemoryInfo.getVirtualMemoryUsage()) + "MB";
	std::string pMemUsage = "Physical MemUsage: " + std::to_string(m_MemoryInfo.getPhysicalMemoryUsage()) + "MB";
	std::string gMemUsage = "Video MemUsage: " + std::to_string(m_Graphics->getVRAMMemUsage()) + "MB";

	std::string speed = "DeltaTime: " + std::to_string(p_dt) + " FPS: " + std::to_string(1.0f/p_dt);

	m_Window.setTitle(getGameTitle() + " | " + vMemUsage + " " + pMemUsage + " " + gMemUsage + " " + speed);
}
