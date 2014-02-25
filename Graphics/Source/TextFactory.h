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
	typedef int Text_Id;
	ID2D1Factory *m_D2DFactory;
	IDWriteFactory *m_WriteFactory;
	ID3D11Device *m_Device;

	Vector2 m_Dpi;
	D2D1_RENDER_TARGET_PROPERTIES m_DefaultProperties;
	
	std::map<Text_Id, TextResource> m_TextResources;

	int m_NextTextId;
public:
	/**
	* Constructor.
	*/
	TextFactory(void);

	/**
	* Destructor.
	*/
	~TextFactory(void);

	/**
	* Initializes the text factory. Throws exception if failing.
	* @param p_Device, the DirecX device to be used
	*/
	void initialize(ID3D11Device *p_Device);

	/**
	* Shuts the text factory down and releases the text resources created.
	*/
	void shutdown(void);

	/**
	* Creates a text resource. Throws exception if failing.
	* @param p_Text the text to be created as resource
	* @param p_TextureSize the size, in pixels, of the texture the text should be rendered to
	* @param p_Font the font to be used
	* @param p_FontSize the size of the font to be used
	* @param p_Color the color the text should be rendered with, RGBA from 0.0f to 1.0f
	* @return the ID of the created text as unsigned int
	*/
	Text_Id createText(const wchar_t *p_Text, Vector2 p_TextureSize, const char *p_Font, float p_FontSize,
		Vector4 p_Color);
	
	/**
	* Creates a text resource. Throws exception if failing.
	* @param p_Text the text to be created as resource
	* @param p_TextureSize the size, in pixels, of the texture the text should be rendered to
	* @param p_Font the font to be used
	* @param p_FontSize the size of the font to be used
	* @param p_Color the color the text should be rendered with, RGBA from 0.0f to 1.0f
	* @param p_TextAlignment alignment of text in a paragraph, relative to the leading and trailing edge of a layout box
	* @param p_ParagraphAlignment alignment option of a paragraph relative to the layout box' top and bottom edge
	* @param p_WordWrapping if word wrapping should be used or not for text that do not fit the texture's width
	* @return the ID of the created text as unsigned int
	*/
	Text_Id createText(const wchar_t *p_Text, Vector2 p_TextureSize, const char *p_Font, float p_FontSize,
		Vector4 p_Color, TEXT_ALIGNMENT p_TextAlignment, PARAGRAPH_ALIGNMENT p_ParagraphAlignment,
		WORD_WRAPPING p_WordWrapping);

	/**
	* Change the rendered text of an existing text object. Throws exception if object does not exist.
	* @param p_Identifier the ID of the text object to be changed
	* @param p_Text the text to be rendered
	*/
	void updateText(Text_Id p_Identifier, const wchar_t *p_Text);

	/**
	* Deletes an existing text object. Throws exception if object does not exist.
	* @param p_Identifier the ID of the text object to be deleted
	*/
	void deleteText(Text_Id p_Identifier);
	
	/**
	* Change the color of the text of an existing text object. Throws exception if object does not exist.
	* @param p_Identifier the ID of the text object to be changed
	* @param p_Color the color to be used, in RGBA 0.0f to 1.0f
	*/
	void setTextColor(Text_Id p_Identifier, Vector4 p_Color);
	
	/**
	* Change the background color of an existing text object. Throws exception if object does not exist.
	* @param p_Identifier the ID of the text object to be changed
	* @param p_Color the color to be used, in RGBA 0.0f to 1.0f
	*/
	void setBackgroundColor(Text_Id p_Identifier, Vector4 p_Color);
	
	/**
	* Change the alignment of text in a paragraph, relative to the leading and trailing edge of a layout box.
	*	Throws exception if object does not exist.
	* @param p_Identifier the ID of the text object to be changed
	* @param p_Alignment the alignment to be used
	*/
	void setTextAlignment(Text_Id p_Identifier, TEXT_ALIGNMENT p_Alignment);
	
	/**
	* Change the alignment option of a paragraph relative to the layout box' top and bottom edge.
	*	Throws exception if object does not exist.
	* @param p_Identifier the ID of the text object to be changed
	* @param p_Alignment the alignment to be used
	*/
	void setParagraphAlignment(Text_Id p_Identifier, PARAGRAPH_ALIGNMENT p_Alignment);
	
	/**
	* Change if word wrapping should be used or not for text that do not fit the texture's width.
	*	Throws exception if object does not exist.
	* @param p_Identifier the ID of the text object to be changed
	* @param p_Wrapping the wrapping to be used, on or off
	*/
	void setWordWrapping(Text_Id p_Identifier, WORD_WRAPPING p_Wrapping);
	
	/**
	* Gets the texture with of an existing text object. Throws exception if object does not exist.
	* @return the texture object
	*/
	ID3D11ShaderResourceView *getSRV(Text_Id p_Identifier) const;

private:
	ID3D11ShaderResourceView *createSRV(Vector2 p_TextureSize);
	IDXGISurface *getIDXGISurfaceFromSRV(ID3D11ShaderResourceView *p_SRV);
	HRESULT setTextAlignment(IDWriteTextFormat *p_Format, TEXT_ALIGNMENT p_Alignment);
	HRESULT setParagraphAlignment(IDWriteTextFormat *p_Format, PARAGRAPH_ALIGNMENT p_Alignment);
	HRESULT setWordWrapping(IDWriteTextFormat *p_Format, WORD_WRAPPING p_Wrapping);
};

