#include <boost/test/unit_test.hpp>
#include "..\..\Graphics\Source\TextureLoader.h"
BOOST_AUTO_TEST_SUITE(TestTextureLoader)


class testTexLoader : public TextureLoader
{
public:
	testTexLoader() : TextureLoader(nullptr, nullptr)
	{
	}

	HRESULT CreateWICTextureFromFile(ID3D11Device* d3dDevice,
                                           ID3D11DeviceContext* d3dContext,
                                           const wchar_t* fileName,
                                           ID3D11Resource** texture,
                                           ID3D11ShaderResourceView** textureView,
                                           size_t maxsize)
	{
		*textureView = (ID3D11ShaderResourceView*)15;
		return S_OK;
	}

	HRESULT CreateDDSTextureFromFile(ID3D11Device* d3dDevice,
                                           const wchar_t* fileName,
                                           ID3D11Resource** texture,
                                           ID3D11ShaderResourceView** textureView,
                                           size_t maxsize,
                                           DirectX::DDS_ALPHA_MODE* alphaMode)
	{
		*textureView = (ID3D11ShaderResourceView*)15;
		return S_OK;
	}
};
BOOST_AUTO_TEST_CASE(TestLoader)
{
	testTexLoader loader;

	BOOST_CHECK(loader.createTextureFromFile("test.png") != nullptr);

	BOOST_CHECK(loader.createTextureFromFile("test.dds") != nullptr);

	BOOST_CHECK(loader.createTextureFromFile("test.klok") == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()