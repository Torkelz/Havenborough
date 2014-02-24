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

class TextFactory
{
private:
	ID2D1Factory *m_D2DFactory;
	IDWriteFactory *m_WriteFactory;
	ID3D11Device *m_Device;

	Vector2 m_Dpi;
	D2D1_RENDER_TARGET_PROPERTIES m_DefaultProperties;
	
	std::map<std::string, TextResource> m_TextResources;

	int m_NextTextId;
public:
	TextFactory(void);
	~TextFactory(void);

	void initialize(ID3D11Device *p_Device);
	void shutdown(void);

	int createText(std::string p_Identifier, const wchar_t *p_Text, Vector2 p_TextureSize,
		const char *p_Font, float p_FontSize, Vector4 p_Color);
	int createText(std::string p_Identifier, const wchar_t *p_Text, Vector2 p_TextureSize,
		const char *p_Font, float p_FontSize, Vector4 p_Color, TEXT_ALIGNMENT p_TextAlignment,
		PARAGRAPH_ALIGNMENT p_ParagraphAlignment, WORD_WRAPPING p_WordWrapping);

	/**
	* Change the rendered text of an existing text object. Throws exception if object does not exist.
	* @param p_Identifier the ID of the text object to be changed
	* @param p_Text the text to be rendered
	*/
	void updateText(std::string p_Identifier, const wchar_t *p_Text);

	void deleteText(std::string p_Identifier);
	void setTextColor(std::string p_Identifier, Vector4 p_Color);
	void setBackgroundColor(std::string p_Identifier, Vector4 p_Color);
	void setTextSize(std::string p_Identifier, float p_Size);
	void setTextAlignment(std::string p_Identifier, TEXT_ALIGNMENT p_Alignment);
	void setParagraphAlignment(std::string p_Identifier, PARAGRAPH_ALIGNMENT p_Alignment);
	void setWordWrapping(std::string p_Identifier, WORD_WRAPPING p_Wrapping);

	ID3D11ShaderResourceView *getSRV(std::string p_Identifier) const;

private:
	ID3D11ShaderResourceView *createSRV(Vector2 p_TextureSize);
	IDXGISurface *getIDXGISurfaceFromSRV(ID3D11ShaderResourceView *p_SRV);
	HRESULT setTextAlignment(IDWriteTextFormat *p_Format, TEXT_ALIGNMENT p_Alignment);
	HRESULT setParagraphAlignment(IDWriteTextFormat *p_Format, PARAGRAPH_ALIGNMENT p_Alignment);
	HRESULT setWordWrapping(IDWriteTextFormat *p_Format, WORD_WRAPPING p_Wrapping);
};

