#include "Components.h"
#include "Logger.h"

class SoundComponent : public SoundInterface
{
private:
	Vector3 m_Position;
	float m_MinDistance;
	std::string m_SoundID;
	std::string m_FilePath;
public:

	~SoundComponent() override
	{
		
	}

	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		m_MinDistance = 10.0f;
		m_Position = Vector3(0,0,0);
		m_SoundID = p_Data->Attribute("SoundID");
		m_FilePath = p_Data->Attribute("FilePath");
		p_Data->QueryAttribute("MinDistance", &m_MinDistance);
	}
	
	void postInit() override
	{
		m_Sound->load3DSound(m_SoundID.c_str(), m_FilePath.c_str(), m_MinDistance); //Event baserat
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("3DSound");
		p_Printer.PushAttribute("SoundID", m_SoundID.c_str());
		p_Printer.PushAttribute("FilePath", m_FilePath.c_str());
		p_Printer.PushAttribute("MinDistance", m_MinDistance);
		p_Printer.CloseElement();
	}

	void onUpdate(float p_DeltaTime) override
	{
		m_Sound->onFrameSound(m_SoundID.c_str(), &m_Owner->getPosition(), NULL); //Event baserat
	}
};