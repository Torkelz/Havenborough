#include <boost/test/unit_test.hpp>
#include "..\..\Client\Source\Subject.h"

BOOST_AUTO_TEST_SUITE(TestObserver)

void foo(bool *p)
{
	*p = !(*p);
}

int g_Var = 0;
void fuu(void)
{
	g_Var = 343;
}
void fuuu(void)
{
	g_Var *= 7;
}

BOOST_AUTO_TEST_CASE(TestObserverWithStringAsEvent)
{
	Subject<std::string> s;
	bool test = false;
	s.registerObserver("Yo", std::bind(foo, &test));

	BOOST_CHECK(test == false);
	BOOST_CHECK_THROW(s.notify("Yooo"), EventException);
	BOOST_CHECK(test == false);
	BOOST_CHECK_NO_THROW(s.notify("Yo"));
	BOOST_CHECK(test == true);
}

BOOST_AUTO_TEST_CASE(TestObserverWithStringAsEventNoParam)
{
	Subject<std::string> s;
	s.registerObserver("Yo", fuu);
	g_Var = 0;

	BOOST_CHECK(g_Var == 0);
	BOOST_CHECK_THROW(s.notify("Yooo"), EventException);
	BOOST_CHECK(g_Var == 0);
	BOOST_CHECK_NO_THROW(s.notify("Yo"));
	BOOST_CHECK(g_Var == 343);
}

BOOST_AUTO_TEST_CASE(TestObserverWithStringAsEventTwoFunctionsNoParam)
{
	Subject<std::string> s;
	s.registerObserver("Yo", fuu);
	s.registerObserver("Yo", fuuu);
	g_Var = 0;

	BOOST_CHECK(g_Var == 0);
	BOOST_CHECK_THROW(s.notify("Yooo"), EventException);
	BOOST_CHECK(g_Var == 0);
	BOOST_CHECK_NO_THROW(s.notify("Yo"));
	BOOST_CHECK(g_Var == 2401);
}

BOOST_AUTO_TEST_CASE(TestObserverWithStringAsEventTwoKeys)
{
	Subject<std::string> s;
	bool test = false;
	s.registerObserver("Yo", std::bind(foo, &test));
	s.registerObserver("Yooo", std::bind(foo, &test));

	BOOST_CHECK(test == false);
	BOOST_CHECK_NO_THROW(s.notify("Yooo"));
	BOOST_CHECK(test == true);
	BOOST_CHECK_NO_THROW(s.notify("Yo"));
	BOOST_CHECK(test == false);
}

BOOST_AUTO_TEST_SUITE_END()