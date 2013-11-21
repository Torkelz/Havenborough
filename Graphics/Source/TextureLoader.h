#pragma once

#include <d3d11.h>
#include <vector>
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"


class TextureLoader
{
private:
	ID3D11Device*				m_Device;
	ID3D11DeviceContext*		m_DeviceContext;
	std::vector<char*>			m_CompabilityList;
public:
	TextureLoader(ID3D11Device* p_Device, ID3D11DeviceContext* p_DeviceContext);
	TextureLoader();
	~TextureLoader();
	void destroy();
	ID3D11ShaderResourceView* CreateTextureFromFile(char* p_FileName, char* p_FileType);
private:
	char* CheckCompability(char* p_FileType);
};