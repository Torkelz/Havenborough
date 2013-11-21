#include "TextureLoader.h"

TextureLoader::TextureLoader(ID3D11Device* p_Device, ID3D11DeviceContext* p_DeviceContext)
{
	m_Device = p_Device;
	m_DeviceContext = p_DeviceContext;
	m_CompabilityList.push_back("bmp");
	m_CompabilityList.push_back("gif");
	m_CompabilityList.push_back("ico");
	m_CompabilityList.push_back("jpeg");
	m_CompabilityList.push_back("jpe");
	m_CompabilityList.push_back("jpg");
	m_CompabilityList.push_back("png");
	m_CompabilityList.push_back("tiff");
	m_CompabilityList.push_back("tif");
	m_CompabilityList.push_back("phot");
	m_CompabilityList.push_back("dds");
	m_CompabilityList.shrink_to_fit();
}

TextureLoader::TextureLoader(){}

TextureLoader::~TextureLoader(){}

void TextureLoader::destroy()
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
	m_CompabilityList.clear();
}

ID3D11ShaderResourceView* TextureLoader::CreateTextureFromFile(char* p_Filename, char* p_FileType)
{
	HRESULT hr = S_OK;
	char* type;
	type = CheckCompability(p_FileType);
	if(type == "err")
		return nullptr;
	ID3D11Resource*				textureResource = nullptr;
	ID3D11ShaderResourceView*	textureSRV = nullptr;
	std::vector<wchar_t> filename(strlen(p_Filename)+1);
	mbstowcs(filename.data(), p_Filename, strlen(p_Filename)+1);

	if(type == "wic")
	{
		
		hr = DirectX::CreateWICTextureFromFile(m_Device, m_DeviceContext, filename.data(), 
														&textureResource, &textureSRV);
	}
	else
	{
		DirectX::DDS_ALPHA_MODE mode;
		DirectX::CreateDDSTextureFromFile(m_Device, filename.data(), 
											&textureResource, &textureSRV, 0, &mode);
	}
	//textureResource->Release();
	textureResource = nullptr;
	filename.clear();

	return textureSRV;
}

char* TextureLoader::CheckCompability(char* p_FileType)
{
	unsigned int size = m_CompabilityList.size() ;
	for(unsigned int i = 0; i < size-1; i++)
	{
		if(p_FileType == m_CompabilityList.at(i))
		{
			return "wic";
		}
	}
	if(p_FileType == m_CompabilityList.at(size - 1))
	{
		return "dds";
	}
	return "err";
}