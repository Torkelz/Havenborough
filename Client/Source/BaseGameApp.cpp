#include "BaseGameApp.h"

#include "ActorFactory.h"
#include "Components.h"
#include "Input\InputTranslator.h"
#include "Logger.h"

#include <iomanip>
#include <sstream>

#include <tinyxml2.h>

const double pi = 3.14159265358979323846264338;

const std::string BaseGameApp::m_GameTitle = "The Apprentice of Havenborough";


void BaseGameApp::init()
{
	Logger::log(Logger::Level::INFO, "Initializing game app");
	//1:P
	m_GameLogic = nullptr;

	m_MemUpdateDelay = 0.1f;
	m_TimeToNextMemUpdate = 0.f;
	
	m_Window.init(getGameTitle(), getWindowSize());

	m_Graphics = IGraphics::createGraphics();
	m_Graphics->setLogFunction(&Logger::logRaw);
	
	//TODO: Need some input setting variable to handle fullscreen.
	bool fullscreen = false;
	m_Graphics->initialize(m_Window.getHandle(), (int)m_Window.getSize().x, (int)m_Window.getSize().y, fullscreen);
	m_Window.registerCallback(WM_CLOSE, std::bind(&BaseGameApp::handleWindowClose, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	m_Physics = IPhysics::createPhysics();
	m_Physics->setLogFunction(&Logger::logRaw);
	m_Physics->initialize();

	m_ResourceManager = new ResourceManager();
	using namespace std::placeholders;
	m_Graphics->setLoadModelTextureCallBack(&ResourceManager::loadModelTexture, m_ResourceManager);
	m_Graphics->setReleaseModelTextureCallBack(&ResourceManager::releaseModelTexture, m_ResourceManager);
	m_ResourceManager->registerFunction( "model", std::bind(&IGraphics::createModel, m_Graphics, _1, _2), std::bind(&IGraphics::releaseModel, m_Graphics, _1) );
	m_ResourceManager->registerFunction( "texture", std::bind(&IGraphics::createTexture, m_Graphics, _1, _2), std::bind(&IGraphics::releaseTexture, m_Graphics, _1));
	m_ResourceManager->registerFunction( "volume", std::bind(&IPhysics::createBV, m_Physics, _1, _2), std::bind(&IPhysics::releaseBV, m_Physics, _1));

	InputTranslator::ptr translator(new InputTranslator);
	translator->init(&m_Window);
	
	//TODO: This should be loaded from file
	Logger::log(Logger::Level::DEBUG_L, "Adding input mappings");
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
	translator->addKeyboardMapping('R', "releaseObject");

	translator->addKeyboardMapping('J', "changeSceneP");
	translator->addKeyboardMapping('K', "pauseScene");
	translator->addKeyboardMapping('L', "changeSceneN");
	translator->addKeyboardMapping(VK_RETURN, "goToMainMenu");

	translator->addMouseMapping(InputTranslator::Axis::HORIZONTAL, "mousePosHori", "mouseMoveHori");
	translator->addMouseMapping(InputTranslator::Axis::VERTICAL, "mousePosVert", "mouseMoveVert");

	translator->addMouseButtonMapping(InputTranslator::MouseButton::LEFT, "gogogogo");
	translator->addMouseButtonMapping(InputTranslator::MouseButton::MIDDLE, "rollMe!");

	m_InputQueue.init(std::move(translator));

	m_Network = INetwork::createNetwork();
	m_Network->setLogFunction(&Logger::logRaw);
	m_Network->initialize();
	m_Connected = false;	

	m_EventManager.reset(new EventManager());
	m_GameLogic.reset(new GameLogic());
	m_SceneManager.init(m_Graphics, m_ResourceManager, &m_InputQueue, m_GameLogic.get(), m_EventManager.get());
					
	m_MemoryInfo.update();
	
	m_ActorFactory.setPhysics(m_Physics);
	m_ActorFactory.setGraphics(m_Graphics);
	m_ActorFactory.setEventManager(m_EventManager.get());

	m_GameLogic->initialize(m_ResourceManager, m_Physics, &m_ActorFactory, m_EventManager.get());
}

void BaseGameApp::run()
{
	Logger::log(Logger::Level::INFO, "Running the game");

	m_ShouldQuit = false;

	resetTimer();

	while (!m_ShouldQuit)
	{
		Logger::log(Logger::Level::TRACE, "New frame");

		m_InputQueue.onFrame();
		m_Window.pollMessages();

		handleInput();
		handleNetwork();

		updateLogic();

		render();
		
		m_MemoryInfo.update();
		updateDebugInfo();
	}

	m_ServerActors.clear();
}

void BaseGameApp::shutdown()
{
	Logger::log(Logger::Level::INFO, "Shutting down the game app");
	
	m_GameLogic->shutdown();
	m_GameLogic.reset();

	INetwork::deleteNetwork(m_Network);	
	m_Network = nullptr;
	
	m_SceneManager.destroy();

	delete m_ResourceManager;

	m_InputQueue.destroy();
	
	IPhysics::deletePhysics(m_Physics);
	m_Physics = nullptr;

	IGraphics::deleteGraphics(m_Graphics);
	m_Graphics = nullptr;

	m_EventManager.reset();

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
	Logger::log(Logger::Level::DEBUG_L, "Handling window close");

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

void BaseGameApp::updateDebugInfo()
{
	m_TimeToNextMemUpdate -= m_DeltaTime;
	if (m_TimeToNextMemUpdate > 0.f)
	{
		return;
	}

	m_TimeToNextMemUpdate = m_MemUpdateDelay;

	std::string vMemUsage = "Virtual MemUsage: " + std::to_string(m_MemoryInfo.getVirtualMemoryUsage()) + "MB";
	std::string pMemUsage = "Physical MemUsage: " + std::to_string(m_MemoryInfo.getPhysicalMemoryUsage()) + "MB";
	std::string gMemUsage = "Video MemUsage: " + std::to_string(m_Graphics->getVRAMMemUsage()) + "MB";

	std::string speed = "DeltaTime: " + std::to_string(m_DeltaTime) + " FPS: " + std::to_string(1.0f/m_DeltaTime);

	m_Window.setTitle(getGameTitle() + " | " + vMemUsage + " " + pMemUsage + " " + gMemUsage + " " + speed);
}

void BaseGameApp::resetTimer()
{
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	m_SecsPerCnt = 1.0f / (float)cntsPerSec;

	m_PrevTimeStamp = 0;
	m_CurrTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_CurrTimeStamp);
	m_CurrTimeStamp--;
}

void BaseGameApp::updateTimer()
{
	m_PrevTimeStamp = m_CurrTimeStamp;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_CurrTimeStamp);
	m_DeltaTime = (m_CurrTimeStamp - m_PrevTimeStamp) * m_SecsPerCnt;
	static const float maxDeltaTime = 1.f / 5.f;
	if (m_DeltaTime > maxDeltaTime)
	{
		Logger::log(Logger::Level::WARNING, "Computer to slow or something");
		m_DeltaTime = maxDeltaTime;
	}
}

void BaseGameApp::handleInput()
{
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
		else if (in.m_Action == "releaseObject" && in.m_Value == 1.f)
		{
			IScene::ptr scene = m_SceneManager.getScene()[0];
			GameScene* gameScene = dynamic_cast<GameScene*>(scene.get());
			if (gameScene)
			{
				m_GameLogic->setPlayerActor(Actor::ptr());
			}
		}
	}
}

void BaseGameApp::handleNetwork()
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

						Actor::ptr actor = m_ActorFactory.createActor(obj, data.m_Id);
						actor->setPosition(Vector3(data.m_Position[0], data.m_Position[1], data.m_Position[2]));
						actor->setRotation(Vector3(data.m_Rotation[0], data.m_Rotation[1], data.m_Rotation[2]));
						m_ServerActors.push_back(actor);
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

						Actor::ptr actor;
						for (auto& act : m_ServerActors)
						{
							if (act->getId() == actorId)
							{
								actor = act;
								break;
							}
						}

						if (!actor)
						{
							Logger::log(Logger::Level::ERROR_L, "Could not find actor (" + std::to_string(actorId) + ")");
							continue;
						}

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

	IPhysics::deletePhysics(m_Physics);
	m_Physics = nullptr;

					if (std::string(action->Value()) == "Pulse")
					{
						for (auto& actor : m_ServerActors)
						{
							if (actor->getId())
							{
								std::shared_ptr<PulseInterface> pulseComp(actor->getComponent<PulseInterface>(4));
								if (pulseComp)
								{
									pulseComp->pulseOnce();
								}
								break;
							}
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
						IScene::ptr scene = m_SceneManager.getScene()[0];
						GameScene* gameScene = dynamic_cast<GameScene*>(scene.get());
						if (gameScene)
						{
							m_GameLogic->setPlayerActor(actor);
						}
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

void BaseGameApp::updateLogic()
{
	updateTimer();

	for (auto& actor : m_ServerActors)
	{
		actor->onUpdate(m_DeltaTime);
	}

	m_SceneManager.onFrame(m_DeltaTime);
	m_GameLogic->onFrame(m_DeltaTime);

	m_EventManager->processEvents();
}

void BaseGameApp::render()
{
	for (auto& actor : m_ServerActors)
	{
		auto weakGraphicsComponent = actor->getComponent<ModelInterface>(2);
		std::shared_ptr<ModelInterface> strongGraphicsComponent(weakGraphicsComponent);

		if (strongGraphicsComponent)
		{
			//strongGraphicsComponent->render(m_Graphics);
		}
	}

	m_SceneManager.render();
	m_Graphics->drawFrame();
}

Actor::ptr BaseGameApp::getActor(Actor::Id p_Actor)
{
	for (auto actor : m_ServerActors)
	{
		if (actor->getId() == p_Actor)
		{
			return actor;
		}
	}

	return Actor::ptr();
}

void BaseGameApp::removeActor(Actor::Id p_Actor)
{
	for (size_t i = 0; i < m_ServerActors.size(); ++i)
	{
		if (m_ServerActors[i]->getId() == p_Actor)
		{
			std::swap(m_ServerActors[i], m_ServerActors.back());
			m_ServerActors.pop_back();
			return;
		}
	}
}

