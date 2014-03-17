#include <boost/test/unit_test.hpp>
#include "../../Common/Source/SplineControlComponent.h"
#include "../../Common/Source/LookComponent.h"
#include "../../Physics/Source/Physics.h"
#include "../../Physics/Source/PhysicsExceptions.h"


BOOST_AUTO_TEST_SUITE(SplineTest)

class dummyPhysics : public Physics
{
public:
	dummyPhysics(){}
	~dummyPhysics(){}

	Vector3 getBodyPosition(BodyHandle p_Body) override
	{
		return Vector3(0,0,0);
	}

	void setBodyPosition(BodyHandle p_Body, Vector3 p_newPos) override
	{
		
	}

	void applyImpulse(BodyHandle p_Body, Vector3 p_Impulse) override
	{
		
	}

	Vector3 getBodyVelocity(BodyHandle p_Body) override
	{
		return Vector3(0.f, 0.f, 0.f);
	}

	void releaseBody(BodyHandle p_Body) override
	{

	}
};

BOOST_AUTO_TEST_CASE(SplineInit)
{
	float maxSpeed = 1000.f;
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("SplineControl");
	printer.PushAttribute("MaxSpeed", maxSpeed);
	printer.PushAttribute("Acceleration", 600.f);
	printer.CloseElement();

	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr(), printer.CStrSize());
	SplineControlComponent testComp;
	testComp.initialize(doc.FirstChildElement("SplineControl"));

	BOOST_CHECK(testComp.getMaxSpeed() == maxSpeed);
}

BOOST_AUTO_TEST_CASE(SplineSave)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("SplineControl");
	printer.PushAttribute("MaxSpeed", 1000.f);
	printer.PushAttribute("Acceleration", 600.f);
	printer.CloseElement();

	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr(), printer.CStrSize());
	SplineControlComponent testComp;
	testComp.initialize(doc.FirstChildElement("SplineControl"));
	testComp.reset();
	dummyPhysics ps;

	std::shared_ptr<LookComponent> l(new LookComponent());
	tinyxml2::XMLDocument doc2;
	doc2.InsertFirstChild(doc2.NewElement("Look"));
	l->initialize(doc2.RootElement());
	std::shared_ptr<CollisionSphereComponent> p(new CollisionSphereComponent());
	p->setPhysics(&ps);

	testComp.setComponents(p,l);
	testComp.setPhysics(&ps);
	BOOST_CHECK(testComp.savePathTest("SplineTest") == false);

	testComp.recordPoint();
	BOOST_CHECK(testComp.savePathTest("SplineTest") == true);

	testComp.removePreviousPoint();
	BOOST_CHECK(testComp.savePathTest("SplineTest") == false);
}

BOOST_AUTO_TEST_CASE(SplineMove)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("SplineControl");
	printer.PushAttribute("MaxSpeed", 1000.f);
	printer.PushAttribute("Acceleration", 600.f);
	printer.CloseElement();

	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr(), printer.CStrSize());
	SplineControlComponent testComp;
	testComp.initialize(doc.FirstChildElement("SplineControl"));
	testComp.reset();
	dummyPhysics ps;

	std::shared_ptr<LookComponent> l(new LookComponent());
	tinyxml2::XMLDocument doc2;
	doc2.InsertFirstChild(doc2.NewElement("Look"));
	l->initialize(doc2.RootElement());
	std::shared_ptr<CollisionSphereComponent> p(new CollisionSphereComponent());
	p->setPhysics(&ps);

	testComp.setComponents(p,l);
	testComp.setPhysics(&ps);
	testComp.recordPoint();
	testComp.recordPoint();
	testComp.recordPoint();
	BOOST_CHECK_NO_THROW(testComp.move(0.0015f));//, PhysicsException);
	testComp.runSplineTest(0);
	BOOST_CHECK_NO_THROW(testComp.move(0.0015f));//, PhysicsException);
}

BOOST_AUTO_TEST_CASE(SplineLoad)
{
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("SplineControl");
	printer.PushAttribute("MaxSpeed", 1000.f);
	printer.PushAttribute("Acceleration", 600.f);
	printer.CloseElement();

	tinyxml2::XMLDocument doc;
	doc.Parse(printer.CStr(), printer.CStrSize());
	SplineControlComponent testComp;
	testComp.initialize(doc.FirstChildElement("SplineControl"));

	BOOST_CHECK(testComp.loadPathTest("SplineTest") == true);
}

BOOST_AUTO_TEST_SUITE_END()