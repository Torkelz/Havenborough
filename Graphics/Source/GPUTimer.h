#pragma once
#pragma warning (disable : 4005)

#include <D3D11.h>
#include <vector>

class GPUTimer
{
	ID3D11Query*	m_Disjoint;
	ID3D11Query*	m_Start;
	ID3D11Query*	m_Stop;

	ID3D11Device*			m_Device;
	ID3D11DeviceContext*	m_DeviceContext;
	std::vector<double> m_Samples;

	unsigned int m_MaxSamples;

public:
	explicit GPUTimer(ID3D11Device* p_Device, ID3D11DeviceContext* p_DeviceContext);
	~GPUTimer();

	void Start();
	void Stop();

	//Returns in MS
	double GetTime();
	//Returns in MS
	void GetAverageTime(double &p_Average);
	void resetAverageTimer();
};