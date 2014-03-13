#include "Components.h"
#include "Logger.h"
#include "ISound.h"

class SoundComponent : public SoundInterface
{
private:
	Vector3 m_Position;
	float m_MinDistance;
	const char* m_SoundID;
	const char* m_FilePath;
	ISound* m_Sound;
public:

	~SoundComponent() override
	{

	}

	void initialize(const tinyxml2::XMLElement* p_Data) override
	{
		m_MinDistance = 10.0f;
		m_Position = Vector3(0,0,0);
		m_SoundID = p_Data->GetText();
		p_Data->QueryAttribute("MinDistance", &m_MinDistance);
		queryVector(p_Data->FirstChildElement("Position"), m_Position);
	}
	
	void postInit() override
	{
		m_Sound->load3DSound(m_SoundID, m_FilePath, m_MinDistance);
	}

	void serialize(tinyxml2::XMLPrinter& p_Printer) const override
	{
		p_Printer.OpenElement("3DSound");
		p_Printer.PushText(m_SoundID);
		p_Printer.PushAttribute("MinDistance", m_MinDistance);
		pushVector(p_Printer, "Position", m_Position);
		p_Printer.CloseElement();
	}

	void onUpdate(float p_DeltaTime) override
	{
		
	}
};