#include "TextResource.h"
#include "Utilities/MemoryUtil.h"
#include <algorithm>

#include <dwrite.h>

TextResource::TextResource() : 
	m_ShaderResourceView(nullptr),
	m_RenderTarget(nullptr),
	m_TextFormat(nullptr),
	m_Brush(nullptr),
	m_Text(nullptr),
	m_ClearColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f))
{
}

TextResource::TextResource(TextResource &&p_Other)
	: m_ShaderResourceView(std::move(p_Other.m_ShaderResourceView)),
	m_RenderTarget(std::move(p_Other.m_RenderTarget)),
	m_TextFormat(std::move(p_Other.m_TextFormat)),
	m_Brush(std::move(p_Other.m_Brush)),
	m_LayoutRect(std::move(p_Other.m_LayoutRect)),
	m_Text(std::move(p_Other.m_Text)),
	m_ClearColor(std::move(p_Other.m_ClearColor))
{
	p_Other.m_ShaderResourceView = nullptr;
	p_Other.m_RenderTarget = nullptr;
	p_Other.m_TextFormat = nullptr;
	p_Other.m_Brush = nullptr;
	p_Other.m_Text = nullptr;
}

TextResource::TextResource(ID2D1RenderTarget* p_RT, ID3D11ShaderResourceView *p_SRV, IDWriteTextFormat *p_TextFormat, 
	ID2D1SolidColorBrush *p_Brush, D2D1_RECT_F p_Rectangle, const wchar_t *p_Text,
	D2D1::ColorF p_ClearColor /* = D2D::ColorF(0.0f, 0.0f, 0.0f, 0.0f) */) 
	: m_ShaderResourceView(p_SRV), m_RenderTarget(p_RT), m_TextFormat(p_TextFormat), m_Brush(p_Brush),
	m_LayoutRect(p_Rectangle), m_ClearColor(p_ClearColor), m_Text(p_Text)
{
}

TextResource &TextResource::operator=(TextResource &&p_Other)
{
	std::swap(m_ShaderResourceView, p_Other.m_ShaderResourceView);
	std::swap(m_RenderTarget, p_Other.m_RenderTarget);
	std::swap(m_TextFormat, p_Other.m_TextFormat);
	std::swap(m_Brush, p_Other.m_Brush);
	std::swap(m_LayoutRect, p_Other.m_LayoutRect);
	std::swap(m_Text, p_Other.m_Text);
	std::swap(m_ClearColor, p_Other.m_ClearColor);

	return *this;
}

TextResource::~TextResource()
{
	release();
}

void TextResource::release()
{
	SAFE_RELEASE(m_TextFormat);
	SAFE_RELEASE(m_RenderTarget);
	SAFE_RELEASE(m_ShaderResourceView);
	SAFE_RELEASE(m_Brush);
}

void TextResource::draw()
{
	m_RenderTarget->BeginDraw();
	m_RenderTarget->Clear(m_ClearColor);
	m_RenderTarget->DrawText(m_Text, (UINT32)wcslen(m_Text), m_TextFormat, m_LayoutRect, m_Brush);
	m_RenderTarget->EndDraw();
}

ID3D11ShaderResourceView *TextResource::getSRV() const
{
	return m_ShaderResourceView;
}
