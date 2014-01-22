#pragma once

#include <DirectXMath.h>
#include <string.h>

#include "Buffer.h"

struct Particle
{
	DirectX::XMFLOAT4 startPosition; //pos from the syspos, in cm
	DirectX::XMFLOAT4 startVelocity;
	DirectX::XMFLOAT4 color;
	float sizeX;
	float sizeY;
	float life;
	float lifeMax;

	Particle(DirectX::XMFLOAT3 p_startPosition, DirectX::XMFLOAT3 p_startVelocity, DirectX::XMFLOAT4 p_Color,
		float p_SizeX, float p_SizeY, float p_Life, float p_LifeMax)
	{
		startPosition = DirectX::XMFLOAT4(p_startPosition.x, p_startPosition.y, p_startPosition.z, 1.0f);
		startVelocity = DirectX::XMFLOAT4(p_startVelocity.x, p_startVelocity.y, p_startVelocity.z, 1.0f);
		color = p_Color;
		sizeX	= p_SizeX;
		sizeY	= p_SizeY;
		life	= p_Life;
		lifeMax = p_LifeMax;
	}
};

struct  VertexType
{
	//???
};

class ParticleSystem
{
private:
	DirectX::XMFLOAT4	m_SysPosition; //world pos, in cm
	std::string			m_SysType;

	//Buffer				m_Buffer; //probably getting fixed out in the graphic class
	//Shader				m_Shader; //probably getting fixed out in the graphic class

	//Header
	std::string			m_SysName;
	std::string			m_TextureFilePath;
	float				m_TextureU; 
	float				m_TextureV;	
	float				m_ParticleLife;

	unsigned int		m_MaxParticles;
	DirectX::XMFLOAT4	m_ParticleDeviation;
	float				m_VelocityDeviation;
	float				m_ParticlesPerSecound;

public:
	ParticleSystem(void);
	~ParticleSystem(void);

	bool loadParticleSystemFromFile(const char* p_filename);

	void init();
	void update(float p_DeltaTime, ID3D11DeviceContext* p_DeviceContext);

	void render();
	

private:
	void emitNewParticles(float p_DeltaTime);
	void killOldParticles();
	void updateParticles(float p_DeltaTime);
	void updateBuffers(ID3D11DeviceContext* p_DeviceContext);
};

