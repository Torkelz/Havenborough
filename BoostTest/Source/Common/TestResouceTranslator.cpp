#include <boost\test\unit_test.hpp>
#include <ResourceTranslator.h>
#include "..\..\Client\Source\ClientExceptions.h"

BOOST_AUTO_TEST_SUITE(ResourceTranslatorTest)

	//expecting exception throw
BOOST_AUTO_TEST_CASE(TranslateRandomString)
{
	ResourceTranslator rt;

//	BOOST_CHECK_THROW(rt.translate("model", "hejsan"), ResourceManagerException);
}

	//expceting no exception throw.
BOOST_AUTO_TEST_CASE(TranslateStringThatExists)
{
	ResourceTranslator rt;
	std::string string = "<Resources><ResourceType Type=\"model\"><Resource Name=\"House1\" Path=\"assets/models/House1.btx\"/></ResourceType></Resources>";
	std::istringstream path(string);
	rt.loadResourceList(path);
	BOOST_CHECK_NO_THROW(rt.translate("model", "House1"));
}

BOOST_AUTO_TEST_SUITE_END()