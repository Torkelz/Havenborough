#include "Components.h"
#include "Logger.h"
#include "ISound.h"

class SoundComponent : public SoundInterface
{
private:
	Vector3 m_Position;
	float m_MinDistance;
public:

	~SoundComponent() override
	{

	}

	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		m_MinDistance = 10.0f;
		m_Position = Vector3(0,0,0);
		p_Data->QueryAttribute("MinDistance", &m_MinDistance);
		queryVector(p_Data->FirstChildElement("Position"), m_Position);
	}
	
	void postInit() override
	{
		m_Model = m_Owner->getComponent<ModelInterface>(ModelInterface::m_ComponentId);
		if(!m_Model.lock())
		{
			return;
		}
		m_Position = m_Model.lock()->getOffset();
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("3DSound");
		p_Printer.PushAttribute("Random", m_Random);
		pushVector(p_Printer, "Offset", m_Offset);
		p_Printer.CloseElement();
	}

	void onUpdate(float p_DeltaTime) override
	{
		m_Time += p_DeltaTime;

		float sinus = std::sin(m_Time+m_Random);
		Vector3 result = m_Position + m_Offset * sinus;
		if(!m_Model.lock())
		{
			return;
		}
		m_Model.lock()->setOffset(result);
	}
};