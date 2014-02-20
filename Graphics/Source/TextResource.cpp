#include "TextResource.h"
#include "Utilities/MemoryUtil.h"

TextResource::TextResource( ID2D1RenderTarget* p_RT, ID3D11ShaderResourceView *p_SRV, IDWriteTextFormat *p_TextFormat, 
	ID2D1SolidColorBrush *p_Brush, D2D1_RECT_F p_Rectangle ) 
	: m_ShaderResourceView(p_SRV), m_RenderTarget(p_RT), m_TextFormat(p_TextFormat), m_Brush(p_Brush), m_LayoutRect(p_Rectangle)
{

}

TextResource::~TextResource()
{
	SAFE_RELEASE(m_RenderTarget);
	SAFE_RELEASE(m_ShaderResourceView);
	SAFE_DELETE(m_TextFormat);
	SAFE_RELEASE(m_Brush);
}

void TextResource::Draw( const wchar_t *p_Text )
{
	m_RenderTarget->BeginDraw();
	m_RenderTarget->Clear(D2D1::ColorF(0,0,0,0));
	m_RenderTarget->DrawText(p_Text, (UINT32)wcslen(p_Text), m_TextFormat, m_LayoutRect, m_Brush);
	m_RenderTarget->EndDraw();
}

const ID3D11ShaderResourceView *TextResource::getSRV() const
{
	return m_ShaderResourceView;
}