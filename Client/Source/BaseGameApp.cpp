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

	translator->addMouseMapping(InputTranslator::Axis::HORIZONTAL, "mousePosHori", "mouseMoveHori");
	translator->addMouseMapping(InputTranslator::Axis::VERTICAL, "mousePosVert", "mouseMoveVert");

	translator->addMouseButtonMapping(InputTranslator::MouseButton::LEFT, "gogogogo");
	translator->addMouseButtonMapping(InputTranslator::MouseButton::MIDDLE, "rollMe!");

	m_InputQueue.init(std::move(translator));
	m_Network = INetwork::createNetwork();
	m_Connected = false;
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
				m_Connected = false;
				m_Network->connectToServer("localhost", 31415, &connectedCallback, this);
			}
			else
			{
				//printf("Received input action: %s (%.2f)\n", in.m_Action.c_str(), in.m_Value);
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
