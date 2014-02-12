#define BOOST_TEST_MODULE "Unit tests for Havenborough"
#include <boost/test/unit_test.hpp>

/**
 * Place tests in separate files. Look at TestWindow for example.
 */

/********** UNCOMMENT FOR Code Coverage ***************/
#include <CodeCoverage/CodeCoverage.h>

ExcludeFromCodeCoverage(Exclusion1, L"boost::*");
ExcludeFromCodeCoverage(Exclusion2, L"boost_132::*");
ExcludeFromCodeCoverage(Exclusion3, L"boost_asio_handler_alloc_helpers::*");
ExcludeFromCodeCoverage(Exclusion4, L"boost_asio_handler_invoke_helpers::*");
ExcludeFromCodeCoverage(Exclusion5, L"DirectX::*");
ExcludeFromCodeCoverage(Exclusion6, L"Microsoft::*");
ExcludeFromCodeCoverage(Exclusion7, L"tinyxml2::*");
ExcludeFromCodeCoverage(Exclusion8, L"windows_file_codecvt::*");
ExcludeFromCodeCoverage(Exclusion9, L"IUnknown::*");
ExcludeSourceFromCodeCoverage(Exclusion10, L"*\\boost*\\libs\\*.cpp");
ExcludeFromCodeCoverage(Exclusion11, L"codecvt_error_cat::*");
ExcludeFromCodeCoverage(Exclusion12, L"handle_closer::*");
ExcludeFromCodeCoverage(Exclusion13, L"handle_wrapper::*");
ExcludeFromCodeCoverage(Exclusion14, L"fastdelegate::*");
ExcludeSourceFromCodeCoverage(Exclusion15, L"*\\DDSTextureLoader.cpp");
ExcludeSourceFromCodeCoverage(Exclusion16, L"*\\WICTextureLoader.cpp");
ExcludeSourceFromCodeCoverage(Exclusion17, L"*\\boost\\*.hpp");

