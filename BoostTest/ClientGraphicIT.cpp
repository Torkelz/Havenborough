#include <boost/test/unit_test.hpp>
#include "..\Client\Source\BaseGameApp.h"

class DummyBaseGameApp : public BaseGameApp
{
public:
	Window m_Window;
	IGraphics *m_Graphics;

public:
	DummyBaseGameApp()
		: m_Graphics(nullptr) {}

	bool init2()
	{
		m_Window.init(getGameTitle(), getWindowSize());
		m_Graphics = IGraphics::createGraphics();
		
		bool fullscreen = false;
		if(!m_Graphics->initialize(m_Window.getHandle(), m_Window.getSize().x, m_Window.getSize().y, fullscreen))
			return false;

		return true;
	}
	
	void shutdown()
	{
		if (m_Graphics)
		{
			IGraphics::deleteGraphics(m_Graphics);
			m_Graphics = nullptr;
		}

		m_Window.destroy();
	}
};

BOOST_AUTO_TEST_SUITE(ClientGraphicIT)

BOOST_AUTO_TEST_CASE(CGIT)
{
	DummyBaseGameApp fakeBaseGameApp;
	//BOOST_MESSAGE("TESTING"); WTF IS THIS SHITE
	BOOST_CHECK_EQUAL(fakeBaseGameApp.init2(), true);
	BOOST_CHECK(fakeBaseGameApp.m_Window.getHandle() != nullptr);
	
	fakeBaseGameApp.shutdown();
}


BOOST_AUTO_TEST_SUITE_END()
