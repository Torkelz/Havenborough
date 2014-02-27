#pragma once
#include <d2d1.h>
#include <d3d11.h>
#include <string>

class TextResource
{
public:
	ID3D11ShaderResourceView *m_ShaderResourceView;
	ID2D1RenderTarget *m_RenderTarget;
	IDWriteTextFormat *m_TextFormat;
	ID2D1SolidColorBrush *m_Brush;
	D2D1_RECT_F m_LayoutRect;
	std::wstring m_Text;
	D2D1::ColorF m_ClearColor;
	bool m_Update;

	TextResource();
	TextResource(TextResource &&p_Other);
	TextResource(ID2D1RenderTarget* p_RT, ID3D11ShaderResourceView *p_SRV, IDWriteTextFormat *p_TextFormat,
		ID2D1SolidColorBrush *p_Brush, D2D1_RECT_F p_Rectangle, std::wstring p_Text,
		D2D1::ColorF p_ClearColor = D2D1::ColorF(0,0,0,0));
	~TextResource();

	//TextResource &operator=(TextResource p_Other);
	TextResource &operator=(TextResource &&p_Other);

	void draw();

	ID3D11ShaderResourceView *getSRV() const;

	void release();

private:
	TextResource(const TextResource &p_Other);
};