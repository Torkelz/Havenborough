#pragma once
#include "PhysicsTypes.h"

class IPhysics
{	
public:
	__declspec(dllexport) static IPhysics *createPhysics(void);
	__declspec(dllexport) static void deletePhysics(IPhysics* p_Physics);

	/**
	 * Deconstructor
	 */
	virtual ~IPhysics() {};

	/**
	 * Initialize all the variables
	 * @param p_IsServer, used to determine wether the collisions should modify body position or not, 
	 * true if the server is initializing the physics, false when the clients do.
	 */
	virtual void initialize(bool p_IsServer) = 0;

	/**
	 * Create a boundingVolume sphere with a body.
	 *
	 * @param p_Mass is defining the mass for the body
	 * @param p_IsImmovable tells if the body are movable
	 * @param p_Position sets the position for the bounding volume(sphere). Body gets the same position. cm.
	 * @param p_Radius sets the radius for the sphere in cm
	 * @return a BodyHandle so it can be mapped outside of Physics
	 */
	virtual BodyHandle createSphere(float p_Mass, bool p_IsImmovable, Vector3 p_Position, float p_Radius) = 0;

	/**
	 * Create a boundingVolume AABB with a body.
	 *
	 * @param p_Mass is defining the mass for the body
	 * @param p_IsImmovable tells if the body are movable
	 * @param p_CenterPos the center position for box in world space in cm
	 * @param p_Extents the half lengths (extents) for the box in cm
	 * @param p_IsEdge true if the bounding volume should be an edge, otherwise false
	 * @return a BodyHandle so it can be mapped outside of Physics
	 */
	virtual BodyHandle createAABB(float p_Mass, bool p_IsImmovable, Vector3 p_CenterPos, Vector3 p_Extents, bool p_IsEdge) = 0;

	/**
	 * Create a boundingVolume OBB with a body.
	 *
	 * @param p_Mass, define the mass for the body.
	 * @param p_IsImmovable, tells if the body are movable.
	 * @param p_CenterPos, Center of the box.
	 * @param p_Extents, Box half lengths.
	 * @param p_IsEdge true if the bounding volume should be an edge, otherwise false
	 * @return a BodyHandle so it can be mapped outside of Physics.
	 */
	virtual BodyHandle createOBB(float p_Mass, bool p_IsImmovable, Vector3 p_CenterPos, Vector3 p_Extents, bool p_IsEdge) = 0;

	/**
	 * Create a single instance of a bounding volume.
	 *
	 * @param p_VolumeId, which loaded resource to 
	 */
	virtual BodyHandle createBVInstance(const char* p_VolumeID) = 0;
	/**
	 * Create bounding volumes based on a level description. Only hulls will be created.
	 *
	 * @param p_VolumeID are the identifier to the volume working with
	 * @param p_FilePath to the filename of the volume
	 * @return true if the volume was successfully created, otherwise false
	 */
	virtual bool createBV(const char* p_VolumeID, const char* p_FilePath) = 0;

	/**
	 * Add a boundingVolume Sphere to an existing body.
	 *
	 * @param p_BodyHandle, which body to add the volume to.
	 * @param p_CenterPos, Center of the box.
	 * @param p_Extents, Box half lengths.
	 */
	virtual void addSphereToBody(BodyHandle p_BodyHandle, Vector3 p_Position, float p_Radius) = 0;

	/**
	 * Add a boundingVolume OBB to an existing body.
	 *
	 * @param p_BodyHandle, which body to add the volume to.
	 * @param p_CenterPos, Center of the box.
	 * @param p_Extents, Box half lengths.
	 */
	virtual void addOBBToBody(BodyHandle p_BodyHandle, Vector3 p_CenterPos, Vector3 p_Extents) = 0;

	/**
	 * Release a previously created volume.
	 *
	 * @param p_VolumeID are the identifier of the volume working with
	 * @return true if the volume existed and was successfully released
	 */
	virtual bool releaseBV(const char* p_VolumeID) = 0;

	/**
	 * Release an existing body.
	 *
	 * @param p_Body a handle to the body to remove
	 */
	virtual void releaseBody(BodyHandle p_Body) = 0;

	/**
     * Releases all the existing bounding volumes.
     */
	virtual void releaseAllBoundingVolumes(void) = 0;

	/**
	 * Used to get the position of the target body.
	 *
	 * @param p_Body is an identifier to know what to work with.
	 * @return a Vector3 containing the position of the target body in cm
	 */
	virtual Vector3 getBodyPosition(BodyHandle p_Body) = 0;

	/**
	 * Used to get the size of the target body.
	 *
	 * @param p_Body is an identifier to know what to work with.
	 * @return a Vector3 with the size of the body in cm
	 */
	virtual Vector3 getBodySize(BodyHandle p_Body) =0;

	/**
	 * Edit the position of the target body and its bounding volume.
	 *
	 * @param p_Body are what body to work with
	 * @param p_Position is a vector3 with all the position coordinates in cm
	 */
	virtual void setBodyPosition(BodyHandle p_Body, Vector3 p_Position) = 0;	
	
	/**
	 * Edit the position of the target body's volume.
	 *
	 * @param p_Body are what body to work with
	 * @param p_Position is a vector3 with all the position coordinates in cm
	 */
	virtual void setBodyVolumePosition(BodyHandle p_Body, unsigned p_Volume, Vector3 p_Position) = 0;

	/**
	 * Sets the velocity for a body.
	 *
	 * @param p_Body, which body to change velocity on.
	 * @param p_Velocity, the new velocity for the body in cm/s
	 */
	virtual void setBodyVelocity(BodyHandle p_Body, Vector3 p_Velocity) = 0;

	/**
	 * Edit the rotation of the target body's bounding volume. Only works on OBB and Hull.
	 *
	 * @param p_Body are what body to work with
	 * @param p_Rotation is a vector3 with all the rotation coordinates 
	 */
	virtual void setBodyRotation(BodyHandle p_Body, Vector3 p_Rotation) = 0;
	/**
	 * Edit the rotation of the target body's bounding volume. Only works on OBB and Hull.
	 *
	 * @param p_Body are what body to work with
	 * @param p_Rotation is a matrix representing a rotation
	 */
	virtual void setBodyRotationMatrix(BodyHandle p_Body, DirectX::XMFLOAT4X4 p_Rotation) = 0;
	/**
	 * Get the velocity for the chosen body.
	 *
	 * @param p_Body what body to get the data from
	 * @return objects velocity in cm/s
     */
	virtual Vector3 getBodyVelocity(BodyHandle p_Body) = 0;
	/**
	 * Edit the scale of the target body's BoundingVolume. When scaling spheres only the x-coordinate is needed. 
	 *
	 * @param p_BodyHandle are what body to work with
	 * @param p_Scale is a vector3 with all the scale coordinates 
	 */
	virtual void setBodyScale(BodyHandle p_BodyHandle, Vector3 p_Scale) = 0;
	
	/**
	 * Keeps physics updated, collision checks etc.
	 *
	 * @param p_DeltaTime are a timestep
	 * @param p_FPSCheckLimit, when the fps has reached this value the physics update will happend more than once / frame used for making it less likely .
	 */
	virtual void update(float p_DeltaTime, unsigned p_FPSCheckLimit) = 0;
	/**
	 * Apply a force on an object.
	 *
	 * @param p_Body are on what body to apply the force
	 * @param p_Force are what force to be added, in Newton (N = (kg*m)/s^2) 
	 */
	virtual void applyForce(BodyHandle p_Body, Vector3 p_Force) = 0;
	/**
	 * Apply an impulse to an object.
	 *
	 * @param p_Body the body to apply the impulse on
	 * @param p_Impulse the impulse to use on the body, in Newton-seconds (N*S = (kg*m)/s)
	 */
	virtual void applyImpulse(BodyHandle p_Body, Vector3 p_Impulse) = 0;
	/**
	 * Used to change the gravity constant.
	 *
	 * @param p_Gravity set the new gravity
	 */
	virtual void setGlobalGravity(float p_Gravity) = 0;
	/**
	 * Sets the amount of gravity the body should be affected by.
	 *
	 * @param p_Gravity the gravity constant to apply to this body from now on, in m/s^2.
	 */
	virtual void setBodyGravity(BodyHandle p_Body, float p_Gravity) = 0;
	/**
     * Get if the target body is in the air (affected by gravity)
     *
	 * @param p_Body the body to work with.
     * @return true if the body is in the air, otherwise false.
     */
    virtual bool getBodyInAir(BodyHandle p_Body) = 0;

	/**
	 * Get the hitdata from the vector containing all the collision hitdata for the last frame.
	 *
	 * @param p_Indexare the index number in the vector
	 * @return the hitdata on that index
	 */
	virtual HitData getHitDataAt(unsigned int p_Index) = 0;

	/**
	 * A bool that turns on the frame where a body lands ontop of something.
	 * @param p_Body, the body to check if its landed.
	 * @return true if this body has landed on something this frame, otherwise false.
	 */
	virtual bool getBodyLanded(BodyHandle p_Body) = 0;
	/**
	 * Vector size, with hitData.
	 *
	 * @param return m_HitDatas.size
	 */
	virtual unsigned int getHitDataSize() = 0;

	/**
	 * Sets if a specific body should interact with physics or just check if the volume has been hit.
	 *
	 * @param p_Body the body which to change the interaction on.
	 * @param p_State, true if if it should be affected by physics, false if not
	 */
	virtual void setBodyCollisionResponse(BodyHandle p_Body, bool p_State) = 0;

	/**
	 * Sets if a specific volume in a body should interact with physics or just check if the volume has been hit.
	 *
	 * @param p_Body which body the volume belongs to.
	 * @param p_Volume, the volume which to change the interaction on.
	 * @param p_State, true if if it should be affected by physics, false if not
	 */
	virtual void setBodyVolumeCollisionResponse(BodyHandle p_Body, int p_Volume, bool p_State) = 0;
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

	/**
	 * Get a made up triangle from a body so that its boundingvolume can be drawn.
	 * @param p_Body are what body to work with
	 * @param p_TriangleIndex, which triangle to draw. Works similar to a index buffer.
	 * @param p_BoundingVolume, which bounding volume in the body.
	 */
	virtual Triangle getTriangleFromBody(unsigned int p_BodyHandle, unsigned int p_TriangleIndex, int p_BoundingVolume) = 0;
	/**
	 * Returns the number of triangles from the body's boundingvolume.
	 * @param p_BodyHandle what body to work with.
	 * @param p_BoundingVolume, which bounding volume in the body.
	 * @return the number of triangles.
	 */
	virtual unsigned int getNrOfTrianglesFromBody(unsigned int p_BodyHandle, int p_BoundingVolume) = 0;

	/**
	 * Returns the number of bounding volumes that exzist in the body.
	 * @param p_BodyHandle what body to work with.
	 * @return the number of volumes in the specdified body.
	 */
	virtual unsigned int getNrOfVolumesInBody(BodyHandle p_BodyHandle) = 0; 

	/**
	 * Returns the 2D orientation of a body's main bounding volume.
	 * @param p_Body, which body to get orientation from.
	 * @returns the orientation of specified body.
	 */
	virtual Vector3 getBodyOrientation(BodyHandle p_Body) = 0;
	/**
	 * Resets all force and velocity on a body.
	 * @param p_Body, which body to reset.
	 */
	virtual void resetForceOnBody(BodyHandle p_Body) = 0;
};