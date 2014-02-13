#include "GameScene.h"
#include <Components.h>
#include <EventData.h>
#include "Logger.h"

using namespace DirectX;

GameScene::GameScene()
{
	m_SceneID = 0;
	m_Visible = false;
	m_NewSceneID = 0;
	m_ChangeScene = false;
	m_ChangeList = false;
	
	m_GameLogic = nullptr;
	m_Graphics = nullptr;
	m_InputQueue = nullptr;
	m_Network = nullptr;

	m_UseThirdPersonCamera = false;
	m_UseFlippedCamera = false;
}

GameScene::~GameScene()
{
	m_Graphics = nullptr;
	m_InputQueue = nullptr;
}

bool GameScene::init(unsigned int p_SceneID, IGraphics *p_Graphics, ResourceManager *p_ResourceManager,
	Input *p_InputQueue, GameLogic *p_GameLogic, EventManager *p_EventManager)
{
	m_SceneID = p_SceneID;
	m_Graphics = p_Graphics;
	m_InputQueue = p_InputQueue;
	m_ResourceManager = p_ResourceManager;
	m_GameLogic = p_GameLogic;
	m_EventManager = p_EventManager;
	
	// Added from Skydome branch
	m_SkyboxID = m_ResourceManager->loadResource("texture","SKYBOXDDS");
	m_Graphics->createSkydome("SKYBOXDDS",50000.f);

	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::addLight), LightEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::removeLight), RemoveLightEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::createMesh), CreateMeshEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::removeMesh), RemoveMeshEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::updateModelPosition), UpdateModelPositionEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::updateModelRotation), UpdateModelRotationEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::updateModelScale), UpdateModelScaleEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::updateAnimation), UpdateAnimationEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::changeColorTone), ChangeColorToneEvent::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::createParticleEffect), CreateParticleEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::removeParticleEffect), RemoveParticleEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::updateParticlePosition), UpdateParticlePositionEventData::sk_EventType);
	m_CurrentDebugView = IGraphics::RenderTarget::FINAL;
	m_RenderDebugBV = false;
	loadSandboxModels();
	return true;
}

void GameScene::destroy()
{
	releaseSandboxModels();
	m_ResourceManager->releaseResource(m_SkyboxID);
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

	m_Graphics->updateParticles(p_DeltaTime);

	//for (auto& model : m_Models)
	//{
	//	for (const ReachIK& ik : model.activeIKs)
	//	{
	//		//m_Graphics->applyIK_ReachPoint(model.modelId, ik.group.c_str(), ik.target);
	//	}
	//}
}

void GameScene::onFocus()
{
	std::shared_ptr<MouseEventDataShow> showMouse(new MouseEventDataShow(false));
	m_EventManager->queueEvent(showMouse);
	std::shared_ptr<MouseEventDataLock> lockMouse(new MouseEventDataLock(true));
	m_EventManager->queueEvent(lockMouse);
}

void GameScene::render()
{
	m_Graphics->setClearColor(Vector4(0,0,0,1));
	Vector3 playerPos = m_GameLogic->getPlayerEyePosition();
	Vector3 playerForward = m_GameLogic->getPlayerViewForward();
	Vector3 playerUp = m_GameLogic->getPlayerViewUp();
	
	if (m_UseFlippedCamera)
	{
		playerForward.x *= -1.f;
		playerForward.z *= -1.f;
		playerUp.x *= -1.f;
		playerUp.z *= -1.f;
	}

	if (m_UseThirdPersonCamera)
	{
		playerPos = playerPos + playerForward * -500.f;
	}

	m_Graphics->updateCamera(playerPos, playerForward, playerUp);

	for (auto& mesh : m_Models)
	{
		m_Graphics->renderModel(mesh.modelId);
	}

	if(m_RenderDebugBV)
	{
		for(auto &object : *m_GameLogic->getObjects())
		{
			for (BodyHandle body : object.second->getBodyHandles())
			{
				renderBoundingVolume(body);
			}
		}

		renderBoundingVolume(m_GameLogic->getPlayerBodyHandle());
	}

	for(auto &light : m_Lights)
	{
		switch(light.type)
		{
		case Light::Type::DIRECTIONAL:
			{
				m_Graphics->useFrameDirectionalLight(light.color, light.direction);
				break;
			}
		case Light::Type::POINT:
			{
				m_Graphics->useFramePointLight(light.position, light.color, light.range);

				break;
			}
		case Light::Type::SPOT:
			{
				m_Graphics->useFrameSpotLight(light.position, light.color, light.direction,
					light.spotlightAngles, light.range);

				break;
			}
		}
		
	}

	//From skybox branch, move later if needed.
	m_Graphics->renderSkydome();

	m_Graphics->setRenderTarget(m_CurrentDebugView);

	//Render test arrow, remove when HUD scene is implemented
	m_Graphics->set2D_ObjectLookAt(1, Vector3(0,0,0));
	m_Graphics->render2D_Object(1);
	//m_Graphics->set2D_ObjectRotationZ(2, playerPos.x);
	m_Graphics->render2D_Object(2);
}

bool GameScene::getIsVisible()
{
	return m_Visible;
}

void GameScene::setIsVisible(bool p_SetVisible)
{
	m_Visible = p_SetVisible;
}

void GameScene::registeredInput(std::string p_Action, float p_Value, float p_PrevValue)
{
	static const float sensitivity = 0.01f;
	
	if(p_Action == "changeSceneN" && p_Value == 1 && p_PrevValue == 0)
	{
		m_NewSceneID = (int)RunScenes::GAMEPAUSE;
		m_ChangeScene = true;
	}
	else if(p_Action == "changeSceneP" && p_Value == 1 && p_PrevValue == 0)
	{
		m_ChangeList = true;
	}
	else if(p_Action ==  "changeViewN" && p_Value == 1)
	{
		m_CurrentDebugView = (IGraphics::RenderTarget)((unsigned int)m_CurrentDebugView - 1);
		if((unsigned int)m_CurrentDebugView < 0)
			m_CurrentDebugView = (IGraphics::RenderTarget)4;
		Logger::log(Logger::Level::DEBUG_L, "Selecting previous view");
	}
	else if(p_Action ==  "changeViewP" && p_Value == 1)
	{
		m_CurrentDebugView = (IGraphics::RenderTarget)((unsigned int)m_CurrentDebugView + 1);
		if((unsigned int)m_CurrentDebugView >= 5)
			m_CurrentDebugView = (IGraphics::RenderTarget)0;
		Logger::log(Logger::Level::DEBUG_L, "Selecting next view");
	}
	else if (p_Action == "mouseMoveHori")
	{
		m_GameLogic->movePlayerView(p_Value * sensitivity, 0.f);
	}
	else if (p_Action == "mouseMoveVert")
	{
		m_GameLogic->movePlayerView(0.f, -p_Value * sensitivity);
	}
	else if( p_Action == "jump" && p_Value == 1)
	{
		m_GameLogic->playerJump();
	}
	else if (p_Action == "toggleIK" && p_Value == 1.f)
	{
		m_GameLogic->toggleIK();
	}
	else if( p_Action == "switchBVDraw" && p_Value == 1.f && p_PrevValue == 0)
	{
		m_RenderDebugBV = !m_RenderDebugBV;
	}
	else if( p_Action == "blendAnimation" && p_Value == 1.0f && p_PrevValue == 0)
	{
		m_GameLogic->testBlendAnimation();
	}
	else if( p_Action == "resetAnimation" && p_Value == 1.0f && p_PrevValue == 0 )
	{
		m_GameLogic->testResetAnimation();
	}
	else if( p_Action == "layerAnimation" && p_Value == 1.0f && p_PrevValue == 0 )
	{
		m_GameLogic->testLayerAnimation();
	}
	else if( p_Action == "resetLayerAnimation" && p_Value == 1.0f && p_PrevValue == 0 )
	{
		m_GameLogic->testResetLayerAnimation();
	}
	else if (p_Action == "leaveGame" && p_Value == 1.f)
	{
		m_GameLogic->leaveGame();
	}
	else if (p_Action == "thirdPersonCamera" && p_Value == 1.f)
	{
		m_UseThirdPersonCamera = !m_UseThirdPersonCamera;
	}
	else if (p_Action == "flipCamera" && p_Value == 1.f)
	{
		m_UseFlippedCamera = !m_UseFlippedCamera;
	}
	else if(p_Action == "spellCast" && p_Value == 1.f)
	{
		m_GameLogic->throwSpell("TestSpell");
	}
}

/*########## TEST FUNCTIONS ##########*/

int GameScene::getID()
{
	return m_SceneID;
}

void GameScene::addLight(IEventData::Ptr p_Data)
{
	std::shared_ptr<LightEventData> lightData = std::static_pointer_cast<LightEventData>(p_Data);
	Light light = lightData->getLight();
	m_Lights.push_back(light);
}

void GameScene::removeLight(IEventData::Ptr p_Data)
{
	std::shared_ptr<RemoveLightEventData> lightData = std::static_pointer_cast<RemoveLightEventData>(p_Data);

	auto remIt = std::remove_if(m_Lights.begin(), m_Lights.end(),
		[&lightData] (Light& p_Light)
		{
			return p_Light.id == lightData->getId();
		});
	m_Lights.erase(remIt, m_Lights.end());
}

void GameScene::createMesh(IEventData::Ptr p_Data)
{
	std::shared_ptr<CreateMeshEventData> meshData = std::static_pointer_cast<CreateMeshEventData>(p_Data);

	int resource = m_ResourceManager->loadResource("model", meshData->getMeshName());
	m_ResourceIDs.push_back(resource);

	MeshBinding mesh =
	{
		meshData->getId(),
		resource,
		m_Graphics->createModelInstance(meshData->getMeshName().c_str())
	};
	m_Graphics->setModelScale(mesh.modelId, meshData->getScale());
	m_Graphics->setModelColorTone(mesh.modelId, meshData->getColorTone());
	
	m_Models.push_back(mesh);
}

void GameScene::removeMesh(IEventData::Ptr p_Data)
{
	std::shared_ptr<RemoveMeshEventData> meshData = std::static_pointer_cast<RemoveMeshEventData>(p_Data);

	for (auto& model : m_Models)
	{
		if (model.meshId == meshData->getId())
		{
			m_ResourceManager->releaseResource(model.resourceId);
			auto it = std::find(m_ResourceIDs.begin(), m_ResourceIDs.end(), model.resourceId);
			if (it != m_ResourceIDs.end())
			{
				m_ResourceIDs.erase(it);
			}

			m_Graphics->eraseModelInstance(model.modelId);

			std::swap(model, m_Models.back());
			m_Models.pop_back();
			return;
		}
	}
}

void GameScene::updateModelPosition(IEventData::Ptr p_Data)
{
	std::shared_ptr<UpdateModelPositionEventData> positionData = std::static_pointer_cast<UpdateModelPositionEventData>(p_Data);
	for(auto &model : m_Models)
	{
		if(model.meshId == positionData->getId())
		{
			m_Graphics->setModelPosition(model.modelId, positionData->getPosition());
		}
	}
}

void GameScene::updateModelRotation(IEventData::Ptr p_Data)
{
	std::shared_ptr<UpdateModelRotationEventData> rotationData = std::static_pointer_cast<UpdateModelRotationEventData>(p_Data);
	for(auto &model : m_Models)
	{
		if(model.meshId == rotationData->getId())
		{
			m_Graphics->setModelRotation(model.modelId, rotationData->getRotation());
		}
	}
}

void GameScene::updateModelScale(IEventData::Ptr p_Data)
{
	std::shared_ptr<UpdateModelScaleEventData> scaleData = std::static_pointer_cast<UpdateModelScaleEventData>(p_Data);
	for(auto &model : m_Models)
	{
		if(model.meshId == scaleData->getId())
		{
			m_Graphics->setModelScale(model.modelId, scaleData->getScale());
		}
	}
}

void GameScene::updateAnimation(IEventData::Ptr p_Data)
{
	std::shared_ptr<UpdateAnimationEventData> animationData = std::static_pointer_cast<UpdateAnimationEventData>(p_Data);
	for(auto &model : m_Models)
	{
		if(model.meshId == animationData->getId())
		{
			m_Graphics->animationPose(model.modelId, animationData->getAnimationData().data(), animationData->getAnimationData().size());
		}
	}
}

void GameScene::changeColorTone(IEventData::Ptr p_Data)
{
	std::shared_ptr<ChangeColorToneEvent> data = std::static_pointer_cast<ChangeColorToneEvent>(p_Data);

	for (auto& model : m_Models)
	{
		if (model.meshId == data->getMeshId())
		{
			m_Graphics->setModelColorTone(model.modelId, data->getColorTone());
			return;
		}
	}
}

void GameScene::createParticleEffect(IEventData::Ptr p_Data)
{
	std::shared_ptr<CreateParticleEventData> data = std::static_pointer_cast<CreateParticleEventData>(p_Data);

	int resource = m_ResourceManager->loadResource("particleSystem", data->getEffectName());
	m_Graphics->linkShaderToParticles("DefaultParticleShader", data->getEffectName().c_str());

	ParticleBinding particle =
	{
		resource,
		m_Graphics->createParticleEffectInstance(data->getEffectName().c_str())
	};

	m_Graphics->setParticleEffectPosition(particle.instance, data->getPosition());

	m_Particles[data->getId()] = particle;
}

void GameScene::removeParticleEffect(IEventData::Ptr p_Data)
{
	std::shared_ptr<RemoveParticleEventData> data = std::static_pointer_cast<RemoveParticleEventData>(p_Data);

	auto it = m_Particles.find(data->getId());

	if (it != m_Particles.end())
	{
		m_Graphics->releaseParticleEffectInstance(it->second.instance);
		m_ResourceManager->releaseResource(it->second.resourceId);
		m_Particles.erase(it);
	}
}

void GameScene::updateParticlePosition(IEventData::Ptr p_Data)
{
	std::shared_ptr<UpdateParticlePositionEventData> data = std::static_pointer_cast<UpdateParticlePositionEventData>(p_Data);

	auto it = m_Particles.find(data->getId());

	if (it != m_Particles.end())
	{
		m_Graphics->setParticleEffectPosition(it->second.instance, data->getPosition());
	}
}

void GameScene::renderBoundingVolume(BodyHandle p_BodyHandle)
{
	unsigned int size =  m_GameLogic->getPhysics()->getNrOfTrianglesFromBody(p_BodyHandle);

	for(unsigned int i = 0; i < size; i++)
	{
		Triangle triangle;
		triangle = m_GameLogic->getPhysics()->getTriangleFromBody(p_BodyHandle, i);
		m_Graphics->addBVTriangle(triangle.corners[0].xyz(), triangle.corners[1].xyz(), triangle.corners[2].xyz());
	}
}

void GameScene::loadSandboxModels()
{
	m_Graphics->createShader("DefaultShader", L"assets/shaders/GeometryPass.hlsl",
								"VS,PS","5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);
	m_Graphics->createShader("DefaultShaderForward", L"assets/shaders/ForwardShader.hlsl",
		"VS,PS","5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);

	m_Graphics->createShader("DefaultParticleShader", L"assets/shaders/ParticleSystem.hlsl",
		"VS,PS,GS", "5_0", ShaderType::VERTEX_SHADER | ShaderType::GEOMETRY_SHADER | ShaderType::PIXEL_SHADER);

	m_Graphics->createShader("AnimatedShader", L"assets/shaders/AnimatedGeometryPass.hlsl",
		"VS,PS","5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER);

	static const std::string preloadedModels[] =
	{
		"Arrow1",
		"Barrel1",
        "Crate1", 
		"Grass1", 
        "House1", 
		"House2", 
        "House3", 
		"House4", 
		"House5", 
        "House6", 
        "MarketStand1", 
        "MarketStand2", 
		"Road1", 
		"Road2", 
		"Road3", 
		"Road4", 
		"Road5", 
        "Sidewalk1", 
        "Stair1",
		"Stallning1",
		"Stallning2",
		"Stallning3",
		"Stallning4",
        "StoneBrick2",
        "Street1",
        "Tree1",
		"Vege1",
		"Vege2",
        "WoodenShed1",
	};

	for (const std::string& model : preloadedModels)
	{
		m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", model));
		m_Graphics->linkShaderToModel("DefaultShader", model.c_str());		
	}

	static const std::string preloadedTextures[] =
	{
		"TEXTURE_NOT_FOUND",
		"MANA_BAR",
	};
	for (const std::string &texture : preloadedTextures)
	{
		m_ResourceIDs.push_back(m_ResourceManager->loadResource("texture", texture));
	}
	m_Graphics->create2D_Object(Vector3(-500, 300, 150.f), 1.f, 0.f, "Arrow1");
	m_Graphics->create2D_Object(Vector3(-400, -320, 2), Vector2(160, 30), 0.0f, "MANA_BAR");

	static const std::string preloadedModelsTransparent[] =
	{
		"Checkpoint1",
	};

	for (const std::string& model : preloadedModelsTransparent)
	{
		m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", model));
		m_Graphics->setModelDefinitionTransparency(model.c_str(), true);
		m_Graphics->linkShaderToModel("DefaultShaderForward", model.c_str());
	}

	m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", "WITCH"));
	m_Graphics->linkShaderToModel("AnimatedShader", "WITCH");

	m_ResourceIDs.push_back(m_ResourceManager->loadResource("particleSystem", "TestParticle"));
	m_Graphics->linkShaderToParticles("DefaultParticleShader", "TestParticle");
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
	m_Graphics->deleteShader("DefaultParticleShader");
}