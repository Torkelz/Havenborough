#include "Components.h"

#include "SpellFactory.h"

class SpellComponent : public SpellInterface
{

private:
	int m_SpellId;
	std::string m_SpellName;
	ResourceManager* m_ResourceManager;
	SpellFactory* m_SpellFactory;
	Vector3 m_StartDirection;
	SpellInstance::ptr m_SpellInstance;

public:
	~SpellComponent() override
	{
		m_ResourceManager->releaseResource(m_SpellId);
	}

	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		const char* spellName = p_Data->Attribute("SpellName");
		if (!spellName)
		{
			throw CommonException("Missing spell name", __LINE__, __FILE__);
		}

		m_SpellName = spellName;
		m_SpellId = m_ResourceManager->loadResource("spell", m_SpellName);

		m_StartDirection = Vector3(0.f, 0.f, 0.f);
		queryVector(p_Data->FirstChildElement("Direction"), m_StartDirection);
	}

	void postInit() override
	{
		m_SpellInstance = m_SpellFactory->createSpellInstance(m_SpellName, m_StartDirection, m_Owner->getPosition());
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("Spell");
		p_Printer.PushAttribute("SpellName", m_SpellName.c_str());
		p_Printer.CloseElement();
	}

	void onUpdate(float p_DeltaTime) override
	{
		m_SpellInstance->update(p_DeltaTime);
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
};