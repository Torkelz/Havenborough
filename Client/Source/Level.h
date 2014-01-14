#include "LevelBinaryLoader.h"
#include "IGraphics.h"
#include "IPhysics.h"
#include "ResourceManager.h"
#include "../Utilities/Util.h"

class Level
{
public:
private:
	IGraphics* m_Graphics;
	IPhysics* m_Physics;
	ResourceManager* m_Resources;
	LevelBinaryLoader m_LevelLoader, m_CollisionLoader;
	std::vector<LevelBinaryLoader::ModelData> m_LevelData, m_LevelCollisionData;
	std::vector<int> m_ResourceID, m_BVResourceID;
	std::vector<int> m_DrawID;

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
	 * @param p_Graphics, Creates a reference to the main graphic source.
	 * @param p_Resources, Creates a reference to the main resource source. 
	 * @param p_Physics, Creates a reference to the main physic source.
	 **/
	Level(IGraphics* p_Graphics, ResourceManager* p_Resources, IPhysics* p_Physics);

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
	bool loadLevel(std::string p_LevelFilePath, std::string p_CollisionFilePath);

	/**
	 * Calls a draw function and send the information about what to draw to the Graphics.
	 */
	void drawLevel();
private:
};