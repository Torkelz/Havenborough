#include "Components.h"
#include "Logger.h"
#include "SpellFactory.h"

#include <chrono>
#include <random>

class SpellComponent : public SpellInterface
{

private:
	int m_SpellId;
	std::string m_SpellName;
	ResourceManager* m_ResourceManager;
	SpellFactory* m_SpellFactory;
	IPhysics* m_Physics;
	Vector3 m_StartDirection;
	SpellInstance::ptr m_SpellInstance;
	BodyHandle m_Body;
	Actor::wPtr m_Caster;
	Actor::Id m_CasterId;
	std::default_random_engine m_RandomEngine;

public:
	/**
	 * De-constructor for this component
	 */
	~SpellComponent() override
	{
		m_ResourceManager->releaseResource(m_SpellId);
		if (m_Body != 0)
		{
			m_Physics->releaseBody(m_Body);
		}
	}

	/**
	 * Function called to initialize variables withing this component
	 * 
	 * @param p_Data XML-document containing variable information
	 */
	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		const char* spellName = p_Data->Attribute("SpellName");
		if (!spellName)
		{
			throw CommonException("Missing spell name", __LINE__, __FILE__);
		}

		m_CasterId = -1;
		p_Data->QueryAttribute("CasterId", &m_CasterId);

		m_SpellName = spellName;
		m_SpellId = m_ResourceManager->loadResource("spell", m_SpellName);

		m_StartDirection = Vector3(0.f, 0.f, 0.f);
		queryVector(p_Data->FirstChildElement("Direction"), m_StartDirection);

		m_RandomEngine.seed((unsigned long)std::chrono::system_clock::now().time_since_epoch().count());
	}

	/**
	 * Function called after initialize to apply variables with the new initialized variables
	 */
	void postInit() override
	{
		if (m_CasterId != -1)
		{
			m_Caster = m_Owner->findActor(m_CasterId);
		}

		if (!m_SpellFactory)
		{
			return;
		}

		m_SpellInstance = m_SpellFactory->createSpellInstance(m_SpellName, m_StartDirection);
		m_Body = m_Physics->createOBB(0.f, false, m_Owner->getPosition(), m_SpellInstance->getSize(), false);

		DirectX::XMFLOAT4X4 rot = m_Caster.lock()->getComponent<LookInterface>(LookInterface::m_ComponentId).lock()->getRotationMatrix();
		
		m_Physics->setBodyRotationMatrix(m_Body, rot);

		m_Physics->setBodyCollisionResponse(m_Body, false);
		m_Physics->setBodyVelocity(m_Body, m_SpellInstance->getVelocity());

		std::weak_ptr<ModelInterface> asdff = m_Owner->getComponent<ModelInterface>(ModelInterface::m_ComponentId);
		if (asdff.lock())
		{
			asdff.lock()->setColorTone(Vector3(0.0f, 0.0f, 0.8f));
		}
	}

	/**
	 * Called to make an virtual XML-document to send over the network
	 * 
	 * @param p_Printer a XML-printer used to write an virtual XML-document
	 */
	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("Spell");
		p_Printer.PushAttribute("SpellName", m_SpellName.c_str());
		p_Printer.PushAttribute("CasterId", m_CasterId);
		pushVector(p_Printer, "Direction", m_StartDirection);
		p_Printer.CloseElement();
	}

	/**
	 * Called when an update wave pulses through, updating the spell and its components
	 * 
	 * @param p_DeltaTime an time step between frames
	 */
	void onUpdate(float p_DeltaTime) override
	{
		if (m_Body == 0)
		{
			return;
		}

		m_Owner->setPosition(m_Physics->getBodyPosition(m_Body));

		m_SpellInstance->update(p_DeltaTime);
		Actor::ptr actor = m_Caster.lock();
		BodyHandle casterBody = 0;
		if(actor)
		{
			casterBody = actor->getBodyHandles().at(0);
		}
		
		HitData boom;

		if (!m_SpellInstance->hasCollided())
		{
			for(unsigned i = 0; i < m_Physics->getHitDataSize(); i++)
			{
				HitData hit = m_Physics->getHitDataAt(i);
				if((hit.collider == m_Body && hit.collisionVictim != casterBody) || (hit.collisionVictim == m_Body && hit.collider != casterBody))
				{
					Actor::ptr caster = m_Caster.lock();
					if(caster)
					{
						auto casterBodies = caster->getBodyHandles();
						if (std::find(casterBodies.begin(), casterBodies.end(), hit.collisionVictim) != casterBodies.end() || hit.colType == Type::OBBVSAABB)
						{
							continue;
						}
					}
					boom = hit;
					m_SpellInstance->collisionHappened();
					break;
				}
			}
		}

		if (m_SpellInstance->isColliding())
		{
			Vector3 currentPosition = m_Physics->getBodyPosition(m_Body) + boom.colNorm.xyz() * boom.colLength;
			m_Physics->releaseBody(m_Body);
			m_Body = m_Physics->createSphere(0.f, true, currentPosition, m_SpellInstance->getRadius());
			m_Physics->setBodyCollisionResponse(m_Body, false);

			std::weak_ptr<ModelInterface> asdff = m_Owner->getComponent<ModelInterface>(ModelInterface::m_ComponentId);
			if (asdff.lock())
			{
				std::uniform_real_distribution<float> rotationDistributionYaw(-6.28f, 6.28f);
				std::uniform_real_distribution<float> rotationDistributionPitch(-6.28f, 6.28f);
				std::uniform_real_distribution<float> rotationDistributionRoll(-6.28f, 6.28f);

				DirectX::XMFLOAT3 rot(
					rotationDistributionYaw(m_RandomEngine),
					rotationDistributionPitch(m_RandomEngine),
					rotationDistributionRoll(m_RandomEngine));
				

				asdff.lock()->setRotation(rot);
				asdff.lock()->updateScale(m_SpellName ,Vector3(50.f, 50.f, 50.f));
				asdff.lock()->setColorTone(Vector3(0.15f, 0.0f, 0.85f));
			}
			
			m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new SpellHitEventData(*m_Owner, currentPosition )));
		}

		if (m_SpellInstance->isDead())
		{
			m_Physics->releaseBody(m_Body);
			m_Body = 0;
			m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new RemoveActorEventData(m_Owner->getId())));
		}

		if (m_SpellInstance->hasCollided())
		{
			for(unsigned i = 0; i < m_Physics->getHitDataSize(); i++)
			{
				if(m_Physics->getHitDataAt(i).collisionVictim == m_Body)
				{
					m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new SpellHitSphereEventData( m_Physics->getHitDataAt(i).collider )));
					m_SpellInstance->spellHit(p_DeltaTime, m_Physics, m_Physics->getHitDataAt(i), m_Caster.lock()->getBodyHandles()[0]);
				}
			}
		}
	}
	
	/**
	 * Set the resource manager for the component.
	 *
	 * @param p_ResourceManager the resource manager to use
	 */
	void setResourceManager(ResourceManager* p_ResourceManager)
	{
		m_ResourceManager = p_ResourceManager;
	}
	/**
	 * Set the spell factory for the component.
	 *
	 * @param p_SpellFactory the spell factory to use
	 */
	void setSpellFactory(SpellFactory* p_SpellFactory)
	{
		m_SpellFactory = p_SpellFactory;
	}
	
	/**
	 * Set the physics to use for the component.
	 *
	 * @param p_Physics the physics library to use
	 */
	void setPhysics(IPhysics* p_Physics)
	{
		m_Physics = p_Physics;
	}

	/**
	 * Get the bodyhandle for the bounding volume used in the component.
	 * 
	 * @return a bodyhandle to the bounding volume within the component
	 */
	BodyHandle getBodyHandle() const override
	{
		return m_Body;
	}
};