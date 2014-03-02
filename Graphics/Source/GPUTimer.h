#pragma once
#pragma warning (disable : 4005)

#include <D3D11.h>

class GPUTimer
{
	ID3D11Query*	m_Disjoint;
	ID3D11Query*	m_Start;
	ID3D11Query*	m_Stop;

	ID3D11Device*			m_Device;
	ID3D11DeviceContext*	m_DeviceContext;

public:
	explicit GPUTimer(ID3D11Device* p_Device, ID3D11DeviceContext* p_DeviceContext);
	~GPUTimer();

	void Start();
	void Stop();

	double GetTime();
};