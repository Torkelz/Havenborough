#pragma once
#include "igraphics.h"

#include <d3d11.h>

class Graphics :
	public IGraphics
{
public:
	ID3D11Device *m_Device;
	ID3D11DeviceContext *m_DeviceContext;
	IDXGISwapChain *m_SwapChanin;
	

public:
	Graphics(void);
	~Graphics(void);

	bool initialize(void);
	void shutdown(void);
	
	void renderModel(void);
	void renderText(void);
	void renderQuad(void);

	void addStaticLight(void);
	void removeStaticLight(void);
	void useFrameLight(void);
	
	void drawFrame(void);

};

