#pragma once

#include "PhysicsTypes.h"

#include <cstdint>

class IPhysics
{	
public:
	__declspec(dllexport) static IPhysics *createPhysics(void);
	__declspec(dllexport) static void deletePhysics(IPhysics* p_Physics);

	/**
	 * de-constructor
	 */
	virtual ~IPhysics() {};

	/**
	 * initalize all the varables
	 */
	virtual void initialize() = 0;

	/**
	 * Create a boundingVolume sphere with a body.
	 *
	 * @param p_Mass is defineing the mass for the body
	 * @param p_IsImmovable tells if the body are movable
	 * @param p_Position sets the position for the boundingvolume(sphere). Body gets the same position.
	 * @param p_Radius sets the radius for the sphere
	 * @return a BodyHandle so it can be mapped outside of Physics
	 */
	virtual BodyHandle createSphere(float p_Mass, bool p_IsImmovable, Vector3 p_Position, float p_Radius) = 0;

	/**
	 * Create a boundingVolume AABB with a body.
	 *
	 * @param p_Mass is defineing the mass for the body
	 * @param p_IsImmovable tells if the body are movable
	 * @param p_CenterPos the center position for box in world space
	 * @param p_Extents the half lengths (extents) for the box
	 * @param p_IsEdge true if the bounding volume should be an edge, otherwise false
	 * @return a BodyHandle so it can be mapped outside of Physics
	 */
	virtual BodyHandle createAABB(float p_Mass, bool p_IsImmovable, Vector3 p_CenterPos, Vector3 p_Extents, bool p_IsEdge) = 0;

	/**
	 * Create a boundingVolume OBB with a body.
	 *
	 * @param p_Mass, define the mass for the body.
	 * @param p_IsImmovable, tells if the body are movible.
	 * @param p_CenterPos, Center of the box.
	 * @param p_Extents, Box half lengths.
	 * @return a BodyHandle so it can be mapped outside of Physics.
	 */
	virtual BodyHandle createOBB(float p_Mass, bool p_IsImmovable, Vector3 p_CenterPos, Vector3 p_Extents, bool p_IsEdge) = 0;

	/**
	 * Create boundingvolumes based on a level description.
	 *
	 * @param p_VolumeID are the identifier to the volume working with
	 * @param p_FilePath to the filename of the volume
	 * @return true if the volume was successfully created, otherwise false
	 */
	virtual bool createLevelBV(const char* p_VolumeID, const char* p_FilePath) = 0;

	/**
	 * Release a previously created volume.
	 *
	 * @param p_VolumeID are the identifier of the volume working with
	 * @return true if the volume existed and was successfully released
	 */
	virtual bool releaseLevelBV(const char* p_VolumeID) = 0;

	/**
	 * Edit the position of the target boundingvolume.
	 *
	 * @param p_Instance are what boundingvolume to work with
	 * @param p_Position is a vector3 with all the position coordinates  
	 */
	virtual void setBVPosition(int p_Instance, Vector3 p_Position) = 0;

	/**
	 * Edit the rotation of the target boundingvolume.
	 *
	 * @param p_Instance are what boundingvolume to work with
	 * @param p_Rotation is a vector3 with all the rotation coordinates 
	 */
	virtual void setBVRotation(int p_Instance, Vector3 p_Rotation) = 0;

	/**
	 * Edit the scale of the target boundingvolume.
	 *
	 * @param p_Instance are what boundingvolume to work with
	 * @param p_Scale is a vector3 with all the scale coordinates 
	 */
	virtual void setBVScale(int p_Instance, Vector3 p_Scale) = 0;

	/**
	 * Used to get the position of the target body.
	 *
	 * @param p_Body is an identifier to know what to work with.
	 * @return a Vector4 containing the position of the target body
	 */
	virtual Vector4 getBodyPosition(BodyHandle p_Body) = 0;

	/**
	 * Used to get the size of the target body.
	 *
	 * @param p_Body is an identifier to know what to work with.
	 * @return a Vector3 with the size of the body
	 */
	virtual Vector3 getBodySize(BodyHandle p_Body) =0;

	/**
	 * Edit the position of the target body.
	 *
	 * @param p_Position is a vector3 with all the position coordinates
	 * @param p_Body are what body to work with
	 */
	virtual void setBodyPosition(Vector3 p_Position, BodyHandle p_Body) = 0;

	/**
	 * Edit the rotation of the target body.
	 *
	 * @param p_Body are what body to work with
	 * @param p_Yaw the rotation around the Y-axis, in radians, positive to the right
	 * @param p_Pitch the rotation around the X-axis, in radians, positive up
	 * @param p_Roll the rotation around the Z-axis, in radians, positive clock-wise
	 */
	virtual void setBodyRotation(BodyHandle p_Body, float p_Yaw, float p_Pitch, float p_Roll) = 0;

	/**
	 * Sets the velocity for a body.
	 *
	 * @param p_Velocity, the new velocity for thee body.
	 * @param p_Body, which body to change velocity on.
	 */
	virtual void setBodyVelocity(Vector3 p_Velocity, BodyHandle p_Body) = 0;
	
	/**
	 * Keeps physics updated, collision checks etc.
	 *
	 * @param p_DeltaTime are a timestep
	 */
	virtual void update(float p_DeltaTime) = 0;
	/**
	 * Apply a force on an object.
	 *
	 * @param p_Force are what force to be added
	 * @param p_Body are on what body to apply the force
	 */
	virtual void applyForce(Vector4 p_Force, BodyHandle p_Body) = 0;
	/**
	 * Used to change the gravity constant.
	 *
	 * @param p_Gravity set the new gravity
	 */
	virtual void setGlobalGravity(float p_Gravity) = 0;
	/**
	 * Get the velocity for the chosen body.
	 *
	 * @param p_Body what body to get the data from
	 * @return objects velocity
	 */
	virtual Vector4 getVelocity(BodyHandle p_Body) = 0;
	/**
	 * Get the hitdata from the vector containing all the collision hitdata for the last frame.
	 *
	 * @param p_Indexare the index number in the vector
	 * @return the hitdata on that index
	 */
	virtual HitData getHitDataAt(unsigned int p_Index) = 0;

	/**
	 * Removes the hitdata from the vector containing all the collision hitdata for the last frame at specific position.
	 *
	 * @param p_Index are the index number in the vector
	 */
	virtual void removedHitDataAt(unsigned int p_index) = 0;
	/**
	 * Vector size, with hitData.
	 *
	 * @param return m_HitDatas.size
	 */
	virtual unsigned int getHitDataSize() = 0;

	/**
	 * Return the boundingvolume from the body.
	 *
	 * @param p_Body are what body to get the data from
	 * @return a BoundingVolum pointer
	 */
	//virtual BoundingVolume* getVolume(BodyHandle p_Body) = 0;

	/**
	 * Callback for logging.
	 *
	 * @param p_Level log priority level. Higher is more important
	 * @param p_Message the log message
	 */
	typedef void (*clientLogCallback_t)(uint32_t p_Level, const char* p_Message);

	/**
	 * Set the function to handle log messages.
	 *
	 * @param p_LogCallback the function to be called whenever a message is to
	 *			be logged from this component. Set to null to disable logging
	 */
	virtual void setLogFunction(clientLogCallback_t p_LogCallback) = 0;
};