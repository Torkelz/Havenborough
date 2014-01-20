#pragma once

#include <INetwork.h>
#include "../../Client/Utilities/Util.h"

#include <mutex>
#include <thread>
#include <vector>

#include <tinyxml2.h>

class Server
{
private:
	struct TestBox
	{
		uint16_t actorId;

		Vector3 position;
		Vector3 velocity;
		Vector3 rotation;
		Vector3 rotationVelocity;

		Vector3 circleCenter;
		float circleRadius;
		float circleRotationSpeed;
		float circleRotation;
	};

	struct TestPlayerBox
	{
		uint16_t actorId;

		Vector3 position;
		Vector3 velocity;
		Vector3 rotation;
		Vector3 rotationVelocity;
	};
	
	struct TestPlayer
	{
		IConnectionController* m_Connection;

		TestPlayerBox m_PlayerBox;
	};

	INetwork* m_Network;

	static const float m_PlayerSphereRadius;

	uint16_t m_LastActorId;
	std::vector<TestBox> m_Boxes;
	std::vector<TestPlayer> m_Players;

	bool m_RemoveBox;
	bool m_PulseObject;

	std::mutex m_UserLock;
	
	bool m_Running;
	std::thread m_UpdateThread;

public:
	Server();

	void initialize();
	void run();
	void shutdown();
	
	std::vector<std::string> getUserNames();
	void sendTestData();
	void sendPulseObject();

private:
	static void clientConnected(IConnectionController* p_Connection, void* /*p_UserData*/);
	static void clientDisconnected(IConnectionController* p_Connection, void* /*p_UserData*/);

	void updateBox(TestBox& p_Box, float p_DeltaTime);
	void updatePlayerBox(TestPlayerBox& p_Box, float p_DeltaTime);
	UpdateObjectData getUpdateData(const TestBox& p_Box);
	UpdateObjectData getUpdateData(const TestPlayerBox& p_Box);
	void Server::pushVector(tinyxml2::XMLPrinter& p_Printer, const std::string& p_ElementName, const Vector3& p_Vec);
	std::string getBoxDescription(const TestBox& p_Box);
	std::string getPlayerBoxDescription(const TestPlayerBox& p_Box);
	ObjectInstance getBoxInstance(const TestBox& p_Box, uint16_t p_DescIdx);
	ObjectInstance getBoxInstance(const TestPlayerBox& p_Box, uint16_t p_DescIdx);
	void removeLastBox();
	void pulse();
	void handlePackages();
	void updateClients();
};
