#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "ShaderDefinitions.h"
#include "TextEnums.h"
#include <TweakSettings.h>
#include <Utilities/Util.h>


class IGraphics
{
public:
	enum class RenderTarget : unsigned int
	{
		DIFFUSE,
		NORMAL,
		W_POSITION,
		SSAO,
		FINAL,
		SSAOPing,
		CSM,
		NONE,
	};

	/**
	* Unique ID for a model instance, starts on 1.
	*/
	typedef int InstanceId;

	/**
	* Unique ID for a 2D object instance, starts on 1.
	*/
	typedef int Object2D_Id;

	/**
	* Unique ID for a text instance, starts on 1.
	*/
	typedef int Text_Id;
		
	/**
	 * Callback for loading a texture to a model.
	 *
	 * @param p_ResourceName the resource name of the texture
	 * @param p_FilePath path to where the texture is located
	 * @param p_UserData user defined data
	 */
	typedef void (*loadModelTextureCallBack)(const char *p_ResourceName, const char *p_FilePath, void *p_Userdata);

	/**
	 * Callback for releasing a texture to a model.
	 *
	 * @param p_ResourceName the resource name of the texture
	 * @param p_UserData user defined data
	 */
	typedef void (*releaseModelTextureCallBack)(const char *p_ResourceName, void *p_Userdata);

	/**
	 * Callback for logging.
	 *
	 * @param p_Level log priority level. Higher is more important.
	 * @param p_Message the log message.
	 */
	typedef void (*clientLogCallback_t)(uint32_t p_Level, const char* p_Message);

public:
	/**
	* Destructor.
	*/
	virtual ~IGraphics(void)
	{}

	/**
	 * Create a pointer from which the graphics library can be accessed.
	 *
	 * @return the graphics pointer
	 */
	__declspec(dllexport) static IGraphics *createGraphics(void);
	
	/**
	 * Initialize the graphics API.
	 *
	 * @param p_Hwnd the handle to the window which the graphics should be connected to
	 * @param p_ScreenWidth input of the window's width
	 * @param p_ScreenHeight input of the window's height
	 * @param p_Fullscreen input whether the program should run in full screen or not
	 * @return true if successful, otherwise false
	 */
	virtual bool initialize(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight,	bool p_Fullscreen) = 0;

	/**
	 * Reinitialize parts of the graphics API when switching full screen on/off or changing resolution.
	 *
	 * @param p_Hwnd the handle to the window which the graphics should be connected to
	 * @param p_ScreenWidth input of the window's width
	 * @param p_ScreenHeight input of the window's height
	 * @param p_Fullscreen input whether the program should run in full screen or not
	 * @return true if successful, otherwise false
	 */
	virtual bool reInitialize(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight, bool p_Fullscreen) = 0;

	/**
	 * Clear sub resources allocated by the graphics API and delete the pointer. 
	 */
	__declspec(dllexport) static void deleteGraphics(IGraphics *p_Graphics);

	/**
	 * Creates a new static or animated model and stores in a vector connected with an ID.
	 *
	 * @param p_ModelId the ID of the model
	 * @param p_Filename the filename of the model
	 * @return true if the model was successfully created, otherwise false
	 */
	virtual bool createModel(const char *p_ModelId, const char *p_Filename) = 0;

	/**
	* Release a previously created model.
	*
	* @param p_ResourceName the resource name of the resource
	* @return true if the resource existed and was successfully released
	*/
	virtual bool releaseModel(const char* p_ResourceName) = 0;

	/**
	 * Automatically creates a shader based on layout in the shader file and stores in a vector connected with and ID.
	 *
	 * @param p_ShaderId the ID of the shader
	 * @param p_Filename the file where the shader code is located
	 * @param p_EntryPoint the main entry point in the shader file, can be combined as e.g.
	 *		 "mainVS,mainPS,mainGS,mainHS,mainDS", note this order is important to be kept but all steps are not necessary,
	 *		 note the ',' is the separator
	 * @param p_ShaderModel the shader model version to be used, e.g. "5_0"
	 * @param p_ShaderType the shader types to be created, can be combined as
	 *		 ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER | ShaderType::GEOMETRY_SHADER | ShaderType::HULL_SHADER | ShaderType::DOMAIN_SHADER
	 */
	virtual void createShader(const char *p_shaderId, LPCWSTR p_Filename, const char *p_EntryPoint,
		const char *p_ShaderModel, ShaderType p_Type) = 0;

	/**
	 * Creates a new shader object with user defined vertex layout. If shader ID already exists or no vertex shader type 
	 * is added an exception is thrown.
	 *
	 * @param p_ShaderId the ID of the shader that should be created, note if the ID already exists an exception will be thrown
	 * @param p_Filename the file where the shader code is located
	 * @param p_EntryPoint the main entry point in the shader file, can be combined as e.g.
	 *		 "mainVS,mainPS,mainGS,mainHS,mainDS", note this order is important to be kept but all steps are not necessary,
	 *		 note the ',' is the separator
	 * @param p_ShaderModel the shader model version to be used, e.g. "5_0"
	 * @param p_ShaderType the shader types to be created, can be combined as
	 *		 ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER | ShaderType::GEOMETRY_SHADER | ShaderType::HULL_SHADER | ShaderType::DOMAIN_SHADER,
	 *		 note that vertex shader needs to be included or an exception will be thrown
	 * @param p_VertexLayout the user defined vertex layout
	 * @param p_NumOfElement the number of elements in the layout
	 */
	virtual void createShader(const char *p_shaderId, LPCWSTR p_Filename, const char *p_EntryPoint,
		const char *p_ShaderModel, ShaderType p_Type, ShaderInputElementDescription *p_VertexLayout,
		unsigned int p_NumOfElements) = 0;

	/**
	 * Establish a map of shader name to a model name.
	 *
	 * @param p_ShaderId name of the shader
	 * @param p_ModelId name of the model
	 */
	virtual void linkShaderToModel(const char *p_ShaderId, const char *p_ModelId) = 0;

	/**
	* Deletes an existing shader.
	*
	* @param p_ShaderId name of the shader
	*/
	virtual void deleteShader(const char *p_ShaderId) = 0;

	/**
	 * Creates a new texture and stores in a vector connected with an ID.
	 * WARNING: Should only be called by the resource manager.
	 *
	 * @param p_TextureId the ID of the texture
	 * @param p_Filename the filename of the texture
	 * @return true if the texture was successfully loaded, otherwise false
	 */
	virtual bool createTexture(const char *p_TextureId, const char *p_Filename) = 0;

	/**
	 * Release a previously created texture.
	 *
	 * @param p_TextureId the ID of the texture
	 * @return true if the texture existed and was successfully released.
	 */
	virtual bool releaseTexture(const char *p_TextureId) = 0;
	
	/** 
	 * Creates a new particle system and stores in a vector connected with an ID.
	 *
	 * @param p_ParticleSystemId the ID of the particle system
	 * @param p_Filename the filename of the particle system
	 * @return true if the particle system was successfully loaded, otherwise false
	 */
	virtual bool createParticleEffectDefinition(const char *p_FileId, const char *p_FilePath) = 0;

	/** 
	 * Release a previously created particle system.
	 * 
	 * @param p_ParticleSystemId the ID of the particle system
	 * @return true if the particle system existed and was successfully released.
	 */
	virtual bool releaseParticleEffectDefinition(const char *p_ParticleEffectId) = 0;

	/**
	 * Call the particle factory to create an instance from a definition
	 * 
	 * @param p_ParticleEffectId name and work as an identity pointer
	 * @return an index in the form of an int
	 */
	virtual InstanceId createParticleEffectInstance(const char *p_ParticleEffectId) = 0;

	/**
	 * Called to release instances of particle effects
	 * 
	 * @param p_ParticleEffectId index to what instance to work with
	 */
	virtual void releaseParticleEffectInstance(InstanceId p_ParticleEffectId) = 0;

	/**
	 * Update a particle instances position with a completely new position, in cm
	 * 
	 * @param p_ParticleEffectId what instance to work with
	 * @param p_Position the new position, in cm
	 */
	virtual void setParticleEffectPosition(InstanceId p_ParticleEffectId, Vector3 p_Position) = 0;

	virtual void setParticleEffectRotation(InstanceId p_ParticleEffectId, Vector3 p_Rotation) = 0;

	virtual void setParticleEffectBaseColor(InstanceId p_ParticleEffectId, Vector4 p_BaseColor) = 0;
	/**
	 * Called when updating the particles
	 * 
	 * @param p_DeltaTime time between frames
	 */
	virtual void updateParticles(float p_DeltaTime) = 0;
		
	/**
	 * Establish a map of shader name to a particle name.
	 *
	 * @param p_ShaderId name of the shader
	 * @param p_ModelId name of the particle
	 */
	virtual void linkShaderToParticles(const char *p_ShaderId, const char *p_ParticlesId) = 0;

	/**
	* Creates a 2D object from a texture to be used by the screen renderer.
	* @param p_Position the xy-pixel coordinates to place the object on, z the depth in range of 0.0f to 1280.0f
	* @param p_HalfSize the object's pixel size in xy from the center point
	* @param p_Scale the scaling factor where 1.0f is the default model size
	* @param p_Rotation the rotation around the z-axis in radians, left-handed
	* @param p_TextureId the ID of the texture to be used
	* @return the Object2D ID of the created object
	*/
	virtual Object2D_Id create2D_Object(Vector3 p_Position, Vector2 p_HalfSize, Vector3 p_Scale, float p_Rotation,
		const char *p_TextureId) = 0;

	/**
	* Creates a 2D object from a text texture to be used by the screen renderer.
	* @param p_Position the xy-pixel coordinates to place the object on, z the depth in range of 0.0f to 1280.0f
	* @param p_Scale the scaling factor where 1.0f is the default model size
	* @param p_Rotation the rotation around the z-axis in radians, left-handed
	* @param p_TextureId the ID of the texture to be used
	* @return the Object2D ID of the created object
	*/
	virtual Object2D_Id create2D_Object(Vector3 p_Position, Vector3 p_Scale, float p_Rotation, Text_Id p_TextureId) = 0;
	
	/**
	* Creates a 2D object from a model definition to be used by the screen renderer.
	* @param p_Position the xy-pixel coordinates to place the object on, z the depth in range of 0.0f to 1280.0f
	* @param p_Scale the scaling factor where 1.0f is the default model size
	* @param p_Rotation the rotation around the z-axis in radians, left-handed
	* @param p_ModelDefinition the ID of the model definition
	* @return the Object2D ID of the created object
	*/
	virtual Object2D_Id create2D_Object(Vector3 p_Position, Vector3 p_Scale, float p_Rotation,
		const char *p_ModelDefinition) = 0;

	/**
	* Creates a text resource which can be used as texture.
	* @param p_Text the text to be created as resource
	* @param p_TextureSize the size, in pixels, of the texture the text should be rendered to
	* @param p_Font the font to be used
	* @param p_FontSize the size of the font to be used
	* @param p_FontColor the color the text should be rendered with, RGBA from 0.0f to 1.0f
	* @param p_Position the world position the text should be rendered at, in cm's
	* @param p_Scale the uniform scale for the object, 1.0f is default value
	* @param p_Rotation the rotation in model space in radians
	*/
	virtual Text_Id createText(const wchar_t *p_Text, Vector2 p_TextureSize, const char *p_Font, float p_FontSize,
		Vector4 p_FontColor, Vector3 p_Position, float p_Scale, float p_Rotation) = 0;

	/**
	* Creates a text resource which can be used as texture.
	* @param p_Text the text to be created as resource
	* @param p_TextureSize the size, in pixels, of the texture the text should be rendered to
	* @param p_Font the font to be used
	* @param p_FontSize the size of the font to be used
	* @param p_FontColor the color the text should be rendered with, RGBA from 0.0f to 1.0f
	* @param p_TextAlignment alignment of text in a paragraph, relative to the leading and trailing edge of a layout box
	* @param p_ParagraphAlignment alignment option of a paragraph relative to the layout box' top and bottom edge
	* @param p_WordWrapping if word wrapping should be used or not for text that do not fit the texture's width
	* @param p_Position the world position the text should be rendered at, in cm's
	* @param p_Scale the uniform scale for the object, 1.0f is default value
	* @param p_Rotation the rotation in model space in radians
	*/
	virtual Text_Id createText(const wchar_t *p_Text, Vector2 p_TextureSize, const char *p_Font, float p_FontSize,
		Vector4 p_FontColor, TEXT_ALIGNMENT p_TextAlignment, PARAGRAPH_ALIGNMENT p_ParagraphAlignment, 
		WORD_WRAPPING p_WordWrapping, Vector3 p_Position, float p_Scale, float p_Rotation) = 0;

	/**
	 * Creates a point light which is removed after each draw.
	 *
	 * @ p_LightPosition, the position of the light object.
	 * @ p_LightColor, the color of the light.
	 * @ p_LightRange, the range of the point light.
	 */
	virtual void useFramePointLight(Vector3 p_LightPosition, Vector3 p_LightColor, float p_LightRange) = 0;
	/**
	 * Creates a spot light which is removed after each draw.
	 *
	 * @ p_LightPosition, the position of the light object.
	 * @ p_LightColor, the color of the light.
	 * @ p_LightDirection, the direction of the spot light.
	 * @ p_SpotLightAngles, angles in radians where the x component is smaller than the y component.
	 * @ p_LightRange, the range of the spot light.
	 */
	virtual void useFrameSpotLight(Vector3 p_LightPosition, Vector3 p_LightColor, Vector3 p_LightDirection,
		Vector2 p_SpotLightAngles,	float p_LightRange) = 0;
	/**
	 * Creates a directional light which is removed after each draw.
	 *
	 * @ p_LightColor, the color of the light.
	 * @ p_LightDirection, the direction of the directional light.
	 */
	virtual void useFrameDirectionalLight(Vector3 p_LightColor, Vector3 p_LightDirection, float p_Intensity) = 0;
	
	/**
	* Sets which color the final render target should be cleared in.
	*
	* @param the color in RGBA, 0 to 1;
	*/
	virtual void setClearColor(Vector4 p_Color) = 0;
	
	/**
	 * Renders a model specified with an ID.
	 *
	 * @param p_ModelId the ID of the model to be rendered
	 */
	virtual void renderModel(InstanceId p_ModelId) = 0;
	/**
	 * Renders the created Skydome.
	 */
	virtual void renderSkydome(void) = 0;

	/**
	* Renders a text specified with an ID.
	* @param p_Id the ID of the text to be rendered
	*/
	virtual void renderText(Text_Id p_Id) = 0;

	/**
	* Renders a 2D object specified with an ID.
	* @param p_Id the ID of the object to be rendered.
	*/
	virtual void render2D_Object(Object2D_Id p_Id) = 0;
	
	/**
	 * Draw the current frame.
	 */
	virtual void drawFrame(void) = 0;

	/**
	 * Enable or disable if a model should be rendered with transparency or not, using forward shader.
	 *
	 * @param p_ModelId the model's ID to be set transparency on/off
	 * @param p_State the state if transparency should be enabled or not, true = transparency, false = no transparency
	 */
	virtual void setModelDefinitionTransparency(const char *p_ModelId, bool p_State) = 0;

	/**
	 * Set the pose of the model. Requires the model to be animated.
	 *
	 * @param p_Instance the model instance to update the pose of
	 * @param p_Pose an array of joint matrices describing the pose
	 * @param p_Size the number of matrices in p_Pose
	 */
	virtual void animationPose(int p_Instance, const DirectX::XMFLOAT4X4* p_Pose, unsigned int p_Size) = 0;

	/**
	 * Gets the amount of VRAM usage of the program.
	 *
	 * @return the usage in MB
	 */
	virtual int getVRAMUsage(void) = 0;
	
	/**
	 * Create an instance of a model. Call {@link #eraseModelInstance(int)} to remove.
	 *
	 * @param p_ModelId the resource identifier for the model to draw the instance with.
	 * @return a unique id used to reference the instance with in later calls.
	 */
	virtual InstanceId createModelInstance(const char *p_ModelId) = 0;

	/**
	 * Create an instance of a model. Call {@link #eraseModelInstance(int)} to remove.
	 *
	 * @param p_ModelId the resource identifier for the model to draw the instance with.
	 */
	virtual void createSkydome(const char *p_TextureResource, float p_Radius) = 0;

	/**
	 * Erase an existing model instance.
	 *
	 * @param p_Instance an identifier to a model instance.
	 */
	virtual void eraseModelInstance(InstanceId p_Instance) = 0;

	/**
	 * Set the position of a model instance in absolute world coordinates.
	 *
	 * @param p_Instance an identifier to a model instance.
	 * @param p_Position the new position in the world from origin in cm's
	 */
	virtual void setModelPosition(InstanceId p_Instance, Vector3 p_Position) = 0;

	/**
	 * Set the rotation of a model instance in radians.
	 *
	 * @param p_Instance an identifier to a model instance.
	 * @param p_YawPitchRoll rotation around the YXZ axises, left-handed.
	 */
	virtual void setModelRotation(InstanceId p_Instance, Vector3 p_YawPitchRoll) = 0;

	/**
	 * Set the scale of a model instance.
	 *
	 * @param p_Instance an identifier to a model instance.
	 * @param p_Scale the model scale, Vector3(1.0f, 1.0f, 1.0f) equals no scale 
	 */
	virtual void setModelScale(InstanceId p_Instance, Vector3 p_Scale) = 0;

	/**
	 * Set the color tone of a model instance.
	 *
	 * @param p_Instance an identifier to a model instance.
	 * @param p_ColorTone the color tone to shade the model in, RGB range 0.0f to 1.0f
	 */
	virtual void setModelColorTone(InstanceId p_Instance, Vector3 p_ColorTone) = 0;

	/**
	* Get the pixel position on screen of a 2D object.
	* @param p_Instance an identifier to an object
	*/
	virtual Vector3 get2D_ObjectPosition(Object2D_Id p_Instance) = 0;
	
	/**
	* Get the size of a 2D object in xyz.
	* @param p_Instance an identifier to an object
	*/
	virtual Vector2 get2D_ObjectHalfSize(Object2D_Id p_Instance) = 0;

	/**
	* Set the pixel position on screen of a 2D object.
	* @param p_Instance an identifier to an object
	* @param p_Position xy the pixel coordinates to place the center of the object, z the position relative to
	*	other 2D objects where lower z renders in front of higher
	*/
	virtual void set2D_ObjectPosition(Object2D_Id p_Instance, Vector3 p_Position) = 0;
	
	/**
	* Set the scale of a 2D object in xyz.
	* @param p_Instance an identifier to an object
	* @param p_Scale scaling factor where 1.0f is the default model size
	*/
	virtual void set2D_ObjectScale(Object2D_Id p_Instance, Vector3 p_Scale) = 0;
	
	/**
	* Set the rotation of a 2D object around the screen z-axis.
	* @param p_Instance an identifier to an object
	* @param p_Rotation the rotation in radians, left-handed
	*/
	virtual void set2D_ObjectRotationZ(Object2D_Id p_Instance, float p_Rotation) = 0;
	
	/**
	* Set a position in world space which a 2D object should point towards.
	* @param p_Instance an identifier to an object
	* @param p_LookAt the position in the world
	*/
	virtual void set2D_ObjectLookAt(Object2D_Id p_Instance, Vector3 p_LookAt) = 0;
	
	/**
	* releases a 2D model
	* @param p_ObjectID an identifier to an object
	* @return true if it succeds to remove. false if it did not.
	*/
	virtual bool release2D_Model(Object2D_Id p_ObjectID) = 0;
	
	/**
	* Change the rendered text of an existing text object.
	* @param p_Identifier the ID of the text object to be changed
	* @param p_Text the text to be rendered
	*/
	virtual void updateText(Text_Id p_Identifier, const wchar_t *p_Text) = 0;
	
	/**
	* Deletes an existing text object.
	* @param p_Identifier the ID of the text object to be deleted
	*/
	virtual void deleteText(Text_Id p_Identifier) = 0;
	
	/**
	* Change the color of the text of an existing text object.
	* @param p_Identifier the ID of the text object to be changed
	* @param p_Color the color to be used, in RGBA 0.0f to 1.0f
	*/
	virtual void setTextColor(Text_Id p_Identifier, Vector4 p_Color) = 0;
	
	/**
	* Change the background color of an existing text object.
	* @param p_Identifier the ID of the text object to be changed
	* @param p_Color the color to be used, in RGBA 0.0f to 1.0f
	*/
	virtual void setTextBackgroundColor(Text_Id p_Identifier, Vector4 p_Color) = 0;
	
	/**
	* Change the alignment of text in a paragraph, relative to the leading and trailing edge of a layout box.
	* @param p_Identifier the ID of the text object to be changed
	* @param p_Alignment the alignment to be used
	*/
	virtual void setTextAlignment(Text_Id p_Identifier, TEXT_ALIGNMENT p_Alignment) = 0;
	
	/**
	* Change the alignment option of a paragraph relative to the layout box' top and bottom edge.
	* @param p_Identifier the ID of the text object to be changed
	* @param p_Alignment the alignment to be used
	*/
	virtual void setTextParagraphAlignment(Text_Id p_Identifier, PARAGRAPH_ALIGNMENT p_Alignment) = 0;
	
	/**
	* Gets the texture with of an existing text object.
	* @return the texture object
	*/
	virtual void setTextWordWrapping(Text_Id p_Identifier, WORD_WRAPPING p_Wrapping) = 0;
	
	/**
	* Sets a position in world space for a text object.
	* @param p_InstanceId the ID of the text object
	* @param p_Position the world position the text should be rendered at, in cm's
	*/
	virtual void setTextPosition(Text_Id p_Identifier, Vector3 p_Position) = 0;
	
	/**
	* Sets a uniform scaling factor of a text object.
	* @param p_InstanceId the ID of the text object
	* @param p_Scale the scale for the object, 1.0f is default value
	*/
	virtual void setTextScale(Text_Id p_Identifier, float p_Scale) = 0;
	
	/**
	* Sets a rotation around the z-axis of the text in model space.
	* @param p_InstanceId the ID of the text object
	* @param p_Rotation the rotation in radians
	*/
	virtual void setTextRotation(Text_Id p_Identifier, float p_Rotation) = 0;

	/**
	 * Update the position and viewing direction of the camera.
	 *
	 * @param p_PosX camera position X in absolute world coordinates.
	 * @param p_PosY camera position Y in absolute world coordinates.
	 * @param p_PosZ camera position Z in absolute world coordinates.
	 * @param p_Yaw the camera rotation around the up axis, positive to the right.
	 * @param p_Pitch the camera pitch, positive down.
	 */
	virtual void updateCamera(Vector3 p_Position, Vector3 p_Forward, Vector3 p_Up) = 0;

	/**
	 * Add the triangles for a bounding volume to graphics making it possible to render BV. 
	 *
	 * @param p_Corner1, specified in world coordinates.
	 * @param p_Corner2, specified in world coordinates.
	 * @param p_Corner3, specified in world coordinates.
	 */
	virtual void addBVTriangle(Vector3 p_Corner1, Vector3 p_Corner2, Vector3 p_Corner3) = 0;
	
	/**
	 * Set the function to load a texture to a model.
	 *
	 * @param p_LoadModelTexture the function to be called whenever a texture is to be loaded.
	 * @param p_UserData user defined data
	 */
	virtual void setLoadModelTextureCallBack(loadModelTextureCallBack p_LoadModelTexture, void *p_Userdata) = 0;

	/**
	 * Set the function to release a texture to a model.
	 *
	 * @param p_LoadModelTexture the function to be called whenever a texture is to be released.
	 * @param p_UserData user defined data
	 */
	virtual void setReleaseModelTextureCallBack(releaseModelTextureCallBack p_ReleaseModelTexture, void *p_Userdata) = 0;

	/**
	 * Set the function to handle log messages.
	 *
	 * @param p_LogCallback the function to be called whenever a message is to
	 *			be logged from this component. Set to null to disable logging.
	 */
	virtual void setLogFunction(clientLogCallback_t p_LogCallback) = 0;

	/**
	 * Set the tweaker object to use to tweak variables at runtime.
	 *
	 * @param p_Tweaker pointer to the master tweaker
	 */
	virtual void setTweaker(TweakSettings* p_Tweaker) = 0;

	/**
	 * Change the render target.
	 *
	 * @param p_RenderTarget the render target to display on the next drawFrame call
	 */
	virtual void setRenderTarget(RenderTarget p_RenderTarget) = 0;

	/**
	* Sebbobi: What does the world mean?
	*/
	virtual void renderJoint(DirectX::XMFLOAT4X4 p_World) = 0;
	
	/*
	 * Enables or disables Vsync depending on parameter.
	 *
	 * @param p_State, true enables Vsync. false disables Vsync.
	 */
	virtual void enableVsync(bool p_State) = 0;

	/*
	 * Enables or disables SSAO depending on parameter.
	 *
	 * @param p_State, true enables SSAO. false disables SSAO.
	 */
	virtual void enableSSAO(bool p_State) = 0;

	/*
	 * Enables or disables Shadow Map depending on parameter.
	 *
	 * @param p_State, true enables shadow mapping. false disables shadow mapping.
	 */
	virtual void enableShadowMap(bool p_State) = 0;

	/*
	 * Set the shadow map resolution size.
	 *
	 * @param p_ShadowMapResolution is the size of one side of the quadric map.
	 */
	virtual void setShadowMapResolution(int p_ShadowMapResolution) = 0;

private:

	/**
	 * Release the sub resources allocated by the graphics API.
	 */
	virtual void shutdown(void) = 0;

	/**
	* Creates the least necessary shaders to run the game.
	*/
	virtual void createDefaultShaders(void) = 0;
};
