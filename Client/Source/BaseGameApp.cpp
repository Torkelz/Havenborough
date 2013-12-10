#include "BaseGameApp.h"
#include "Input\InputTranslator.h"

const std::string BaseGameApp::m_GameTitle = "The Apprentice of Havenborough";

void BaseGameApp::init(std::string p_ProjectDirectory)
{
	m_SceneManager.init();
	m_Window.init(getGameTitle(), getWindowSize());

	m_Graphics = IGraphics::createGraphics();
	//TODO: Need some input setting variable to handle fullscreen.
	bool fullscreen = false;
	m_Graphics->initialize(m_Window.getHandle(), m_Window.getSize().x, m_Window.getSize().y, fullscreen);
	m_Window.registerCallback(WM_CLOSE, std::bind(&BaseGameApp::handleWindowClose, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	m_ResourceManager = new ResourceManager(p_ProjectDirectory);
	m_ResourceManager->registerFunction( "model", std::bind(&IGraphics::createModel, m_Graphics, std::placeholders::_1, std::placeholders::_2), std::bind(&IGraphics::releaseModel, m_Graphics, std::placeholders::_1) );

	m_ResourceManager->loadResource("model", "DZALA");
	m_ResourceManager->loadResource("model", "DZALA");
	m_ResourceManager->loadResource("model", "MARIM");

	m_ResourceManager->releaseResource(0);

	InputTranslator::ptr translator(new InputTranslator);
	translator->init(&m_Window);
	translator->addKeyboardMapping(VK_ESCAPE, "exit");
	translator->addKeyboardMapping('W', "moveForward");
	translator->addKeyboardMapping('S', "moveBackward");
	translator->addKeyboardMapping('A', "moveLeft");
	translator->addKeyboardMapping('D', "moveRight");
	translator->addKeyboardMapping('C', "connect");

	translator->addMouseMapping(InputTranslator::Axis::HORIZONTAL, "mousePosHori", "mouseMoveHori");
	translator->addMouseMapping(InputTranslator::Axis::VERTICAL, "mousePosVert", "mouseMoveVert");

	translator->addMouseButtonMapping(InputTranslator::MouseButton::LEFT, "gogogogo");
	translator->addMouseButtonMapping(InputTranslator::MouseButton::MIDDLE, "rollMe!");

	m_InputQueue.init(std::move(translator));

	m_Physics = IPhysics::createPhysics();

	m_Body = m_Physics->createSphere(50.f, false, Vector3(0.f, 5.f, 0.f), 1.f);
	m_Object = m_Physics->createSphere(50.f, true, Vector3(0.f, 0.f, 0.f), 1.f);

	dt = (1.f/60.f);
}

void BaseGameApp::run()
{
	m_ShouldQuit = false;
	while (!m_ShouldQuit)
	{
		m_Physics->update(dt);

		for(unsigned int i = 0; i < m_Physics->getHitDataSize(); i++)
		{
			HitData hit = m_Physics->getHitDataAt(i);
			if(hit.intersect)
			{
				int i = 0;
			}
		}

		m_InputQueue.onFrame();
		m_Window.pollMessages();
		m_Graphics->drawFrame();

		for (auto& in : m_InputQueue.getFrameInputs())
		{
			if (in.m_Action == "exit")
			{
				m_ShouldQuit = true;
			}
			else if (in.m_Action == "connect" && in.m_Value == 1.0f)
			{
				m_Network.connect("localhost");
			}
			else
			{
				//printf("Received input action: %s (%.2f)\n", in.m_Action.c_str(), in.m_Value);
			}
		}
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
	
	const static UVec2 size = {800, 480};
	return size;
}

bool BaseGameApp::handleWindowClose(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result)
{
	m_ShouldQuit = true;
	p_Result = 0;
	return true;
}
