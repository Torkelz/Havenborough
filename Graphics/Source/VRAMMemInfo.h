#pragma once
#define KB 1000
#define MB (1000 * KB)
#define OVERHEAD_USAGE (10 * MB) //This is a theoretical overhead for graphics resources in the video memory

#include <d3d11.h>
#include "Util.h"
#include "MyGraphicsExceptions.h"
#include <memory>


class VRAMMemInfo
{
private:
	enum Size : unsigned int
	{
		R32G32B32A32_FLOAT = 16,
		R32G32B32A32_UINT = 16,
		R32G32B32_FLOAT = 12,
		R32G32B32_UINT = 12,
		R16G16B16A16_FLOAT = 8,
		R32G32_FLOAT = 8,
		R32G32_UINT = 8,
		R8G8B8A8_UNORM = 4
	};
	
	static std::unique_ptr<VRAMMemInfo> m_Instance;
	unsigned int m_Usage;

public:
	/**
	* Gets an instance of the VRAM memory info.
	* @return a pointer to the instance
	*/
	static VRAMMemInfo *getInstance(void);

	/**
	* Shuts down the info and release the memory allocated.
	*/
	void shutdown(void);

	/*
	* Gets the amount of video RAM the program uses.
	* @return the amount in MB
	*/
	int getUsage(void) const;

	/*
	* Calculates the video RAM usage by texture format.
	* when releasing something from memory.
	* @param p_Format the format of the texture
	* @param p_Width the width of the texture in pixels
	* @param p_Height the height of the texture in pixels
	* @return the video RAM usage in bytes
	*/
	unsigned int calculateFormatUsage(DXGI_FORMAT p_Format, int p_Width, int p_Height);

	/*
	* Updates the video RAM usage counter. Use positive value when storing something in memory, use negative 
	* when releasing something from memory.
	* @param p_Size the amount of usage in bytes
	*/
	void updateUsage(int p_Size);

	~VRAMMemInfo(void);
private:
	VRAMMemInfo(void);
};