#pragma once

#include "Joint.h"
#include "LoaderStructs.h"
#include "WrapperFactory.h"

#include <memory>
#include <vector>

struct ModelDefinition
{
	std::unique_ptr<Buffer> vertexBuffer;
	std::vector<std::pair<int,int>> drawInterval;
	Shader						*shader;
	std::vector<std::pair< std::string, ID3D11ShaderResourceView*>> diffuseTexture;
	std::vector<std::pair< std::string, ID3D11ShaderResourceView*>> normalTexture;
	std::vector<std::pair< std::string, ID3D11ShaderResourceView*>> specularTexture;
	unsigned int				numOfMaterials;
	bool						m_IsAnimated;

	std::vector<Joint>					m_Joints;

	ModelDefinition(ModelDefinition&& p_Other)
		:	vertexBuffer(std::move(p_Other.vertexBuffer)),
			drawInterval(p_Other.drawInterval),
			shader(p_Other.shader),
			diffuseTexture(p_Other.diffuseTexture),
			normalTexture(p_Other.normalTexture),
			specularTexture(p_Other.specularTexture),
			numOfMaterials(p_Other.numOfMaterials),
			m_IsAnimated(p_Other.m_IsAnimated),
			m_Joints(std::move(p_Other.m_Joints)),
			m_FinalTransform(std::move(p_Other.m_FinalTransform)),
			m_Time(p_Other.m_Time),
			m_currentFrame(p_Other.m_currentFrame)
	{}

	ModelDefinition& operator=(ModelDefinition&& p_Other)
	{
		std::swap(vertexBuffer, p_Other.vertexBuffer);
		std::swap(drawInterval, p_Other.drawInterval);
		std::swap(shader, p_Other.shader);
		std::swap(diffuseTexture, p_Other.diffuseTexture);
		std::swap(normalTexture, p_Other.normalTexture);
		std::swap(specularTexture, p_Other.specularTexture);
		std::swap(numOfMaterials, p_Other.numOfMaterials);
		std::swap(m_IsAnimated, p_Other.m_IsAnimated);
		std::swap(m_Joints, p_Other.m_Joints);
		std::swap(m_FinalTransform, p_Other.m_FinalTransform);
		std::swap(m_Time, p_Other.m_Time);
		std::swap(m_currentFrame, p_Other.m_currentFrame);

		return *this;
	}

private:
	ModelDefinition(const ModelDefinition&);

	std::vector<DirectX::XMFLOAT4X4>	m_FinalTransform;
	float								m_Time;
	float								m_currentFrame;

public:
	ModelDefinition(){
		m_Time = 0.0f;
		m_currentFrame = 0.0f;
	}
	~ModelDefinition(){} // may have to release content to avoid memory leaks.

	void getFinalTransform(float p_Time, std::vector<DirectX::XMFLOAT4X4> &ref);

private:
	void getAnimation();
};