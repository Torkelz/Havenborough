#include <boost/test/unit_test.hpp>
#include "../../Client/Source/Window.h"

BOOST_AUTO_TEST_SUITE(WindowTests)

/*
 * Test cases for each get and set method in Window.h
 */
BOOST_AUTO_TEST_CASE(WindowTestGetSet)
{
	//Basic init
	Window win;
	win.setIsVisible(false);
	UVec2 size = {800,600};
	win.init("Havenborough", size);

	//Change title check
	std::string title = "hejsan";
	win.setTitle(title);
	BOOST_CHECK_EQUAL(win.getTitle(), title);

	//Change visible check
	bool isVisible = false;
	win.setIsVisible(isVisible);
	BOOST_CHECK_EQUAL(win.getIsVisible(), isVisible);

	//Change showCursor check
	bool showCursor = false;
	win.setShowCursor(showCursor);
	BOOST_CHECK_EQUAL(win.getShowCursor(), showCursor);

	//Change size check
	UVec2 newSize = {300,400};
	win.setSize(newSize);
	UVec2 retSize = win.getSize();
	BOOST_CHECK_EQUAL(retSize.x, newSize.x);
	BOOST_CHECK_EQUAL(retSize.y, newSize.y);

	//Change cursor check
	HICON newIcon = LoadCursor(NULL, IDC_HAND);
	win.setIcon(newIcon);
	BOOST_CHECK_EQUAL(win.getIcon(), newIcon);
	win.destroy();
}

/*
 * Test cases for specific methods in Window.h
 */
BOOST_AUTO_TEST_CASE(WindowTestMethods)
{
	Window win;
	win.setIsVisible(false);

	//GetHandle before init check
	BOOST_CHECK(win.getHandle() == NULL);

	//GetHandle after init check
	UVec2 size = {800,600};
	win.init("Havenborough", size);
	BOOST_CHECK(win.getHandle() != NULL);
	win.destroy();
}

/**
 * Test that several windows can be created with the same window class.
 */
BOOST_AUTO_TEST_CASE(WindowTestMultipleWindows)
{
	Window win1, win2;
	win1.setIsVisible(false);
	win2.setIsVisible(false);

	UVec2 size = {800,600};
	win1.init("Window1", size);
	win2.init("Window2", size);
	BOOST_CHECK(win1.getHandle() != NULL);
	BOOST_CHECK(win2.getHandle() != NULL);
	BOOST_CHECK(win1.getHandle() != win2.getHandle());
	win1.destroy();
	win2.destroy();
}

BOOST_AUTO_TEST_SUITE_END()
