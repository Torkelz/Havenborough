#pragma once
#include <d2d1.h>
#include <d3d11.h>

class TextResource
{
public:
	ID3D11ShaderResourceView *m_ShaderResourceView;
	ID2D1RenderTarget *m_RenderTarget;
	IDWriteTextFormat *m_TextFormat;
	ID2D1SolidColorBrush *m_Brush;
	D2D1_RECT_F m_LayoutRect;


	TextResource();
	TextResource(TextResource &&p_Other);
	TextResource(ID2D1RenderTarget* p_RT, ID3D11ShaderResourceView *p_SRV, IDWriteTextFormat *p_TextFormat,
		ID2D1SolidColorBrush *p_Brush, D2D1_RECT_F p_Rectangle);
	~TextResource();

	//TextResource &operator=(TextResource p_Other);
	TextResource &operator=(TextResource &&p_Other);

	void draw(const wchar_t *p_Text);

	const ID3D11ShaderResourceView *getSRV() const;

	void release();

private:
	TextResource(const TextResource &p_Other);
};