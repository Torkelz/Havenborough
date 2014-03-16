#include "Components.h"
#include "Logger.h"

class SoundComponent : public SoundInterface
{
private:
	Vector3 m_Velocity;
	float m_MinDistance;
	std::string m_SoundName;
	int m_SoundID;
	bool m_3D;
	bool m_Loop;
public:

	~SoundComponent() override
	{
		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new Release3DSoundEventData(m_Owner->getId(), m_SoundID)));
	}

	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		m_MinDistance = 10.0f;
		m_Velocity = Vector3(0,0,0);
		m_SoundID = -1;
		m_3D = true;
		m_Loop = true;
		m_SoundName = p_Data->Attribute("FileName");
		p_Data->QueryAttribute("SoundID", &m_SoundID);
		p_Data->QueryBoolAttribute("MultiD", &m_3D);
		p_Data->QueryBoolAttribute("Loop", &m_Loop);
		p_Data->QueryAttribute("MinDistance", &m_MinDistance);
		queryVector(p_Data->FirstChildElement("Velocity"), m_Velocity);
	}
	
	void postInit() override
	{	
		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new Create3DSoundEventData(m_SoundName.c_str(), m_Owner->getId(), m_MinDistance, m_SoundID, m_3D, m_Loop)));
		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new Play3DSoundEventData(m_Owner->getId(), m_SoundID, m_Owner->getPosition(), m_Velocity)));
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("Sound");
		p_Printer.PushAttribute("FileName" , m_SoundName.c_str());
		p_Printer.PushAttribute("SoundID", m_SoundID);
		p_Printer.PushAttribute("MultiD", m_3D);
		p_Printer.PushAttribute("Loop", m_Loop);
		p_Printer.PushAttribute("MinDistance", m_MinDistance);
		pushVector(p_Printer, "Velocity", m_Velocity);
		p_Printer.CloseElement();
	}

	void onUpdate(float p_DeltaTime) override
	{
		m_Owner->getEventManager()->queueEvent(IEventData::Ptr(new Update3DSoundEventData(m_Owner->getId(), m_SoundID, m_Owner->getPosition(), m_Velocity)));
	}
};