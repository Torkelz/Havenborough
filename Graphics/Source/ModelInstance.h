#pragma once

#include "Joint.h"

#include <DirectXMath.h>
#include <string>
#include <vector>

/**
 * A single instance of a model object.
 */
 class ModelInstance
 {
 private:
	std::string m_ModelName;
	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_Rotation;
	DirectX::XMFLOAT3 m_Scale;
	
	mutable bool m_IsCalculated;
	mutable DirectX::XMFLOAT4X4 m_World;

	// Animation data
	std::vector<DirectX::XMFLOAT4X4> m_LocalTransforms;
	std::vector<DirectX::XMFLOAT4X4> m_FinalTransform;
	float m_CurrentFrame;

 public:
	ModelInstance(void);
	~ModelInstance(void);
	
	void setIsCalculated(bool p_IsCalculated);
	bool getIsCalculated(void) const;
	void setModelName(const std::string& p_Name);
	std::string getModelName(void) const;

	const DirectX::XMFLOAT4X4 &getWorldMatrix(void) const;
	void setPosition(const DirectX::XMFLOAT3 &p_Position);
	void setRotation(const DirectX::XMFLOAT3 & p_Rotation);
	void setScale(const DirectX::XMFLOAT3 &p_Scale);
	void calculateWorldMatrix(void) const;
	 
	void updateAnimation(float p_DeltaTime, const std::vector<Joint>& p_Joints);
	const std::vector<DirectX::XMFLOAT4X4>& getFinalTransform() const;

	void applyIK_ReachPoint(const std::string& p_JointName, const DirectX::XMFLOAT3& p_Position, const std::vector<Joint>& p_Joints);

 private:
	void updateFinalTransforms(const std::vector<Joint>& p_Joints);
 };