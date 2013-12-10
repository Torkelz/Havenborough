#include "BaseGameApp.h"
#include "Input\InputTranslator.h"

const double pi = 3.14159265358979323846264338;

const std::string BaseGameApp::m_GameTitle = "The Apprentice of Havenborough";

void BaseGameApp::init()
{
	m_SceneManager.init();
	m_Window.init(getGameTitle(), getWindowSize());
	m_Graphics = IGraphics::createGraphics();
	//TODO: Need some input setting variable to handle fullscreen.
	bool fullscreen = false;
	m_Graphics->initialize(m_Window.getHandle(), m_Window.getSize().x, m_Window.getSize().y, fullscreen);
	m_Window.registerCallback(WM_CLOSE, std::bind(&BaseGameApp::handleWindowClose, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	InputTranslator::ptr translator(new InputTranslator);
	translator->init(&m_Window);
	translator->addKeyboardMapping(VK_ESCAPE, "exit");
	translator->addKeyboardMapping('W', "moveForward");
	translator->addKeyboardMapping('S', "moveBackward");
	translator->addKeyboardMapping('A', "moveLeft");
	translator->addKeyboardMapping('D', "moveRight");
	translator->addKeyboardMapping('C', "connect");
	translator->addKeyboardMapping('Z', "changeViewN");
	translator->addKeyboardMapping('X', "changeViewP");

	translator->addMouseMapping(InputTranslator::Axis::HORIZONTAL, "mousePosHori", "mouseMoveHori");
	translator->addMouseMapping(InputTranslator::Axis::VERTICAL, "mousePosVert", "mouseMoveVert");

	translator->addMouseButtonMapping(InputTranslator::MouseButton::LEFT, "gogogogo");
	translator->addMouseButtonMapping(InputTranslator::MouseButton::MIDDLE, "rollMe!");

	m_InputQueue.init(std::move(translator));
	m_Network = INetwork::createNetwork();
	m_Connected = false;
	
	m_Physics = IPhysics::createPhysics();

	m_Body = m_Physics->createSphere(50.f, false, Vector3(0.f, 5.f, 0.f), 1.f);
	m_Object = m_Physics->createSphere(50.f, true, Vector3(0.f, 0.f, 0.f), 1.f);
	
	m_Graphics->createModel("BOX", "../../Graphics/Resources/Sample135.tx", false);
	m_Graphics->createShader("BOXShader", L"../../Graphics/Source/DeferredShaders/GeometryPass.hlsl",
							"VS,PS","5_0", IGraphics::ShaderType::VERTEX_SHADER | IGraphics::ShaderType::PIXEL_SHADER);
	m_Graphics->linkShaderToModel("BOXShader", "BOX");

	m_Graphics->createModel("skyBox", "assets/SkyBox.tx", false);
	m_Graphics->linkShaderToModel("BOXShader", "skyBox");

	m_Graphics->createModel("house1", "assets/House1/House1.tx", false);
	m_Graphics->linkShaderToModel("BOXShader", "house1");

	m_Graphics->createModel("Test", "assets/Sample165_Animated_Legs.tx", true);
	m_Graphics->createShader("AnimatedShader", L"../../Graphics/Source/DeferredShaders/AnimatedGeometryPass.hlsl",
							"VS,PS","5_0", IGraphics::ShaderType::VERTEX_SHADER | IGraphics::ShaderType::PIXEL_SHADER);
	m_Graphics->linkShaderToModel("AnimatedShader", "Test");
}

void BaseGameApp::run()
{
	m_ShouldQuit = false;

	int currView = 3; // FOR DEBUGGING

	//BodyHandle groundBody = m_Physics->createAABB(1.f, true, Vector3(-20.f, -1.f, -20.f), Vector3(20.f, 0.f, 20.f));
	//BodyHandle playerBody = m_Physics->createSphere(80.f, false, Vector3(0.f, 1.8f, 20.f), 1.8f);

	const static int NUM_BOXES = 16;
	int boxIds[NUM_BOXES];
	for (int i = 0; i < NUM_BOXES; i++)
	{
		boxIds[i] = m_Graphics->createModelInstance("BOX");

		const float scale = 1.f + i * 3.f / NUM_BOXES;
		m_Graphics->setModelScale(boxIds[i], scale, scale, scale);
		m_Graphics->setModelPosition(boxIds[i], (float)(i / 4) * 4.f, 1.f, (float)(i % 4) * 4.f);
	}
	
	int skyBox = m_Graphics->createModelInstance("skyBox");
	m_Graphics->setModelScale(skyBox, 0.1f, 0.1f, 0.1f);

	int ground = m_Graphics->createModelInstance("BOX");
	m_Graphics->setModelScale(ground, 100.f, 0.0001f, 100.f);

	int house = m_Graphics->createModelInstance("house1");
	m_Graphics->setModelPosition(house, -10.f, 0.f, -10.f);
	m_Graphics->setModelScale(house, 0.01f, 0.01f, 0.01f);

	int cactus = m_Graphics->createModelInstance("Test");
	//m_Graphics->setModelPosition(cactus, 10.f, 0.f, -10.f);
	//m_Graphics->setModelScale(cactus, 1.0f, 1.0f, 1.0f);

	//int witch = m_Graphics->createModelInstance("Dzala");
	//m_Graphics->setModelPosition(witch, 10.f, 0.f, -10.f);
	//m_Graphics->setModelScale(witch, 0.01f, 0.01f, 0.01f);

	float position[] = {0.f, 1.6f, 20.f};
	float viewRot[] = {0.f, 0.f};

	float speed = 5.f;
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
		prevTimeStamp = currTimeStamp;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
		float dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;

		m_Physics->update(dt);

		for(unsigned int i = 0; i < m_Physics->getHitDataSize(); i++)
		{
			HitData hit = m_Physics->getHitDataAt(i);
			if(hit.intersect)
			{
				int i = 0;
				//hit.
			}
		}

		const InputState& state = m_InputQueue.getCurrentState();

		float forward = state.getValue("moveForward") - state.getValue("moveBackward");
		float right = state.getValue("moveRight") - state.getValue("moveLeft");

		if (forward != 0.f || right != 0.f)
		{
			float dir = atan2f(right, forward) + viewRot[0];

			position[2] -= cosf(dir) * speed * dt;
			position[0] -= sinf(dir) * speed * dt;
		}

		m_Graphics->updateCamera(position[0], position[1], position[2], viewRot[0], viewRot[1]);
		m_Graphics->setModelPosition(skyBox, position[0], position[1], position[2]);

		yaw += yawSpeed * dt;
		pitch += pitchSpeed * dt;
		roll += rollSpeed * dt;

		//for (int i = 0; i < NUM_BOXES; i++)
		//{
		//	m_Graphics->setModelRotation(boxIds[i], yaw * i, pitch * i, roll * i);
		//	m_Graphics->renderModel(boxIds[i]);
		//}
		//m_Graphics->renderModel(ground);
		//m_Graphics->renderModel(skyBox);
		//m_Graphics->renderModel(house);
		m_Graphics->renderModel(cactus);
		//m_Graphics->renderModel(witch);

		m_Graphics->drawFrame(currView);
		
		m_MemoryInfo.update();
		
		updateDebugInfo(dt);

		m_InputQueue.onFrame();
		m_Window.pollMessages();

		for (auto& in : m_InputQueue.getFrameInputs())
		{
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
				printf("Changeview--\n", in.m_Action.c_str(), in.m_Value);
			}
			else if(in.m_Action ==  "changeViewP" && in.m_Value == 1)
			{
				currView++;
				if(currView >= 4)
					currView = 0;
				printf("Changeview++\n", in.m_Action.c_str(), in.m_Value);
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
			else if (in.m_Action == "mousePosHori")
			{
			}
			else if (in.m_Action == "mousePosVert")
			{
			}
			else
			{
				printf("Received input action: %s (%.2f)\n", in.m_Action.c_str(), in.m_Value);
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

				switch (type)
				{
				case PackageType::ADD_OBJECT:
					{
						AddObjectData data = conn->getAddObjectData(package);
						std::cout << "Adding object at (" 
							<< data.m_Position[0] << ", "
							<< data.m_Position[1] << ", " 
							<< data.m_Position[2] << ")" << std::endl;
					}
					break;

				default:
					std::cout << "Received unhandled package" << std::endl;
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
	m_InputQueue.destroy();
	
	IGraphics::deleteGraphics(m_Graphics);
	m_Graphics = nullptr;

	m_Window.destroy();
}

std::string BaseGameApp::getGameTitle() const
{
	return m_GameTitle;
}

UVec2 BaseGameApp::getWindowSize() const
{
	// TODO: Read from user option
	
	const static UVec2 size = {1280, 720};
	return size;
}

bool BaseGameApp::handleWindowClose(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result)
{
	m_ShouldQuit = true;
	p_Result = 0;
	return true;
}

void BaseGameApp::connectedCallback(Result p_Res, void* p_UserData)
{
	if (p_Res == Result::SUCCESS)
	{
		((BaseGameApp*)p_UserData)->m_Connected = true;
		std::cout << "Connected successfully" << std::endl;
	}
	else
	{
		std::cout << "Connection failed" << std::endl;
	}
}

void BaseGameApp::updateDebugInfo(float p_dt)
{
	std::string vMemUsage = "Virtual MemUsage: " + std::to_string(m_MemoryInfo.getVirtualMemoryUsage()) + "MB";
	std::string pMemUsage = "Physical MemUsage: " + std::to_string(m_MemoryInfo.getPhysicalMemoryUsage()) + "MB";
	std::string gMemUsage = "Video MemUsage: " + std::to_string(m_Graphics->getVRAMMemUsage()) + "MB";

	std::string speed = "DeltaTime: " + std::to_string(p_dt) + " FPS: " + std::to_string(1.0f/p_dt);

	m_Window.setTitle(getGameTitle() + " | " + vMemUsage + " " + pMemUsage + " " + gMemUsage + " " + speed);
}
