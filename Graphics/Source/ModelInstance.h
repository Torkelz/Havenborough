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
	struct AnimationTrack
	{
		AnimationClip clip; // Constant animation data

		// Dynamic animation data
		bool active;
		float fadedFrames; // The amount of frames faded.
		float currentFrame;
		float destinationFrame;
		float dynamicWeight;
	};

	std::string m_ModelName;
	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_Rotation;
	DirectX::XMFLOAT3 m_Scale;
	
	mutable bool m_IsCalculated;
	mutable DirectX::XMFLOAT4X4 m_World;

	DirectX::XMFLOAT3 m_ColorTone;

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
	 * The animation tracks contain the timestamp information and animation clip data needed for animations and blends.
	 * Track 0 is the forward track. It contains whole body animations that are in the z-axis in Maya. It also contains
	 *		axis-less animations like jump and dance.
	 * Track 1 is the fade in or layered track for track 0.
	 * Track 2 is the first extra track. It contains whole body animations that are in the x-axis in Maya. E.g. strafe.
	 * Track 3 is the fade in or layered track for track 2.
	 * Track 4 is the second extra track. Is supports partial blends like wave and has logic for fade ins and fade outs.
	 * Track 5 is the fade in or layered track for track 4.
	 */
	AnimationTrack m_Tracks[6];
	std::vector<AnimationClip> m_Queue;

 public:
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
	void applyIK_ReachPoint(const std::string& p_TargetJointName, const std::string& p_HingeJointName, 
		const std::string& p_BaseJointName, const DirectX::XMFLOAT3& p_Position, const std::vector<Joint>& p_Joints);
	
	/**
	 * Get the position of a joint.
	 *
	 * @param p_JointName the name of an existing joint.
	 * @param p_Joints the joints associated with the model instance.
	 */
	DirectX::XMFLOAT3 getJointPos(const std::string& p_JointName, const std::vector<Joint>& p_Joints);

	/**
	 * Play an animation clip.
	 * @param p_Clip the AnimationClip struct contains all the frame and blend information needed.
	 * @param p_Override, false if you want standard behavior and true if you want to skip blending etc.
	 */
	void playClip( AnimationClip p_Clip, bool p_Override );

	/**
	 * Queue animation clip.
	 * @param p_Clip the AnimationClip struct contains all the frame and blend information needed.
	 * NOTE: Queued clips cannot override the main track of a pair.
	 */
	void queueClip( AnimationClip p_Clip );

	/**
	 * Use this function to dynamicly change weight between e.g. the forward and strafe animations.
	 * @param p_MainTrack the first track in the track pair that has to change weight.
	 * @param p_Weight the new goal weight.
	 * NOTE: Can only be used on track pairs.
	 */
	void changeWeight(int p_MainTrack, float p_Weight );

 private:
	void calculateWorldMatrix(void) const;
	void updateFinalTransforms(const std::vector<Joint>& p_Joints);
	bool affected(const std::vector<Joint>& p_Joints, int p_ID, std::string p_FirstAffectedJoint);
	bool playQueuedClip(int p_Track);
	void checkFades();
	void updateTimeStamp(float p_DeltaTime);
	MatrixDecomposed updateKeyFrameInformation(Joint p_Joint, unsigned int p_CurrentTrack, MatrixDecomposed p_ToParentData);
 };