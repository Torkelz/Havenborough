#include <ActorFactory.h>
#include "LevelBinaryLoader.h"
#include "IPhysics.h"
#include "ResourceManager.h"
#include <Utilities/Util.h>

class Level
{
public:
private:
	IPhysics* m_Physics;
	ResourceManager* m_Resources;
	ActorFactory* m_ActorFactory;
	LevelBinaryLoader m_LevelLoader, m_CollisionLoader;
	std::vector<LevelBinaryLoader::ModelData> m_LevelData, m_LevelCollisionData;
	std::vector<LevelBinaryLoader::DirectionalLight> m_LevelDirectionalLightList;
	std::vector<LevelBinaryLoader::PointLight> m_LevelPointLightList;
	std::vector<LevelBinaryLoader::SpotLight> m_LevelSpotLightList;
	std::vector<LevelBinaryLoader::CheckPointStruct> m_LevelCheckPointList;
	DirectX::XMFLOAT3 m_LevelCheckPointStart;
	DirectX::XMFLOAT3 m_LevelCheckPointEnd;
	DirectX::XMFLOAT3 m_StartPosition;
	DirectX::XMFLOAT3 m_GoalPosition;

public:
	/*
	* Get the starting position for a player in cm.
	*
	* @return an XMFLOAT3 with the coordinates in cm
	*/
	const DirectX::XMFLOAT3 &getStartPosition(void) const;

	/*
	* Sets the starting position for a player in cm.
	*
	* @param p_StartPosition not in relative coordinates in cm
	*/
	void setStartPosition(const DirectX::XMFLOAT3 &p_StartPosition);

	/*
	* Get the position of the goal in cm.
	*
	* @return an XMFLOAT3 with the coordinates in cm
	*/
	const DirectX::XMFLOAT3 &getGoalPosition(void) const;

	/*
	* Set the position of the goal in cm
	*
	* @param p_GoalPosition not in relative coordinate in cm
	*/
	void setGoalPosition(const DirectX::XMFLOAT3 &p_GoalPosition);

	/**
	 * Default constructor
	 */
	Level(){}

	/**
	 * Constructor
	 *
	 * @param p_Resources, Creates a reference to the main resource source. 
	 * @param p_Physics, Creates a reference to the main physic source.
	 **/
	Level(ResourceManager* p_Resources, IPhysics* p_Physics, ActorFactory* p_ActorFactory);

	/**
	 * Destructor
	 **/
	~Level();

	/**
	 * Releases reasources and de-allocates vector memory.
	 **/
	void releaseLevel();

	/**
	 * Loades a .txl file and send the information to Physics and Graphics, 
	 * the collision map is also a .txl file but loades information about collision.
	 *
	 * @param p_LevelFilePath the complete path to the environment .txl file.
	 * @param p_CollisionFilePath the complete path to the collision .txl file.
	 */
	bool loadLevel(std::string p_LevelFilePath, std::string p_CollisionFilePath, std::vector<Actor::ptr>& p_ActorOut);

	/**
	 * Calls a draw function and send the information about what to draw to the Graphics.
	 */
	void drawLevel();

private:
	Actor::ptr createObjectActor(std::string p_MeshName, Vector3 p_Position, Vector3 p_Rotation, Vector3 p_Scale);
	Actor::ptr createCollisionActor(std::string p_MeshName, Vector3 p_Translation, Vector3 p_Rotation, Vector3 p_Scale);
};