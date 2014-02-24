#include <boost\test\unit_test.hpp>
#include <ResourceTranslator.h>
#include "..\..\Client\Source\ClientExceptions.h"

BOOST_AUTO_TEST_SUITE(ResourceTranslatorTest)

	//expecting exception throw
BOOST_AUTO_TEST_CASE(TranslateRandomString)
{
	ResourceTranslator rt;

	BOOST_CHECK_THROW(rt.translate("model", "hejsan"), ResourceManagerException);
	std::string string = "<Resources><ResourceType Type=\"volume\"><Resource Name=\"House1\" Path=\"assets/models/House1.btx\"/></ResourceType>"
		"<ResourceType Type=\"model\"><Resource Name=\"House1\" Path=\"assets/models/House1.btx\"/></ResourceType>"
		"<ResourceType Type=\"animation\"><Resource Name=\"House1\" Path=\"assets/models/House1.btx\"/></ResourceType>"
		"<ResourceType Type=\"model\"><Resource Name=\"House1\" Path=\"assets/models/House1.btx\"/></ResourceType></Resources>";
	std::istringstream path(string);
	rt.loadResourceList(path);
	BOOST_CHECK_THROW(rt.translate("model", "hejsan"), ResourceManagerException);
}

	//expceting no exception throw.
BOOST_AUTO_TEST_CASE(TranslateStringThatExists)
{
	ResourceTranslator rt;
	std::string string = "<Resources><ResourceType Type=\"volume\"><Resource Name=\"House1\" Path=\"assets/models/House1.btx\"/></ResourceType>"
		"<ResourceType Type=\"model\"><Resource Name=\"House1\" Path=\"assets/models/House1.btx\"/></ResourceType></Resources>";
	std::istringstream path(string);
	rt.loadResourceList(path);
	BOOST_CHECK_NO_THROW(rt.translate("model", "House1"));
}

BOOST_AUTO_TEST_CASE(testLoadResourceTest)
{
	ResourceTranslator rt;
	std::string string = "<Resources><ResourceType Type=\"model\"><Resource Name=\"House1\" Path=\"assets/models/House1.btx\"/></ResourceType></Resourcessa>";
	std::istringstream path(string);
	BOOST_CHECK_THROW(rt.loadResourceList(path), ResourceManagerException);
	string = "<Reaseas><TestType Type=\"model\"><Resource Name=\"House1\" Path=\"assets/models/House1.btx\"/></TestType></Reaseas>";
	path.clear();
	path = std::istringstream(string);
	BOOST_CHECK_THROW(rt.loadResourceList(path), ResourceManagerException);
	string = "<Resources><ResourceType Model=\"model\"><Resource Name=\"House1\" Path=\"assets/models/House1.btx\"/></ResourceType>"
		"<ResourceType Type=\"model\"><Resource Name=\"House1\" Path=\"assets/models/House1.btx\"/></ResourceType></Resources>";
	path = std::istringstream(string);
	BOOST_CHECK_NO_THROW(rt.loadResourceList(path));
	string = "<Resources><ResourceType Type=\"model\"><Resource Na=\"House1\" Path=\"assets/models/House1.btx\"/></ResourceType></Resources>";
	path = std::istringstream(string);
	BOOST_CHECK_THROW(rt.loadResourceList(path),ResourceManagerException);
	string = "<Resources><ResourceType Type=\"model\"><Resource Name=\"House1\" Pa=\"assets/models/House1.btx\"/></ResourceType></Resources>";
	path = std::istringstream(string);
	BOOST_CHECK_THROW(rt.loadResourceList(path),ResourceManagerException);
}

BOOST_AUTO_TEST_SUITE_END()