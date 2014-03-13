#include "GameScene.h"

#include <Components.h>
#include <EventData.h>
#include "Logger.h"
#include <TweakSettings.h>

using namespace DirectX;

GameScene::GameScene()
{
	m_SceneID = 0;
	m_Visible = false;
	m_NewSceneID = 0;
	m_ChangeScene = false;
	m_ChangeList = false;
	m_SoundExist = false;

	m_GameLogic = nullptr;
	m_Graphics = nullptr;
	m_InputQueue = nullptr;
	m_Network = nullptr;

	m_UseThirdPersonCamera = false;
	m_UseFlippedCamera = false;
	m_DebugAnimations = false;

	m_ViewSensitivity = 0.01f;
}

GameScene::~GameScene()
{
	m_Graphics = nullptr;
	m_InputQueue = nullptr;

	m_SoundManager = nullptr;
}

bool GameScene::init(unsigned int p_SceneID, IGraphics *p_Graphics, ResourceManager *p_ResourceManager,
	Input *p_InputQueue, GameLogic *p_GameLogic, EventManager *p_EventManager)
{
	m_ExtraParticleID = 500000;
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
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::removeParticleEffectInstance), RemoveParticleEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::updateParticlePosition), UpdateParticlePositionEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::updateParticleRotation), UpdateParticleRotationEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::updateParticleBaseColor), UpdateParticleBaseColorEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::spellHit), SpellHitEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::spellHitSphere), SpellHitSphereEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::createWorldText), createWorldTextEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::removeWorldText), removeWorldTextEventData::sk_EventType);
	m_EventManager->addListener(EventListenerDelegate(this, &GameScene::updateWorldTextPosition), updateWorldTextPositionEventData::sk_EventType);

	m_SelectableRenderTargets.push_back(IGraphics::RenderTarget::FINAL);
	m_SelectableRenderTargets.push_back(IGraphics::RenderTarget::SSAO);
	m_SelectableRenderTargets.push_back(IGraphics::RenderTarget::NORMAL);
	m_SelectableRenderTargets.push_back(IGraphics::RenderTarget::CSM);
	m_CurrentDebugView = 0;
	m_RenderDebugBV = false;
	preLoadModels();

	TweakSettings* tweakSettings = TweakSettings::getInstance();
	tweakSettings->setListener("camera.flipped", std::function<void(bool)>([&] (bool p_Val) { m_UseFlippedCamera = p_Val; }));

	m_RandomEngine.seed((unsigned long)std::chrono::system_clock::now().time_since_epoch().count());
	m_SoundFolderPath = "assets/sounds/background";


	unsigned int id = m_Graphics->createText(L"Press again to exit \nPress any key to continue     ", "Verdana", 27.f, Vector4(0.8f, 0.8f, 0.8f, 1.f), Vector3(0.0f, 0.0f, 0.0f), 1.0f, 0.f);
	m_Graphics->setTextBackgroundColor(id, Vector4(0.f, 0.f, 0.f, 0.4f));
	m_PauseId = m_Graphics->create2D_Object(Vector3(0,0,0), Vector3(1,1,1), 0.f, id);
	m_RenderPause = false;

	return true;
}

void GameScene::destroy()
{
	releasePreLoadedModels();
	m_ResourceManager->releaseResource(m_SkyboxID);
	m_SoundManager->releaseSound("CurrentSound");
	m_BackGroundSoundsList.clear();
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
	float up = state.getValue("moveUp") - state.getValue("moveDown");

	if( up != 0.0f || right != 0.0f || forward != 0.0f )
	{
		m_RenderPause = false;
	}
	m_GameLogic->setPlayerDirection(Vector3(forward, up, right));

	float vert = state.getValue("turnUp") - state.getValue("turnDown");
	float hori = state.getValue("turnRight") - state.getValue("turnLeft");
	const float turnSize = p_DeltaTime * 600.f * m_ViewSensitivity;
	m_GameLogic->movePlayerView(hori * turnSize, -vert * turnSize);

	m_Graphics->updateParticles(p_DeltaTime);

	m_SoundManager->onFrame();

	if (m_SoundPath != "NULL")
	{
		if (!m_SoundExist || !m_SoundManager->isPlaying("CurrentSound"))
		{
			m_SoundPath = changeBackGroundSound(m_SoundFolderPath);
			if (m_SoundPath != "NULL")
			{		
				m_SoundManager->loadSoundWithoutLoop("CurrentSound", m_SoundPath.c_str());
				m_SoundExist = true;
				m_SoundManager->playSound("CurrentSound");
				m_SoundManager->setSoundVolume("CurrentSound", 0.2f);
			}
		}
	}	
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
	}

	for(auto &light : m_Lights)
	{
		switch(light.type)
		{
		case LightClass::Type::DIRECTIONAL:
			{
				m_Graphics->useFrameDirectionalLight(light.color, light.direction, light.intensity);
				break;
			}
		case LightClass::Type::POINT:
			{
				m_Graphics->useFramePointLight(light.position, light.color, light.range);

				break;
			}
		case LightClass::Type::SPOT:
			{
				m_Graphics->useFrameSpotLight(light.position, light.color, light.direction,
					light.spotlightAngles, light.range);

				break;
			}
		}
	}

	//From skybox branch, move later if needed.
	m_Graphics->renderSkydome();

	m_Graphics->setRenderTarget(m_SelectableRenderTargets[m_CurrentDebugView]);
	
	for( auto &wText : m_WorldText)
	{
		//Skip rendering own player tag.
		if(m_GameLogic->getPlayerTextComponentId() != wText.first)
			m_Graphics->renderText(wText.second);
	}

	if(m_RenderPause)
		m_Graphics->render2D_Object(m_PauseId);
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
	bool handled = false;
	// Binary triggers
	if (p_Value > 0.5f && p_PrevValue <= 0.5f)
	{
		handled = true;

		if(p_Action == "changeSceneN")
		{
			//m_NewSceneID = (int)RunScenes::GAMEPAUSE;
			//m_ChangeScene = true;
		}
		else if(p_Action == "changeSceneP")
		{
			m_ChangeList = true;
		}
		else if(p_Action ==  "changeViewN")
		{
			if(m_CurrentDebugView >= m_SelectableRenderTargets.size() - 1)
				m_CurrentDebugView = 0;
			else
				++m_CurrentDebugView;

			Logger::log(Logger::Level::DEBUG_L, "Selecting next view");
		}
		else if(p_Action ==  "changeViewP")
		{
			if(m_CurrentDebugView == 0)
				m_CurrentDebugView = m_SelectableRenderTargets.size() - 1;
			else
				--m_CurrentDebugView;

			Logger::log(Logger::Level::DEBUG_L, "Selecting previous view");
		}
		else if( p_Action == "jump")
		{
			m_GameLogic->playerJump();
		}
		else if( p_Action == "switchBVDraw")
		{
			m_RenderDebugBV = !m_RenderDebugBV;
		}
		else if (p_Action == "leaveGame")
		{
 			if(m_RenderPause)
				m_GameLogic->leaveGame();
			m_RenderPause = true;
			handled =  false;
		}
		else if (p_Action == "thirdPersonCamera")
		{
			m_UseThirdPersonCamera = !m_UseThirdPersonCamera;
		}
		else if (p_Action == "flipCamera")
		{
			m_UseFlippedCamera = !m_UseFlippedCamera;
		}
		else if(p_Action == "spellCast")
		{
			m_GameLogic->throwSpell("TestSpell");
		}
		else if(p_Action == "drawPivots")
		{
			m_DebugAnimations = !m_DebugAnimations;
		}
		else if(p_Action == "wave")
		{
			m_GameLogic->playerWave();
		}
		else if (m_GameLogic->getSplineCameraActive())
		{
			if(p_Action == "splineRecord")
			{
				m_GameLogic->recordSpline();
			}
			else if(p_Action == "splineRemove")
			{
				m_GameLogic->removeLastSplineRecord();
			}
			else if(p_Action == "splineClear")
			{
				m_GameLogic->clearSplineSequence();
			}
			else
			{
				handled = false;
			}
		}
		else
		{
			handled = false;
		}
		if(handled)
			m_RenderPause = false;
	}

	if (handled)
		return;

	// Analog triggers
	if(p_Action == "climbEdge")
	{
		m_GameLogic->setPlayerClimb(p_Value > 0.5f);
	}
	else if (p_Action == "lookRight")
	{
		m_GameLogic->movePlayerView(p_Value * m_ViewSensitivity, 0.f);
	}
	else if (p_Action == "lookLeft")
	{
		m_GameLogic->movePlayerView(-p_Value * m_ViewSensitivity, 0.f);
	}
	else if (p_Action == "lookUp")
	{
		m_GameLogic->movePlayerView(0.f, -p_Value * m_ViewSensitivity);
	}
	else if (p_Action == "lookDown")
	{
		m_GameLogic->movePlayerView(0.f, p_Value * m_ViewSensitivity);
	}
}

void GameScene::setMouseSensitivity(float p_Value)
{
	m_ViewSensitivity *= p_Value;
}

void GameScene::setSoundManager(ISound *p_SoundManager)
{
	m_SoundManager = p_SoundManager;
}

std::string GameScene::changeBackGroundSound(const std::string& p_FontFolderPath)
{
	
	if (m_SoundExist)
	{
		m_SoundManager->releaseSound("CurrentSound");
		m_SoundExist = false;
	}

	m_BackGroundSoundsList.clear();

	boost::filesystem::directory_iterator currFile(p_FontFolderPath);
	for (; currFile != boost::filesystem::directory_iterator(); ++currFile)
	{
		auto filename = currFile->path();
		m_BackGroundSoundsList.push_back(filename.string());
	}
	int soundCount = m_BackGroundSoundsList.size();
	if (soundCount == 0)
	{
		std::string failToFindAFile = "NULL";
		return failToFindAFile;
	}
	std::uniform_int_distribution<int> newBackGroundSound(0, soundCount-1);
	int newSoundTrack = 0;
	newSoundTrack = newBackGroundSound(m_RandomEngine);

	return m_BackGroundSoundsList[newSoundTrack];
}

/*########## TEST FUNCTIONS ##########*/

int GameScene::getID()
{
	return m_SceneID;
}


void GameScene::addLight(IEventData::Ptr p_Data)
{
	std::shared_ptr<LightEventData> lightData = std::static_pointer_cast<LightEventData>(p_Data);
	LightClass light = lightData->getLight();
	m_Lights.push_back(light);
}

void GameScene::removeLight(IEventData::Ptr p_Data)
{
	std::shared_ptr<RemoveLightEventData> lightData = std::static_pointer_cast<RemoveLightEventData>(p_Data);

	auto remIt = std::remove_if(m_Lights.begin(), m_Lights.end(),
		[&lightData] (LightClass& p_Light)
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
	m_Graphics->setModelStyle(mesh.modelId, meshData->getStyle().c_str());
	
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
			const std::vector<DirectX::XMFLOAT4X4>& animation = animationData->getAnimationData();
			m_Graphics->animationPose(model.modelId, animation.data(), animation.size());

			if( m_DebugAnimations )
			{
				const AnimationData::ptr poseData = animationData->getAnimation();

				for (unsigned int i = 0; i < animation.size(); ++i)
				{
					//if( i == 31 || i == 30 || i == 29 || i == 6 || i == 7 || i == 8 || i == 4 || i == 3 )
					//{
						XMMATRIX toBind = XMLoadFloat4x4(&poseData->joints[i].m_TotalJointOffset);
						XMMATRIX toObject = XMLoadFloat4x4(&animation[i]);
						XMMATRIX toWorld = XMLoadFloat4x4(&animationData->getWorld());
						XMMATRIX objectTransform = toWorld * toObject * toBind;
						XMFLOAT4X4 fTransform;
						XMStoreFloat4x4(&fTransform, objectTransform);

						m_Graphics->renderJoint(fTransform);
					//}
				}
			}
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
	
	ParticleBinding particle =
	{
		data->getEffectName(),
		m_Graphics->createParticleEffectInstance(data->getEffectName().c_str())
	};

	m_Graphics->setParticleEffectPosition(particle.instance, data->getPosition());

	m_Particles[data->getId()] = particle;
}

void GameScene::removeParticleEffectInstance(IEventData::Ptr p_Data)
{
	std::shared_ptr<RemoveParticleEventData> data = std::static_pointer_cast<RemoveParticleEventData>(p_Data);

	auto it = m_Particles.find(data->getId());

	if (it != m_Particles.end())
	{
		m_Graphics->releaseParticleEffectInstance(it->second.instance);
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

void GameScene::updateParticleRotation(IEventData::Ptr p_Data)
{
	std::shared_ptr<UpdateParticleRotationEventData> data = std::static_pointer_cast<UpdateParticleRotationEventData>(p_Data);

	auto it = m_Particles.find(data->getId());

	if (it != m_Particles.end())
	{
		m_Graphics->setParticleEffectRotation(it->second.instance, data->getRotation());
	}
}

void GameScene::updateParticleBaseColor(IEventData::Ptr p_Data)
{
	std::shared_ptr<UpdateParticleBaseColorEventData> data = std::static_pointer_cast<UpdateParticleBaseColorEventData>(p_Data);

	auto it = m_Particles.find(data->getId());

	if (it != m_Particles.end())
	{
		m_Graphics->setParticleEffectBaseColor(it->second.instance, data->getBaseColor());
	}
}

void GameScene::spellHit(IEventData::Ptr p_Data)
{
	std::shared_ptr<SpellHitEventData> data = std::static_pointer_cast<SpellHitEventData>(p_Data);

	m_EventManager->queueEvent((IEventData::Ptr(new CreateParticleEventData(++m_ExtraParticleID, "spellExplosion", data->getPosition()))));
}

void GameScene::spellHitSphere(IEventData::Ptr p_Data)
{
	std::shared_ptr<SpellHitSphereEventData> data = std::static_pointer_cast<SpellHitSphereEventData>(p_Data);

	if(data->getCollisionVictim() == m_GameLogic->getPlayerBodyHandle())
	{
		m_EventManager->queueEvent((IEventData::Ptr(new PlayerIsHitBySpellEventData())));
	}
}

void GameScene::createWorldText(IEventData::Ptr p_Data)
{
	std::shared_ptr<createWorldTextEventData> data = std::static_pointer_cast<createWorldTextEventData>(p_Data);

	IGraphics::Text_Id id = m_Graphics->createText(data->getText().c_str(), data->getFont().c_str(), data->getFontSize(),
		data->getFontColor(), data->getPosition(), data->getScale(), data->getRotation());

	m_Graphics->setTextBackgroundColor(id, data->getBackgroundColor());

	m_WorldText[data->getComponentId()] = id;
}

void GameScene::removeWorldText(IEventData::Ptr p_Data)
{
	std::shared_ptr<removeWorldTextEventData> data = std::static_pointer_cast<removeWorldTextEventData>(p_Data);

	if(m_WorldText.count(data->getId()) > 0)
	{
		m_Graphics->deleteText(m_WorldText.at(data->getId()));
		m_WorldText.erase(data->getId());
	}
}

void GameScene::updateWorldTextPosition(IEventData::Ptr p_Data)
{
	std::shared_ptr<updateWorldTextPositionEventData> data = std::static_pointer_cast<updateWorldTextPositionEventData>(p_Data);

	if(m_WorldText.count(data->getId()) > 0)
	{
		m_Graphics->setTextPosition(m_WorldText.at(data->getId()), data->getPosition());
	}
}

void GameScene::renderBoundingVolume(BodyHandle p_BodyHandle)
{
	unsigned int nrVolumes = m_GameLogic->getPhysics()->getNrOfVolumesInBody(p_BodyHandle);

	for(unsigned int j = 0; j < nrVolumes; j++)
	{
		unsigned int size =  m_GameLogic->getPhysics()->getNrOfTrianglesFromBody(p_BodyHandle, j);

		for(unsigned int i = 0; i < size; i++)
		{
			Triangle triangle;
			triangle = m_GameLogic->getPhysics()->getTriangleFromBody(p_BodyHandle, i, j);
			m_Graphics->addBVTriangle(triangle.corners[0].xyz(), triangle.corners[1].xyz(), triangle.corners[2].xyz());
		}
	}
}

void GameScene::preLoadModels()
{
	//DO NOT MAKE ANY CALLS TO GRAPHICS IN HERE!
	m_ResourceIDs.push_back(m_ResourceManager->loadResource("particleSystem", "TestParticle"));
	m_ResourceIDs.push_back(m_ResourceManager->loadResource("model", "Pivot1"));

}

void GameScene::releasePreLoadedModels()

{
	for (int res : m_ResourceIDs)
	{
		m_ResourceManager->releaseResource(res);
	}
	m_ResourceIDs.clear();
}