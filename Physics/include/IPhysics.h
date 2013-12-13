#pragma once

#include "PhysicsTypes.h"

#include <cstdint>

class IPhysics
{	
public:
	__declspec(dllexport) static IPhysics *createPhysics(void);
	__declspec(dllexport) static void deletePhysics(IPhysics* p_Physics);

	virtual ~IPhysics() {};

	virtual void initialize() = 0;
	/**
	 * Create a boundingVolume sphere with a body.
	 * @p_Mass, define the mass for the body.
	 * @p_IsImmovable, tells if the body are movible.
	 * @p_Position, position for the boundingvolume(sphere). Body get same position.
	 * @p_Radius, the radius for the sphere.
	 * @return a BodyHandle so it can be mapped outside of Physics.
	 */
	virtual BodyHandle createSphere(float p_Mass, bool p_IsImmovable, Vector3 p_Position, float p_Radius) = 0;
	/**
	 * Create a boundingVolume AABB with a body, the center position is calculated from the bot and top corner.
	 * @p_Mass, define the mass for the body.
	 * @p_IsImmovable, tells if the body are movible.
	 * @p_Bot, bottom corner for box.
	 * @p_Top, top corner for box.
	 * @return a BodyHandle so it can be mapped outside of Physics.
	 */
	virtual BodyHandle createAABB(float p_Mass, bool p_IsImmovable, Vector3 p_Bot, Vector3 p_Top) = 0;

	virtual BodyHandle createOBB(float p_Mass, bool p_IsImmovable, Vector3 p_CenterPos, Vector3 p_Extent, Matrix4x4 p_RotMatrix) = 0;


	virtual Vector4 getBodyPosition(BodyHandle p_Body) = 0;
	

	virtual void setBodyPosition(Vector3 p_Position, BodyHandle p_Body) = 0;

	virtual void setBodyRotation(BodyHandle p_Body, float p_Yaw, float p_Pitch, float p_Roll) = 0;

	/**
	 * Keeps physics updated, collision checks etc.
	 * @p_DeltaTime, timestep.
	 */
	virtual void update(float p_DeltaTime) = 0;
	/**
	 * Apply a force on an object.
	 * @p_Force, force to be added.
	 * @p_Body, what body to apply the force.
	 */
	virtual void applyForce(Vector4 p_Force, BodyHandle p_Body) = 0;
	/**
	 * Used to change the gravity constant.
	 * @p_Gravity, the new gravity.
	 */
	virtual void setGlobalGravity(float p_Gravity) = 0;
	/**
	 * Get the velocity for the chosen body.
	 * @p_Body, what body to get the data from.
	 * @return objects velocity.
	 */
	virtual Vector4 getVelocity(BodyHandle p_Body) = 0;
	/**
	 * Get the hitdata from the vector containing all the collision hitdata for the last frame.
	 * @p_Index, index number in the vector.
	 * @return the hitdata on that index.
	 */
	virtual HitData getHitDataAt(unsigned int p_Index) = 0;
	/**
	 * Vector size, with hitData.
	 * @return m_HitDatas.size
	 */
	virtual unsigned int getHitDataSize() = 0;

	/**
	* Return the boundingvolume from the body.
	* @p_Body, what body to get the data from.
	* @return a BoundingVolum pointer.
	*/
	//virtual BoundingVolume* getVolume(BodyHandle p_Body) = 0;


	/**
	 * Callback for logging.
	 *
	 * @param p_Level log priority level. Higher is more important.
	 * @param p_Message the log message.
	 */
	typedef void (*clientLogCallback_t)(uint32_t p_Level, const char* p_Message);

	/**
	 * Set the function to handle log messages.
	 *
	 * @param p_LogCallback the function to be called whenever a message is to
	 *			be logged from this component. Set to null to disable logging.
	 */
	virtual void setLogFunction(clientLogCallback_t p_LogCallback) = 0;
};