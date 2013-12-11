#include <boost\test\unit_test.hpp>
#include "..\..\Client\Source\ResourceTranslator.h"
#include "..\..\Client\Source\ClientExceptions.h"

BOOST_AUTO_TEST_SUITE(ResourceTranslatorTest)

	//expecting exception throw
BOOST_AUTO_TEST_CASE(TranslateRandomString)
{
	ResourceTranslator rt;

	BOOST_CHECK_THROW(rt.translate("model", "hejsan"), ResourceManagerException);
}

	//expceting no exception throw.
BOOST_AUTO_TEST_CASE(TranslateStringThatExists)
{
	ResourceTranslator rt;

	BOOST_CHECK_NO_THROW(rt.translate("model", "SKYBOX"));
}

BOOST_AUTO_TEST_SUITE_END()