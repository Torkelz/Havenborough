#pragma once

enum class ShaderType
{
	VERTEX_SHADER = 1,
	PIXEL_SHADER = 2,
	GEOMETRY_SHADER = 4,
	HULL_SHADER = 8,
	DOMAIN_SHADER = 16
};
inline bool operator&(ShaderType a, ShaderType b)
{
	return static_cast<int>(a) & static_cast<int>(b) ? true : false;
};
inline ShaderType operator|(ShaderType a, ShaderType b)
{
	return static_cast<ShaderType>(static_cast<int>(a) | static_cast<int>(b));
};

enum class Format
{
	R32G32B32A32_FLOAT = 2,
	R32G32B32A32_UINT = 3,
	R32G32B32_FLOAT = 6,
	R32G32B32_UINT = 7,
	R32G32_FLOAT = 16,
	R32G32_UINT = 17,
	R32_FLOAT = 41,
	R32_UINT = 42,
	R8G8B8A8_UNORM = 28,
};

struct ShaderInputElementDescription
{
	LPCSTR semanticName;
	UINT semanticIndex;
	Format format;
	UINT inputSlot;
	UINT alignedByteOffset;
	UINT inputSlotClass;
	UINT instanceDataStepRate;
};