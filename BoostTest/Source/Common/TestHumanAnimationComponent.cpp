//#include <boost/test/unit_test.hpp>
//#include "Animation.h"
//#include "AnimationLoader.h"
//#include "CommonExceptions.h"
//#include "HumanAnimationComponent.h"
//
//class TestAnimation : public Animation
//{
//	TestAnimation();
//	~TestAnimation();
//
//	void playClip(const std::string&, bool) override{}
//	void queueClip(const std::string&) override{}
//};
//
//BOOST_AUTO_TEST_SUITE(HumanAnimationComponentTest)
//
//BOOST_AUTO_TEST_CASE(humanAnimationComponentTest)
//{
//	tinyxml2::XMLPrinter printer;
//	printer.OpenElement("Object");
//	printer.OpenElement("HumanAnimation");
//	printer.PushAttribute("Animation", "WITCH");
//	printer.CloseElement();
//	printer.CloseElement();
//
//	tinyxml2::XMLDocument doc;
//	doc.Parse(printer.CStr());
//	HumanAnimationComponent testComponent;
//
//	testComponent.initialize(doc.FirstChildElement("Object"));
//}
//
//BOOST_AUTO_TEST_SUITE_END()