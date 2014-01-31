#pragma once
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

#include <d3d11.h>
#include <vector>

class TextureLoader
{
private:
	ID3D11Device*			m_Device;
	ID3D11DeviceContext*	m_DeviceContext;
	std::vector<char*>		m_CompabilityList;
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
	 * @return Success = A pointer to the loaded texture, Fail = nullptr.
	 */
	ID3D11ShaderResourceView* createTextureFromFile(const char* p_Filename);
protected:
	virtual HRESULT CreateWICTextureFromFile(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context,
		const wchar_t* p_Filename, ID3D11Resource** p_Texture, ID3D11ShaderResourceView** p_TextureView, size_t p_MaxSize);
	
	virtual HRESULT CreateDDSTextureFromFile(ID3D11Device* p_Device, const wchar_t* p_Filename, ID3D11Resource** p_Texture,
		ID3D11ShaderResourceView** p_TextureView, size_t p_MaxSize, DirectX::DDS_ALPHA_MODE* p_AlphaMode);
private:
	char* checkCompability(char* p_FileType);
};