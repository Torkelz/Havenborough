#pragma once
#include "IEventData.h"
#include "Light.h"
#include "Utilities/XMFloatUtil.h"

#pragma warning(push)
#pragma warning(disable : 4100)

#pragma region EXAMPLE READ THIS IF YOU DO NOT KNOW HOW TO CREATE AN EVENT
//////////////////////////////////////////////////////////////////////////
/// EXAMPLE EVENT DATA AND USED FOR TESTING
//////////////////////////////////////////////////////////////////////////
class TestEventData : public BaseEventData
{
private: 
	//Parameter for event, can be of any number and type
	bool m_AssumingDirectControl;

public:
	/**
	* Unique identifier for event data type. This one is an example and for testing.
	* E.g. IEventData::Type UniqueEventDataName::sk_EventType(unique_hex);
	*/
	static const IEventData::Type sk_EventType = Type(0x77dd2b3a);
	
	explicit TestEventData(bool p_AssumingControl) :
		m_AssumingDirectControl(p_AssumingControl)
	{
	}

	virtual const IEventData::Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new TestEventData(m_AssumingDirectControl));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
		p_Out << m_AssumingDirectControl;
	}

	virtual const char *getName(void) const override
	{
		return "TestEvent";
	}

	/**
	* Used to get the event data. User defined function.
	* Can be of any number of functions and have any return type.
	*/
	bool directInterventionIsNecessary(void) const
	{
		return m_AssumingDirectControl;
	}
};

#pragma endregion

class LightEventData : public BaseEventData
{
private:
	Light m_Light;

public:
	static const Type sk_EventType = Type(0x748d2b5a);
	
	explicit LightEventData(Light p_Light) :
		m_Light(p_Light)
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new LightEventData(m_Light));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "LightEvent";
	}

	Light getLight(void) const
	{
		return m_Light;
	}
};

class RemoveLightEventData : public BaseEventData
{
private:
	Light::Id m_Id;

public:
	static const Type sk_EventType = Type(0x128d2b5a);
	
	explicit RemoveLightEventData(Light::Id p_Id) :
		m_Id(p_Id)
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new RemoveLightEventData(m_Id));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "RemoveLightEvent";
	}

	Light::Id getId() const
	{
		return m_Id;
	}
};

class CreateMeshEventData : public BaseEventData
{
private:
	unsigned int m_Id;
	std::string m_MeshName;
	Vector3 m_Scale;
	Vector3 m_ColorTone;

public:
	static const Type sk_EventType = Type(0xdeadbeef);

	CreateMeshEventData(unsigned int p_Id, const std::string& p_MeshName, Vector3 p_Scale, Vector3 p_ColorTone)
		:	m_Id(p_Id),
			m_MeshName(p_MeshName),
			m_Scale(p_Scale),
			m_ColorTone(p_ColorTone)
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new CreateMeshEventData(m_Id, m_MeshName, m_Scale, m_ColorTone));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "CreateMeshEvent";
	}

	std::string getMeshName() const
	{
		return m_MeshName;
	}

	unsigned int getId() const
	{
		return m_Id;
	}

	Vector3 getScale() const
	{
		return m_Scale;
	}

	Vector3 getColorTone() const
	{
		return m_ColorTone;
	}
};

class RemoveMeshEventData : public BaseEventData
{
private:
	unsigned int m_Id;

public:
	static const Type sk_EventType = Type(0xdeadebbe);

	RemoveMeshEventData(unsigned int p_Id)
		:	m_Id(p_Id)
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new RemoveMeshEventData(m_Id));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "RemoveMeshEvent";
	}

	unsigned int getId() const
	{
		return m_Id;
	}
};

class UpdateModelPositionEventData : public BaseEventData
{
private:
	unsigned int m_Id;
	Vector3 m_Position;

public:
	static const Type sk_EventType = Type(0x77dd2b5b);

	UpdateModelPositionEventData(unsigned int p_Id, Vector3 p_Position)
		:	m_Id(p_Id), m_Position(p_Position)
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new UpdateModelPositionEventData(m_Id, m_Position));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "UpdateModelPositionEvent";
	}

	unsigned int getId() const
	{
		return m_Id;
	}

	Vector3 getPosition() const
	{
		return m_Position;
	}
};

class UpdateModelScaleEventData : public BaseEventData
{
private:
	unsigned int m_Id;
	Vector3 m_Scale;

public:
	static const Type sk_EventType = Type(0x77dd2b5c);

	UpdateModelScaleEventData(unsigned int p_Id, Vector3 p_Scale)
		:	m_Id(p_Id),m_Scale(p_Scale)
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new UpdateModelScaleEventData(m_Id, m_Scale));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "UpdateModelScaleEvent";
	}

	unsigned int getId() const
	{
		return m_Id;
	}

	Vector3 getScale() const
	{
		return m_Scale;
	}
};

class UpdateModelRotationEventData : public BaseEventData
{
private:
	unsigned int m_Id;
	Vector3 m_Rotation;

public:
	static const Type sk_EventType = Type(0x77dd2b5d);

	UpdateModelRotationEventData(unsigned int p_Id, Vector3 p_Rotation)
		:	m_Id(p_Id), m_Rotation(p_Rotation)
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new UpdateModelRotationEventData(m_Id, m_Rotation));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "UpdateModelRotationEvent";
	}

	unsigned int getId() const
	{
		return m_Id;
	}

	Vector3 getRotation() const
	{
		return m_Rotation;
	}
};

class PlayAnimationEventData : public BaseEventData
{
private:
	unsigned int m_Id;
	std::string m_AnimationName;
	bool m_Override;

public:
	static const Type sk_EventType = Type(0x14dd2b5d);

	PlayAnimationEventData(unsigned int p_Id, std::string p_AnimationName, bool p_Override)
		:	m_Id(p_Id), m_AnimationName(p_AnimationName), m_Override(p_Override)
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new PlayAnimationEventData(m_Id, m_AnimationName, m_Override));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "PlayAnimationEvent";
	}

	unsigned int getId() const
	{
		return m_Id;
	}

	std::string getAnimationName() const
	{
		return m_AnimationName;
	}

	bool getOverride() const
	{
		return m_Override;
	}
};

class QueueAnimationEventData : public BaseEventData
{
private:
	unsigned int m_Id;
	std::string m_AnimationName;

public:
	static const Type sk_EventType = Type(0x140d2b5d);

	QueueAnimationEventData(unsigned int p_Id, std::string p_AnimationName)
		:	m_Id(p_Id), m_AnimationName(p_AnimationName)
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new QueueAnimationEventData(m_Id, m_AnimationName));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "QueueAnimationEvent";
	}

	unsigned int getId() const
	{
		return m_Id;
	}

	std::string getAnimationName() const
	{
		return m_AnimationName;
	}
};

class ChangeAnimationWeightEventData : public BaseEventData
{
private:
	unsigned int m_Id;
	int m_Track;
	float m_Weight;

public:
	static const Type sk_EventType = Type(0x12334455);

	ChangeAnimationWeightEventData(unsigned int p_Id, int p_Track, float p_Weight)
		:	m_Id(p_Id), m_Track(p_Track), m_Weight(p_Weight)
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new ChangeAnimationWeightEventData(m_Id, m_Track, m_Weight));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "ChangeAnimationWeightEvent";
	}

	unsigned int getId() const
	{
		return m_Id;
	}

	int getTrack() const
	{
		return m_Track;
	}

	float getWeight() const
	{
		return m_Weight;
	}
};

class AddReachIK_EventData : public BaseEventData
{
private:
	unsigned int m_Id;
	std::string m_GroupName;
	Vector3 m_Target;

public:
	static const Type sk_EventType = Type(0x12334544);

	AddReachIK_EventData(unsigned int p_Id, std::string p_GroupName, Vector3 p_Target)
		:	m_Id(p_Id),
			m_GroupName(p_GroupName),
			m_Target(p_Target)
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new AddReachIK_EventData(m_Id, m_GroupName, m_Target));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "AddReachIK_Event";
	}

	unsigned int getId() const
	{
		return m_Id;
	}

	std::string getGroupName() const
	{
		return m_GroupName;
	}

	Vector3 getTarget() const
	{
		return m_Target;
	}
};

class RemoveReachIK_EventData : public BaseEventData
{
private:
	unsigned int m_Id;

	std::string m_GroupName;

public:
	static const Type sk_EventType = Type(0x142d2b5d);

	RemoveReachIK_EventData(unsigned int p_Id, std::string p_GroupName)
		:	m_Id(p_Id),
			m_GroupName(p_GroupName)
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new RemoveReachIK_EventData(m_Id, m_GroupName));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "RemoveReachIK_Event";
	}

	unsigned int getId() const
	{
		return m_Id;
	}

	std::string getGroupName() const
	{
		return m_GroupName;
	}
};

class GameStartedEventData : public BaseEventData
{
private:

public:
	static const Type sk_EventType = Type(0x38ae3f31);

	GameStartedEventData()
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new GameStartedEventData);
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "GameStartedEvent";
	}
};

class GameLeftEventData : public BaseEventData
{
private:
	bool m_GoBack;

public:
	static const Type sk_EventType = Type(0x846ef45b);

	explicit GameLeftEventData(bool p_GoBack)
		:	m_GoBack(p_GoBack)
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new GameLeftEventData(m_GoBack));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "GameLeftEvent";
	}

	bool getGoBack() const
	{
		return m_GoBack;
	}
};

class QuitGameEventData : public BaseEventData
{
private:

public:
	static const Type sk_EventType = Type(0x846e56eb);

	QuitGameEventData()
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new QuitGameEventData);
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "QuitGameEvent";
	}
};

class MouseEventDataLock : public BaseEventData
{
private: 
	//Parameter for event, can be of any number and type
	bool m_State;

public:
	/**
	* Unique identifier for event data type. This one is an example and for testing.
	* E.g. IEventData::Type UniqueEventDataName::sk_EventType(unique_hex);
	*/
	static const IEventData::Type sk_EventType = Type(0x72dd2b3a);
	
	explicit MouseEventDataLock(bool p_LockState) :
		m_State(p_LockState)
	{
	}

	virtual const IEventData::Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new MouseEventDataLock(m_State));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
		p_Out << m_State;
	}

	virtual const char *getName(void) const override
	{
		return "MouseEventDataLock";
	}

	/**
	* Used to get the event data. User defined function.
	* Can be of any number of functions and have any return type.
	*/
	bool getLockState(void) const
	{
		return m_State;
	}
};

class MouseEventDataShow : public BaseEventData
{
private: 
	bool m_State;

public:
	static const IEventData::Type sk_EventType = Type(0x22dd2b3a);
	
	explicit MouseEventDataShow(bool p_HideState) :
		m_State(p_HideState)
	{
	}

	virtual const IEventData::Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new MouseEventDataShow(m_State));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
		p_Out << m_State;
	}

	virtual const char *getName(void) const override
	{
		return "MouseEventDataShow";
	}

	bool getShowState(void) const
	{
		return m_State;
	}
};

class ChangeColorToneEvent : public BaseEventData
{
private: 
	Vector3 m_ColorTone;
	unsigned int m_MeshId;

public:
	static const IEventData::Type sk_EventType = Type(0xbabbab3a);
	
	explicit ChangeColorToneEvent(unsigned int p_MeshId, Vector3 p_ColorTone) :
		m_MeshId(p_MeshId),
		m_ColorTone(p_ColorTone)
	{
	}

	virtual const IEventData::Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new ChangeColorToneEvent(m_MeshId, m_ColorTone));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
		//p_Out << m_ColorTone;
	}

	virtual const char *getName(void) const override
	{
		return "ChangeColorToneEvent";
	}

	Vector3 getColorTone(void) const
	{
		return m_ColorTone;
	}

	unsigned int getMeshId(void) const
	{
		return m_MeshId;
	}
};

class CreateParticleEventData : public BaseEventData
{
private:
	unsigned int m_Id;
	std::string m_EffectName;
	Vector3 m_Position;

public:
	static const Type sk_EventType = Type(0x54456edb);

	CreateParticleEventData(unsigned int p_Id, const std::string& p_EffectName, Vector3 p_Position)
		:	m_Id(p_Id),
			m_EffectName(p_EffectName),
			m_Position(p_Position)
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new CreateParticleEventData(m_Id, m_EffectName, m_Position));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "CreateParticleEvent";
	}

	std::string getEffectName() const
	{
		return m_EffectName;
	}

	unsigned int getId() const
	{
		return m_Id;
	}

	Vector3 getPosition() const
	{
		return m_Position;
	}
};

class RemoveParticleEventData : public BaseEventData
{
private:
	unsigned int m_Id;

public:
	static const Type sk_EventType = Type(0x82544aeb);

	RemoveParticleEventData(unsigned int p_Id)
		:	m_Id(p_Id)
	{
	}

	virtual const Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new RemoveParticleEventData(m_Id));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}

	virtual const char *getName(void) const override
	{
		return "RemoveParticleEvent";
	}

	unsigned int getId() const
	{
		return m_Id;
	}
};


#pragma warning(pop)