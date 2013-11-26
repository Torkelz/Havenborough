#include "BaseGameApp.h"

#include "Input\InputTranslator.h"

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
	m_InputQueue.init(std::move(translator));

	//physics = IPhysics::createPhysics();
}

void BaseGameApp::run()
{
	m_ShouldQuit = false;

	while (!m_ShouldQuit)
	{
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
				printf("Received input action: %s (%.1f)\n", in.m_Action.c_str(), in.m_Value);
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
