#include "GameScene.h"
#include "../Components.h"

GameScene::GameScene()
{
	m_SceneID = 0;
	m_Visible = false;
	m_NewSceneID = 0;
	m_ChangeScene = false;
	m_ChangeList = false;
	
	m_GameLogic = nullptr;
	m_Graphics = nullptr;
	m_Physics = nullptr;
	m_InputQueue = nullptr;
}

GameScene::~GameScene()
{
	m_Graphics = nullptr;
	m_InputQueue = nullptr;
}

bool GameScene::init(unsigned int p_SceneID, IGraphics *p_Graphics, ResourceManager *p_ResourceManager,
	Input *p_InputQueue, GameLogic *p_GameLogic)
{
	m_SceneID = p_SceneID;
	m_Graphics = p_Graphics;
	m_InputQueue = p_InputQueue;
	m_ResourceManager = p_ResourceManager;
	m_GameLogic = p_GameLogic;

	m_CurrentDebugView = 3;
	
	loadSandboxModels();

	return true;
}

void GameScene::destroy()
{
	releaseSandboxModels();
}

void GameScene::onFrame(float p_DeltaTime, int* p_IsCurrentScene)
{
	if(m_GameLogic->getChangeScene() != GameLogic::GoToScene::NONE)
	{
		m_ChangeScene = true;
		m_NewSceneID = (int)m_GameLogic->getChangeScene();
		
	}
	if(m_ChangeScene)
	{
		*p_IsCurrentScene = m_NewSceneID;
		m_Visible = false;
		m_ChangeScene = false;

	}
	else if(m_ChangeList)
	{
		*p_IsCurrentScene = -1;
		m_ChangeList = false;
	}

	const InputState& state = m_InputQueue->getCurrentState();

	float forward = state.getValue("moveForward") - state.getValue("moveBackward");
	float right = state.getValue("moveRight") - state.getValue("moveLeft");

	m_GameLogic->setPlayerDirection(Vector2(forward, right));

	m_Graphics->updateAnimations(p_DeltaTime);
}

void GameScene::render()
{
	Vector3 viewRot = m_GameLogic->getPlayerViewRotation();
	Vector3 playerPos = m_GameLogic->getPlayerEyePosition();
	m_Graphics->updateCamera(playerPos, viewRot.x, viewRot.y);

	std::vector<Actor::ptr>& actors = m_GameLogic->getObjects();

	for (auto& actor : actors)
	{
		std::shared_ptr<ModelInterface> modelComp = actor->getComponent<ModelInterface>(ModelComponent::m_ComponentId).lock();
		if (modelComp)
		{
			modelComp->render(m_Graphics);
		}
	}

	m_Graphics->useFrameDirectionalLight(Vector3(1.f, 1.f, 1.f), Vector3(0.f, -1.f, 0.f));
	m_Graphics->setRenderTarget(m_CurrentDebugView);
}

bool GameScene::getIsVisible()
{
	return m_Visible;
}

void GameScene::setIsVisible(bool p_SetVisible)
{
	m_Visible = p_SetVisible;
}

void GameScene::registeredInput(std::string p_Action, float p_Value)
{
	static const float sensitivity = 0.01f;

	if(p_Action == "changeSceneN" && p_Value == 1)
	{
		m_NewSceneID = (int)RunScenes::GAMEPAUSE;
		m_ChangeScene = true;
	}
	else if(p_Action == "changeSceneP" && p_Value == 1)
	{
		m_ChangeList = true;
	}
	else if(p_Action ==  "changeViewN" && p_Value == 1)
	{
		m_CurrentDebugView--;
		if(m_CurrentDebugView < 0)
			m_CurrentDebugView = 3;
		Logger::log(Logger::Level::DEBUG_L, "Selecting previous view");
	}
	else if(p_Action ==  "changeViewP" && p_Value == 1)
	{
		m_CurrentDebugView++;
		if(m_CurrentDebugView >= 4)
			m_CurrentDebugView = 0;
		Logger::log(Logger::Level::DEBUG_L, "Selecting next view");
	}
	else if (p_Action == "mouseMoveHori")
	{
		m_GameLogic->movePlayerView(p_Value * sensitivity, 0.f);
	}
	else if (p_Action == "mouseMoveVert")
	{
		m_GameLogic->movePlayerView(0.f, p_Value * sensitivity);
	}
	else if( p_Action == "jump" && p_Value == 1)
	{
		m_GameLogic->playerJump();
	}
	else if (p_Action == "toggleIK" && p_Value == 1.f)
	{
		m_GameLogic->toggleIK();
	}
}

/*########## TEST FUNCTIONS ##########*/

int GameScene::getID()
{
	return m_SceneID;
}

void GameScene::loadSandboxModels()
{
	m_Graphics->createShader("DefaultShader", L"../../Graphics/Source/DeferredShaders/GeometryPass.hlsl",
								"VS,PS","5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);

	static const std::string preloadedModels[] =
	{
		"BOX",
		"SKYBOX",
		"House",
	};

	for (const std::string& model : preloadedModels)
	{
		m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", model));
		m_Graphics->linkShaderToModel("DefaultShader", model.c_str());
	}

	Logger::log(Logger::Level::DEBUG_L, "Adding IK test tube");
	m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", "IKTest"));
	m_Graphics->createShader("AnimatedShader", L"../../Graphics/Source/DeferredShaders/AnimatedGeometryPass.hlsl",
		"VS,PS","5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
	m_Graphics->linkShaderToModel("AnimatedShader", "IKTest");

	Logger::log(Logger::Level::DEBUG_L, "Adding debug animated Dzala");
	m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", "DZALA"));
	m_Graphics->linkShaderToModel("AnimatedShader", "DZALA");
}

void GameScene::releaseSandboxModels()
{
	for (int res : m_ResourceIDs)
	{
		m_ResourceManager->releaseResource(res);
	}
	m_ResourceIDs.clear();

	m_Graphics->deleteShader("DefaultShader");
	m_Graphics->deleteShader("AnimatedShader");
}
