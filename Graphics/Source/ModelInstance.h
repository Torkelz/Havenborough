#pragma once
#include "Joint.h"
#include "Animation.h"
#include "AnimationClip.h"

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

	DirectX::XMFLOAT3 m_ColorTone;

	/**
	 * The matrices that transforms from bind space to model space with animations.
	 * Row major.
	 */
	std::vector<DirectX::XMFLOAT4X4> m_FinalTransform;

 public:

	Animation m_Animation;
	/**
	 * Constructor. Creates an empty object.
	 */
	ModelInstance(void);
	/**
	 * Destructor.
	 */
	~ModelInstance(void);
	
	/**
	 * Set the name of the model definition containing the static model data.
	 *
	 * @param p_Name model definition identifier.
	 */
	void setModelName(const std::string& p_Name);
	/**
	 * Get the name of the model definition associated with the model.
	 *
	 * @return a model definition identifier.
	 */
	std::string getModelName(void) const;

	/**
	 * Get the total world matrix. Contains the rotation,
	 * scale and translation of the model instance.
	 *
	 * @return a row-major world matrix.
	 */
	const DirectX::XMFLOAT4X4 &getWorldMatrix(void) const;
	/**
	 * Set the position of the model instance.
	 *
	 * @param p_Position the new position in world space.
	 */
	void setPosition(const DirectX::XMFLOAT3 &p_Position);
	/**
	 * Set the rotation of the model instance.
	 *
	 * @param p_Rotation the rotation as (roll, pitch, yaw) using left-handed rotations.
	 */
	void setRotation(const DirectX::XMFLOAT3 & p_Rotation);
	/**
	 * Set the scale of the model instance.
	 *
	 * @param p_Scale the new scale.
	 */
	void setScale(const DirectX::XMFLOAT3 &p_Scale);
	/**
	 * Set the color tone of the model instance.
	 *
	 * @param p_ColorTone the color tone.
	 */
	void setColorTone(const DirectX::XMFLOAT3 &p_ColorTone);
	/**
	 * Gets the color tone of the model instance.
	 *
	 * @return the color tone.
	 */
	const DirectX::XMFLOAT3 &getColorTone(void) const;
	/**
	 * Get the final transformations for the models joints.
	 *
	 * @return the final joint transformations. Row major.
	 */
	const std::vector<DirectX::XMFLOAT4X4>& getFinalTransform() const;

	void animationPose(const DirectX::XMFLOAT4X4* p_Pose, unsigned int p_Size);

 private:
	void calculateWorldMatrix(void) const;
 };