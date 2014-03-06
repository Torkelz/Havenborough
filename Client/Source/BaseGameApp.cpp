#include "BaseGameApp.h"

#include <Logger.h>
#include "Settings.h"
#include <TweakCommand.h>
#include "Scenes/HUDScene.h"
#include "Scenes/GameScene.h"

#include <iomanip>
#include <memory>
#include <sstream>

using namespace DirectX;

const std::string BaseGameApp::m_GameTitle = "The Apprentice of Havenborough";

BaseGameApp::BaseGameApp()
	:	m_ActorFactory(0x10000)
{
}

void BaseGameApp::init()
{
	Logger::log(Logger::Level::INFO, "Initializing game app");
	Settings settings;
	settings.initialize("UserOptions.xml");


	TweakSettings::initializeMaster();

	m_GameLogic = nullptr;

	m_MemUpdateDelay = 0.1f;
	m_TimeToNextMemUpdate = 0.f;
	m_TimeModifier = 1.f;
	
	m_Window.init(getGameTitle(), settings.getResolution());
	
	m_Graphics = IGraphics::createGraphics();
	m_Graphics->setLogFunction(&Logger::logRaw);
	m_Graphics->setTweaker(TweakSettings::getInstance());
	m_Graphics->setShadowMapResolution((int)settings.getSettingValue("ShadowMap"));
	m_Graphics->initialize(m_Window.getHandle(), (int)m_Window.getSize().x, (int)m_Window.getSize().y,
		settings.getIsSettingEnabled("Fullscreen"), settings.getSettingValue("FOV"));

	m_Graphics->enableSSAO(settings.getIsSettingEnabled("SSAO"));
	m_Graphics->enableShadowMap(settings.getIsSettingEnabled("ShadowMap"));
	m_Graphics->enableVsync(settings.getIsSettingEnabled("VSync"));

	m_Window.registerCallback(WM_CLOSE, std::bind(&BaseGameApp::handleWindowClose, this, std::placeholders::_1,
		std::placeholders::_2, std::placeholders::_3));
	m_Window.registerCallback(WM_EXITSIZEMOVE, std::bind(&BaseGameApp::handleWindowExitSizeMove, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	m_Window.registerCallback(WM_SIZE, std::bind(&BaseGameApp::handleWindowSize, this, std::placeholders::_1,
		std::placeholders::_2, std::placeholders::_3));

	m_Physics = IPhysics::createPhysics();
	m_Physics->setLogFunction(&Logger::logRaw);
	m_Physics->initialize(false);

	m_AnimationLoader.reset(new AnimationLoader);
	m_SpellFactory.reset(new SpellFactory());

	m_ResourceManager.reset(new ResourceManager());
	m_Sound = ISound::createSound();
	m_Sound->setLogFunction(&Logger::logRaw);
	m_Sound->initialize();

	using std::placeholders::_1;
	using std::placeholders::_2;
	m_Graphics->setLoadModelTextureCallBack(&ResourceManager::loadModelTexture, m_ResourceManager.get());
	m_Graphics->setReleaseModelTextureCallBack(&ResourceManager::releaseModelTexture, m_ResourceManager.get());

	m_ResourceManager->registerFunction("model",
		std::bind(&IGraphics::createModel, m_Graphics, _1, _2),
		std::bind(&IGraphics::releaseModel, m_Graphics, _1) );
	m_ResourceManager->registerFunction("texture",
		std::bind(&IGraphics::createTexture, m_Graphics, _1, _2),
		std::bind(&IGraphics::releaseTexture, m_Graphics, _1));
	m_ResourceManager->registerFunction("volume",
		std::bind(&IPhysics::createBV, m_Physics, _1, _2),
		std::bind(&IPhysics::releaseBV, m_Physics, _1));
	m_ResourceManager->registerFunction("sound",
		std::bind(&ISound::loadSound, m_Sound, _1, _2),
		std::bind(&ISound::releaseSound, m_Sound, _1));
	m_ResourceManager->registerFunction("particleSystem",
		std::bind(&IGraphics::createParticleEffectDefinition, m_Graphics, _1, _2),
		std::bind(&IGraphics::releaseParticleEffectDefinition, m_Graphics, _1));
	m_ResourceManager->registerFunction("spell",
		std::bind(&SpellFactory::createSpellDefinition, m_SpellFactory.get(), _1, _2),
		std::bind(&SpellFactory::releaseSpellDefinition, m_SpellFactory.get(), _1));
	m_ResourceManager->registerFunction("animation",
		std::bind(&AnimationLoader::loadAnimationDataResource, m_AnimationLoader.get(), _1, _2),
		std::bind(&AnimationLoader::releaseAnimationData, m_AnimationLoader.get(), _1));
	m_ResourceManager->loadDataFromFile("assets\\Resources.xml");

	InputTranslator::ptr translator(new InputTranslator);
	translator->init(&m_Window);

	Logger::log(Logger::Level::DEBUG_L, "Adding input mappings");
	translator->addKeyboardMapping('0', "slowMode");
	translator->addKeyboardMapping('1', "fastMode");

	//Adding the loaded keymaps to the translator
	const std::map<std::string, unsigned short> keys = settings.getKeyMap();
	for(auto k : keys)
		translator->addKeyboardMapping(k.second, k.first);

	//Adding the loaded mousemaps to the translator
	const std::vector<Settings::MouseStruct> mousekeys = settings.getMouseMap();
	for(auto k : mousekeys)
		translator->addMouseMapping(k.axis, k.position, k.movement);

	//Adding the loaded mousebuttonmaps to the translator
	const std::map<std::string, MouseButton> mousebuttonKeys = settings.getMouseButtonMap();
	for(auto k : mousebuttonKeys)
		translator->addMouseButtonMapping(k.second, k.first);

	m_InputQueue.init(std::move(translator));

	m_Network = INetwork::createNetwork();
	m_Network->setLogFunction(&Logger::logRaw);
	m_Network->initialize();

	m_EventManager.reset(new EventManager());

	m_EventManager->addListener(EventListenerDelegate(&m_InputQueue, &Input::lockMouse), MouseEventDataLock::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &BaseGameApp::showMouse), MouseEventDataShow::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &BaseGameApp::startGame), GameStartedEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &BaseGameApp::quitGame), QuitGameEventData::sk_EventType);

	m_GameLogic.reset(new GameLogic());
	m_SceneManager.init(m_Graphics, m_ResourceManager.get(), &m_InputQueue, m_GameLogic.get(), m_EventManager.get());
	Vector2 resolution(m_Window.getSize().x, m_Window.getSize().y);
	((HUDScene*)m_SceneManager.getScene(RunScenes::GAMEHUD).get())->setHUDSettings(settings.getHUDSettings(), resolution);
	((GameScene*)m_SceneManager.getScene(RunScenes::GAMEMAIN).get())->setMouseSensitivity(settings.getSettingValue("MouseSensitivity"));
	m_MemoryInfo.update();
	
	m_ActorFactory.setPhysics(m_Physics);
	m_ActorFactory.setEventManager(m_EventManager.get());
	m_ActorFactory.setResourceManager(m_ResourceManager.get());
	m_ActorFactory.setAnimationLoader(m_AnimationLoader.get());
	m_ActorFactory.setSpellFactory(m_SpellFactory.get());

	m_GameLogic->initialize(m_ResourceManager.get(), m_Physics, &m_ActorFactory, m_EventManager.get(), m_Network);

	// Set Current Size
	m_NewWindowSize = m_Window.getSize();

	m_CommandManager.reset(new CommandManager);
	m_CommandManager->registerCommand(Command::ptr(new TweakCommand));
	m_ConsoleReader.reset(new StreamReader(m_CommandManager, std::cin));

	m_ServerURL = settings.getServerURL();
	m_ServerPort = settings.getServerPort();
	m_LevelName = settings.getLevelName();
	m_Username = settings.getUsername();
}

void BaseGameApp::run()
{
	Logger::log(Logger::Level::INFO, "Running the game");

	m_ShouldQuit = false;

	resetTimer();

	m_GameLogic->connectToServer(m_ServerURL, m_ServerPort, m_LevelName, m_Username);

	while (!m_ShouldQuit)
	{
		Logger::log(Logger::Level::TRACE, "New frame");

		m_ConsoleReader->handleInput();
		m_InputQueue.onFrame();
		m_Window.pollMessages();

		handleInput();

		updateLogic();

		render();
		
		m_MemoryInfo.update();
		updateDebugInfo();
	}
}

void BaseGameApp::shutdown()
{
	Logger::log(Logger::Level::INFO, "Shutting down the game app");

	m_ResourceManager->setReleaseImmediately(true);

	INetwork::deleteNetwork(m_Network);	
	m_Network = nullptr;
	
	m_GameLogic->shutdown();
	m_GameLogic.reset();

	m_EventManager->processEvents();
	
	m_SceneManager.destroy();

	m_ResourceManager->unregisterResourceType("spell");
	m_SpellFactory.reset();

	m_ResourceManager->unregisterResourceType("animation");
	m_AnimationLoader.reset();

	m_InputQueue.destroy();
	
	m_ResourceManager->unregisterResourceType("volume");
	IPhysics::deletePhysics(m_Physics);
	m_Physics = nullptr;

	m_ResourceManager->unregisterResourceType("sound");
	ISound::deleteSound(m_Sound);
	m_Sound = nullptr;

	m_ResourceManager->unregisterResourceType("model");
	m_ResourceManager->unregisterResourceType("particleSystem");
	m_ResourceManager->unregisterResourceType("texture");
	IGraphics::deleteGraphics(m_Graphics);
	m_Graphics = nullptr;

	m_ResourceManager.reset();

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
bool BaseGameApp::handleWindowExitSizeMove(WPARAM /*p_WParam*/, LPARAM p_LParam, LRESULT& p_Result)
{
	Logger::log(Logger::Level::DEBUG_L, "Handling window when the user releases the resize bars.");

	m_Window.setSize(m_NewWindowSize);				

	p_Result = 0;
	return true;
}
bool BaseGameApp::handleWindowSize(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result)
{
	Logger::log(Logger::Level::DEBUG_L, "Handling window when the user resizes the window.");

	m_NewWindowSize = DirectX::XMFLOAT2(LOWORD(p_LParam),HIWORD(p_LParam));

	switch(p_WParam)
	{
	case SIZE_MAXIMIZED:
		{
			m_Window.setSize(m_NewWindowSize);
			m_Window.setIsMaximized(true);
			p_Result = 0;
			return true;
		}
	case SIZE_MAXHIDE:{return false;}
	case SIZE_MAXSHOW:{return false;}
	case SIZE_MINIMIZED:{return false;}
	case SIZE_RESTORED:
		{
			if(m_Window.getIsMaximized())
			{
				m_Window.setIsMaximized(false);
				m_Window.setSize(m_NewWindowSize);
				p_Result = 0;
				return true;
			}
			return false;
		}
	default:
		return false;
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

	std::string test = std::to_string(m_MemoryInfo.getPhysicalMemoryUsage());
	unsigned int VRUsage = m_MemoryInfo.getVirtualMemoryUsage();
	unsigned int PRUsage = m_MemoryInfo.getPhysicalMemoryUsage();
	unsigned int VUsage = m_Graphics->getVRAMUsage();
	unsigned int BToMB = 1024 * 1024;
	std::string vMemUsage = std::to_string(VRUsage) + "B" + " (" + std::to_string(VRUsage/BToMB) + "MB)";
	std::string pMemUsage = std::to_string(PRUsage) + "B" + " (" + std::to_string(PRUsage/BToMB) + "MB)";
	std::string gMemUsage = std::to_string(VUsage) + "B" + " (" + std::to_string(VUsage/BToMB) + "MB)";

	std::shared_ptr<HUDScene> hud_Scene = std::dynamic_pointer_cast<HUDScene>(m_SceneManager.getScene(RunScenes::GAMEHUD));
	if (hud_Scene)
	{
		DebugInfo& info = hud_Scene->getDebugInfo();

		info.updateDebugInfo("Virtual RAM", vMemUsage);
		info.updateDebugInfo("Physical RAM", pMemUsage);
		info.updateDebugInfo("Video RAM", gMemUsage);

		char buffer[10];
		std::sprintf(buffer, "%.1f", 1.0f / m_DeltaTime);
		info.updateDebugInfo("FPS", buffer);
		std::sprintf(buffer, "%.1f ms", m_DeltaTime * 1000.f);
		info.updateDebugInfo("DeltaTime", buffer);
	}
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
	m_DeltaTime = ((m_CurrTimeStamp - m_PrevTimeStamp) * m_SecsPerCnt) / m_TimeModifier;
	static const float maxDeltaTime = 1.f / 24.f; // Up from 5.f. Animations start behaving wierd if frame rate drops below 24. 
	if (m_DeltaTime > maxDeltaTime)
	{
		Logger::log(Logger::Level::WARNING, "Computer too slow or something");
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
		m_SceneManager.registeredInput(in.m_Action, in.m_Value, in.m_PrevValue);

		if (in.m_Value > 0.5f && in.m_PrevValue <= 0.5f)
		{
			if (in.m_Action == "slowMode")
			{
				if (m_TimeModifier <= 1.f)
				{
					m_TimeModifier = 10.f;
				}
				else
				{
					m_TimeModifier = 1.f;
				}
			}
			else if(in.m_Action == "fastMode")
			{
				if (m_TimeModifier >= 1.0f)
					m_TimeModifier = 0.1f;
				else m_TimeModifier = 1.0f;
			}
		}
	}
}

void BaseGameApp::updateLogic()
{
	updateTimer();

	m_SceneManager.onFrame(m_DeltaTime);
	m_GameLogic->onFrame(m_DeltaTime);

	m_EventManager->processEvents();
}

void BaseGameApp::render()
{
	m_SceneManager.render();
	m_Graphics->drawFrame();
}

void BaseGameApp::startGame(IEventData::Ptr p_Data)
{
	m_SceneManager.startRun();
}

void BaseGameApp::quitGame(IEventData::Ptr p_Data)
{
	m_ShouldQuit = true;
}

void BaseGameApp::showMouse(IEventData::Ptr p_Data)
{
	std::shared_ptr<MouseEventDataShow> data = std::static_pointer_cast<MouseEventDataShow>(p_Data);
	m_Window.setShowCursor(data->getShowState());
}
