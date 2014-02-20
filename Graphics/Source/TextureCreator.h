#pragma once
#pragma comment(lib, "Dwrite")
#pragma comment(lib, "D2d1")

#include <dwrite.h>
#include <d2d1.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <map>

#include "TextResource.h"
#include "TextEnums.h"
#include "Utilities/XMFloatUtil.h"

class TextureCreator
{
private:
	ID2D1Factory *m_D2DFactory;
	IDWriteFactory *m_WriteFactory;
	ID3D11Device *m_Device;

	float m_DpiX, m_DpiY;
	D2D1_RENDER_TARGET_PROPERTIES m_DefaultProperties;

	std::map<std::string, TextResource> m_TextResources;
public:
	TextureCreator(void);
	~TextureCreator(void);

	void initialize(ID3D11Device *p_Device);
	void shutdown(void);

	int createText(std::string p_Identifier, const wchar_t *p_Text, Vector2 p_TextureSize,
		const char *p_Font, float p_FontSize, Vector4 p_Color);
	int createText(std::string p_Identifier, const wchar_t *p_Text, Vector2 p_TextureSize,
		const char *p_Font, float p_FontSize, Vector4 p_Color, TEXT_ALIGNMENT p_TextAlignment,
		PARAGRAPH_ALIGNMENT p_ParagraphAlignment, WORD_WRAPPING p_WordWrapping);

	void updateText(std::string p_Identifier, const wchar_t *p_Text);
	void deleteText(std::string p_Identifier);

	ID3D11ShaderResourceView *getSRV(std::string p_Identifier) const;

private:
	ID3D11ShaderResourceView *createSRV(D2D1_RECT_F p_Rect);
	IDXGISurface *getIDXGISurfaceFromSRV(ID3D11ShaderResourceView *p_SRV);
};

