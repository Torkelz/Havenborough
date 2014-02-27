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
	/**
	* Constructor.
	*/
	TextRenderer(void);

	/**
	* Destructor.
	*/
	~TextRenderer(void);

	/**
	* Initializes all needed variables for rendering text. Throws exception if failing.
	* @param p_Device the DirectX device used for rendering
	* @param p_DeviceContex the DirectX device context used for rendering
	* @param p_CameraPosition the position of the camera, used when rendering
	* @param p_ViewMatrix the view matrix, used when rendering
	* @param p_ProjectionMatrix the projection matrix, used when rendering
	* @param p_RenderTarget the target to render to, should be the same as the others
	*/
	void initialize(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext, DirectX::XMFLOAT3 *p_CameraPosition,
		DirectX::XMFLOAT4X4 *p_ViewMatrix, DirectX::XMFLOAT4X4 *p_ProjectionMatrix, ID3D11RenderTargetView *p_RenderTarget);

	/**
	* Adds a new text object to a list of all existing objects. Throws exception if ID already exists.
	* @param p_InstanceId the ID of the text object when created
	* @param p_Object the text object
	*/
	void addTextObject(TextId p_InstanceId, TextInstance &p_Object);

	/**
	* Removes a text object from the list of all existing objects. Throws exception if ID does not exist.
	* @param p_InstanceId the ID of the object to be removed
	*/
	void removeTextObject(TextId p_InstanceId);
	
	/**
	* Adds a text object to be rendered next time renderFrame is called. Throws exception ID does not exist.
	* @param p_InstanceId the ID of the object to be rendered
	*/
	void renderTextObject(TextId p_InstanceId);
	
	/**
	* Call to render text objects. All the objects that are supposed to be rendered
	* must have been added to the list by #renderTextObject each frame before calling this function.
	*/
	void renderFrame(void);

	/**
	* Sets a position in world space for a text object. Throws exception if ID does not exist.
	* @param p_InstanceId the ID of the text object
	* @param p_Position the world position the text should be rendered at, in cm's
	*/
	void setPosition(TextId p_InstanceId, Vector3 p_Position);

	/**
	* Sets a uniform scaling factor of a text object. Throws exception if ID does not exist.
	* @param p_InstanceId the ID of the text object
	* @param p_Scale the scale for the object, 1.0f is default value
	*/
	void setScale(TextId p_InstanceId, float p_Scale);

	/**
	* Sets a rotation around the z-axis of the text in model space. Throws exception if ID does not exist.
	* @param p_InstanceId the ID of the text object
	* @param p_Rotation the rotation in radians
	*/
	void setRotation(TextId p_InstanceId, float p_Rotation);

private:
	void createBuffers(void);
	void createSamplerState(void);
	void createBlendState(void);
	void createRasterizerState(void);

	void updateConstantBuffers(void);

	bool depthSortCompareFunc(const TextInstance &a, const TextInstance &b);
};