#pragma once
#include <dwrite.h>
#include <d2d1.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Utilities/MemoryUtil.h"

#pragma comment(lib, "Dwrite")
#pragma comment(lib, "D2d1")

class SimpleText
{
private:
	IDWriteFactory *m_WriteFactory;
	IDWriteTextFormat *m_TextFormat;

	const wchar_t *m_Text;
	UINT32 m_TextLength;

	ID2D1Factory *m_D2DFactory;
	//ID2D1HwndRenderTarget* m_RenderTarget;
	ID2D1RenderTarget *m_RenderTarget;
	ID2D1SolidColorBrush *m_BlackBrush;

	RECT rc;
public:
	SimpleText(void);
	~SimpleText(void);

	void initialize(HWND *p_Hwnd, IDXGISurface *p_RenderTarget);

	void draw();
};

