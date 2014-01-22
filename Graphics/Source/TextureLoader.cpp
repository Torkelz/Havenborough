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

TextureLoader::TextureLoader()
{
}

TextureLoader::~TextureLoader(){}

void TextureLoader::destroy()
{
	m_Device = nullptr;
	m_DeviceContext = nullptr;
	m_CompabilityList.clear();
}

ID3D11ShaderResourceView* TextureLoader::createTextureFromFile(const char* p_Filename)
{
	//Pick out file extension
	std::vector<char> buffer(strlen(p_Filename)+1);
	strcpy(buffer.data(), p_Filename);
	char *type = nullptr, *tmp;
	tmp = strtok(buffer.data(), ".");
	while(tmp != nullptr)
	{
		type = tmp;
		tmp = strtok(NULL,".");
	}
	char* result = checkCompability(type);

	//If file format is not supported or invalid filename return nullptr.
	if(strcmp(result, "err") == 0)
		return nullptr;

	ID3D11Resource*				textureResource = nullptr;
	ID3D11ShaderResourceView*	textureSRV = nullptr;

	//Convert filename from char pointer to wchar_t 
	//because the texture loader function wants the filename in wchar. 
	std::vector<wchar_t> filename(strlen(p_Filename)+1);
	mbstowcs(filename.data(), p_Filename, strlen(p_Filename)+1);
	filename.end() - 1;

	HRESULT hr = S_OK;
	//All supported file formats except dds uses the WIC TextureLoader.
	if(strcmp(result, "wic") == 0)
	{
		hr = CreateWICTextureFromFile(m_Device, m_DeviceContext, filename.data(), 
																		&textureResource, &textureSRV,0);
		if(FAILED(hr))
		{
			throw TextureLoaderException("WIC Texture load failed", __LINE__, __FILE__);
		}
	}
	else
	{
		//Temporary variable used to save a return value that is not used.
		DirectX::DDS_ALPHA_MODE mode;

		hr = CreateDDSTextureFromFile(m_Device, filename.data(), 
														&textureResource, &textureSRV, 0, &mode);
		if(FAILED(hr))
		{
			static char buffer[1024];
			size_t bufferSize = 1024;

			std::vector<char> bigBuffer;

			char* tBuffer;
			if (filename.size() > 1024 / 4)
			{
				bigBuffer.resize(filename.size() * 4);
				tBuffer = bigBuffer.data();
				bufferSize = bigBuffer.size();
			}
			else
			{
				tBuffer = buffer;
			}

			int ret = wcstombs(tBuffer, filename.data(), bufferSize);
			tBuffer[ret] = '\0';

			throw TextureLoaderException("DDS Texture load failed (\"" + std::string(tBuffer) + "\")", __LINE__, __FILE__);
		}
	}
	//Texture resource is used to catch output from the texture creation function but the
	//data saved in it is not used, So we delete it after.
	if(textureResource != nullptr)
		textureResource->Release();
	filename.clear();

	return textureSRV;
}

char* TextureLoader::checkCompability(char* p_FileType)
{
	unsigned int size = m_CompabilityList.size();

	if (size == 0)
		return "err";

	//Check if the file format we want to load is supported.
	for(unsigned int i = 0; i < size - 1; i++)
	{
		//Loop through the compability list except for the last item in the list which is .dds
		//because dds files is handled by another Loader function.
		if(strcmp(p_FileType, m_CompabilityList.at(i)) == 0)
		{
			return "wic";
		}
	}
	//Check last against the last element in the list which is dds
	if(strcmp(p_FileType, m_CompabilityList.at(size - 1)) == 0)
	{
		return "dds";
	}
	//if the file format is not supported return err.
	return "err";
}

HRESULT TextureLoader::CreateWICTextureFromFile(ID3D11Device* d3dDevice,
                                           ID3D11DeviceContext* d3dContext,
                                           const wchar_t* fileName,
                                           ID3D11Resource** texture,
                                           ID3D11ShaderResourceView** textureView,
                                           size_t maxsize)
{
	return DirectX::CreateWICTextureFromFile(d3dDevice,d3dContext,fileName,texture,textureView,maxsize);
}

HRESULT TextureLoader::CreateDDSTextureFromFile(ID3D11Device* d3dDevice,
                                           const wchar_t* fileName,
                                           ID3D11Resource** texture,
                                           ID3D11ShaderResourceView** textureView,
                                           size_t maxsize,
                                           DirectX::DDS_ALPHA_MODE* alphaMode)
{
	return DirectX::CreateDDSTextureFromFile(d3dDevice,fileName,texture,textureView,maxsize,alphaMode);
}