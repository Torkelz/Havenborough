#pragma once

#include <DirectXMath.h>
#include <string.h>

#include "Buffer.h"

struct Particle
{
	DirectX::XMFLOAT4 position;
	float life;
	float lifeMax;
};

class ParticleSystem
{
private:
	DirectX::XMFLOAT4	m_Position;
	std::string			m_SysType;

	Buffer				m_Buffer;
	Shader				m_Shader;

	//Header
	std::string			m_SysName;
	float				m_ParticleLife;

	unsigned int		m_MaxParticles;
	std::string			m_TextureFilePath;
	DirectX::XMFLOAT4	m_Color;
	

public:
	ParticleSystem(void);
	~ParticleSystem(void);

	bool loadParticleSystemFromFile(const char* p_filename);

	void update();
	void emitNewParticles();
	void killOldParticles();

private:

};

