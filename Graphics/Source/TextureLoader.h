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
	/**
	 * Constructor.
	 * Initializes the compability list.
	 * @param p_Device, Pointer to the application graphics device.
	 * @param p_DeviceContext, Pointer to the application graphics device context.
	 */
	TextureLoader(ID3D11Device* p_Device, ID3D11DeviceContext* p_DeviceContext);
	/**
	* Default constructor.
	*/
	TextureLoader();
	/**
	 * Destructor.
	 */
	~TextureLoader();
	/**
	 * Free memory.
	 */
	void destroy();
	/**
	 * Used to load textures from file.
	 * @param p_Filename, path of the file to be loaded.
	 * @param p_FileType, file extension of the texture file to be loaded.
	 * @return Success = A pointer to the loaded texture, Fail = nullptr.
	 */
	ID3D11ShaderResourceView* createTextureFromFile(char* p_Filename);
protected:
	virtual HRESULT CreateWICTextureFromFile(ID3D11Device* d3dDevice,
                                           ID3D11DeviceContext* d3dContext,
                                           const wchar_t* fileName,
                                           ID3D11Resource** texture,
                                           ID3D11ShaderResourceView** textureView,
                                           size_t maxsize);
	virtual HRESULT CreateDDSTextureFromFile(ID3D11Device* d3dDevice,
                                           const wchar_t* fileName,
                                           ID3D11Resource** texture,
                                           ID3D11ShaderResourceView** textureView,
                                           size_t maxsize,
                                           DirectX::DDS_ALPHA_MODE* alphaMode);
private:
	char* checkCompability(char* p_FileType);
};