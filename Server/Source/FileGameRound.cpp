#include "FileGameRound.h"

#include <Logger.h>
#include <fstream>
#include <sstream>

void FileGameRound::setup()
{
	for (size_t i = 0; i < m_Players.size(); ++i)
	{
		Vector3 position(500.f - i * 200.f, 50 + 400.f, 600.f);

		Actor::ptr actor = m_ActorFactory->createPlayerActor(position);
		m_Players[i].setActor(actor);
		m_Actors.push_back(actor);
	}
	m_FileLoader.loadBinaryFile(m_FilePath);
}

void FileGameRound::setFilePath(std::string p_Filepath)
{
	m_FilePath = p_Filepath;
}

void FileGameRound::sendLevel()
{
	std::vector<std::string> descriptions;
	std::vector<ObjectInstance> instances;

	for (const auto& actor : m_Actors)
	{
		std::ostringstream descStream;
		actor->serialize(descStream);
		descriptions.push_back(descStream.str());
		ObjectInstance inst = 
		{
			descriptions.back().c_str(),
			actor->getId()
		};
		instances.push_back(inst);
	}

	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Object");
	printer.OpenElement("Light");
	printer.PushAttribute("Type", "Directional");
	pushVector(printer, "Direction", Vector3(0.f, -1.f, 0.f));
	static const Vector3 color(1.f, 1.f, 1.f);
	printer.OpenElement("Color");
	printer.PushAttribute("r", color.x);
	printer.PushAttribute("g", color.y);
	printer.PushAttribute("b", color.z);
	printer.CloseElement();
	printer.CloseElement();
	printer.CloseElement();

	descriptions.push_back(printer.CStr());
	ObjectInstance lightData =
	{
		descriptions.back().c_str(),
		0
	};
	instances.push_back(lightData);

	for (auto& player : m_Players)
	{
		User::ptr user = player.getUser().lock();
		if (user)
		{
			Actor::ptr actor = player.getActor().lock();
			if(actor)
			{
				user->getConnection()->sendCreateObjects(instances.data(), instances.size());
				std::string stream = m_FileLoader.getDataStream();
				user->getConnection()->sendLevelData(stream.c_str(), stream.size());
				user->getConnection()->sendAssignPlayer(actor->getId());
			}
		}
	}
}

void FileGameRound::pushVector(tinyxml2::XMLPrinter& p_Printer, const std::string& p_ElementName, const Vector3& p_Vec)
{
	p_Printer.OpenElement(p_ElementName.c_str());
	p_Printer.PushAttribute("x", p_Vec.x);
	p_Printer.PushAttribute("y", p_Vec.y);
	p_Printer.PushAttribute("z", p_Vec.z);
	p_Printer.CloseElement();
}