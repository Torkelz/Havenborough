#pragma once
#include "WrapperFactory.h"
#include "Joint.h"

class Model
{
public:
	Buffer						*vertexBuffer;
	Buffer						**indexBuffer;
	Shader						*shader;
	ID3D11ShaderResourceView	**diffuseTexture;
	ID3D11ShaderResourceView	**normalTexture;
	ID3D11ShaderResourceView	**specularTexture;
	unsigned int				numOfMaterials;
	bool						animated;

	std::vector<Joint>			m_Joints;
	float						m_Time;

private:
	std::vector<DirectX::XMFLOAT4X4>	m_FinalTransform;

	// Temp
	float								m_currentFrame;
public:
	Model(){
		m_Time = 0.0f;
		m_currentFrame = 0.0f;
	}
	~Model(){} // may have to release content to avoid memory leaks.

	void getFinalTransform(float p_Time, std::vector<DirectX::XMFLOAT4X4> &ref);

private:
	void getAnimation();
};

class AnimatedModel : public Model
{
public:
	std::vector<Joint>			m_Joints;
	float						m_Time;

private:
	std::vector<DirectX::XMFLOAT4X4>	m_FinalTransform;
	float								m_currentFrame;
public:
	AnimatedModel() : Model(){
		m_Time = 0.0f;
		m_currentFrame = 0.0f;
	}
	~AnimatedModel(){} // may have to release content to avoid memory leaks.

	void getFinalTransform(float p_Time, std::vector<DirectX::XMFLOAT4X4> &ref);

private:
	void getAnimation();
};