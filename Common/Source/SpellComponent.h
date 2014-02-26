#include "Components.h"
#include "Logger.h"
#include "SpellFactory.h"

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
	BodyHandle m_Sphere;
	Actor::wPtr m_Caster;
	Actor::Id m_CasterId;

public:
	~SpellComponent() override
	{
		m_ResourceManager->releaseResource(m_SpellId);
		if (m_Sphere != 0)
		{
			m_Physics->releaseBody(m_Sphere);
		}
	}

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
	}

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

		m_Sphere = m_Physics->createSphere(0.f, false, m_Owner->getPosition(), m_SpellInstance->getRadius());
		m_Physics->setBodyCollisionResponse(m_Sphere, false);
		m_Physics->setBodyVelocity(m_Sphere, m_SpellInstance->getVelocity());
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("Spell");
		p_Printer.PushAttribute("SpellName", m_SpellName.c_str());
		p_Printer.PushAttribute("CasterId", m_CasterId);
		pushVector(p_Printer, "Direction", m_StartDirection);
		p_Printer.CloseElement();
	}

	void onUpdate(float p_DeltaTime) override
	{
		if (m_Sphere == 0)
		{
			return;
		}

		m_Owner->setPosition(m_Physics->getBodyPosition(m_Sphere));

		m_SpellInstance->update(p_DeltaTime);
		Actor::ptr actor = m_Caster.lock();
		BodyHandle casterBody = 0;
		if(actor)
		{
			casterBody = actor->getBodyHandles().at(0);
		}

		if (!m_SpellInstance->hasCollided())
		{
			for(unsigned i = 0; i < m_Physics->getHitDataSize(); i++)
			{
				HitData hit = m_Physics->getHitDataAt(i);
				if((hit.collider == m_Sphere && hit.collisionVictim != casterBody) || (hit.collisionVictim == m_Sphere && hit.collider != casterBody))
				{
					Actor::ptr caster = m_Caster.lock();
					if(caster)
					{
						auto casterBodies = caster->getBodyHandles();
						if (std::find(casterBodies.begin(), casterBodies.end(), hit.collisionVictim) != casterBodies.end())
						{
							continue;
						}
					}
					m_SpellInstance->collisionHappened();
					break;
				}
			}
		}

		if (m_SpellInstance->isColliding())
		{
			Vector3 currentPosition = m_Physics->getBodyPosition(m_Sphere);
			m_Physics->releaseBody(m_Sphere);
			m_Sphere = m_Physics->createSphere(0.f, true, currentPosition, m_SpellInstance->getRadius());
			m_Physics->setBodyCollisionResponse(m_Sphere, false);
			//m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new SpellhitEventData(m_Owner, currentPosition)));
		}

		if (m_SpellInstance->isDead())
		{
			m_Physics->releaseBody(m_Sphere);
			m_Sphere = 0;
			m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new RemoveActorEventData(m_Owner->getId())));
		}

		if (m_SpellInstance->hasCollided())
		{
			for(unsigned i = 0; i < m_Physics->getHitDataSize(); i++)
			{
				if(m_Physics->getHitDataAt(i).collisionVictim == m_Sphere)
				{
					m_SpellInstance->spellHit(p_DeltaTime, m_Physics, m_Physics->getHitDataAt(i));
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
		return m_Sphere;
	}
};