#include "LemonEngine.h"

#define AWAITING_TRACKED_ID -1

static int outgoing = 0;

void initialiseNetworkData(void)
{
	Networking.connectMode = OFFLINE;
	Networking.connectionStatus = CONNECT_STATE_DISCONNECTED;

	strcpy(Networking.myUsername, "Unnamed");

	Networking.serverPort = 0;
	Networking.serverAddress = NULL;
	Networking.clientID = NO_CLIENT_ID;

	Networking.timeElapsed = 0.0;
	Networking.updateRate = NewConsoleVariable("net_updaterate", "time between updates", CONVAR_FLOAT, "0.05", 0);
	Networking.connectionTimeout = NewConsoleVariable("net_connectiontimeout", "max time allowed before a connection is considered lost", CONVAR_FLOAT, "5.0", 0);

	Networking.Server = NULL;
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		Networking.connectedClients[i] = NULL;
		Networking.clientStates[i] = CLIENT_STATE_DISCONNECTED;
		Networking.clientTimers[i] = 0.0;
	}
	Networking.clientCount = 0;

	for (int i = 0; i < MAX_BLOCKED_CLIENTS; i++)
	{
		Networking.blockedIPs[i] = NULL;
	}
	Networking.blockedIndex = 0;

	memset(Networking.serverPassword, 0, MAX_LEN);

	Networking.myClient = NULL;

	memset(Networking.clientUsernames, 0, MAX_CLIENTS * MAX_LEN);
	memset(Networking.serverUsername, 0,  MAX_LEN);
	
	memset(&Networking.NetworkEvents, 0, sizeof(GameEventManager));
	resetTrackedObjects();

	strcpy(Networking.setUpString, LEMON_VERSION);
	if (DEBUG_MODE)
	{
		strcat(Networking.setUpString, "_DebugMode");
	}

	if (LEMON_NETWORKING_ENABLED)
	{
		randinit();
	}

	return;
}


bool openServer(const char ip[], Uint16 portNumber, World *GameWorld)
{	
	if (Networking.connectMode == SERVER)
	{
		putConsole("Server already running!");
		return false;
	}

	if (Networking.connectMode == CLIENT)
	{
		putConsole("Currently connected to server; disconnect before attempting to create own server.");
		return false;
	}

	if (ip == NULL || ip[0] == '\0' || strcmp(ip, "NULL") == 0)
	{
		Networking.serverAddress = NULL;
		Networking.connectionStatus = CONNECT_STATE_CREATING_SOCKET;
	}
	else
	{
		Networking.serverAddress = NET_ResolveHostname(ip);
		Networking.connectionStatus = CONNECT_STATE_RESOLVING_ADDRESS;
	}

	Networking.connectMode = SERVER;
	Networking.serverPort = portNumber;
	Networking.timeElapsed = 0.0;
	memset(Networking.serverPassword, 0, MAX_LEN);

	setConsoleVariable(EngineSettings.cheats, "false", GameWorld);

	return true;
}

void closeServer(void)
{
	if (Networking.connectMode != SERVER)
	{
		return;
	}

	Networking.connectMode = OFFLINE;
	Networking.connectionStatus = CONNECT_STATE_DISCONNECTED;

	for (int i = 0; i < MAX_BLOCKED_CLIENTS; i++)
	{
		if (Networking.blockedIPs[i] != NULL)
		{
			NET_UnrefAddress(Networking.blockedIPs[i]);
			Networking.blockedIPs[i] = NULL;
		}
	}

	if (Networking.serverAddress != NULL)
	{
		NET_UnrefAddress(Networking.serverAddress);
		Networking.serverAddress = NULL;
	}

	if (Networking.Server != NULL)
	{
		NET_DestroyServer(Networking.Server);
		Networking.Server = NULL;
	}
	
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (Networking.connectedClients[i] != NULL)
		{
			NET_DestroyStreamSocket(Networking.connectedClients[i]);
			Networking.connectedClients[i] = NULL;
		}
	}
	Networking.clientCount = 0;

	deleteUnownedTrackedObjects();
	resetTrackedObjects();

	return;
}

bool connect(const char ip[], Uint16 portNumber)
{
	if (Networking.connectMode == SERVER)
	{
		putConsole("Server already running!");
		return false;
	}

	if (Networking.connectMode == CLIENT)
	{
		disconnect();
	}

	Networking.serverAddress = NET_ResolveHostname(ip);
	if (Networking.serverAddress == NULL)
	{
		return false;
	}

	Networking.connectMode = CLIENT;
	Networking.connectionStatus = CONNECT_STATE_RESOLVING_ADDRESS;
	Networking.serverPort = portNumber;
	Networking.timeElapsed = 0.0;
	memset(Networking.serverPassword, 0, MAX_LEN);

	return true;
}

void disconnect(void)
{
	if (Networking.connectMode != CLIENT)
	{
		return;
	}

	Networking.connectMode = OFFLINE;
	Networking.connectionStatus = CONNECT_STATE_DISCONNECTED;

	if (Networking.serverAddress != NULL)
	{
		NET_UnrefAddress(Networking.serverAddress);
		Networking.serverAddress = NULL;
	}

	if (Networking.myClient != NULL)
	{
		NET_DestroyStreamSocket(Networking.myClient);
		Networking.myClient = NULL;
	}

	deleteUnownedTrackedObjects();
	resetTrackedObjects();

	return;
}

void disconnectWithMessage(const char message[])
{
	if (message != NULL)
	{
		putConsole("Disconnecting... (%s)", message);
	}
	else
	{
		putConsole("Disconnecting...");
	}

	disconnect();

	return;
}

void cleanUpNetworkData(void)
{
	if (Networking.connectMode == SERVER)
	{
		closeServer();
	}
	else if (Networking.connectMode == CLIENT)
	{
		disconnect();
	}

	return;
}

void resetTrackedObjects(void)
{
	for (int i = 0; i < MAX_TRACKED_OBJECTS; i++)
	{
		Networking.TrackedObjects[i].clientID = NO_CLIENT_ID;
		Networking.TrackedObjects[i].object = NULL;
		Networking.TrackedObjects[i].clientDeleted = false;
		Networking.TrackedIDs[i] = NO_CLIENT_ID;
	}

	Networking.TrackedObjectCount = 0;

	return;
}

void deleteUnownedTrackedObjects(void)	// should be used for clean-up only
{
	if (Networking.connectionStatus == CONNECT_STATE_CONNECTED)
	{
		return;
	}

	TrackedObject *list = Networking.TrackedObjects;

	for (int i = 0; i < MAX_TRACKED_OBJECTS && list[i].clientID != NO_CLIENT_ID; i++)
	{
		if (list[i].clientID == Networking.clientID || list[i].clientID == NO_CLIENT_ID)
		{
			continue;
		}

		MarkObjectInstanceForDeletion(list[i].object, list[i].instance);

		list[i].object = NULL;
	}

	return;
}

void deleteTrackedObjectsOwnedBy(int ownerClientID)
{
	TrackedObject *list = Networking.TrackedObjects;

	for (int i = 0; i < MAX_TRACKED_OBJECTS; i++)
	{
		if (list[i].clientID != ownerClientID)
		{
			continue;
		}

		removeTrackedObject(i);
	}


	return;
}

void removeDisconnectedClient(int index, const char reason[])
{
	if (!inRange(index, 0, MAX_CLIENTS - 1) || Networking.connectMode == OFFLINE)
	{
		return;
	}

	if (Networking.connectMode == SERVER)
	{
		Networking.clientStates[index] = CLIENT_STATE_LEAVING;
		Networking.clientTimers[index] = 0.0;

		// tell other clients that this client has left the game
		NetworkPacket packet = {0};
		packet.type = PACKET_CLIENT_DISCONNECTED;
		packet.tickSent = TickNumber();
		packet.data.disconnect.clientID = index;
		strcpy(packet.data.disconnect.reason, reason);

		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			if (Networking.connectedClients[i] != NULL)
			{
				NET_WriteToStreamSocket(Networking.connectedClients[i], &packet, sizeof(NetworkPacket));
				outgoing += sizeof(NetworkPacket);
			}
		}

		deleteTrackedObjectsOwnedBy(index);
	}

	const char *name = getClientUsername(index);

	if (name == NULL || name[0] == '\0')
	{
		return;
	}

	if (reason != NULL)
	{
		putConsole("%s left the game (%s)", name, reason);
	}
	else
	{
		putConsole("%s left the game", name);
	}

	memset(Networking.clientUsernames[index], 0, MAX_LEN);

	return;
}


void ServerOpened(World *GameWorld)
{

	return;
}

void setServerPassword(const char input[])
{
	if (input == NULL)
	{
		return;
	}

	if (strlen(input) < 1 || strlen(input) >= MAX_LEN)
	{
		putConsoleError("Password must be between 1 and %d characters long!", MAX_LEN);
	}

	LemonStrncpy(Networking.serverPassword, input, MAX_LEN);


	return;
}

void NewClientJoinedServer(int clientID)
{
	putConsole("%s joined the game", getClientUsername(clientID));
	
	return;
}

void JoinedServer(World *GameWorld)
{

	return;
}

const char* getClientUsername(int clientID)
{
	if (clientID == SERVER_CLIENT_ID)
	{
		return Networking.serverUsername;
	}

	if (clientID < 0)
	{
		return NULL;
	}

	return Networking.clientUsernames[clientID];
}

void setClientUsername(int clientID, const char username[])
{
	if (clientID == SERVER_CLIENT_ID)
	{
		LemonStrncpy(Networking.serverUsername, username, MAX_LEN);
	}
	else if (clientID >= 0)
	{
		LemonStrncpy(Networking.clientUsernames[clientID], username, MAX_LEN);
	}

	if (clientID == Networking.clientID)
	{
		LemonStrncpy(Networking.myUsername, username, MAX_LEN);
	}

	return;
}

void setMyUsername(const char newUsername[])
{
	setUsername(newUsername, Networking.clientID);
}

void setUsername(const char newUsername[], int clientID)
{
	if (Networking.connectMode != SERVER && clientID != Networking.clientID)
	{
		return;
	}

	setClientUsername(clientID, newUsername);

	if (Networking.connectMode == OFFLINE)
	{
		return;
	}
	
	NetworkPacket packet;
	packet.type = PACKET_CLIENT_NAME;
	packet.tickSent = TickNumber();
	packet.data.clientInfo.clientID = Networking.clientID;
	strcpy(packet.data.clientInfo.clientUsername, getClientUsername(Networking.clientID));

	if (Networking.connectMode == CLIENT)
	{
		NET_WriteToStreamSocket(Networking.myClient, &packet, sizeof(NetworkPacket));
	}
	else if (Networking.connectMode == SERVER)
	{
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			if (Networking.connectedClients[i] != NULL)
			{
				NET_WriteToStreamSocket(Networking.connectedClients[i], &packet, sizeof(NetworkPacket));
			}
		}
	}

	return;
}

void KickClient(const char username[])
{
	if (Networking.connectMode != SERVER)
	{
		return;
	}

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (Networking.connectedClients[i] != NULL && strcmp(Networking.clientUsernames[i], username) == 0)
		{
			NetworkPacket packet = {0};
			packet.type = PACKET_CLIENT_DISCONNECTED;
			packet.tickSent = TickNumber();
			packet.data.disconnect.clientID = i;
			strcpy(packet.data.disconnect.reason, "Kicked");
			NET_WriteToStreamSocket(Networking.connectedClients[i], &packet, sizeof(NetworkPacket));

			// save IP to block it from reconnecting
			// Networking.blockedIndex = abs(Networking.blockedIndex) % MAX_BLOCKED_CLIENTS; 
			// Networking.blockedIPs[Networking.blockedIndex] = NET_GetStreamSocketAddress(Networking.connectedClients[i]);
			// Networking.blockedIndex = (Networking.blockedIndex + 1) % MAX_BLOCKED_CLIENTS;

			removeDisconnectedClient(i, "Kicked");

			return;
		}
	}

	putConsole("Couldn't find '%s'", username);

	return;
}


void updateNetworking(World *GameWorld)
{
	if (!LEMON_NETWORKING_ENABLED || Networking.connectMode == OFFLINE)
	{
		return;
	}

	outgoing = 0;

	if (Networking.connectionStatus == CONNECT_STATE_RESOLVING_ADDRESS)
	{
		attemptResolveAddress();
	}
	
	if (Networking.connectionStatus == CONNECT_STATE_CREATING_SOCKET)
	{
		if (Networking.connectMode == SERVER)
		{
			attemptServerHost(GameWorld);
		}
		else if (Networking.connectMode == CLIENT)
		{
			attemptClientConnect(GameWorld);	
		}
	}

	if (Networking.connectionStatus == CONNECT_STATE_CONNECTED)
	{
		Networking.timeElapsed += deltaTime;

		if (Networking.timeElapsed < ConVarAsFloat(Networking.updateRate))
		{
			return;
		}

		if (Networking.connectMode == SERVER)
		{
			updateServer(GameWorld);
		}
		else if (Networking.connectMode == CLIENT)
		{
			updateClient(GameWorld);
		}

		Networking.timeElapsed = 0.0;
	}

	//putConsole("Outgoing: %d bytes / %.2f kb", outgoing, (float)outgoing / 1024.0);

	return;
}


void attemptResolveAddress(void)
{
	NET_Status status = NET_GetAddressStatus(Networking.serverAddress);

	if (status == NET_SUCCESS)
	{
		Networking.connectionStatus = CONNECT_STATE_CREATING_SOCKET;
		return;
	}

	Networking.timeElapsed += deltaTime;
								 
	if (Networking.timeElapsed > ConVarAsFloat(Networking.connectionTimeout) || status == NET_FAILURE)
	{
		NET_UnrefAddress(Networking.serverAddress);
		Networking.serverAddress = NULL;
		Networking.connectionStatus = CONNECT_STATE_DISCONNECTED;
		Networking.connectMode = OFFLINE;

		putConsoleError("Failed to find address after %f seconds", Networking.timeElapsed);

		return;
	}
	
	return;
}

void attemptServerHost(World *GameWorld)
{
	if (Networking.Server == NULL)
	{
		Networking.Server = NET_CreateServer(Networking.serverAddress, Networking.serverPort, 0);
	}

	if (Networking.Server == NULL)
	{
		closeServer();
		putConsoleError("Failed to open server on port %d", Networking.serverPort);
		return;
	}

	Networking.connectionStatus = CONNECT_STATE_CONNECTED;
	Networking.timeElapsed = 0.0;
	Networking.clientID = SERVER_CLIENT_ID;
	LemonStrncpy(Networking.serverUsername, Networking.myUsername, MAX_LEN);
	putConsole("Server online!");

	loadLevel(GameWorld, GameWorld->level);

	ServerOpened(GameWorld);

	return;
}

void attemptClientConnect(World *GameWorld)
{
	if (Networking.myClient == NULL)
	{
		Networking.myClient = NET_CreateClient(Networking.serverAddress, Networking.serverPort, 0);
		Networking.timeElapsed = 0.0;
	}
	else
	{
		NET_Status status = NET_GetConnectionStatus(Networking.myClient);
		Networking.timeElapsed += deltaTime;

		if (status == NET_SUCCESS)
		{
			Networking.connectionStatus = CONNECT_STATE_CONNECTED;
			Networking.timeElapsed = 0.0;
			Networking.clientID = AWAITING_CLIENT_ID;
			putConsole("Connection established to server");

			return;
		}

		if (status == NET_FAILURE || Networking.timeElapsed > ConVarAsFloat(Networking.connectionTimeout))
		{
			disconnect();
			putConsoleError("Server not found!");
		
			return;
		}
	}

	return;
}

void sendServerPassword(const char password[])
{
	if (Networking.connectMode != CLIENT)
	{
		return;
	}

	if (Networking.myClient == NULL)
	{
		LemonStrncpy(Networking.serverPassword, password, MAX_LEN);
		return;
	}

	NetworkPacket clientEntrance = {0};
	clientEntrance.type = PACKET_CLIENT_ENTRANCE;
	clientEntrance.tickSent = TickNumber();
	strcpy(clientEntrance.data.clientInfo.clientUsername, getClientUsername(Networking.clientID));
	LemonStrncpy(clientEntrance.data.clientInfo.password, password, MAX_LEN);

	NET_WriteToStreamSocket(Networking.myClient, &clientEntrance, sizeof(NetworkPacket));
	outgoing += sizeof(NetworkPacket);

	return;
}

void updateServer(World *GameWorld)
{
	if (Networking.clientCount < MAX_CLIENTS)
	{
		acceptClients(GameWorld);
	}

	int count = Networking.clientCount;

	for (int i = 0; i < MAX_CLIENTS && count > 0; i++)
	{
		if (Networking.connectedClients[i] != NULL)
		{
			if (Networking.clientStates[i] == CLIENT_STATE_JOINED)
			{
				receiveClientData(i, GameWorld);
			}
			else
			{
				handlePendingClient(i, GameWorld);
			}
		
			count--;
		}
	}

	// calculate new state of server from received inputs

	count = Networking.clientCount;
	for (int i = 0; i < MAX_CLIENTS && count > 0; i++)
	{
		if (Networking.connectedClients[i] != NULL)
		{
			sendGameEventPackets(i);
			sendTrackedObjectPacketsToClient(i, GameWorld);
			count--;
		}
	}

	deleteAllGameEvents(&Networking.NetworkEvents);

	return;
}


void updateClient(World *GameWorld)
{
	sendGameEventPackets(SERVER_CLIENT_ID);
	deleteAllGameEvents(&Networking.NetworkEvents);

	sendTrackedObjectPacketsToServer(GameWorld);


	receiveServerData(GameWorld);

	// adjust result to match server

	return;
}

void handlePendingClient(int clientID, World *GameWorld)
{	
	if (Networking.clientTimers[clientID] > ConVarAsFloat(Networking.connectionTimeout) || Networking.clientStates[clientID] == CLIENT_STATE_DISCONNECTED)
	{
		Networking.clientStates[clientID] = CLIENT_STATE_DISCONNECTED;
		NET_DestroyStreamSocket(Networking.connectedClients[clientID]);
		Networking.connectedClients[clientID] = NULL;
		Networking.clientCount--;
		putConsole("Removing client %d", clientID);

		return;
	}

	Networking.clientTimers[clientID] += Networking.timeElapsed;

	if (Networking.clientStates[clientID] != CLIENT_STATE_PENDING)
	{
		return;
	}

	NetworkPacket packet = {0};
	NET_StreamSocket *socket = Networking.connectedClients[clientID];
	int bytes = NET_ReadFromStreamSocket(socket, &packet, sizeof(NetworkPacket));
	int packetsRead = 0;	// don't let a client throttle the server by sending too many packets at once

	while (bytes > 0 && packetsRead < MAX_PACKETS_READ)
	{
		packetsRead++;

		if (packet.type == PACKET_CLIENT_ENTRANCE)
		{
			processClientEntrancePacket(&packet, clientID, GameWorld);

			return;
		}

		bytes = NET_ReadFromStreamSocket(socket, &packet, sizeof(NetworkPacket));
	}

	return;
}

void processClientEntrancePacket(NetworkPacket *packet, int clientID, World *GameWorld)
{
	NET_StreamSocket *socket = Networking.connectedClients[clientID];

	if (Networking.serverPassword[0] != '\0' && strcmp(Networking.serverPassword, packet->data.clientInfo.password))
	{
		removeDisconnectedClient(clientID, "Incorrect password");
	}
	else
	{
		Networking.clientStates[clientID] = CLIENT_STATE_JOINED;
		setClientUsername(clientID, packet->data.clientInfo.clientUsername);
		NewClientJoinedServer(clientID);

		NetworkPacket response = {0};
		response.type = PACKET_CLIENT_WELCOME;
		response.tickSent = TickNumber();
		response.data.welcome.level = GameWorld->level;
		NET_WriteToStreamSocket(socket, &response, sizeof(NetworkPacket));
		outgoing += sizeof(NetworkPacket);


		// send all currently joined clients
		sendAllJoinedClients(socket);

		// send all currently tracked objects 
		sendAllTrackedObjects(socket);

		// send all variables that should be replicated on the client
		sendAllServerConVars(socket);

		// tell other clients that this client is joining
		response.type = PACKET_CLIENT_JOINED;
		response.data.clientInfo.clientID = clientID;
		strcpy(response.data.clientInfo.clientUsername, getClientUsername(clientID));

		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			if (Networking.connectedClients[i] != NULL && i != clientID)
			{
				NET_WriteToStreamSocket(Networking.connectedClients[i], &response, sizeof(NetworkPacket));
				outgoing += sizeof(NetworkPacket);
			}
		}
	}

	return;
}

void sendAllJoinedClients(NET_StreamSocket *socket)
{
	if (Networking.connectMode != SERVER || socket == NULL)
	{
		return;
	}

	NetworkPacket packet = {0};
	packet.tickSent = TickNumber();
	packet.type = PACKET_CLIENT_JOINED;

	packet.data.clientInfo.clientID = SERVER_CLIENT_ID;
	strcpy(packet.data.clientInfo.clientUsername, Networking.serverUsername);
	NET_WriteToStreamSocket(socket, &packet, sizeof(NetworkPacket));
	outgoing += sizeof(NetworkPacket);

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (Networking.connectedClients[i] != NULL && Networking.connectedClients[i] != socket)
		{
			packet.data.clientInfo.clientID = i;
			strcpy(packet.data.clientInfo.clientUsername, Networking.clientUsernames[i]);
			NET_WriteToStreamSocket(socket, &packet, sizeof(NetworkPacket));
			outgoing += sizeof(NetworkPacket);
		}
	}

	return;
}

void sendAllTrackedObjects(NET_StreamSocket *socket)
{
	if (Networking.connectMode != SERVER || socket == NULL)
	{
		return;
	}

	NetworkPacket packet = {0};
	packet.tickSent = TickNumber();
	packet.type = PACKET_OBJECT_ADDED;

	TrackedObject *TrackedObjects = Networking.TrackedObjects;
	Object *object;

	for (int index = 0; index < MAX_TRACKED_OBJECTS; index++)
	{
		if (TrackedObjects[index].clientID == NO_CLIENT_ID)
		{
			continue;
		}

		object = TrackedObjects[index].object;

		// double check object exists before copying data
		if (object == NULL || object->State == EMPTY_OBJECT || TrackedObjects[index].instance != object->instanceNumber)
		{
			packet.data.objectData.object.State = EMPTY_OBJECT;
		}
		else
		{
			copyObjectToPacketData(object, &packet);
		}

		putConsole("Sending %d", index);

		packet.data.objectData.trackedID = index;
		packet.data.objectData.ownerClientID = TrackedObjects[index].clientID;
		NET_WriteToStreamSocket(socket, &packet, sizeof(NetworkPacket));
		outgoing += sizeof(NetworkPacket);
	}

	return;
}

void sendAllServerConVars(NET_StreamSocket *socket)
{
	if (Networking.connectMode != SERVER || socket == NULL)
	{
		return;
	}

	NetworkPacket Packet = {0};
	Packet.type = PACKET_CONSOLE_VARIABLE;
	Packet.tickSent = TickNumber();

	ConsoleVariable *list = DebugSettings.consoleVariables;

	for (int i = 0; i < MAX_CONSOLE_VARIABLES; i++)
	{
		if (list[i].name[0] != '\0' && (list[i].flags & CONFLAG_SVR_AND_PRO) == CONFLAG_SERVER_SIDE)
		{
			memcpy(&Packet.data.convar, &list[i], sizeof(ConsoleVariable));

			NET_WriteToStreamSocket(socket, &Packet, sizeof(NetworkPacket));
			outgoing += sizeof(NetworkPacket);
		}
	}
	
	return;
}

Uint64 getRandom64Bits()
{
	static Uint64 seed = 8291474;

	seed = (((seed + 841837) % 1343116) << 7) + (seed / 17);

	return seed;
}

bool receiveClientData(int index, World *GameWorld)
{
	NetworkPacket buffer = {0};
	NET_StreamSocket *socket = Networking.connectedClients[index];
	int bytes = NET_ReadFromStreamSocket(socket, &buffer, sizeof(NetworkPacket));
	int packetsRead = 0;	// don't let a client throttle the server by sending too many packets at once

	while (bytes > 0 && packetsRead < MAX_PACKETS_READ)
	{
		packetsRead++;

		switch (buffer.type)
		{
		case PACKET_CLIENT_DISCONNECTED:
			{
				removeDisconnectedClient(index, buffer.data.disconnect.reason);

				return false;
			} break;

		case PACKET_GAME_EVENT:
			{
				GameEvent event = {0};
				memcpy(&event, &buffer.data, sizeof(PacketData));
				NET_ReadFromStreamSocket(socket, ((Uint8 *)&event) + sizeof(PacketData), sizeof(GameEvent) - sizeof(PacketData));

				int permission = eventNetworkingPermission(event.EventID);

				if (event.clientID != index || permission == LOCAL_CLIENT_ID)
				{
					break;
				}
		
				triggerGameEvent(&event, GameWorld);
			} break;

		case PACKET_OBJECT_REQUEST_ADD:
			{
				RespondToTrackObjectRequest(&buffer, index, GameWorld);
			} break;

		case PACKET_OBJECT_REQUEST_DELETE:
			{
				RespondToDeleteObjectRequest(&buffer, index, GameWorld);
			} break;

		case PACKET_OBJECT_UPDATE:
			{
				updateTrackedObject(&buffer, GameWorld);
			} break;

		case PACKET_HEALTH_UPDATE:
			{
				updateTrackedObjectHealth(&buffer, GameWorld);
			} break;

		case PACKET_CLIENT_NAME:
			{
				setUsername(buffer.data.clientInfo.clientUsername, index);
			} break;

		default:
			break;
		}


		bytes = NET_ReadFromStreamSocket(socket, &buffer, sizeof(NetworkPacket));
	}

	if (bytes < 0)
	{
		removeDisconnectedClient(index, "Connection timed out");

		return false;
	}

	return true;
}


bool receiveServerData(World *GameWorld)
{
	NetworkPacket buffer = {0};

	int bytes = NET_ReadFromStreamSocket(Networking.myClient, &buffer, sizeof(NetworkPacket));

	while (bytes > 0) 
	{
		switch (buffer.type)
		{
		case PACKET_SERVER_SETUP:
			{
				if (!processSetupPacket(&buffer, GameWorld))
				{
					return false;
				}
			} break;

		case PACKET_CLIENT_WELCOME:
			{
				loadLevel(GameWorld, buffer.data.welcome.level);
				JoinedServer(GameWorld);
			} break;
	
		case PACKET_CLIENT_JOINED:
			{
				setClientUsername(buffer.data.clientInfo.clientID, buffer.data.clientInfo.clientUsername);
				NewClientJoinedServer(buffer.data.clientInfo.clientID);
			} break;

		case PACKET_CLIENT_DISCONNECTED:
			{
				if (buffer.data.disconnect.clientID == Networking.clientID)
				{
					disconnectWithMessage(buffer.data.disconnect.reason);
					return false;
				}
				else
				{
					removeDisconnectedClient(buffer.data.disconnect.clientID, buffer.data.disconnect.reason);
				}
			} break;

		case PACKET_CLIENT_NAME:
			{
				setClientUsername(buffer.data.clientInfo.clientID, buffer.data.clientInfo.clientUsername);
			} break;

		case PACKET_OBJECT_RESPONSE_ADD:
			{
				processTrackedObjectResponse(&buffer);
			} break;

		case PACKET_OBJECT_ADDED:
			{
				processTrackedObjectAdded(&buffer, GameWorld);
			} break;

		case PACKET_OBJECT_UPDATE:
			{
				updateTrackedObject(&buffer, GameWorld);
			} break;

		case PACKET_HEALTH_UPDATE:
			{
				updateTrackedObjectHealth(&buffer, GameWorld);
			} break;

		case PACKET_OBJECT_DELETED:
			{
				int deletedID = buffer.data.deletion.deletedID;

				processTrackedObjectDeleted(deletedID);
			} break;

		case PACKET_OBJECT_REJECT_DELETE:
			{
				int deletedID = buffer.data.objectData.trackedID;
				int clientID = buffer.data.objectData.ownerClientID;

				int index = Networking.TrackedIDs[deletedID];

				Networking.TrackedObjects[index].clientID = clientID;
				Networking.TrackedObjects[index].clientDeleted = false;

				updateTrackedObject(&buffer, GameWorld);
			} break;

		case PACKET_GAME_EVENT:
			{
				GameEvent event = {0};
				memcpy(&event, &buffer.data, sizeof(PacketData));
				NET_ReadFromStreamSocket(Networking.myClient, ((Uint8 *)&event) + sizeof(PacketData), sizeof(GameEvent) - sizeof(PacketData));

				// ensure that the received game event isn't only meant to be run locally
				int permission = eventNetworkingPermission(event.EventID);
				if (permission == LOCAL_CLIENT_ID)
				{
					break;
				}
				
				// If the server has sent an event with your own ID, then this is a response to an earlier request to trigger this event;
				// the request should not be sent again so in order to ensure it is not sent a second time to the server its ID is changed 
				// to SERVER_CLIENT_ID to pretend it actually belongs to the server (technically it does)
				if (event.clientID == Networking.clientID)	
				{
					event.clientID = SERVER_CLIENT_ID;
				}

				triggerGameEvent(&event, GameWorld);
			} break;

		case PACKET_CONSOLE_VARIABLE:
			{
				ConsoleVariable *received = &buffer.data.convar;
				ConsoleVariable *found = getConsoleVariable(received->name);
				if (found == NULL || (found->flags & CONFLAG_SERVER_SIDE) == 0)
				{
					break;
				}
				found->flags &= ~CONFLAG_SERVER_SIDE;
				char number[32] = {0};
				setConsoleVariable(found, ConVarValueAsString(received, number), GameWorld);
				found->flags |= CONFLAG_SERVER_SIDE;
			} break;

		case PACKET_CONSOLE_COMMAND:
			{
				ConsoleCommand *received = &buffer.data.command;
				ConsoleCommand *found = getConsoleCommand(received->name);

				if (found == NULL || (found->flags & CONFLAG_SERVER_SIDE) == 0)
				{
					break;
				}

				DebugSettings.argIndex = 0;
				found->function(received->helpString, GameWorld);
			} break;

		case PACKET_GAME_FLAG:
			{
				if (getGameFlag(buffer.data.flag.name) < 0)
				{
					addGameFlag(buffer.data.flag.name, buffer.data.flag.value);
				}
				else
				{
					setGameFlag(buffer.data.flag.name, buffer.data.flag.value);
				}
			} break;

		default:
			break;
		}

		bytes = NET_ReadFromStreamSocket(Networking.myClient, &buffer, sizeof(NetworkPacket));
	} 

	if (bytes < 0)
	{
		disconnectWithMessage("Connection dropped");

		return false;
	}

	return true;
}


bool clientPermitted(NET_StreamSocket *inputSocket)
{
	// check if IP is blacklisted

	bool accepted = true;
	NET_Address *IP = NET_GetStreamSocketAddress(inputSocket);

	for (int i = 0; i < MAX_BLOCKED_CLIENTS; i++)
	{
		if (Networking.blockedIPs[i] != NULL && NET_CompareAddresses(IP, Networking.blockedIPs[i]) == 0)
		{
			accepted = false;
		}
	}

	NET_UnrefAddress(IP);

	return accepted;
}

void acceptClients(World *GameWorld)
{
	int client = 0;
	NET_StreamSocket *newSocket = NULL;
	bool result;

	if (Networking.clientCount >= MAX_CLIENTS)
	{
		client = MAX_CLIENTS;
	}

	while (true)
	{
		result = NET_AcceptClient(Networking.Server, &newSocket);

		if (newSocket == NULL || !result)
		{
			// no pending connections or error
			return;
		}

		while (Networking.connectedClients[client] != NULL && client < MAX_CLIENTS)
		{
			client++;
		}
		

		if (!clientPermitted(newSocket) || NET_GetConnectionStatus(newSocket) != NET_SUCCESS)
		{
			// no empty slots found?
			NET_DestroyStreamSocket(newSocket);
		}
		else
		{
			Networking.connectedClients[client] = newSocket;
			Networking.clientStates[client] = CLIENT_STATE_PENDING;
			Networking.clientTimers[client] = 0.0;

			Networking.clientCount++;

			NetworkPacket packet = {0};

			// send set-up packet for server settings
			packet.type = PACKET_SERVER_SETUP;
			packet.tickSent = TickNumber();

			strcpy(packet.data.setup.setupString, Networking.setUpString);

			packet.data.setup.assignedClientID = client;
			packet.data.setup.settings.tickRate = EngineSettings.GameTicksPerSecond;
			packet.data.setup.settings.trackedObjectCapacity = MAX_TRACKED_OBJECTS;
			packet.data.setup.settings.WorldBoundX = EngineSettings.WorldBoundX;
			packet.data.setup.settings.WorldBoundY = EngineSettings.WorldBoundY;
			packet.data.setup.passwordRequired = (Networking.serverPassword[0] != '\0');
			
			NET_WriteToStreamSocket(newSocket, &packet, sizeof(NetworkPacket));
			outgoing += sizeof(NetworkPacket);
		}
	}

	return;
}


bool processSetupPacket(NetworkPacket *packet, World *GameWorld)
{
	if (Networking.connectMode != CLIENT || packet == NULL)	// only clients should accept set-up packets
	{
		return false;
	}

	ServerSetup *setup = &packet->data.setup;

	if (strcmp(setup->setupString, Networking.setUpString))
	{
		// setup is not what we expected, likely incompatible
		disconnectWithMessage("Incompatible server");

		return false;
	}

	processSettingsPacket(packet);

	if (Networking.connectMode == OFFLINE)
	{
		return false;
	}

	// get clientID; server is telling the client which ID they are
	Networking.clientID = setup->assignedClientID;

	setClientUsername(Networking.clientID, Networking.myUsername);

	if (packet->data.setup.passwordRequired && Networking.serverPassword[0] < 32)
	{
		putConsole("Server waiting for password...");
		return true;
	}

	// tell server your username (and give password if necessary)
	NetworkPacket clientEntrance = {0};
	clientEntrance.type = PACKET_CLIENT_ENTRANCE;
	clientEntrance.tickSent = TickNumber();
	strcpy(clientEntrance.data.clientInfo.clientUsername, getClientUsername(Networking.clientID));
	LemonStrncpy(clientEntrance.data.clientInfo.password, Networking.serverPassword, MAX_LEN);

	NET_WriteToStreamSocket(Networking.myClient, &clientEntrance, sizeof(NetworkPacket));
	outgoing += sizeof(NetworkPacket);

	return true;
}


void processSettingsPacket(NetworkPacket *packet)
{
	if (Networking.connectMode != CLIENT || packet == NULL)	// only clients should accept set-up packets
	{
		return;
	}

	ServerSettings *settings = &packet->data.setup.settings;

	// if (packet->type == PACKET_SERVER_SETUP)
	// {
	// 	settings = &packet->data.setup.settings;
	// }
	// else
	// {
	// 	settings = &packet->data.settings;
	// }

	if (settings->trackedObjectCapacity != MAX_TRACKED_OBJECTS)
	{
		// value is not what we expected, likely incompatible
		disconnectWithMessage("Incompatible server (Settings)");

		return;
	}

	// match tickrate
	Networking.connectMode = OFFLINE;
	setTickRate(settings->tickRate);
	setTickNumber(packet->tickSent + 1);
	Networking.connectMode = CLIENT;

	//EngineSettings.cheats = settings->cheatsVal;
	EngineSettings.WorldBoundX = settings->WorldBoundX;
	EngineSettings.WorldBoundY = settings->WorldBoundY;

	return;
}

// void updateServerSettings(void)
// {
// 	if (Networking.connectMode != SERVER)
// 	{
// 		return;
// 	}

// 	NetworkPacket Packet = {0};
// 	Packet.type = PACKET_SERVER_SETTINGS;
// 	Packet.tickSent = TickNumber();

// 	Packet.data.settings.trackedObjectCapacity = MAX_TRACKED_OBJECTS;
// 	Packet.data.settings.tickRate = EngineSettings.GameTicksPerSecond;
// 	//Packet.data.settings.cheatsVal = EngineSettings.cheats;
// 	Packet.data.settings.WorldBoundX = EngineSettings.WorldBoundX;
// 	Packet.data.settings.WorldBoundY = EngineSettings.WorldBoundY;


// 	for (int i = 0; i < MAX_CLIENTS; i++)
// 	{
// 		if (Networking.connectedClients[i] != NULL)
// 		{
// 			Packet.data.setup.assignedClientID = i;
// 			NET_WriteToStreamSocket(Networking.connectedClients[i], &Packet, sizeof(NetworkPacket));
// 			outgoing += sizeof(NetworkPacket);
// 		}
// 	}
	

// 	return;
// }

void updateServerConVar(ConsoleVariable *input)
{
	if (Networking.connectMode != SERVER || input == NULL || (input->flags & CONFLAG_SERVER_SIDE) == 0)
	{
		return;
	}

	NetworkPacket Packet = {0};
	Packet.type = PACKET_CONSOLE_VARIABLE;
	Packet.tickSent = TickNumber();

	memcpy(&Packet.data.convar, input, sizeof(ConsoleVariable));

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (Networking.connectedClients[i] != NULL)
		{
			NET_WriteToStreamSocket(Networking.connectedClients[i], &Packet, sizeof(NetworkPacket));
			outgoing += sizeof(NetworkPacket);
		}
	}

	return;
}

void sendServerCommand(ConsoleCommand *input, char consoleInput[USER_INPUT_MAX_LEN], int argStartIndex)
{
	if (Networking.connectMode != SERVER || input == NULL)
	{
		return;
	}

	// Server-Side commands can be run by the server but clients must be told by the server to run them
	// protected commands can be run by the server but should NOT be broadcasted to client machines (its info is sensitive)
	if ((input->flags & CONFLAG_PROTECTED) != 0) 
	{
		return;
	}

	consoleInput[USER_INPUT_MAX_LEN - 1] = '\0';
	NetworkPacket Packet = {0};
	Packet.type = PACKET_CONSOLE_COMMAND;
	Packet.tickSent = TickNumber();

	memcpy(&Packet.data.command, input, sizeof(ConsoleVariable));

	LemonStrncpy(Packet.data.command.helpString, consoleInput + argStartIndex, CONSOLE_HELP_MAX_LEN);

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (Networking.connectedClients[i] != NULL)
		{
			NET_WriteToStreamSocket(Networking.connectedClients[i], &Packet, sizeof(NetworkPacket));
			outgoing += sizeof(NetworkPacket);
		}
	}

	return;
}

void updateServerFlag(GameFlag *input)
{
	if (Networking.connectMode != SERVER)
	{
		return;
	}

	NetworkPacket Packet = {0};
	Packet.type = PACKET_GAME_FLAG;
	Packet.tickSent = TickNumber();

	strcpy(Packet.data.flag.name, input->name);
	Packet.data.flag.value = input->value;


	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (Networking.connectedClients[i] != NULL)
		{
			Packet.data.setup.assignedClientID = i;
			NET_WriteToStreamSocket(Networking.connectedClients[i], &Packet, sizeof(NetworkPacket));
			outgoing += sizeof(NetworkPacket);
		}
	}
	

	return;
}


int TrackObjectOverNetwork(Object *input)
{
	if (Networking.connectMode == OFFLINE)
	{
		return ACTION_DISABLED;
	}

	int index = addNewTrackedObject(input, Networking.clientID);

	if (index < 0)
	{
		return LEMON_ERROR;
	}

	NetworkPacket buffer = {0};

	buffer.tickSent = TickNumber();
	copyObjectToPacketData(input, &buffer);
	buffer.data.objectData.ownerClientID = Networking.clientID;
	buffer.data.objectData.trackedID = index;

	if (Networking.connectMode == SERVER)
	{
		int count = Networking.clientCount;
		buffer.type = PACKET_OBJECT_ADDED;

		for (int i = 0; i < MAX_CLIENTS && count > 0; i++)
		{
			if (Networking.connectedClients[i] != NULL)
			{
				NET_WriteToStreamSocket(Networking.connectedClients[i], &buffer, sizeof(NetworkPacket));
				outgoing += sizeof(NetworkPacket);
				count--;
			}
		}
	}
	else if (Networking.connectMode == CLIENT)
	{
		buffer.type = PACKET_OBJECT_REQUEST_ADD;
		
		NET_WriteToStreamSocket(Networking.myClient, &buffer, sizeof(NetworkPacket));
		outgoing += sizeof(NetworkPacket);
	}
	
	return LEMON_SUCCESS;
}

int RespondToTrackObjectRequest(NetworkPacket *packet, int clientIndex, World *GameWorld)
{
	if (Networking.connectMode != SERVER)
	{
		return ACTION_DISABLED;
	}

	NET_StreamSocket *ownerSocket = Networking.connectedClients[clientIndex];
	if (ownerSocket == NULL || packet == NULL)
	{
		return MISSING_DATA;
	}


	// for now, always accept if there is capacity

	// attempt to add newly tracked object
	Object *sentObject = &packet->data.objectData.object;
	Object *newObject = NULL;

	if (sentObject->State != EMPTY_OBJECT)
	{
		newObject = AddNamedObject(GameWorld, sentObject->name, sentObject->ObjectID, 0, 0);
		copyPacketDataToObject(newObject, packet);
	}

	int trackedID = addNewTrackedObject(newObject, clientIndex);

	// send response
	NetworkPacket reponsePacket = {0};
	reponsePacket.type = PACKET_OBJECT_RESPONSE_ADD;
	reponsePacket.tickSent = TickNumber();
	reponsePacket.data.response.clientTrackedID = packet->data.objectData.trackedID;
	reponsePacket.data.response.serverTrackedID = trackedID;	// if trackedID is < 0 it's a rejection, otherwise it's an acceptance

	NET_WriteToStreamSocket(ownerSocket, &reponsePacket, sizeof(NetworkPacket));
	outgoing += sizeof(NetworkPacket);

	// if trackedID is positive, the object was added successfully and all clients except requesting one should have it added
	if (trackedID < 0)	
	{
		return LEMON_ERROR;
	}

	packet->type = PACKET_OBJECT_ADDED;
	packet->tickSent = TickNumber();
	packet->data.objectData.trackedID = trackedID;

	int count = Networking.clientCount;
	for (int i = 0; i < MAX_CLIENTS && count > 1; i++)
	{
		if (i != clientIndex && Networking.connectedClients[i] != NULL)
		{
			NET_WriteToStreamSocket(Networking.connectedClients[i], packet, sizeof(NetworkPacket));
			outgoing += sizeof(NetworkPacket);
			count--;
		}
	}

	return LEMON_SUCCESS;
}


int addNewTrackedObject(Object *input, int owner)
{
	if (Networking.clientID == AWAITING_CLIENT_ID || Networking.clientID == NO_CLIENT_ID || input == NULL)
	{
		return -1;
	}

	TrackedObject *TrackedObjects = Networking.TrackedObjects;

	int index = 0;
	while (TrackedObjects[index].clientID != NO_CLIENT_ID && index < 1)
	{
		if (TrackedObjects[index].object == input && TrackedObjects[index].instance == input->instanceNumber)
		{
			// Object is already being tracked
			return -1;
		}

		index++;
	}
		
	if (index >= MAX_TRACKED_OBJECTS)
	{
		return -1;
	}
			
	TrackedObjects[index].clientID = owner;
	TrackedObjects[index].clientDeleted = false;

	if (Networking.clientID == SERVER_CLIENT_ID)
	{
		TrackedObjects[index].trackedID = index;
	}
	else
	{
		TrackedObjects[index].trackedID = AWAITING_TRACKED_ID;
	}

	TrackedObjects[index].object = input;
	
	TrackedObjects[index].instance = input->instanceNumber;

	Networking.TrackedObjectCount++;

	return index;
}

void processTrackedObjectResponse(NetworkPacket *packet)
{
	if (packet == NULL)
	{
		return;
	}

	int clientTrackedID = packet->data.response.clientTrackedID;
	int serverTrackedID = packet->data.response.serverTrackedID;

	if (clientTrackedID < 0 || clientTrackedID >= MAX_TRACKED_OBJECTS || serverTrackedID >= MAX_TRACKED_OBJECTS)
	{
		return;
	}

	TrackedObject *TrackedObjects = Networking.TrackedObjects;

	if (TrackedObjects[clientTrackedID].clientID == NO_CLIENT_ID)
	{
		return;
	}

	if (serverTrackedID < 0)
	{
		// rejection

		TrackedObjects[clientTrackedID].clientID = NO_CLIENT_ID;
		TrackedObjects[clientTrackedID].object = NULL;
	}
	else
	{
		// acceptance

		TrackedObjects[clientTrackedID].trackedID = serverTrackedID;
		Networking.TrackedIDs[serverTrackedID] = clientTrackedID;

		// if object has since been deleted or replaced, set to NULL so that the next update can re-create this object
		Object *object = TrackedObjects[clientTrackedID].object;
		int instance = TrackedObjects[clientTrackedID].instance;
		
		if (object != NULL && (object->State == EMPTY_OBJECT || object->instanceNumber != instance))
		{
			TrackedObjects[clientTrackedID].object = NULL;
		}
	}
}

void processTrackedObjectAdded(NetworkPacket *packet, World *GameWorld)
{
	if (packet == NULL || Networking.connectMode != CLIENT)
	{
		return;
	}

	Object *addedObject = &packet->data.objectData.object;
	int serverTrackedID = packet->data.objectData.trackedID;
	int owner = packet->data.objectData.ownerClientID;


	Object *newObject = AddNamedObject(GameWorld, addedObject->name, addedObject->ObjectID, 0, 0);

	if (newObject == NULL || serverTrackedID < 0 || serverTrackedID >= MAX_TRACKED_OBJECTS)
	{
		return;
	}

	copyPacketDataToObject(newObject, packet);

	int index = addNewTrackedObject(newObject, owner);

	if (index < 0)
	{
		return;
	}

	Networking.TrackedObjects[index].trackedID = serverTrackedID;
	Networking.TrackedIDs[serverTrackedID] = index;

	return;
}


void removeTrackedObject(int deletedID)
{
	if (deletedID < 0 || deletedID >= MAX_TRACKED_OBJECTS || Networking.connectMode == OFFLINE)
	{
		return;
	}

	TrackedObject *TrackedObjects = Networking.TrackedObjects;

	if (Networking.connectMode == CLIENT)
	{
		if (Networking.TrackedIDs[deletedID] < 0)
		{
			// no server-assigned object in this slot
			return;
		}

		// send deletion request
		NetworkPacket packet;
		packet.type = PACKET_OBJECT_REQUEST_DELETE;
		packet.tickSent = TickNumber();
		packet.data.objectData.trackedID = deletedID;
		packet.data.objectData.ownerClientID = Networking.clientID;
		NET_WriteToStreamSocket(Networking.myClient, &packet, sizeof(NetworkPacket));
		outgoing += sizeof(NetworkPacket);

		TrackedObjects[Networking.TrackedIDs[deletedID]].clientDeleted = true;
		TrackedObjects[Networking.TrackedIDs[deletedID]].object = NULL;

		return;
	}

	Networking.TrackedObjectCount--;

	MarkObjectInstanceForDeletion(TrackedObjects[deletedID].object, TrackedObjects[deletedID].instance);

	TrackedObjects[deletedID].clientID = NO_CLIENT_ID;
	TrackedObjects[deletedID].object = NULL;

	if (Networking.connectionStatus != CONNECT_STATE_CONNECTED)
	{
		return;
	}

	NetworkPacket packet;
	packet.type = PACKET_OBJECT_DELETED;
	packet.tickSent = TickNumber();
	packet.data.deletion.deletedID = deletedID;

	int count = Networking.clientCount;
	for (int i = 0; i < MAX_CLIENTS && count > 0; i++)
	{
		if (Networking.connectedClients[i] != NULL)
		{
			NET_WriteToStreamSocket(Networking.connectedClients[i], &packet, sizeof(NetworkPacket));
			outgoing += sizeof(NetworkPacket);
			count--;
		}
	}

	return;
}

int RespondToDeleteObjectRequest(NetworkPacket *packet, int clientIndex, World *GameWorld)
{
	if (Networking.connectMode != SERVER)
	{
		return ACTION_DISABLED;
	}

	NET_StreamSocket *ownerSocket = Networking.connectedClients[clientIndex];
	if (ownerSocket == NULL || packet == NULL)
	{
		return MISSING_DATA;
	}

	// for now, always accept
	int trackedID = packet->data.objectData.trackedID;

	if (trackedID < 0 || trackedID >= MAX_TRACKED_OBJECTS || Networking.TrackedObjects[trackedID].clientID == NO_CLIENT_ID)
	{
		return INVALID_DATA;
	}

	// reject if object doesn't belong to client (in future it should evaluate somehow whether the client is correct before deciding)
	// however, accept if belongs to server

	int objectOwner = Networking.TrackedObjects[trackedID].clientID;
	if (objectOwner != clientIndex && objectOwner != SERVER_CLIENT_ID)
	{
		NetworkPacket reponsePacket = {0};
		reponsePacket.type = PACKET_OBJECT_REJECT_DELETE;
		reponsePacket.tickSent = TickNumber();
		reponsePacket.data.objectData.ownerClientID = Networking.TrackedObjects[trackedID].clientID;
		reponsePacket.data.objectData.trackedID = trackedID;
		copyObjectToPacketData(Networking.TrackedObjects[trackedID].object, &reponsePacket);

		NET_WriteToStreamSocket(ownerSocket, &reponsePacket, sizeof(NetworkPacket));
		outgoing += sizeof(NetworkPacket);
	}
	else
	{
		removeTrackedObject(trackedID);
	}
	
	return LEMON_SUCCESS;
}

void processTrackedObjectDeleted(int deletedID)
{
	if (deletedID < 0 || deletedID >= MAX_TRACKED_OBJECTS || Networking.connectMode != CLIENT)
	{
		return;
	}

	int index = Networking.TrackedIDs[deletedID];

	if (index < 0)
	{
		return;
	}

	TrackedObject *TrackedObjects = Networking.TrackedObjects;

	Networking.TrackedObjectCount--;
	Networking.TrackedIDs[deletedID] = NO_CLIENT_ID;
	TrackedObjects[index].clientID = NO_CLIENT_ID;

	// only delete if it is the correct instance
	MarkObjectInstanceForDeletion(TrackedObjects[index].object, TrackedObjects[index].instance);

	TrackedObjects[index].object = NULL;

	
	return;
}


TrackedObject* getTrackedObject(int trackedID, int claimedOwnerID, World *GameWorld)
{
	if (claimedOwnerID == NO_CLIENT_ID || trackedID < 0)
	{
		return NULL;
	}

	int index;

	if (Networking.connectMode == SERVER)
	{
		index = trackedID;
	}
	else
	{
		index = Networking.TrackedIDs[trackedID];
	}

	TrackedObject *TrackedObjects = Networking.TrackedObjects;

	if (index < 0 || claimedOwnerID != TrackedObjects[index].clientID)
	{
		return NULL;
	}


	Object *tracked = TrackedObjects[index].object;

	// check if object has been deleted
	if (tracked == NULL || tracked->State == EMPTY_OBJECT || tracked->instanceNumber != TrackedObjects[index].instance)
	{
		TrackedObjects[index].object = NULL;
	}

	return &TrackedObjects[index];
}


void updateTrackedObject(NetworkPacket *packet, World *GameWorld)
{
	if (packet == NULL || Networking.connectMode == OFFLINE)
	{
		return;
	}

	int trackedID = packet->data.objectData.trackedID;
	int claimedOwnerID = packet->data.objectData.ownerClientID;
	
	TrackedObject *tracked = getTrackedObject(trackedID, claimedOwnerID, GameWorld);

	if (tracked == NULL || tracked->clientDeleted)
	{
		return;
	}

	if (tracked->object == NULL || tracked->object->State == EMPTY_OBJECT)
	{
		if (Networking.connectMode == SERVER)
		{
			return;
		}

		Object *packetObject = &packet->data.objectData.object;
		tracked->object = AddNamedObject(GameWorld, packetObject->name, packetObject->ObjectID, 0, 0);

		if (tracked->object == NULL)
		{
			return;
		}

		tracked->instance = tracked->object->instanceNumber;
	}

	copyPacketDataToObject(tracked->object, packet);

	return;
}

void updateTrackedObjectHealth(NetworkPacket *packet, World *GameWorld)
{
	if (packet == NULL || Networking.connectMode == OFFLINE)
	{
		return;
	}

	int trackedID = packet->data.objectData.trackedID;
	int claimedOwnerID = packet->data.objectData.ownerClientID;
	
	TrackedObject *tracked = getTrackedObject(trackedID, claimedOwnerID, GameWorld);

	if (tracked == NULL || tracked->object == NULL)
	{
		return;
	}

	HealthComponent *health = getHealthComponent(tracked->object, GameWorld);

	if (health == NULL)
	{
		health = addHealthComponent(tracked->object, 1, 0, GameWorld);

		if (health == NULL)
		{
			return;
		}
	}

	memcpy(health, &packet->data.componentData.data.HealthComponent, sizeof(HealthComponent));

	return;
}

void sendTrackedObjectPacketsToServer(World *GameWorld)
{
	if (Networking.clientID == AWAITING_CLIENT_ID || Networking.clientID == NO_CLIENT_ID)	
	{
		// if client hasn't received its clientID yet, it should not send packets
		return;
	}


	TrackedObject *Tracked = Networking.TrackedObjects;
	int count = Networking.TrackedObjectCount;


	NET_StreamSocket *socket = Networking.myClient;

	if (socket == NULL)
	{
		return;
	}

	short *indexes = Networking.TrackedIDs;
	int index;


	for (int i = 0; i < MAX_TRACKED_OBJECTS && count > 0; i++)
	{
		index = indexes[i];

		if (index < 0 || Tracked[index].clientDeleted)
		{
			continue;
		}

		count--;
	
		sendTrackedObjectPacket(index, socket, GameWorld);
	}
	

	return;
}


void sendTrackedObjectPacketsToClient(int recipientID, World *GameWorld)
{
	TrackedObject *Tracked = Networking.TrackedObjects;
	int count = Networking.TrackedObjectCount;


	NET_StreamSocket *socket = NULL;

	if (recipientID >= 0)
	{
		socket = Networking.connectedClients[recipientID];
	}

	if (socket == NULL)
	{
		return;
	}
	

	for (int i = 0; i < MAX_TRACKED_OBJECTS && count > 0; i++)
	{
		if (Tracked[i].clientID == NO_CLIENT_ID)
		{
			continue;
		}

		count--;
		
		sendTrackedObjectPacket(i, socket, GameWorld);
	}

	return;
}

void sendTrackedObjectPacket(int index, NET_StreamSocket *socket, World *GameWorld)
{
	TrackedObject *Tracked = &Networking.TrackedObjects[index];

	if (Tracked->clientDeleted && Networking.connectMode == CLIENT)
	{
		return;
	}

	if (Tracked->object == NULL || Tracked->object->State == EMPTY_OBJECT || Tracked->object->instanceNumber != Tracked->instance)
	{
		removeTrackedObject(Tracked->trackedID);
	}
	else
	{
		if (Tracked->clientID != Networking.clientID)
		{
			return;
		}

		NetworkPacket packet = {0};
		packet.tickSent = TickNumber();

		packet.type = PACKET_OBJECT_UPDATE;
		copyObjectToPacketData(Tracked->object, &packet);
		packet.data.objectData.trackedID = Tracked->trackedID;
		packet.data.objectData.ownerClientID = Tracked->clientID;
		NET_WriteToStreamSocket(socket, &packet, sizeof(NetworkPacket));
		outgoing += sizeof(NetworkPacket);


		HealthComponent *health = getHealthComponent(Tracked->object, GameWorld);

		if (health != NULL)
		{
			packet.type = PACKET_HEALTH_UPDATE;
			memcpy(&packet.data.componentData.data.HealthComponent, health, sizeof(HealthComponent));
			packet.data.componentData.trackedID = Tracked->trackedID;
			packet.data.componentData.ownerClientID = Tracked->clientID;
			NET_WriteToStreamSocket(socket, &packet, sizeof(NetworkPacket));
			outgoing += sizeof(NetworkPacket);
		}
	}

	return;
}


void copyObjectToPacketData(Object *input, NetworkPacket *packet)
{
	if (input == NULL || packet == NULL)
	{
		return;
	}

	Object *packetObject = &packet->data.objectData.object;
	PhysicsBox *packetBox = &packet->data.objectData.box;
	DisplayData *packetDisplay = &packet->data.objectData.display;

	packetObject->ObjectID = input->ObjectID;
	strcpy(packetObject->name, input->name);
	packetObject->State = input->State;
	packetObject->Action = input->Action;
	packetObject->ParentLink = input->ParentLink;
	packetObject->reserved = input->reserved;
	packetObject->arg1 = input->arg1;
	packetObject->arg2 = input->arg2;
	packetObject->arg3 = input->arg3;
	packetObject->arg4 = input->arg4;

	memcpy(packetBox, input->ObjectBox, sizeof(PhysicsBox));
	memcpy(packetDisplay, input->ObjectDisplay, sizeof(DisplayData));

	// clear pointers
	packetBox->GroundBox = NULL;
	packetDisplay->spriteBuffer = NULL;
	packetDisplay->frameBuffer = NULL;
	packetDisplay->animationBuffer = NULL;
	packetDisplay->spriteSetSource = NULL;

	return;
}

void copyPacketDataToObject(Object *input, NetworkPacket *packet)
{
	if (input == NULL || packet == NULL)
	{
		return;
	}

	Object *packetObject = &packet->data.objectData.object;
	PhysicsBox *packetBox = &packet->data.objectData.box;
	DisplayData *packetDisplay = &packet->data.objectData.display;

	if (packetObject->State == EMPTY_OBJECT)
	{
		return;
	}

	input->ObjectID = packetObject->ObjectID;
	strcpy(input->name, packetObject->name);
	input->State = packetObject->State;
	input->Action = packetObject->Action;
	input->ParentLink = packetObject->ParentLink;
	input->reserved = packetObject->reserved;
	
	input->arg1 = packetObject->arg1;
	input->arg2 = packetObject->arg2;
	input->arg3 = packetObject->arg3;
	input->arg4 = packetObject->arg4;

	if (INTERPOLATION_ENABLED)
	{
		packetBox->prevXPos = packetBox->xPos;
		packetBox->prevYPos = packetBox->yPos;
	}
	memcpy(input->ObjectBox, packetBox, sizeof(PhysicsBox));


	DisplayData *display = input->ObjectDisplay;
	SpriteSet *prevSource = display->spriteSetSource;
	memcpy(display, packetDisplay, sizeof(DisplayData));
	display->spriteSetSource = prevSource;

	UpdateObjectDisplay(input, 0.0);
	if (input->ObjectDisplay->currentSprite < 0)
	{
		input->ObjectDisplay->spriteBuffer = EngineSettings.DefaultTexture;
	}

	return;
}

void sendPlayerPacket(World *GameWorld)
{
	if (Networking.myClient == NULL || Networking.connectMode != CLIENT || GameWorld == NULL)
	{
		return;
	}
	
	NetworkPacket packet = {0};
	packet.type = PACKET_PLAYER_UPDATE;
	packet.tickSent = TickNumber();
	packet.data.playerData = GameWorld->Player;
	packet.data.playerData.PlayerPtr = NULL;

	NET_WriteToStreamSocket(Networking.myClient, &packet, sizeof(NetworkPacket));
	outgoing += sizeof(NetworkPacket);

	return;
}

void sendGameEventPackets(int recipientID)
{
	if (Networking.clientID == AWAITING_CLIENT_ID || Networking.clientID == NO_CLIENT_ID)	
	{
		// if client hasn't received its clientID yet, it should not send packets
		return;
	}

	NET_StreamSocket *socket = NULL;
	if (recipientID == SERVER_CLIENT_ID)
	{
		socket = Networking.myClient;
	}
	else if (recipientID >= 0)
	{
		socket = Networking.connectedClients[recipientID];
	}

	if (socket == NULL)
	{
		return;
	}


	GameEvent *events = Networking.NetworkEvents.Events;
	NetworkPacket buffer = {0};
	buffer.type = PACKET_GAME_EVENT;
	buffer.tickSent = TickNumber();

	if (events[0].EventID == NO_EVENT)
	{
		return;
	}

	bool result;

	for (int i = 0; i < MAX_QUEUED_GAME_EVENTS; i++)  
	{
		if (events[i].EventID == NO_EVENT)
		{
			break;
		}

		if (recipientID == SERVER_CLIENT_ID && events[i].clientID != Networking.clientID)	// don't send any events to the server that you don't own
		{
			continue;
		}

		//putConsole("sending %s to %d", getEventName(events[i].EventID), recipientID);

		memcpy(&buffer.data, &events[i], sizeof(PacketData));	// copy first half of game event into networkPacket's data section

		result = NET_WriteToStreamSocket(socket, &buffer, sizeof(NetworkPacket));
		outgoing += sizeof(NetworkPacket);

		if (result == false)
		{
			return;
		}

		NET_WriteToStreamSocket(socket, ((Uint8 *)&events[i]) + sizeof(PacketData), sizeof(GameEvent) - sizeof(PacketData));	// copy any remaining data to the end of the packet
		outgoing += sizeof(NetworkPacket);
	}

	return;
}

void saveEventForNetworkTransmission(GameEvent *input)
{
	if (Networking.clientID != SERVER_CLIENT_ID && input->clientID != Networking.clientID)	// if we are not the server, don't send events you don't own
	{
		return;
	}

	if (input->clientID == LOCAL_CLIENT_ID)	 // don't send if it is a local event
	{
		return;
	}

	GameEvent *eventPtr = findAvailableEvent(&Networking.NetworkEvents);

	if (eventPtr != NULL)
	{
		// copy the data over to be sent in the next network transmission
		memcpy(eventPtr, input, sizeof(GameEvent));
	}

	return;
}

void sendCommandToClients(const char command[])
{
	if (Networking.connectMode != SERVER)
	{
		return;
	}

	GameEvent event = {0};
	event.EventID = EVENT_CONSOLE_COMMAND;
	event.clientID = SERVER_CLIENT_ID;

	addGameEventString(&event, "command", command);

	saveEventForNetworkTransmission(&event);

	return;
}