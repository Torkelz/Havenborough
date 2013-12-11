#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class IGraphics
{
public:
	enum class ShaderType
	{
		VERTEX_SHADER = 1,
		PIXEL_SHADER = 2,
		GEOMETRY_SHADER = 4,
		HULL_SHADER = 8,
		DOMAIN_SHADER = 16
	};
	inline friend ShaderType operator|(ShaderType a, ShaderType b)
	{
		return static_cast<ShaderType>(static_cast<int>(a) | static_cast<int>(b)); 
	};

	inline friend bool operator&(ShaderType a, ShaderType b)
	{
		return static_cast<int>(a) & static_cast<int>(b) ? true : false; 
	};

	enum class Format
	{
		R32G32B32A32_FLOAT = 2,
		R32G32B32A32_UINT = 3,
		R32G32B32_FLOAT = 6,
		R32G32B32_UINT = 7,
		R32G32_FLOAT = 16,
		R32G32_UINT = 17,
		R8G8B8A8_UNORM = 28,
	};

	struct ShaderInputElementDescription
	{
		LPCSTR semanticName;
		UINT semanticIndex; 
		Format format;
		UINT inputSlot;
		UINT alignedByteOffset;
		UINT inputSlotClass;
		UINT instanceDataStepRate;
	};

	virtual ~IGraphics(void)
	{}

	/**
	* Create a pointer from which the graphics library can be accessed.
	* @return the graphics pointer
	*/
	__declspec(dllexport) static IGraphics *createGraphics(void);
	
	/**
	* Initialize the graphics API.
	* @param p_Hwnd the handle to the window which the graphics should be connected to
	* @param p_ScreenWidth input of the window's width
	* @param p_ScreenHeight input of the window's height
	* @param p_Fullscreen input whether the program should run in fullscreen or not
	* @return true if successful, otherwise false
	*/
	virtual bool initialize(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight,	bool p_Fullscreen) = 0;

	/**
	* Reinitialize parts of the graphics API when switching fullscreen on/off or changing resoluton.
	* @param p_Hwnd the handle to the window which the graphics should be connected to
	* @param p_ScreenWidth input of the window's width
	* @param p_ScreenHeight input of the window's height
	* @param p_Fullscreen input whether the program should run in fullscreen or not
	* @return true if successful, otherwise false
	*/
	virtual bool reInitialize(HWND p_Hwnd, int p_ScreenWidth, int p_ScreenHeight, bool p_Fullscreen) = 0;

	
	/**
	* Clear sub resources allocated by the graphics API and delete the pointer. 
	*/
	__declspec(dllexport) static void deleteGraphics(IGraphics *p_Graphics);

	/**
	* Creates a new model and stores in a vector connected with an ID.
	* @param p_ModelId the ID of the model
	* @param p_Filename the filename of the model
	*/
	virtual bool createModel(const char *p_ModelId, const char *p_Filename) = 0;
	//virtual void renderModel(Buffer *p_Buffer,Buffer *p_ConstantBuffer,
	virtual bool releaseModel(const char* p_ResourceName) = 0;
	/**
	* Automatically creates a shader based on layout in the shader file and stores in a vector connected with and ID.
	* @param p_ShaderId the ID of the shader
	* @param p_Filename the file where the shader code is located
	* @param p_EntryPoint the main entry point in the shader file, can be combined as e.g.
	*		 "mainVS,mainPS,mainGS,mainHS,mainDS", note this order is important to be kept but all steps are not necessary,
	*		 note the ',' is the separator
	* @param p_ShaderModel the shader model version to be used, e.g. "5_0" 
	* @param p_ShaderType the shader types to be created, can be combined as
	*		 ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER | ShaderType::GEOMETRY_SHADER | ShaderType::HULL_SHADER | ShaderType::DOMAIN_SHADER
	*/
	virtual void createShader(const char *p_shaderId, LPCWSTR p_Filename,
		const char *p_EntryPoint, const char *p_ShaderModel, ShaderType p_Type) = 0;

	/**
	* Creates a new shader object with user defined vertex layout. If shader ID already exists or no vertex shader type 
	* is added an exception is thrown.
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
	virtual void createShader(const char *p_shaderId, LPCWSTR p_Filename,
		const char *p_EntryPoint, const char *p_ShaderModel, ShaderType p_Type,
		ShaderInputElementDescription *p_VertexLayout, unsigned int p_NumOfElements) = 0;

	/**
	* Establish a map of shader name to a model name.
	* @param p_ShaderId name of the shader
	* @param p_ModelId name of the model
	*/
	virtual void linkShaderToModel(const char *p_ShaderId, const char *p_ModelId) = 0;

	/**
	* Creates a new texture and stores in a vector connected with an ID.
	* @param p_TextureId the ID of the texture
	* @param p_Filename the filename of the texture
	* @return true if the texture was successfully loaded, otherwise false
	*/
	virtual bool createTexture(const char *p_TextureId, const char *p_filename) = 0;

	/**
	 * Release a previously created texture.
	 *
	 * @param p_TextureId the ID of the texture
	 * @return true if the texture existed and was successfully released.
	 */
	virtual bool releaseTexture(const char *p_TextureId) = 0;
	
	/**
	* 
	*/
	virtual void addStaticLight(void) = 0;

	/**
	* 
	*/
	virtual void removeStaticLight(void) = 0;

	/**
	* 
	*/
	virtual void useFrameLight(void) = 0;

	/**
	* Renders a model specified with an ID.
	* @param p_ModelId the ID of the model to be rendered
	*/
	virtual void renderModel(int p_ModelId) = 0;

	/**
	* 
	*/
	virtual void renderText(void) = 0;

	/**
	* 
	*/
	virtual void renderQuad(void) = 0;
	
	/**
	* Draw the current frame.
	*/
	virtual void drawFrame(int i) = 0;

	/**
	* Gets the amount of VRAM usage of the program.
	* @return the usage in MB
	*/
	virtual int getVRAMMemUsage(void) = 0;
	
	/**
	 * Create an instance of a model. Call {@link #eraseModelInstance(int)} to remove.
	 *
	 * @param p_ModelId the resource identifier for the model to draw the instance with.
	 * @return a unique id used to reference the instance with in later calls.
	 */
	virtual int createModelInstance(const char *p_ModelId) = 0;

	/**
	 * Erase an existing model instance.
	 *
	 * @param p_Instance an identifier to a model instance.
	 */
	virtual void eraseModelInstance(int p_Instance) = 0;

	/**
	 * Set the position of an model instance in absolute world coordinates.
	 *
	 * @param p_Instance an identifier to a model instance.
	 * @param p_X position in X direction.
	 * @param p_Y position in Y direction.
	 * @param p_Z position in Z direction.
	 */
	virtual void setModelPosition(int p_Instance, float p_X, float p_Y, float p_Z) = 0;

	/**
	 * Set the rotation of an model instance in radians.
	 *
	 * @param p_Instance an identifier to a model instance.
	 * @param p_Yaw rotation around the Y axis, left-handed.
	 * @param p_Pitch rotation around the X axis, left-handed.
	 * @param p_Roll rotation around the Z axis, left-handed.
	 */
	virtual void setModelRotation(int p_Instance, float p_Yaw, float p_Pitch, float p_Roll) = 0;

	/**
	 * Set the scale of an model instance.
	 *
	 * @param p_Instance an identifier to a model instance.
	 * @param p_X scale in X direction.
	 * @param p_Y scale in Y direction.
	 * @param p_Z scale in Z direction.
	 */
	virtual void setModelScale(int p_Instance, float p_X, float p_Y, float p_Z) = 0;

	/**
	 * Update the position and viewing direction of the camera.
	 *
	 * @param p_PosX camera position X in absolute world coordinates.
	 * @param p_PosY camera position Y in absolute world coordinates.
	 * @param p_PosZ camera position Z in absolute world coordinates.
	 * @param p_Yaw the camera rotation around the up axis, positive to the right.
	 * @param p_Pitch the camera pitch, positive up.
	 */
	virtual void updateCamera(float p_PosX, float p_PosY, float p_PosZ, float p_Yaw, float p_Pitch) = 0;
	
	typedef void (*loadModelTextureCallBack)(const char *p_ResourceName, const char *p_FilePath, void* p_Userdata);
	virtual void setLoadModelTextureCallBack(loadModelTextureCallBack p_LoadModelTexture, void* p_Userdata) = 0;

private:

	/**
	* Release the sub resources allocated by the graphics API.
	*/
	virtual void shutdown(void) = 0;
};

