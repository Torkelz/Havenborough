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
	translator->addKeyboardMapping('C', "connect");
	translator->addKeyboardMapping('Z', "changeViewN");
	translator->addKeyboardMapping('X', "changeViewP");
	translator->addKeyboardMapping('I', "toggleIK");
	translator->addKeyboardMapping(VK_SPACE, "jump");

	translator->addKeyboardMapping('J', "changeSceneP");
	translator->addKeyboardMapping('K', "pauseScene");
	translator->addKeyboardMapping('L', "changeSceneN");
	
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

	m_SceneManager.init(m_Graphics, m_ResourceManager, m_Physics, &m_InputQueue);
	
	m_ResourceManager->registerFunction( "volume", std::bind(&IPhysics::createLevelBV, m_Physics, _1, _2), std::bind(&IPhysics::releaseLevelBV, m_Physics, _1));
				
	m_MemoryInfo.update();
}

void BaseGameApp::run()
{
	Logger::log(Logger::Level::INFO, "Running the game");

	m_ShouldQuit = false;
	
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


		m_SceneManager.onFrame(dt);
		m_SceneManager.render();
		
		m_MemoryInfo.update();
		updateDebugInfo(dt);

		m_InputQueue.onFrame();
		m_Window.pollMessages();

		for (auto& in : m_InputQueue.getFrameInputs())
		{
			std::ostringstream msg;
			msg << "Received input action: " << in.m_Action << " (" << std::setprecision(2) << std::fixed << in.m_Value << ")";
			Logger::log(Logger::Level::TRACE, msg.str());

			// Pass keystrokes to all active scenes.
			m_SceneManager.registeredInput(in.m_Action, in.m_Value);

			if (in.m_Action == "exit")
			{
				m_ShouldQuit = true;
			}
			else if (in.m_Action == "connect" && in.m_Value == 1.0f)
			{
				m_Connected = false;
				m_Network->connectToServer("localhost", 31415, &connectedCallback, this);
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
				case PackageType::CREATE_OBJECTS:
					{
						unsigned int numInstances = conn->getNumCreateObjectInstances(package);
						const ObjectInstance* instances = conn->getCreateObjectInstances(package);
						for (unsigned int i = 0; i < numInstances; ++i)
						{
							ObjectInstance data = instances[i];
							std::ostringstream msg;
							msg << "Adding object at (" 
								<< data.m_Position[0] << ", "
								<< data.m_Position[1] << ", " 
								<< data.m_Position[2] << ")";
							Logger::log(Logger::Level::INFO, msg.str());
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
}

void BaseGameApp::shutdown()
{
	Logger::log(Logger::Level::INFO, "Shutting down the game app");
	
	IPhysics::deletePhysics(m_Physics);
	m_Physics = nullptr;

	INetwork::deleteNetwork(m_Network);	
	m_Network = nullptr;
	
	m_SceneManager.destroy();

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
