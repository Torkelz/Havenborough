#pragma once
#include "WrapperFactory.h"
#include "ConstantBuffers.h"
#include "Utilities/XMFloatUtil.h"
#include <d3d11.h>
#include <map>
#include <vector>

class TextRenderer
{
private:
	typedef int TextId;
public:
	struct TextInstanceShaderData
	{
		Vector3 position;
		Vector2 size;
		float scale;
		float rotation;
	};

	struct TextInstance
	{
		TextInstanceShaderData data;
		ID3D11ShaderResourceView *srv;

		TextInstance(Vector3 p_Position, Vector2 p_Size, float p_Scale, float p_Rotation,
			ID3D11ShaderResourceView *p_Srv)
		{
			data.position = p_Position;
			data.size = p_Size;
			data.scale = p_Scale;
			data.rotation = p_Rotation;
			srv = p_Srv;
		}

		~TextInstance()
		{
			srv = nullptr;
		}
	};

private:
	ID3D11Device *m_Device;
	ID3D11DeviceContext *m_DeviceContext;

	DirectX::XMFLOAT3 *m_CameraPosition;
	DirectX::XMFLOAT4X4 *m_ViewMatrix;
	DirectX::XMFLOAT4X4 *m_ProjectionMatrix;

	ID3D11RenderTargetView *m_RenderTargetView;

	std::map<TextId, TextInstance> m_TextList;
	std::vector<TextInstance> m_RenderList;

	Shader *m_Shader;
	Buffer *m_Buffer;
	Buffer *m_ConstantBuffer;

	ID3D11SamplerState *m_Sampler;
	ID3D11RasterizerState *m_RasterState;
	ID3D11BlendState *m_TransparencyAdditiveBlend;

public:
	TextRenderer(void);
	~TextRenderer(void);

	void initialize(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext, DirectX::XMFLOAT3 *p_CameraPosition,
		DirectX::XMFLOAT4X4 *p_ViewMatrix, DirectX::XMFLOAT4X4 *p_ProjectionMatrix, ID3D11RenderTargetView *p_RenderTarget);

	void addTextObject(TextId p_Instance, TextInstance &p_Object);
	void removeTextObject(TextId p_Instance);
	
	void renderTextObject(TextId p_Instance);
	
	/**
	* Call to render objects to specific screen coordinates. All the objects that are supposed to be rendered
	* must have been added to the list each frame before calling this function.
	*/
	void renderFrame(void);

	void setPosition(TextId p_Instance, Vector3 p_Position);
	void setScale(TextId p_Instance, float p_Scale);
	void setRotation(TextId p_Instance, float p_Rotation);

private:
	void createBuffers(void);
	void createSamplerState(void);
	void createBlendState(void);
	void createRasterizerState(void);

	void updateConstantBuffers(void);

	bool depthSortCompareFunc(const TextInstance &a, const TextInstance &b);
};