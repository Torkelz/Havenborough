#include "TextFactory.h"
#include "FontCollectionLoader.h"
#include "GraphicsExceptions.h"
#include "Utilities/MemoryUtil.h"
#include <vector>

TextFactory::TextFactory(void)
{
	m_D2DFactory = nullptr;
	m_WriteFactory = nullptr;
	m_Device = nullptr;
	m_Dpi = Vector2(0.0f, 0.0f);
	m_NextTextId = 1;
	m_FileFontCollection = nullptr;
}


TextFactory::~TextFactory(void)
{
}

void TextFactory::initialize(ID3D11Device *p_Device)
{
	if(!p_Device)
		throw TextFactoryException("Failed to initialize TextFactory, nullpointer not allowed. ", __LINE__, __FILE__);
	m_Device = p_Device;
	
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_D2DFactory);
	if(FAILED(hr))
		throw TextFactoryException("Failed to create D2DFactory. ", __LINE__, __FILE__);

	m_D2DFactory->GetDesktopDpi(&m_Dpi.x, &m_Dpi.y);

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_WriteFactory));


	if(FAILED(hr))
		throw TextFactoryException("Failed to create WriteFactory. ", __LINE__, __FILE__);

	char fullpath[1024];
	_fullpath(fullpath, "assets/fonts", 1024);

	if (FAILED(m_FontContext.CreateFontCollection(m_WriteFactory, fullpath, &m_FileFontCollection)))
		throw TextFactoryException("Failed to create file font collection.", __LINE__, __FILE__);

	m_DefaultProperties = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), m_Dpi.x, m_Dpi.y);
}

void TextFactory::update(void)
{
	for(auto &r : m_TextResources)
		r.second.draw();
}

void TextFactory::shutdown(void)
{
	SAFE_RELEASE(m_FileFontCollection);
	m_FontContext.shutdown();

	m_Device = nullptr;
	SAFE_RELEASE(m_D2DFactory);
	SAFE_RELEASE(m_WriteFactory);

	m_TextResources.clear();
}

TextFactory::Text_Id TextFactory::createText(const wchar_t *p_Text, const char *p_Font, float p_FontSize, Vector4 p_Color)
{
	IDWriteTextLayout *layout = nullptr;
	DWRITE_TEXT_METRICS m;

	IDWriteTextFormat *textFormat = nullptr;
	ID2D1RenderTarget *renderTarget = nullptr;
	ID2D1SolidColorBrush *brush = nullptr;
	ID3D11ShaderResourceView *SRV = nullptr;
	IDXGISurface *surface = nullptr;

	std::vector<wchar_t> wc(strlen(p_Font)+1);
	mbstowcs (wc.data(), p_Font, wc.size());

	HRESULT hr = m_WriteFactory->CreateTextFormat(wc.data(), m_FileFontCollection, DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, p_FontSize, L"en-us", &textFormat);

	if (FAILED(hr))
	{
		hr = m_WriteFactory->CreateTextFormat(wc.data(), NULL, DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, p_FontSize, L"en-us", &textFormat);
	}
	hr = m_WriteFactory->CreateTextLayout(p_Text,(UINT32)wcslen(p_Text), textFormat, 1280,720, &layout);
	if(SUCCEEDED(hr))
	{
		std::wstring t(p_Text);
		layout->GetMetrics(&m);
		SAFE_RELEASE(layout);
		SRV = createSRV(Vector2(m.widthIncludingTrailingWhitespace, m.height));
		surface = getIDXGISurfaceFromSRV(SRV);
	}
	else
	{
		SAFE_RELEASE(layout);
		SAFE_RELEASE(textFormat);
		throw TextFactoryException("Failed to read text dimensions for the texture.", __LINE__, __FILE__);
	}

	if(SUCCEEDED(hr))
	{
		hr = textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		if(SUCCEEDED(hr))
		{
			hr = textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			if(SUCCEEDED(hr))
			{
				hr = textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
				if(SUCCEEDED(hr))
				{
					hr = m_D2DFactory->CreateDxgiSurfaceRenderTarget(surface, &m_DefaultProperties,	&renderTarget);
					if(SUCCEEDED(hr))
					{
						hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(p_Color.x, p_Color.y, p_Color.z, 
							p_Color.w), &brush);
						renderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
						if(SUCCEEDED(hr))
						{
							m_TextResources.insert(std::pair<Text_Id, TextResource>(m_NextTextId, 
								TextResource(renderTarget, SRV, textFormat, brush,
								D2D1::RectF(0.0f, 0.0f,	m.widthIncludingTrailingWhitespace, m.height), p_Text)));
							m_TextResources.at(m_NextTextId).draw();
						}
					}
				}
			}			
		}
	}
	SAFE_RELEASE(surface);

	if(FAILED(hr))
	{
		SAFE_RELEASE(brush);
		SAFE_RELEASE(renderTarget);
		SAFE_RELEASE(textFormat);
		SAFE_RELEASE(SRV);
		throw TextFactoryException("Text to texture creation failed.", __LINE__, __FILE__);
	}

	return m_NextTextId++;
}

TextFactory::Text_Id TextFactory::createText(const wchar_t *p_Text, Vector2 p_TextureSize,
	const char *p_Font, float p_FontSize, Vector4 p_Color)
{
	IDWriteTextFormat *textFormat = nullptr;
	ID2D1RenderTarget *renderTarget = nullptr;
	ID2D1SolidColorBrush *brush = nullptr;
	ID3D11ShaderResourceView *SRV = createSRV(p_TextureSize);
	IDXGISurface *surface = getIDXGISurfaceFromSRV(SRV);

	std::vector<wchar_t> wc(strlen(p_Font)+1);
	mbstowcs (wc.data(), p_Font, wc.size());

	HRESULT hr = m_WriteFactory->CreateTextFormat(wc.data(), m_FileFontCollection, DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, p_FontSize, L"en-us", &textFormat);

	if (FAILED(hr))
	{
		hr = m_WriteFactory->CreateTextFormat(wc.data(), NULL, DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, p_FontSize, L"en-us", &textFormat);
	}

	if(SUCCEEDED(hr))
	{
		hr = textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		if(SUCCEEDED(hr))
		{
			hr = textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			if(SUCCEEDED(hr))
			{
				hr = textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
				if(SUCCEEDED(hr))
				{
					hr = m_D2DFactory->CreateDxgiSurfaceRenderTarget(surface, &m_DefaultProperties,	&renderTarget);
					if(SUCCEEDED(hr))
					{
						hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(p_Color.x, p_Color.y, p_Color.z, 
							p_Color.w), &brush);
						renderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
						if(SUCCEEDED(hr))
						{
							m_TextResources.insert(std::pair<Text_Id, TextResource>(m_NextTextId, 
								TextResource(renderTarget, SRV, textFormat, brush,
								D2D1::RectF(0.0f, 0.0f,	p_TextureSize.x, p_TextureSize.y), p_Text)));
							m_TextResources.at(m_NextTextId).draw();
						}
					}
				}
			}			
		}
	}
	SAFE_RELEASE(surface);
	
	if(FAILED(hr))
	{
		SAFE_RELEASE(brush);
		SAFE_RELEASE(renderTarget);
		SAFE_RELEASE(textFormat);
		SAFE_RELEASE(SRV);
		throw TextFactoryException("Text to texture creation failed.", __LINE__, __FILE__);
	}

	return m_NextTextId++;
}

TextFactory::Text_Id TextFactory::createText(const wchar_t *p_Text, Vector2 p_TextureSize,
	const char *p_Font, float p_FontSize, Vector4 p_Color, TEXT_ALIGNMENT p_TextAlignment,
	PARAGRAPH_ALIGNMENT p_ParagraphAlignment, WORD_WRAPPING p_WordWrapping)
{
	IDWriteTextFormat *textFormat = nullptr;
	ID2D1RenderTarget *renderTarget = nullptr;
	ID2D1SolidColorBrush *brush = nullptr;
	ID3D11ShaderResourceView *SRV = createSRV(p_TextureSize);
	IDXGISurface *surface = getIDXGISurfaceFromSRV(SRV);

	std::vector<wchar_t> wc(strlen(p_Font)+1);
	mbstowcs (wc.data(), p_Font, wc.size());

	HRESULT hr = m_WriteFactory->CreateTextFormat(wc.data(), NULL, DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, p_FontSize, L"en-us", &textFormat);

	if(SUCCEEDED(hr))
	{
		hr = setTextAlignment(textFormat, p_TextAlignment);
		if(SUCCEEDED(hr))
		{
			hr = setParagraphAlignment(textFormat, p_ParagraphAlignment);
			if(SUCCEEDED(hr))
			{
				hr = setWordWrapping(textFormat, p_WordWrapping);
				if(SUCCEEDED(hr))
				{
					hr = m_D2DFactory->CreateDxgiSurfaceRenderTarget(surface, &m_DefaultProperties,	&renderTarget);
					if(SUCCEEDED(hr))
					{
						hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(p_Color.x, p_Color.y, p_Color.z,
							p_Color.w), &brush);
						if(SUCCEEDED(hr))
						{
							m_TextResources.insert(std::pair<Text_Id, TextResource>(m_NextTextId, 
								TextResource(renderTarget, SRV, textFormat, brush,
								D2D1::RectF(0.0f, 0.0f, p_TextureSize.x, p_TextureSize.y), p_Text)));
							m_TextResources.at(m_NextTextId).draw();
						}
					}
				}
			}
		}
	}
	SAFE_RELEASE(surface);

	if(FAILED(hr))
	{
		SAFE_RELEASE(brush);
		SAFE_RELEASE(renderTarget);
		SAFE_RELEASE(textFormat);
		SAFE_RELEASE(SRV);
		throw TextFactoryException("Text to texture creation failed.", __LINE__, __FILE__);
	}

	return m_NextTextId++;
}

void TextFactory::updateText(Text_Id p_Identifier, const wchar_t *p_Text)
{
	if(m_TextResources.count(p_Identifier) > 0)
	{
		std::wstring text = std::wstring(p_Text);
		if(m_TextResources.at(p_Identifier).m_Text != text)
		{
			m_TextResources.at(p_Identifier).m_Text = text;
			m_TextResources.at(p_Identifier).m_Update = true;
		}
	}
	else
		throw TextFactoryException("Failed to update text with identifier: " +
			std::to_string(p_Identifier), __LINE__, __FILE__);
}

void TextFactory::deleteText(Text_Id p_Identifier)
{
	if(m_TextResources.count(p_Identifier) > 0)
		m_TextResources.erase(p_Identifier);
	else
		throw TextFactoryException("Failed to delete text with identifier: " + 
		std::to_string(p_Identifier), __LINE__, __FILE__);

}

void TextFactory::setTextColor(Text_Id p_Identifier, Vector4 p_Color)
{
	if(m_TextResources.count(p_Identifier) > 0)
	{
		m_TextResources.at(p_Identifier).m_Brush->SetColor(D2D1::ColorF(p_Color.x, p_Color.y, p_Color.z, p_Color.w));
		m_TextResources.at(p_Identifier).m_Update = true;
	}
	else
		throw TextFactoryException("Failed to delete text with identifier: " + 
		std::to_string(p_Identifier), __LINE__, __FILE__);

}

void TextFactory::setBackgroundColor(Text_Id p_Identifier, Vector4 p_Color)
{
	if(m_TextResources.count(p_Identifier) > 0)
	{
		m_TextResources.at(p_Identifier).m_ClearColor = D2D1::ColorF(p_Color.x, p_Color.y, p_Color.z, p_Color.w);
		m_TextResources.at(p_Identifier).m_Update = true;
	}
	else
		throw TextFactoryException("Failed to delete text with identifier: " + 
		std::to_string(p_Identifier), __LINE__, __FILE__);

}

void TextFactory::setTextAlignment(Text_Id p_Identifier, TEXT_ALIGNMENT p_Alignment)
{
	if(m_TextResources.count(p_Identifier) > 0)
	{
		setTextAlignment(m_TextResources.at(p_Identifier).m_TextFormat, p_Alignment);
		m_TextResources.at(p_Identifier).m_Update = true;
	}
	else
		throw TextFactoryException("Failed to delete text with identifier: " + 
		std::to_string(p_Identifier), __LINE__, __FILE__);

}

void TextFactory::setParagraphAlignment(Text_Id p_Identifier, PARAGRAPH_ALIGNMENT p_Alignment)
{
	if(m_TextResources.count(p_Identifier) > 0)
	{
		setParagraphAlignment(m_TextResources.at(p_Identifier).m_TextFormat, p_Alignment);
		m_TextResources.at(p_Identifier).m_Update = true;
	}
	else
		throw TextFactoryException("Failed to delete text with identifier: " +
		std::to_string(p_Identifier), __LINE__, __FILE__);

}

void TextFactory::setWordWrapping(Text_Id p_Identifier, WORD_WRAPPING p_Wrapping)
{
	if(m_TextResources.count(p_Identifier) > 0)
	{
		setWordWrapping(m_TextResources.at(p_Identifier).m_TextFormat, p_Wrapping);
		m_TextResources.at(p_Identifier).m_Update = true;
	}
	else
		throw TextFactoryException("Failed to delete text with identifier: " + 
		std::to_string(p_Identifier), __LINE__, __FILE__);
}

ID3D11ShaderResourceView *TextFactory::getSRV(Text_Id p_Identifier) const
{
	if(m_TextResources.count(p_Identifier) > 0)
		return m_TextResources.at(p_Identifier).getSRV();
	else
		throw TextFactoryException("Failed to get ShaderResourceView with identifier: " +
		std::to_string(p_Identifier), __LINE__, __FILE__);
}

ID3D11ShaderResourceView *TextFactory::createSRV(Vector2 p_TextureSize)
{
	ID3D11Texture2D *tex;
	D3D11_TEXTURE2D_DESC desc = {0};
	ID3D11ShaderResourceView *srv;

	desc.ArraySize          = 1 ;
	desc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags     = 0;
	desc.Format             = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.Width              = UINT(p_TextureSize.x);
	desc.Height             = UINT(p_TextureSize.y);
	desc.MipLevels          = 1;
	desc.MiscFlags          = 0;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage              = D3D11_USAGE_DEFAULT;

	m_Device->CreateTexture2D(&desc,nullptr,&tex);
	m_Device->CreateShaderResourceView(tex, nullptr, &srv);
	SAFE_RELEASE(tex);

	return srv;
}

IDXGISurface *TextFactory::getIDXGISurfaceFromSRV(ID3D11ShaderResourceView *p_SRV)
{
	if(!p_SRV)
		throw TextFactoryException("Invalid ShaderResourceView to get IDXGISurface from.", __LINE__, __FILE__);

	ID3D11Resource *res;
	ID3D11Texture2D *tex;
	IDXGISurface *surface;
	p_SRV->GetResource(&res);
	res->QueryInterface(&tex);
	tex->QueryInterface(&surface);
	SAFE_RELEASE(tex);
	SAFE_RELEASE(res);
	return surface;
}

HRESULT TextFactory::setTextAlignment(IDWriteTextFormat *p_Format, TEXT_ALIGNMENT p_Alignment)
{
	HRESULT hr = E_FAIL;

	switch(p_Alignment)
	{
	case TEXT_ALIGNMENT::LEADING:
		{
			hr = p_Format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			break;
		}
	case TEXT_ALIGNMENT::TRAILING:
		{
			hr = p_Format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
			break;
		}
	case TEXT_ALIGNMENT::CENTER:
		{
			hr = p_Format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			break;
		}
	case TEXT_ALIGNMENT::JUSTIFIED:
		{
			hr = p_Format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
			break;
		}

	}

	return hr;
}

HRESULT TextFactory::setParagraphAlignment(IDWriteTextFormat *p_Format, PARAGRAPH_ALIGNMENT p_Alignment)
{
	HRESULT hr = E_FAIL;

	switch (p_Alignment)
	{
	case PARAGRAPH_ALIGNMENT::NEAR_ALIGNMENT:
		{
			hr = p_Format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
			break;
		}
	case PARAGRAPH_ALIGNMENT::FAR_ALIGNMENT:
		{
			hr = p_Format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
			break;
		}
	case PARAGRAPH_ALIGNMENT::CENTER_ALIGNMENT:
		{
			hr = p_Format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
			break;
		}
	}

	return hr;
}

HRESULT TextFactory::setWordWrapping(IDWriteTextFormat *p_Format, WORD_WRAPPING p_Wrapping)
{
	HRESULT hr = E_FAIL;

	switch (p_Wrapping)
	{
	case WORD_WRAPPING::WRAP:
		{
			hr = p_Format->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
			break;
		}
	case WORD_WRAPPING::NO_WRAP:
		{	
			hr = p_Format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
			break;
		}
	}

	return hr;
}