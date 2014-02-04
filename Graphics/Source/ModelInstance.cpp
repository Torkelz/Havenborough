#include "ModelInstance.h"
#include "GraphicsExceptions.h"
#include "GraphicsLogger.h"

using namespace DirectX;
using std::string;
using std::vector;

ModelInstance::ModelInstance()
	: m_IsCalculated(false), 
	m_ColorTone(DirectX::XMFLOAT3(1.f, 1.f, 1.f))
{
}

ModelInstance::~ModelInstance() {}

void ModelInstance::setModelName(const string& p_Name)
{
	m_ModelName = p_Name;
}

string ModelInstance::getModelName(void) const
{
	return m_ModelName;
}

const XMFLOAT4X4& ModelInstance::getWorldMatrix() const
{
	if (!m_IsCalculated)
	{
		calculateWorldMatrix();
	}
	return m_World;
}

void ModelInstance::setPosition(const XMFLOAT3& p_Position)
{
	m_Position = p_Position;
	m_IsCalculated = false;
}

void ModelInstance::setRotation(const XMFLOAT3& p_Rotation)
{
	m_Rotation = p_Rotation;
	m_IsCalculated = false;
}

void ModelInstance::setScale(const XMFLOAT3& p_Scale)
{
	m_Scale = p_Scale;
	m_IsCalculated = false;
}

void ModelInstance::setColorTone(const XMFLOAT3& p_ColorTone)
{
	m_ColorTone = p_ColorTone;
}

const XMFLOAT3 &ModelInstance::getColorTone() const
{
	return m_ColorTone;
}

void ModelInstance::calculateWorldMatrix() const
{
	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_Rotation));
	XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position));
	XMMATRIX scale = XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale));

	XMStoreFloat4x4(&m_World, XMMatrixTranspose(scale * rotation * translation));

	m_IsCalculated = true;
}

const vector<DirectX::XMFLOAT4X4>& ModelInstance::getFinalTransform() const
{
	return m_FinalTransform;
}

void ModelInstance::animationPose(const DirectX::XMFLOAT4X4* p_Pose, unsigned int p_Size)
{
	m_FinalTransform.assign(p_Pose, p_Pose + p_Size);
}