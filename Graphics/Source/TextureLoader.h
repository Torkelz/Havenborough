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
private:
	char* checkCompability(char* p_FileType);
};