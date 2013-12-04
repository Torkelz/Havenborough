#pragma once

#include <IGraphics.h>
#include "Input\Input.h"
#include "IPhysics.h"
#include "NetworkClient.h"
#include "SceneManager.h"
#include "Window.h"

#include <string>

class BaseGameApp
{
private:
	static const std::string m_GameTitle;

	Window	m_Window;
	IGraphics* m_Graphics;
	Input	m_InputQueue;

	NetworkClient m_Network;

	bool	m_ShouldQuit;
	SceneManager m_SceneManager;

	BodyHandle m_Body, m_Object;
	IPhysics *m_Physics;

	float dt;

	//TEMP
	//Buffer	*m_Buffer, *m_CBuffer;
	//Shader	*m_Shader;
	//Buffer	*m_Buffer2;
	//Shader	*m_Shader2;
public:
	/**
	 * Initialize the game and create a window.
	 */
	void init();
	/**
	 * Run the game. Doesn't return until the game is exited.
	 */
	void run();
	/**
	 * Shutdown the game and cleanup any resources.
	 */
	void shutdown();

	/**
	 * Get the title of the game.
	 *
	 * @return Name of the game.
	 */
	std::string getGameTitle() const;
	/**
	 * Get the size of the window.
	 *
	 * @return The size (width, height) of the client area of the window.
	 */
	UVec2 getWindowSize() const;

private:
	bool handleWindowClose(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result);



	//TEMP --------------------------------------------------
	//#define PI (3.14159265358979323846f)
	//struct vertex
	//{
	//	DirectX::XMFLOAT4 position;
	//	DirectX::XMFLOAT3 normal;
	//	DirectX::XMFLOAT2 uv;
	//	DirectX::XMFLOAT3 tangent;
	//	DirectX::XMFLOAT3 binormal;
	//	vertex(){}
	//	vertex(DirectX::XMFLOAT3 _position,
	//			DirectX::XMFLOAT3 _normal,
	//			DirectX::XMFLOAT2 _uv,
	//			DirectX::XMFLOAT3 _tangent,
	//			DirectX::XMFLOAT3 _binormal)
	//	{
	//		position = DirectX::XMFLOAT4(_position.x,_position.y,_position.z,1.0f);
	//		normal = _normal;
	//		uv = _uv;
	//		tangent = _tangent;
	//		binormal = _binormal;
	//	}
	//};
	//
	//
	//vertex* createBOX(unsigned int size, float x, float y, float z);
	//void CalculateTangentBinormal(DirectX::XMFLOAT3 vertex1, DirectX::XMFLOAT3 vertex2, DirectX::XMFLOAT3 vertex3,
	//									   DirectX::XMFLOAT2 uv1,DirectX::XMFLOAT2 uv2, DirectX::XMFLOAT2 uv3,
	//										DirectX::XMFLOAT3& tangent, DirectX::XMFLOAT3& binormal);
	//void CalculateNormal(DirectX::XMFLOAT3 tangent, DirectX::XMFLOAT3 binormal, DirectX::XMFLOAT3& normal);

	//TEMP --------------------------------------------------
};
