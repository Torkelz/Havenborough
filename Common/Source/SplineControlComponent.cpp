#include "SplineControlComponent.h"
#include "TweakSettings.h"
#include "Logger.h"

SplineControlComponent::SplineControlComponent()
	: m_Physics(nullptr),
	m_PhysicsComp(),
	m_MaxSpeed(0.f),
	m_AccConstant(0.f),
	m_SplineActivated(false),
	m_Lifetime(0.0f),
	m_LookComp(),
	m_SplineSequence(0)
{
}

void SplineControlComponent::initialize(const tinyxml2::XMLElement* p_Data)
{
	p_Data->QueryAttribute("MaxSpeed", &m_MaxSpeed);
	p_Data->QueryAttribute("Acceleration", &m_AccConstant);
}

void SplineControlComponent::serialize(tinyxml2::XMLPrinter& p_Printer) const
{
	p_Printer.OpenElement("SplineControl");
	p_Printer.PushAttribute("MaxSpeed", m_MaxSpeed);
	p_Printer.PushAttribute("Acceleration", m_AccConstant);
	p_Printer.CloseElement();
}

void SplineControlComponent::postInit()
{
	m_PhysicsComp = m_Owner->getComponent<PhysicsInterface>(PhysicsInterface::m_ComponentId);
	m_LookComp = m_Owner->getComponent<LookInterface>(LookInterface::m_ComponentId).lock();
	std::shared_ptr<PhysicsInterface> phys = m_PhysicsComp.lock();
	if (phys)
	{
		m_Physics->setBodyGravity(phys->getBodyHandle(), 0.f);
	}

	TweakSettings* settings = TweakSettings::getInstance();
	settings->setSetting("spline.save","Default");
	settings->setListener("spline.save", std::function<void(const std::string&)>(
		[&] (const std::string& p_Filename)
	{
		savePath(p_Filename);
	}));

	settings->setSetting("spline.load","Default");
	settings->setListener("spline.load", std::function<void(const std::string&)>(
		[&] (const std::string& p_Filename)
	{
		loadPath(p_Filename);
	}));

	settings->setSetting("spline.run", 0);
	settings->setListener("spline.run", std::function<void(int)>(
		[&] (int p_Sequence)
	{
		runSpline(p_Sequence);
	}));

	settings->setSetting("spline.newSeq", 0);
	settings->setListener("spline.newSeq", std::function<void(int)>(
		[&] (int)
	{
		newSequence();
	}));

	settings->setSetting("spline.currSeq", 0);
	settings->setListener("spline.currSeq", std::function<void(int)>(
		[&] (int)
	{
		currentSequence();
	}));
	
	settings->setSetting("spline.select", 0);
	settings->setListener("spline.select", std::function<void(int)>(
		[&] (int p_Sequence)
	{
		selectSequence(p_Sequence);
	}));

	m_Sequences.push_back(sequence());
	m_SplineSequence = 0;
}

void SplineControlComponent::move(float p_DeltaTime)
{
	if(m_SplineActivated)
	{
		std::shared_ptr<PhysicsInterface> comp = m_PhysicsComp.lock();
		std::shared_ptr<LookInterface> lookComp = m_LookComp.lock();
		
		if (!comp || !lookComp)
			return;

		const std::vector<Vector3> &positions = m_Sequences.at(m_SplineSequence).m_Positions;
		const std::vector<Vector3> &lookForward = m_Sequences.at(m_SplineSequence).m_LookForward;
		const std::vector<Vector3> &lookUp = m_Sequences.at(m_SplineSequence).m_LookUp;


		BodyHandle body = comp->getBodyHandle();

		using namespace DirectX;
		m_Lifetime += p_DeltaTime;
		if((unsigned int)m_Lifetime > positions.size() - 2)
		{
			m_Lifetime = 0.f;
			m_SplineActivated = false;
			return;
		}
		m_Physics->setBodyPosition(body, catMullRom(positions, m_Lifetime));

		lookComp->setLookForward(catMullRom(lookForward, m_Lifetime));
		lookComp->setLookUp(catMullRom(lookUp, m_Lifetime));

		m_FlyingDirection = Vector3();
	}
	else
	{
		defaultMove(p_DeltaTime);
	}
}

Vector3 SplineControlComponent::getLocalDirection() const
{
	return m_FlyingDirection;
}

void SplineControlComponent::setLocalDirection(const Vector3& p_Direction)
{
	m_FlyingDirection = p_Direction;
}

float SplineControlComponent::getMaxSpeed() const
{
	return m_MaxSpeed;
}

void SplineControlComponent::setMaxSpeed(float p_Speed)
{
	m_MaxSpeed = p_Speed;
}

void SplineControlComponent::setPhysics(IPhysics* p_Physics)
{
	m_Physics = p_Physics;
}

void SplineControlComponent::recordPoint()
{
	std::shared_ptr<PhysicsInterface> comp = m_PhysicsComp.lock();
	std::shared_ptr<LookInterface> lookComp = m_LookComp.lock();

	if (!comp || !lookComp)
		return;
	
	BodyHandle body = comp->getBodyHandle();
	m_Sequences.at(m_SplineSequence).m_Positions.push_back(m_Physics->getBodyPosition(body));
	m_Sequences.at(m_SplineSequence).m_LookForward.push_back(lookComp->getLookForward());
	m_Sequences.at(m_SplineSequence).m_LookUp.push_back(lookComp->getLookUp());
}
void SplineControlComponent::removePreviousPoint()
{
	if(m_Sequences.size() > 0 && m_Sequences.at(m_SplineSequence).m_Positions.size() > 0)
	{
		m_Sequences.at(m_SplineSequence).m_Positions.pop_back();
		m_Sequences.at(m_SplineSequence).m_LookForward.pop_back();
		m_Sequences.at(m_SplineSequence).m_LookUp.pop_back();
	}
}
void SplineControlComponent::clearSequence()
{
	if(m_Sequences.size() > m_SplineSequence)
		m_Sequences.erase(m_Sequences.begin() + m_SplineSequence);

	if(m_Sequences.empty())
		m_Sequences.push_back(sequence());
}

void SplineControlComponent::runSpline(int p_Sequence)
{
	if(!m_SplineActivated && m_Sequences.size() > 0 && m_Sequences.size() > (unsigned int)p_Sequence)
	{
		if((int)m_Sequences.at(p_Sequence).m_Positions.size() - 2 >= 0)
		{
			m_SplineActivated = true;
			m_SplineSequence = p_Sequence;
		}
	}
}

void SplineControlComponent::savePath(const std::string &p_Filename)
{
	using namespace tinyxml2;
	XMLDocument doc;
	doc.InsertFirstChild(doc.NewDeclaration());
	XMLElement *sele = nullptr;

	for(unsigned int j = 0; j < m_Sequences.size(); j++)
	{
		if(m_Sequences.at(j).m_Positions.empty())
			continue;

		sele = doc.NewElement("Sequence");
		for(unsigned int i = 0; i < m_Sequences.at(j).m_Positions.size(); i++)
		{
			XMLElement *node = doc.NewElement("Node");

			XMLElement *ele = doc.NewElement("Position");
			ele->SetAttribute("x", m_Sequences.at(j).m_Positions[i].x);
			ele->SetAttribute("y", m_Sequences.at(j).m_Positions[i].y);
			ele->SetAttribute("z", m_Sequences.at(j).m_Positions[i].z);
			node->InsertEndChild(ele);

			ele = doc.NewElement("Forward");
			ele->SetAttribute("x", m_Sequences.at(j).m_LookForward[i].x);
			ele->SetAttribute("y", m_Sequences.at(j).m_LookForward[i].y);
			ele->SetAttribute("z", m_Sequences.at(j).m_LookForward[i].z);
			node->InsertEndChild(ele);

			ele = doc.NewElement("Up");
			ele->SetAttribute("x", m_Sequences.at(j).m_LookUp[i].x);
			ele->SetAttribute("y", m_Sequences.at(j).m_LookUp[i].y);
			ele->SetAttribute("z", m_Sequences.at(j).m_LookUp[i].z);
			node->InsertEndChild(ele);

			sele->InsertEndChild(node);
		}
		doc.InsertEndChild(sele);
	}
	boost::filesystem::path path = boost::filesystem::initial_path();
	path = path / std::string(p_Filename + ".xml");

	XMLError res = doc.SaveFile(path.string().c_str());
	if(res != XML_NO_ERROR)
		int dummy = 0;
}

void SplineControlComponent::loadPath(const std::string &p_Filename)
{
	boost::filesystem::path path = boost::filesystem::initial_path();
	path = path / std::string(p_Filename + ".xml");

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError res = doc.LoadFile(path.string().c_str());
	if(res != tinyxml2::XML_NO_ERROR)
	{
		Logger::log(Logger::Level::INFO, "Failed to open " + std::string(p_Filename + ".xml"));
		return;
	}
	
	tinyxml2::XMLElement *root = doc.FirstChildElement();
	if(root == nullptr)
	{
		Logger::log(Logger::Level::INFO, "Failed to read root in " + std::string(p_Filename + ".xml"));
		return;
	}

	m_Sequences.clear();
	tinyxml2::XMLElement *seqElement = nullptr;
	//For every node tag
	std::vector<Vector3> pos, forward, up;
	for(seqElement = root; seqElement != nullptr; seqElement = seqElement->NextSiblingElement())
	{
		tinyxml2::XMLElement *element = nullptr;
		//For every node tag
		std::vector<Vector3> pos, forward, up;
		for(element = seqElement->FirstChildElement(); element != nullptr; element = element->NextSiblingElement())
		{
			Vector3 temp;
			//Position
			tinyxml2::XMLElement *posElement = element->FirstChildElement();

			posElement->QueryFloatAttribute("x", &temp.x);
			posElement->QueryFloatAttribute("y", &temp.y);
			posElement->QueryFloatAttribute("z", &temp.z);

			pos.push_back(temp);
			//Forward
			posElement = posElement->NextSiblingElement();

			posElement->QueryFloatAttribute("x", &temp.x);
			posElement->QueryFloatAttribute("y", &temp.y);
			posElement->QueryFloatAttribute("z", &temp.z);

			forward.push_back(temp);
			//Up
			posElement = posElement->NextSiblingElement();

			posElement->QueryFloatAttribute("x", &temp.x);
			posElement->QueryFloatAttribute("y", &temp.y);
			posElement->QueryFloatAttribute("z", &temp.z);

			up.push_back(temp);
		}
		sequence s;
		s.m_Positions = pos;
		s.m_LookForward = forward;
		s.m_LookUp = up;
		m_Sequences.push_back(s);
	}
}

void SplineControlComponent::newSequence()
{
	m_Sequences.push_back(sequence());
	m_SplineSequence++;
}

void SplineControlComponent::currentSequence()
{
	Logger::log(Logger::Level::INFO, "Current spline sequence: " + std::to_string(m_SplineSequence));
}

void SplineControlComponent::selectSequence(unsigned int p_Sequence)
{
	if(m_Sequences.size() > p_Sequence && p_Sequence > 0)
	{
		m_SplineSequence = p_Sequence;

		std::shared_ptr<PhysicsInterface> comp = m_PhysicsComp.lock();
		std::shared_ptr<LookInterface> lookComp = m_LookComp.lock();
		
		if (!comp || !lookComp)
			return;

		const std::vector<Vector3> &positions = m_Sequences.at(m_SplineSequence).m_Positions;
		const std::vector<Vector3> &lookForward = m_Sequences.at(m_SplineSequence).m_LookForward;
		const std::vector<Vector3> &lookUp = m_Sequences.at(m_SplineSequence).m_LookUp;

		BodyHandle body = comp->getBodyHandle();
		m_Physics->setBodyPosition(body, positions.back());

		lookComp->setLookForward(lookForward.back());
		lookComp->setLookUp(lookUp.back());
	}
	else
	{
		std::string msg = "Valid spline values: " + std::string(" to ") + std::to_string(m_Sequences.size() - 1);
		Logger::log(Logger::Level::INFO, msg);
	}
}

Vector3 SplineControlComponent::catMullRom(const std::vector<Vector3> &p_Path, float p_Lifetime)
{
	using namespace DirectX;
	unsigned int lifeTime = (unsigned int)p_Lifetime;
	float proc = p_Lifetime - lifeTime;

	XMVECTOR p0, p4;

	if(lifeTime == 0)
		p0 = Vector3ToXMVECTOR(&p_Path.at(lifeTime), 1.f);
	else
		p0 = Vector3ToXMVECTOR(&p_Path.at(lifeTime - 1), 1.f);

	if(lifeTime + 2 > p_Path.size() - 1)
		p4 = Vector3ToXMVECTOR(&p_Path.at(lifeTime + 1), 1.f);
	else
		p4 = Vector3ToXMVECTOR(&p_Path.at(lifeTime + 2), 1.f);

	XMVECTOR ret = DirectX::XMVectorCatmullRom(p0, Vector3ToXMVECTOR(&p_Path.at(lifeTime), 1.f),
		Vector3ToXMVECTOR(&p_Path.at(lifeTime + 1), 1.f), p4, proc);

	XMFLOAT3 retPos;
	XMStoreFloat3(&retPos, ret);
	return retPos;
}

void SplineControlComponent::defaultMove(float p_DeltaTime)
{
	std::shared_ptr<PhysicsInterface> comp = m_PhysicsComp.lock();

	if (!comp)
		return;
	
	BodyHandle body = comp->getBodyHandle();

	using namespace DirectX;
	XMFLOAT3 velocity = m_Physics->getBodyVelocity(body);
	XMVECTOR currentVelocity = XMLoadFloat3(&velocity);	// cm/s

	XMFLOAT3 maxVelocity(m_FlyingDirection * getMaxSpeed());	// cm/s

	XMVECTOR vMaxVelocity = XMLoadFloat3(&maxVelocity);

	XMVECTOR diffVel = vMaxVelocity - currentVelocity;	// cm/s
	
	XMVECTOR force = diffVel / 100.f * m_AccConstant;		// kg * m/s^2

	XMVECTOR forceDiffImpulse = force * p_DeltaTime;	// kg * m/s

	XMFLOAT3 fForceDiff;
	XMStoreFloat3(&fForceDiff, forceDiffImpulse);

	m_Physics->applyImpulse(body, fForceDiff);

	m_FlyingDirection = Vector3();
}