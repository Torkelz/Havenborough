#include "GPUTimer.h"

GPUTimer::GPUTimer(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dDeviceContext)
	: m_Device(d3dDevice), m_DeviceContext(d3dDeviceContext)
{
	//ugly, should not be done in constructor :-)
	D3D11_QUERY_DESC desc;
	desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	desc.MiscFlags = 0;
	m_Device->CreateQuery(&desc, &m_Disjoint);

	desc.Query = D3D11_QUERY_TIMESTAMP;
	m_Device->CreateQuery(&desc, &m_Start);
	m_Device->CreateQuery(&desc, &m_Stop);
}

GPUTimer::~GPUTimer()
{
	if(m_Start)		m_Start->Release();
	if(m_Stop)		m_Start->Release();
	if(m_Disjoint)	m_Start->Release();
}

void GPUTimer::Start()
{
	m_DeviceContext->Begin(m_Disjoint);

	m_DeviceContext->End(m_Start);
}

void GPUTimer::Stop()
{
	m_DeviceContext->End(m_Stop);

	m_DeviceContext->End(m_Disjoint);
}

double GPUTimer::GetTime()
{
	UINT64 startTime = 0;
	while(m_DeviceContext->GetData(m_Start, &startTime, sizeof(startTime), 0) != S_OK);

	UINT64 endTime = 0;
	while(m_DeviceContext->GetData(m_Stop, &endTime, sizeof(endTime), 0) != S_OK);

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
	while(m_DeviceContext->GetData(m_Disjoint, &disjointData, sizeof(disjointData), 0) != S_OK);

	double time = -1.0f;
	if(disjointData.Disjoint == FALSE)
	{
		UINT64 delta = endTime - startTime;
		double frequency = static_cast<double>(disjointData.Frequency);
		time = (delta / frequency) * 1000.0f;
	}

	return time;
}