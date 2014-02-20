#include "TextureCreator.h"
#include "GraphicsExceptions.h"
#include "Utilities/MemoryUtil.h"


TextureCreator::TextureCreator(void)
{
	m_D2DFactory = nullptr;
	m_WriteFactory = nullptr;
	m_Device = nullptr;
	m_DpiX = m_DpiY = 0.f;
}


TextureCreator::~TextureCreator(void)
{
}

void TextureCreator::initialize(ID3D11Device *p_Device)
{
	m_Device = p_Device;

	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_D2DFactory);
	if(FAILED(hr))
		throw TextureCreationException("Failed to create D2DFactory", __LINE__, __FILE__);

	m_D2DFactory->GetDesktopDpi(&m_DpiX, &m_DpiY);

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_WriteFactory));
	if(FAILED(hr))
		throw TextureCreationException("Failed to create WriteFactory", __LINE__, __FILE__);

	m_DefaultProperties = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), m_DpiX, m_DpiY);
}

void TextureCreator::shutdown()
{
	m_Device = nullptr;
	SAFE_RELEASE(m_D2DFactory);
	SAFE_RELEASE(m_WriteFactory);
	m_TextResources.clear();
}

void TextureCreator::createText(std::string p_Identifier, const wchar_t *p_Text, D2D1_RECT_F p_Rect, Vector4 p_Color)
{
	if(m_TextResources.count(p_Identifier) > 0)
		throw TextureCreationException("Tried to create an already existing text with identifier: " + p_Identifier, __LINE__, __FILE__);
	
	IDWriteTextFormat *TextFormat;
	ID2D1RenderTarget *RenderTarget;
	ID2D1SolidColorBrush *Brush;
	ID3D11ShaderResourceView *SRV = createSRV(p_Rect);
	HRESULT hr = m_WriteFactory->CreateTextFormat(L"Gabriola", NULL, DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 72.f, L"en-us", &TextFormat);

	if(SUCCEEDED(hr))
	{
		hr = TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}
	if(SUCCEEDED(hr))
	{
		hr = TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		TextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
	}

	hr = m_D2DFactory->CreateDxgiSurfaceRenderTarget(getIDXGISurfaceFromSRV(SRV), &m_DefaultProperties,
		&RenderTarget);

	if(SUCCEEDED(hr))
	{
		hr = RenderTarget->CreateSolidColorBrush(D2D1::ColorF(p_Color.x,p_Color.y,p_Color.z,p_Color.w), &Brush);
	}

	m_TextResources.at(p_Identifier) = TextResource(RenderTarget, SRV, TextFormat, Brush, p_Rect);
	m_TextResources.at(p_Identifier).Draw(p_Text);
}

void TextureCreator::updateText(std::string p_Identifier, const wchar_t *p_Text)
{
	if(m_TextResources.count(p_Identifier) > 0)
		return m_TextResources.at(p_Identifier).Draw(p_Text);
	else
		throw TextureCreationException("Failed to update text with identifier: " + p_Identifier, __LINE__, __FILE__);
}

const ID3D11ShaderResourceView *TextureCreator::getSRV(std::string p_Identifier) const
{
	if(m_TextResources.count(p_Identifier) > 0)
		return m_TextResources.at(p_Identifier).getSRV();
	else
		throw TextureCreationException("Failed to get ShaderResourceView with identifier: " + p_Identifier, __LINE__, __FILE__);
}

ID3D11ShaderResourceView *TextureCreator::createSRV(D2D1_RECT_F p_Rect)
{
	ID3D11Texture2D *tex;
	D3D11_TEXTURE2D_DESC desc = {0};
	desc.ArraySize          = 1 ;
	desc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE ;
	desc.CPUAccessFlags     = 0;
	desc.Format             = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.Width              = UINT(p_Rect.right - p_Rect.left);
	desc.Height             = UINT(p_Rect.bottom - p_Rect.top);
	desc.MipLevels          = 1;
	desc.MiscFlags          = 0;//D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX ; // because it will be shared between d3d10.1/d2d1 and d3d11
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage              = D3D11_USAGE_DEFAULT;

	m_Device->CreateTexture2D(&desc,nullptr,&tex);
	ID3D11ShaderResourceView *ret;
	m_Device->CreateShaderResourceView(tex, nullptr, &ret);
	SAFE_RELEASE(tex);

	return ret;
}

IDXGISurface *TextureCreator::getIDXGISurfaceFromSRV(ID3D11ShaderResourceView *p_SRV)
{
	if(!p_SRV)
		throw TextureCreationException("Invalid ShaderResourceView to get IDXGISurface from.", __LINE__, __FILE__);

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