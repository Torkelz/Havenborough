#include "BaseGameApp.h"

const std::string BaseGameApp::m_GameTitle = "The Apprentice of Havenborough";

void BaseGameApp::init()
{
	m_Window.init(getGameTitle(), getWindowSize());
	m_Window.registerCallback(WM_KEYDOWN, std::bind<bool>(&BaseGameApp::handleKeyDown, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	m_Graphics = IGraphics::createGraphics();
	//TODO: Need some input setting variable to handle fullscreen.
	bool fullscreen = true;
	m_Graphics->initialize(m_Window.getHandle(), m_Window.getSize().x, m_Window.getSize().y, fullscreen);
}

void BaseGameApp::run()
{
	m_ShouldQuit = false;

	while (!m_ShouldQuit)
	{
		m_Window.pollMessages();
		m_Graphics->drawFrame();
	}
}

void BaseGameApp::shutdown()
{
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

bool BaseGameApp::handleKeyDown(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result)
{
	if (p_WParam == VK_ESCAPE)
	{
		m_ShouldQuit = true;

		p_Result = 0;
		return true;
	}

	return false;
}
