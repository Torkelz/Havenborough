#pragma once

#include "Joint.h"
#include "AnimationStructs.h"

#include <DirectXMath.h>
#include <string>
#include <vector>

/**
 * A single instance of a model object.
 */
 class ModelInstance
 {
 private:
	struct AnimationTrack{
		AnimationClip clip;
		bool crossfade;
		bool active;
		bool layered;
		int fadeFrames; // Original amount of frames to fade
		float fadedFrames; // The amount of frames faded.
		float currentFrame;
		float destinationFrame;
	};

	std::string m_ModelName;
	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_Rotation;
	DirectX::XMFLOAT3 m_Scale;
	
	mutable bool m_IsCalculated;
	mutable DirectX::XMFLOAT4X4 m_World;

	// Animation data
	/**
	 * The matrices that transforms from the animated joint's space to the parent's joint's space.
	 * Row major.
	 */
	std::vector<DirectX::XMFLOAT4X4> m_LocalTransforms;
	/**
	 * The matrices that transforms from bind space to model space with animations.
	 * Row major.
	 */
	std::vector<DirectX::XMFLOAT4X4> m_FinalTransform;
	/**
	 * The current frame time point. Non-integral values results in interpolation.
	 */
	//float m_CurrentFrame;
	//float m_DestinationFrame;
	//AnimationClip m_ActiveClips[2]; // "Tracks"
	//AnimationClip m_FadeClip;

	AnimationTrack m_Tracks[3];

	// Blend stuff
	//bool m_CrossfadeMainTrack;
	//bool m_CrossfadeOffTrack;
	//bool m_Layer;
	//int m_FadeFramesMainTrack;
	//int m_FadeFramesOffTrack;
	//float m_CurrentFadeFrame;
	//float m_DestinationFadeFrame;
	//float m_CurrentOffTrackFrame;
	//float m_DestinationOffTrackFrame;

 public:
	/**
	 * Contructor. Creates an empty object.
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
	 * Update the animation data to the new time.
	 *
	 * @param p_DeltaTime the time since the previous frame.
	 * @param p_Joints the skeleton to be used for the animation.
	 */
	void updateAnimation(float p_DeltaTime, const std::vector<Joint>& p_Joints);
	/**
	 * Get the final transformations for the models joints.
	 *
	 * @return the final joint transformations. Row major.
	 */
	const std::vector<DirectX::XMFLOAT4X4>& getFinalTransform() const;

	/**
	 * Alter the joint transformation to make a joint "reach" for a target point.
	 *
	 * @param p_TargetJointName the name of the joint that should reach for the target.
	 * @param p_HingeJointName the name of the joint that should bend.
	 * @param p_BaseJointName the name of the rotating joint.
	 * @param p_Position the position in world space to reach for.
	 * @param p_Joints the skeleton used for the model.
	 */
	void applyIK_ReachPoint(const std::string& p_TargetJointName, const std::string& p_HingeJointName, const std::string& p_BaseJointName, const DirectX::XMFLOAT3& p_Position, const std::vector<Joint>& p_Joints);
	/**
	 * Get the position of a joint.
	 *
	 * @param p_JointName the name of an existing joint.
	 * @param p_Joints the joints associated with the model instance.
	 */
	DirectX::XMFLOAT3 getJointPos(const std::string& p_JointName, const std::vector<Joint>& p_Joints);

	void playClip(AnimationClip p_Clip, bool p_Layer, bool p_Crossfade, int p_FadeFrames);

 private:
	void calculateWorldMatrix(void) const;
	void updateFinalTransforms(const std::vector<Joint>& p_Joints);
 };