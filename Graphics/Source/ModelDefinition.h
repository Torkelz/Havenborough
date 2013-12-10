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

	//virtual std::vector<DirectX::XMFLOAT4X4>* getFinalTransform(float p_Time){return nullptr;}
	std::vector<Joint>					m_Joints;
	float								m_Time;

	// Temp
	float								m_currentFrame;
	
	std::vector<DirectX::XMFLOAT4X4>	m_FinalTransform;

	//CactusModel() : Model(){}
	//~CactusModel(){}
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

class CactusModel : public Model
{
public:

};