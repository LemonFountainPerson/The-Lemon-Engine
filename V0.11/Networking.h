void initialiseNetworkData(void);

bool openServer(const char ip[], Uint16 portNumber, World *GameWorld);

void closeServer(void);

bool connect(const char ip[], Uint16 portNumber);

void disconnect(void);

void disconnectWithMessage(const char message[]);

void cleanUpNetworkData(void);

void resetTrackedObjects(void);

void deleteUnownedTrackedObjects(void);

void removeDisconnectedClient(int index, const char reason[]);


void ServerOpened(World *GameWorld);

void NewClientJoinedServer(int clientID);

void JoinedServer(World *GameWorld);

const char* getUsername(int clientID);

void setUsernameLocally(int clientID, const char username[]);

void setMyUsername(const char username[]);

void setUsername(const char username[], int clientID);

void KickClient(const char username[]);


void updateNetworking(World *GameWorld);

void attemptResolveAddress(void);

void attemptServerHost(World *GameWorld);

void updateServer(World *GameWorld);

bool receiveClientData(int index, World *GameWorld);

void acceptClients(World *GameWorld);


void attemptClientConnect(World *GameWorld);

void sendServerPassword(const char password[]);

void updateClient(World *GameWorld);

void handlePendingClient(int clientID, World *GameWorld);

void processClientEntrancePacket(NetworkPacket *packet, int clientID, World *GameWorld);

void sendAllJoinedClients(NET_StreamSocket *socket);

void sendAllTrackedObjects(NET_StreamSocket *socket);

void sendAllServerConVars(NET_StreamSocket *socket);

Uint64 getRandom64Bits();

bool receiveServerData(World *GameWorld);


bool processSetupPacket(NetworkPacket *packet, World *GameWorld);

void processSettingsPacket(NetworkPacket *packet);

void updateServerSettings(void);

void updateServerConVar(ConsoleVariable *input);

void sendServerCommand(ConsoleCommand *input, const char consoleInput[USER_INPUT_MAX_LEN]);

void updateServerFlag(GameFlag *input);


int TrackObjectOverNetwork(Object *input);

int RespondToTrackObjectRequest(NetworkPacket *packet, int index, World *GameWorld);

int RespondToDeleteObjectRequest(NetworkPacket *packet, int clientIndex, World *GameWorld);

int addNewTrackedObject(Object *input, int owner);

void processTrackedObjectResponse(NetworkPacket *packet);

void processTrackedObjectAdded(NetworkPacket *packet, World *GameWorld);

void removeTrackedObject(int deletedID);

void tellClientsToRemoveTrackedObject(int deletedID);

void processTrackedObjectDeleted(int deletedID);

void updateTrackedObject(NetworkPacket *packet, World *GameWorld);

void updateTrackedObjectHealth(NetworkPacket *packet, World *GameWorld);

void sendTrackedObjectPacketsToServer(World *GameWorld);

void sendTrackedObjectPacketsToClient(int recipientID, World *GameWorld);

void sendTrackedObjectPacket(int index, NET_StreamSocket *socket, World *GameWorld);

void copyObjectToPacketData(Object *input, NetworkPacket *packet);

void copyPacketDataToObject(Object *input, NetworkPacket *packet);


void processGameEventPacket(NetworkPacket *input, NET_StreamSocket *socket, World *GameWorld);

void sendGameEventPackets(int recipientID);

void saveEventForNetworkTransmission(GameEvent *input);

void sendCommandToClients(const char command[]);