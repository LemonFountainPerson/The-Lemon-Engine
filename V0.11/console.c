#include "LemonEngine.h"


// command console
static const float consoleWidth = 1024.0;
static const float consoleHeight = 650.0;
static const float insideSpacing = 8.0;
static const int consoleTextWidth =  (int)(consoleWidth - (2.0 * insideSpacing));

void updateConsole(SDL_Window *window, World *GameWorld)
{
	if (DebugSettings.consoleOpen == false)
	{
		// open console
		if (buttonPressed(LMN_CONSOLE_OPEN) && !TextSettings.Typing)
		{
			AcknowledgeButton(LMN_CONSOLE_OPEN);

			DebugSettings.consoleXPos = -(consoleWidth / 2.0);
			DebugSettings.consoleYPos = (ScreenData.screenHeight >> 1) - consoleHeight;

			float inputXPos = DebugSettings.consoleXPos + insideSpacing;
			float inputYPos = DebugSettings.consoleYPos;

			Text *consoleInput = addDebugTextWithName("", "ConsoleUserInput", inputXPos, inputYPos, consoleTextWidth, DTFORMAT_SCREEN_RELATIVE);
			updateConsoleHistoryText(addDebugTextWithName("", "ConsoleHistory", inputXPos, inputYPos, consoleTextWidth, DTFORMAT_JUSTIFY_TOP));
    
			DebugSettings.consoleOpen = true;
			DebugSettings.consoleFocus = false;
			DebugSettings.scrollVal = 0;
			startTyping(window, consoleInput);
		}

		return;
	}

	// close console
	if (buttonPressed(LMN_CONSOLE_OPEN))
	{
		AcknowledgeButton(LMN_CONSOLE_OPEN);

		DebugSettings.consoleOpen = false;
		stopTyping(window);

		removeDebugTextWithName("ConsoleHistory");
		removeDebugTextWithName("ConsoleUserInput");

		return;
	}

	if (TextSettings.Typing == false)
	{
		putConsole("> %s", TextSettings.userInputString);

		int prevIndex = modulo(DebugSettings.userInputHistory.head - 1, INPUT_HISTORY_LEN);
		if (strcmp(TextSettings.userInputString, DebugSettings.userInputHistory.inputs[prevIndex]))
		{
			addMessageHistory(TextSettings.userInputString, &DebugSettings.userInputHistory);
		}
		DebugSettings.userInputHistory.searchIndex = DebugSettings.userInputHistory.head;
		
		consoleInput(TextSettings.userInputString, GameWorld);
		startTyping(window, getDebugTextWithName("ConsoleUserInput"));

		return;
	}

	if (buttonPressed(MOUSE_LEFT))
	{
		bool xOverlap = MouseInput.xPos > DebugSettings.consoleXPos && MouseInput.xPos < DebugSettings.consoleXPos + consoleWidth;
		bool yOverlap = MouseInput.yPos > DebugSettings.consoleYPos && MouseInput.yPos < DebugSettings.consoleYPos + consoleHeight;
		if (xOverlap && yOverlap)
		{
			DebugSettings.consoleFocus = true;
		}
		else
		{
			DebugSettings.consoleFocus = false;
		}
	}

	if (buttonPressed(LMN_UPARROW) || MouseInput.wheelYDir > 0 || GamePadInput.rightStickY > 0.9)
	{
		if (DebugSettings.consoleFocus || GamePadInput.rightStickY > 0.9)
		{
			DebugSettings.scrollVal = clamp(DebugSettings.scrollVal + 1, 0, INPUT_HISTORY_LEN);
		}
		else
		{
			char *next = getNextMessageHistory(&DebugSettings.userInputHistory);

			if (next != NULL && next[0] != '\0')
			{
				strcpy(TextSettings.userInputString, next);
				TextSettings.userInputIndex = strlen(TextSettings.userInputString);

				setCursorPos();

				if (TextSettings.typingText != NULL)
				{
					updateText(TextSettings.typingText, TextSettings.userInputString);
				}
			}
		}
	}

	if (buttonPressed(LMN_DOWNARROW) || MouseInput.wheelYDir < 0 || GamePadInput.rightStickY < -0.9)
	{
		if (DebugSettings.consoleFocus || GamePadInput.rightStickY < -0.9)
		{
			DebugSettings.scrollVal = clamp(DebugSettings.scrollVal - 1, 0, INPUT_HISTORY_LEN);
		}
		else
		{
			char *prev = getPreviousMessageHistory(&DebugSettings.userInputHistory);

			if (prev != NULL && prev[0] != '\0')
			{
				strcpy(TextSettings.userInputString, prev);
				TextSettings.userInputIndex = strlen(TextSettings.userInputString);

				setCursorPos();

				if (TextSettings.typingText != NULL)
				{
					updateText(TextSettings.typingText, TextSettings.userInputString);
				}
			}
		}
	}


	return;
}


ConsoleCommand* getConsoleCommand(const char name[])
{
	ConsoleCommand *list = DebugSettings.commands;

	for (int i = 0; i < MAX_CONSOLE_COMMANDS; i++)
	{
		ConsoleCommandFunction command = list[i].function;

		if (command != NULL && strcmp(list[i].name, name) == 0)
		{
			return &list[i];	
		}
	}

	return NULL;
}


ConsoleVariable* getConsoleVariable(const char name[])
{
	if (name == NULL || name[0] < 32)
	{
		return NULL;
	}

	ConsoleVariable *varList = DebugSettings.consoleVariables;

	int index = hashConVar(name);

	for (int i = 0; i < MAX_CONSOLE_VARIABLES; i++)
	{
		if (strcmp(varList[index].name, name) == 0)
		{
			return &varList[index];	
		}

		index = (index + 1) % MAX_CONSOLE_VARIABLES;

		putConsole("Hash collision %d", i);
	}

	return NULL;
}


ConsoleVariable* NewConsoleVariable(const char name[], const char helpString[], ConsoleVariableType valueType, const char value[], ConsoleCommandFlag flags)
{
	if (name == NULL || name[0] < 32)
	{
		return NULL;
	}

	ConsoleVariable *varList = DebugSettings.consoleVariables;

	int index = hashConVar(name);

	for (int i = 0; i < MAX_CONSOLE_VARIABLES; i++)
	{
		if (varList[index].name[0] < 32)
		{
			LemonStrncpy(varList[index].name, name, MAX_LEN);
			stringToLower(varList[index].name);
			LemonStrncpy(varList[index].helpString, helpString, CONSOLE_HELP_MAX_LEN);
			varList[index].flags = flags;
			varList[index].valueType = valueType;

			setConsoleVariable(&varList[index], value, NULL);

			return &varList[index];
		}

		index = (index + 1) % MAX_CONSOLE_VARIABLES;

		putConsole("Hash collision %d", i);
	}

	return NULL;
}

int hashConVar(const char name[])
{
	unsigned int hash = 5381;

	int i = 0;

	while (name[i] != '\0' && i < MAX_LEN)
	{
		hash = (hash * 33) + name[i];
		i++;
	}

	return (int)(hash % MAX_CONSOLE_VARIABLES);
}


void setConsoleVariable(ConsoleVariable *variable, const char value[], World *GameWorld)
{
	// check if command is allowed to run
	if (variable == NULL || !commandIsAllowed(variable->flags)) 
	{ 
		return; 
	}

	if (variable->valueType == CONVAR_INT || variable->valueType == CONVAR_FLOAT)
	{
		int i = 0;
		while (i < CONVAR_VALUE_LEN && value[i] != '\0')
		{
			if (!inRange(value[i], '0', '9') && value[i] != '.')
			{
				return;
			}
			i++;
		}
	}

	switch (variable->valueType)
	{
	case CONVAR_FLOAT:
		variable->value.fValue = atof(value);
		break;

	case CONVAR_INT:
		variable->value.fValue = atof(value);
		break;

	case CONVAR_BOOL:
		if (strcmp(value, "true") == 0)
		{
			variable->value.bValue = true;
		}
		else if (strcmp(value, "false") == 0)
		{
			variable->value.bValue = false;
		}
		else
		{
			return;
		}
		break;

	default:
		LemonStrncpy(variable->value.string, value, CONVAR_VALUE_LEN);
		break;
	}


	if ((variable->flags & CONFLAG_NOTIFY) != 0)
	{
		char msg[MESSAGE_LENGTH] = {0};
		snprintf(msg, MESSAGE_LENGTH, "'%s' has been set to %s", variable->name, value);
		Message(msg, LOCAL_CLIENT_ID, GameWorld);	
	}

	// sync variable across server
	updateServerConVar(variable);

	return;
}

int ConVarAsInt(ConsoleVariable *variable)
{
	if (variable == NULL || variable->valueType != CONVAR_INT)
	{
		return 0;
	}

	return variable->value.iValue;
}

float ConVarAsFloat(ConsoleVariable *variable)
{
	if (variable == NULL || variable->valueType != CONVAR_FLOAT)
	{
		return 0.0;
	}

	return variable->value.fValue;
}

bool ConVarAsBool(ConsoleVariable *variable)
{
	if (variable == NULL || variable->valueType != CONVAR_BOOL)
	{
		return false;
	}

	return variable->value.bValue;
}

const char* ConVarAsString(ConsoleVariable *variable)
{
	if (variable == NULL || variable->valueType != CONVAR_STRING)
	{
		return NULL;
	}

	return variable->value.string;
}

const char* ConVarValueAsString(ConsoleVariable *variable, char result[32])
{
	if (variable == NULL)
	{
		return result;	
	}

	switch(variable->valueType)
	{
	case CONVAR_FLOAT:
		snprintf(result, 32, "%f", variable->value.fValue);
		break;

	case CONVAR_INT:
		snprintf(result, 32, "%d", variable->value.iValue);
		break;

	case CONVAR_BOOL:
		if (variable->value.bValue)
		{
			strcpy(result, "true");
		}
		else
		{
			strcpy(result, "false");
		}
		break;

	default:
		return variable->value.string;
	}

	return result;
}

void consoleInput(const char inputSource[USER_INPUT_MAX_LEN], World *GameWorld)
{
	if (inputSource[0] == '\0')
	{
		return;
	}

	char input[USER_INPUT_MAX_LEN] = {0};
	strcpy(input, inputSource);
	DebugSettings.scrollVal = 0;

	if (GameWorld == NULL)
	{
		return;
	}

	DebugSettings.argIndex = 0;

	do {
		if (input[DebugSettings.argIndex] == ';')
		{
			DebugSettings.argIndex++;
		}

		executeCommand(input, GameWorld);
	} while (input[DebugSettings.argIndex] == ';');


	return;
}

void executeCommand(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};

	getNextConsoleArg(input, arg);
	stringToLower(arg);

	ConsoleCommand *command = getConsoleCommand(arg);

	if (command != NULL)
	{
		// check if command is allowed to run
		if (!commandIsAllowed(command->flags)) 
		{ 
			return; 
		}

		sendServerCommand(command, input, DebugSettings.argIndex);

		if (command->function(input, GameWorld) != LEMON_SUCCESS)
		{
			putConsole("command unrecognised");
		}

		return;	
	}
	

	ConsoleVariable *variable = getConsoleVariable(arg);

	if (variable == NULL)
	{
		putConsole("command unrecognised");
		return;
	}

	getNextConsoleArg(input, arg);
	if (arg[0] > 32)
	{
		setConsoleVariable(variable, arg, GameWorld);
	}
	else if ((variable->flags & CONFLAG_PROTECTED) == 0)
	{
		char number[32] = {0};
		putConsole("'%s' value: %s", variable->name, ConVarValueAsString(variable, number));
	}


	return;
}

bool commandIsAllowed(ConsoleCommandFlag input)
{
	ConsoleCommandFlag securityCheck = CONFLAG_PROTECTED | CONFLAG_SERVER_SIDE;
	if ((input & securityCheck) != 0 && Networking.connectMode == CLIENT)
	{
		putConsole("This command can only be run by the server!");
		return false;
	}

	if (DEBUG_MODE)
	{
		return true;
	}

	if ((input & CONFLAG_CHEAT) != 0 && !ConVarAsBool(EngineSettings.cheats))
	{
		putConsole("This command requires cheats to be enabled!"); 
		return false;
	}

	return true;
}


void getNextConsoleArg(const char input[], char argDest[])
{
	if (argDest == NULL || input == NULL)
	{
		return;
	}

	memset(argDest, 0, USER_INPUT_MAX_LEN);

	while (input[DebugSettings.argIndex] != '\0' && input[DebugSettings.argIndex] < 33 && DebugSettings.argIndex < USER_INPUT_MAX_LEN - 1)
	{
		DebugSettings.argIndex++;
	}

	bool enclosedCommand = false;
	if (input[DebugSettings.argIndex] =='"')
	{
		enclosedCommand = true;
		DebugSettings.argIndex++;
	}

	int i = 0;
	while (i < USER_INPUT_MAX_LEN - 1 && input[DebugSettings.argIndex] != '\0')
	{
		if (!enclosedCommand && input[DebugSettings.argIndex] == ';')
		{
			return;
		}

		if (input[DebugSettings.argIndex] == '"' || (!enclosedCommand && input[DebugSettings.argIndex] < 33) )
		{
			DebugSettings.argIndex++;
			return;
		}

		argDest[i] = input[DebugSettings.argIndex];
		DebugSettings.argIndex++;
		i++;
	}

	argDest[i] = 0;

	return;
}


bool nextConsoleArgIsNumber(const char input[USER_INPUT_MAX_LEN])
{
	int prevPos = DebugSettings.argIndex;

	char buffer[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, buffer);

	DebugSettings.argIndex = prevPos;

	return inRange(buffer[0], '0', '9');
}

bool hasNextConsoleArg(const char input[USER_INPUT_MAX_LEN])
{
	int prevPos = DebugSettings.argIndex;

	char buffer[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, buffer);

	DebugSettings.argIndex = prevPos;

	return (buffer[0] != '\0');
}

int getNextConsoleInt(const char input[USER_INPUT_MAX_LEN])
{
	int checkPos = DebugSettings.argIndex;

	char buffer[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, buffer);

	if (!inRange(buffer[0], '0', '9'))
	{
		DebugSettings.argIndex = checkPos;
		return 0;
	}

	return atoi(buffer);
}

float getNextConsoleFloat(const char input[USER_INPUT_MAX_LEN])
{
	int checkPos = DebugSettings.argIndex;

	char buffer[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, buffer);

	if (!inRange(buffer[0], '0', '9'))
	{
		DebugSettings.argIndex = checkPos;
		return 0.0;
	}

	return atof(buffer);
}

void getNextConsoleFlag(char input[USER_INPUT_MAX_LEN], char argDest[USER_INPUT_MAX_LEN])
{
	int originalPos = DebugSettings.argIndex;

	while (DebugSettings.argIndex < USER_INPUT_MAX_LEN && input[DebugSettings.argIndex] > 32)
	{
		int prevPos = DebugSettings.argIndex;

		getNextConsoleArg(input, argDest);

		if (argDest[0] == '-')
		{
			argDest[USER_INPUT_MAX_LEN - 1] = '\0';
			stringToLower(argDest);

			int i = 0;
			while (argDest[prevPos + i] > 32 && prevPos + i < USER_INPUT_MAX_LEN)
			{
				input[prevPos + i] = ' ';
				i++;
			}

			return;
		}
	}

	DebugSettings.argIndex = originalPos;
	argDest[0] = '\0';

	return;
}

bool getNextConsoleBool(const char input[USER_INPUT_MAX_LEN])
{
	int originalPos = DebugSettings.argIndex;

	char buffer[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, buffer);

	if (strcmp(buffer, "true") == 0 || buffer[0] == '1')
	{
		return true;
	}
	else if (strcmp(buffer, "false") == 0 || buffer[0] == '0')
	{
		return false;
	}

	DebugSettings.argIndex = originalPos;

	return false;
}


Object* parseArgumentToFindObject(const char input[USER_INPUT_MAX_LEN], ObjectController *ObjectList)
{
	char buffer[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, buffer);

	if (inRange(buffer[0], '0', '9'))
	{
		int index = atoi(buffer);

		if (index >= MAX_OBJECTS)
		{
			putConsole("'%d' index out of bounds. Valid range 0 <-> %d", index, MAX_OBJECTS - 1);
			return NULL;
		}

		return &ObjectList->objectComponents.Objects[index];
	}
	else
	{
		Object *object = FindObject(buffer, ObjectList);
		if (object == NULL)
		{
			putConsole("Cannot find '%s' from objectlist.", buffer);
		}

		return object;
	}
}

// macro used to create new command; name and function are derived from cName
#define NEWCOMMAND(cName, cHelp, cFormat, cFlags) 	strcpy(commandList[i].name, #cName);\
														stringToLower(commandList[i].name); \
														commandList[i].flags = cFlags; \
														strcpy(commandList[i].helpString, cHelp);\
														strcpy(commandList[i].formatString, cFormat);\
														commandList[i].function = &ConsoleCommand_##cName; i++;


void createConsoleCommands(ConsoleCommand commandList[MAX_CONSOLE_COMMANDS])
{
	memset(commandList, 0, MAX_CONSOLE_COMMANDS * sizeof(ConsoleCommand));

	int i = 0;


	NEWCOMMAND(Version, "check engine version information", "version", 0);

	NEWCOMMAND(Quit, "quit the game", "quit", 0);

	NEWCOMMAND(Restart, "restart the game", "restart", 0);

//	NEWCOMMAND(Cheats, "set the game's cheats value", "cheats [val]", CONFLAG_SERVER_SIDE);

	NEWCOMMAND(Tick, "check current tick number", "tick", 0);

	NEWCOMMAND(Say, "send a message to the chat", "say [Message]", 0);

	NEWCOMMAND(SayText, "display a textbox", "saytext [Message] [Preset number] [Portrait]", 0);


	NEWCOMMAND(StartServer, "start a server for other clients to connect to", "StartServer [IP] [port]", 0);

	NEWCOMMAND(CloseServer, "close server if one is open", "CloseServer", 0);
	
	NEWCOMMAND(Connect, "connect to a remote server", "connect [address] [port]", 0);

	NEWCOMMAND(Disconnect, "disconnect from server if connected", "disconnect", 0);

	NEWCOMMAND(Kick, "Kick a client from your server (server only)", "kick [username]", 0);

	NEWCOMMAND(ListClients, "show all clients that are connected with their usernames", "listclients", 0);

	NEWCOMMAND(ResetBlacklist, "reset server blacklist to allow previously banned clients to re-join", "resetblacklist", CONFLAG_SERVER_SIDE);

	NEWCOMMAND(SetServerPassword, "Set a password for the server", "setserverpassword [password]", CONFLAG_SERVER_SIDE | CONFLAG_PROTECTED);

	NEWCOMMAND(ServerPassword, "Input a password to join a server", "serverpassword [password]", 0);

//	NEWCOMMAND(SetNetworkUpdateRate, "set the time interval between network updates", "setnetworkupdaterate [seconds]", 0);

//	NEWCOMMAND(SetTimeOut, "set the maximum time allowed before a network connection is considered lost", "settimeout [seconds]", 0);

	NEWCOMMAND(SetUsername, "set your own username", "setusername [name]", 0);
	
	
	NEWCOMMAND(Fullscreen, "toggle fullscreen (equivalent to calling 'event enablefullscreen' or 'event disablefullscreen')", "fullscreen", 0);

	NEWCOMMAND(Show, "show when a new event/spriteset/etc. is created", "show [events/spritesets/sceneactions/errors/...] [true/false]", CONFLAG_CHEAT);

	NEWCOMMAND(Vsync, "toggle screen vertical sync", "vsync [true/false]", 0);

	NEWCOMMAND(Debug, "set the debug mode", "debug [0/1/2/...]", CONFLAG_CHEAT);

	NEWCOMMAND(Fps, "show the engine's current frames per second", "fps", 0);

	NEWCOMMAND(Draw, "toggle whether a certain element is drawn or not", 
		"draw [sprites/hitboxes/backgrounds/camviews/hud/particles] [true/false]", CONFLAG_CHEAT);

	NEWCOMMAND(HitboxThickness, "set the hitbox thickness in pixels for when hitboxes are being drawn", "hitboxthickness [THICKNESS]", CONFLAG_CHEAT);

	NEWCOMMAND(List, "lists current instances of requested data", "list [objects/text/fonts/spritesets/...]", CONFLAG_CHEAT);

	NEWCOMMAND(AddObject, "create a new object, equivalent to 'object_add [OBJECTID] ...'", 
		"addobject [OBJECTID] [XPOS] [YPOS] [ARG1] [ARG2] [ARG3] [ARG4] [ARG5]", CONFLAG_CHEAT);

	NEWCOMMAND(Object, "perform various actions on a specific object, identified by their index number or name", 
		"object [INDEX/NAME] [info/setpos/setname/add/delete/...]", 1);

	NEWCOMMAND(UsedMemory, "check how much memory in kilobytes is currently being used for specific data", 
		"usedmemory [objects/animations/text/...]", CONFLAG_CHEAT);

	NEWCOMMAND(BackGround, "change the background sprite", "background [backGroundID]", CONFLAG_CHEAT);

	NEWCOMMAND(Level, "switch to a new level", "level [LEVELID]", CONFLAG_CHEAT);

	NEWCOMMAND(Event, "trigger a specific GameEvent", "event [setscreensize/enablefullscreen/switchlevel/...]", CONFLAG_CHEAT);

	NEWCOMMAND(CamView, "add or modify camera views in the gameworld", "camview [add/clear/attach/...]", CONFLAG_CHEAT);

	NEWCOMMAND(Sound, "play or modify sounds", "sound [play/...]", 0);

	NEWCOMMAND(Cutscene, "play or manipulate cutscenes", "cutscene [play/start/...]", CONFLAG_CHEAT);

	NEWCOMMAND(Load, "load some data type into the engine to be used later", "load [spriteset/audio/...]", CONFLAG_CHEAT);

	NEWCOMMAND(DebugText, "show info on debug text or manipulate them", "DebugText [info/...]", CONFLAG_CHEAT);

	NEWCOMMAND(Pause, "toggle engine pause state", "pause", CONFLAG_CHEAT);

	NEWCOMMAND(SetPos, "set the player X and Y position", "setPos [x] [y]", CONFLAG_CHEAT);

	NEWCOMMAND(SetCamPos, "set the main camera X and Y position", "setcampos [x] [y]", CONFLAG_CHEAT);

	NEWCOMMAND(SetCamZoom, "set the main camera X and Y zoom, default: 1.0, 1.0", "setcamzoom [xZoom] [yZoom]", CONFLAG_CHEAT);

	NEWCOMMAND(SetTickRate, "set a new tickrate (GameTicks per second)", "settickrate [newTickRate]", CONFLAG_CHEAT | CONFLAG_SERVER_SIDE);

	NEWCOMMAND(Save, "save the game to a slot", "save [saveID]", 0);

	NEWCOMMAND(LoadSave, "load a save file", "loadsave [saveID]", 0);

	NEWCOMMAND(SaveSettings, "save current settings to a slot", "savesettings [settingsID]", 0);

	NEWCOMMAND(LoadSettings, "load a settings file", "loadsettings [settingsID]", 0);

	NEWCOMMAND(AddGameFlag, "add a new gameflag with a unique identifying name", "addgameflag [name] [startValue]", CONFLAG_CHEAT);

	NEWCOMMAND(SetGameFlag, "set a gameflag to a new value", "setgameflag [NAME/INDEX] [newValue]", CONFLAG_CHEAT);

	NEWCOMMAND(CheckGameFlag, "check the current value of a GameFlag", "checkgameflag [NAME/INDEX]", CONFLAG_CHEAT);

	NEWCOMMAND(Help, "see information on a specific command or just type 'help' to see all help info", "help [command]", 0);

	NEWCOMMAND(DoABarrelRoll, "does a barrel roll.", "barrelroll", CONFLAG_CHEAT);

	NEWCOMMAND(Noclip, "toggles noclip.", "noclip", CONFLAG_CHEAT);


	if (DEBUG_MODE)
	{
		putConsole("Loaded %d commands.", i);
	}
	
	return;
}


int ConsoleCommand_Version(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	putConsole("\n%s\n%s\nScript Version: %s", LEMON_ENGINE_INFO, LEMON_VERSION, LEMON_SCRIPT_VERSION);
	if (DEBUG_MODE)
	{
		putConsole("Running in Debug mode");
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_Quit(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	GameWorld->GameState = CLOSE_GAME;

	return LEMON_SUCCESS;
}

int ConsoleCommand_Restart(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	GameWorld->GameState = RESTART_GAME;

	return LEMON_SUCCESS;
}

// int ConsoleCommand_Cheats(char input[USER_INPUT_MAX_LEN], World *GameWorld)
// {
// 	EngineSettings.cheats = getNextConsoleInt(input);

// 	return LEMON_SUCCESS;
// }

int ConsoleCommand_Tick(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	putConsoleTS("Tickrate: %d", EngineSettings.GameTicksPerSecond);

	return LEMON_SUCCESS;
}

int ConsoleCommand_Say(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char buffer[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, buffer);

	Message(buffer, Networking.clientID, GameWorld);

	return LEMON_SUCCESS;
}

int ConsoleCommand_SayText(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char buffer[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, buffer);

	int preset = getNextConsoleInt(input);

	char portrait[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, portrait);

	SayText(buffer, portrait, preset, GameWorld);

	return LEMON_SUCCESS;
}

int ConsoleCommand_StartServer(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char address[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, address);

	if (strcmp(address, "test") == 0)
	{
		openServer(NULL, 2000, GameWorld);

		return LEMON_SUCCESS;
	}

	Uint16 portNumber = (Uint16)getNextConsoleInt(input);

	openServer(address, portNumber, GameWorld);

	return LEMON_SUCCESS;
}

int ConsoleCommand_CloseServer(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	closeServer();	

	return LEMON_SUCCESS;
}

int ConsoleCommand_Connect(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char address[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, address);

	if (strcmp(address, "test") == 0)
	{
		connect("127.0.0.1", 2000);

		return LEMON_SUCCESS;
	}

	Uint16 portNumber = (Uint16)getNextConsoleInt(input);

	connect(address, portNumber);

	return LEMON_SUCCESS;
}

int ConsoleCommand_Disconnect(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	disconnect();

	return LEMON_SUCCESS;
}

int ConsoleCommand_Kick(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char username[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, username);
	KickClient(username);

	return LEMON_SUCCESS;
}

int ConsoleCommand_ResetBlacklist(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	if (Networking.connectMode != SERVER)
	{
		putConsole("");

		return LEMON_SUCCESS;
	}

	for (int i = 0; i < MAX_BLOCKED_CLIENTS; i++)
	{
		if (Networking.blockedIPs[i] != NULL)
		{
			NET_UnrefAddress(Networking.blockedIPs[i]);
			Networking.blockedIPs[i] = NULL;
		}
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_ListClients(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	putConsole("Server: %s", Networking.serverUsername);

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (Networking.clientUsernames[i][0] != '\0')
		{
			putConsole("%d: %s", i, Networking.clientUsernames[i]);
		}
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_SetServerPassword(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN];
	getNextConsoleArg(input, arg);
	setServerPassword(arg);

	// test
	Uint64 val = getRandom64Bits();

	Uint64 i = 0;
	while (i < 100000000)
	{
		Uint64 new = getRandom64Bits();
		if (val == new)
		{
			putConsole("Took %d loops, got %llu", i, new);
			return LEMON_SUCCESS;
		}
		i++;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_ServerPassword(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN];
	getNextConsoleArg(input, arg);
	sendServerPassword(arg);

	return LEMON_SUCCESS;
}


int ConsoleCommand_SetUsername(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN];
	getNextConsoleArg(input, arg);
	setMyUsername(arg);

	return LEMON_SUCCESS;
}

// convar
// int ConsoleCommand_SetNetworkUpdateRate(char input[USER_INPUT_MAX_LEN], World *GameWorld)
// {
// 	float time = fClamp(getNextConsoleFloat(input), 0.001, 1.0);
// 	Networking.secondsBetweenUpdates = time;

// 	return LEMON_SUCCESS;
// }

// convar
// int ConsoleCommand_SetTimeOut(char input[USER_INPUT_MAX_LEN], World *GameWorld)
// {
// 	Networking.connectionTimeout = fClamp(getNextConsoleFloat(input), 0.1, 60.0);

// 	return LEMON_SUCCESS;
// }


int ConsoleCommand_Fullscreen(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	if (ScreenData.Fullscreen)
	{
		disableFullscreen(GameWorld);
	}
	else
	{
		char flag[USER_INPUT_MAX_LEN] = {0};
		getNextConsoleFlag(input, flag);

		if (strcmp(flag, "-scaled") == 0)
		{
			enableFullscreenScaled(GameWorld);
		}
		else
		{
			enableFullscreen(GameWorld);
		}
	}

	return LEMON_SUCCESS;
}

// convar
int ConsoleCommand_Show(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, arg);
	stringToLower(arg);

	if (strcmp(arg, "events") == 0)
	{
		DebugSettings.showEvents = getNextConsoleBool(input);
	}
	else if (strcmp(arg, "sceneactions") == 0)
	{
		DebugSettings.showSceneActions = getNextConsoleBool(input);
	}
	else if (strcmp(arg, "spritesets") == 0)
	{
		DebugSettings.showSpriteset = getNextConsoleBool(input);
	}
	else if (strcmp(arg, "errors") == 0)
	{
		DebugSettings.showErrors = getNextConsoleBool(input);
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_Vsync(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	setVsync(getNextConsoleBool(input));

	return LEMON_SUCCESS;
}

int ConsoleCommand_Debug(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	DebugSettings.DebugTextDisplayMode = getNextConsoleInt(input);
	RemoveObjectDebugTexts();

	return LEMON_SUCCESS;
}

// convar
int ConsoleCommand_Fps(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	DebugSettings.FPSCounter = getNextConsoleBool(input);

	return LEMON_SUCCESS;
}

// convar
int ConsoleCommand_Draw(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, arg);

	if (strcmp(arg, "hitboxes") == 0 || strcmp(arg, "hitbox") == 0)
	{
		RenderSettings.drawHitboxes = getNextConsoleBool(input);
	}
	else if (strcmp(arg, "sprites") == 0)
	{
		RenderSettings.drawSprites = getNextConsoleBool(input);
	}
	else if (strcmp(arg, "background") == 0)
	{
		RenderSettings.drawBackGround = getNextConsoleBool(input);
	}
	else if (strcmp(arg, "camviews") == 0)
	{
		RenderSettings.drawCamViews = getNextConsoleBool(input);
	}
	else if (strcmp(arg, "hud") == 0)
	{
		RenderSettings.drawHUD = getNextConsoleBool(input);
	}
	else if (strcmp(arg, "particles") == 0)
	{
		RenderSettings.drawParticles = getNextConsoleBool(input);
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

// convar
int ConsoleCommand_HitboxThickness(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	RenderSettings.HitboxThickness = getNextConsoleInt(input);

	return LEMON_SUCCESS;
}


int ConsoleCommand_UsedMemory(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	ObjectController *ObjectList = &GameWorld->ObjectList;
	char arg[USER_INPUT_MAX_LEN] = {0};

	getNextConsoleArg(input, arg);
	double total = 0.0;

	if (strcmp(arg, "text") == 0)
	{
		total = (double)sizeof(TextSettings) / 1000.0;
	}
	else if (strcmp(arg, "consolevariable") == 0)
	{
		total = (double)sizeof(ConsoleVariable) / 1000.0;
	}
	else if (strcmp(arg, "network") == 0)
	{
		total = (double)sizeof(NetworkData) / 1000.0;
	}
	else if (strcmp(arg, "networkpacket") == 0)
	{
		total = (double)sizeof(NetworkPacket) / 1000.0;
	}
	else if (strcmp(arg, "textboxes") == 0)
	{
		putConsole("TextBox data size: %d", sizeof(TextBox));
		return LEMON_SUCCESS;
	}
	else if (strcmp(arg, "debug") == 0)
	{
		total = (double)sizeof(DebugSettings) / 1000.0;
	}
	else if (strcmp(arg, "world") == 0 || strcmp(arg, "gameworld") == 0)
	{
		total = (double)sizeof(World) / 1000.0;
	}
	else if (strcmp(arg, "objlist") == 0 || strcmp(arg, "objectlist") == 0)
	{
		total = (double)sizeof(ObjectController) / 1000.0;
	}
	else if (strcmp(arg, "components") == 0 || strcmp(arg, "comps") == 0)
	{
		putConsole("Component size: %d", sizeof(ComponentType));
		ComponentData *data = &ObjectList->objectComponents;
		total = (double)(sizeof(ComponentData) - sizeof(data->Objects) - sizeof(data->Displays) - sizeof(data->PhysicsBoxes)) / 1000.0;
	}
	else if (strcmp(arg, "objects") == 0)
	{
		putConsole("Object data size: %d", sizeof(Object));
		total = (double)sizeof(ObjectList->objectComponents.Objects) / 1000.0;
	}
	else if (strcmp(arg, "displays") == 0)
	{
		putConsole("Display data size: %d", sizeof(DisplayData));
		total = (double)sizeof(ObjectList->objectComponents.Displays) / 1000.0;
	}
	else if (strcmp(arg, "physboxes") == 0 || strcmp(arg, "physicsboxes") == 0)
	{
		putConsole("PhysicsBox data size: %d", sizeof(PhysicsBox));
		total = (double)sizeof(ObjectList->objectComponents.PhysicsBoxes) / 1000.0;
	}
	else if (strcmp(arg, "sceneactions") == 0)
	{
		putConsole("SceneAction data size: %d  SceneAction count: %d", sizeof(SceneAction), GameWorld->SceneActionCount);
		total = (double)(sizeof(SceneAction) * GameWorld->SceneActionCount) / 1000.0;
	}
	else if (strcmp(arg, "events") == 0)
	{
		putConsole("GameEvent data size: %d", sizeof(GameEvent));
		total = (double)(sizeof(GameEvent) * MAX_QUEUED_GAME_EVENTS) / 1000.0;
	}
	else if (strcmp(arg, "animations") == 0)
	{
		SpriteSet *set = ObjectList->spriteSets.start;
		int setCount = 0;
		int animCount = 0;
		int frameCount = 0;
		int spriteCount = 0;
		double textureData = 0;

		float width = 0;
		float height = 0;

		if (EngineSettings.DefaultTexture != NULL)
		{
			spriteCount++;
			SDL_GetTextureSize(EngineSettings.DefaultTexture->texture, &width, &height);
			textureData += width * height;
		}

		while (set != NULL)
		{
			Animation *anim = set->Animations;
			while (anim != NULL)
			{
				frameCount += anim->frameCount;
				
				anim = anim->nextAnimation;
				animCount++;
			}

			Sprite *sprite = set->firstSprite;
			while (sprite != NULL)
			{
				SDL_GetTextureSize(sprite->texture, &width, &height);
				textureData += width * height;
				sprite = sprite->nextSprite;
				spriteCount++;
			}


			set = set->nextSet;
			setCount++;
		}

		putConsole("Spritesets: %d  Animations: %d  AnimationFrames: %d  Sprites: %d \nTexture data estimate: %.2lfkb", 
			setCount, animCount, frameCount, spriteCount, textureData / 1000.0);
		total = (double)((sizeof(SpriteSet) * setCount) + (sizeof(Animation) * animCount) + (sizeof(AnimationFrame) * frameCount) + (sizeof(Sprite) * spriteCount)) + textureData;
		total /= 1000.0;
	}
	else
	{
		return INVALID_DATA;
	}
	
	putConsole("Total used: %.2lfkb", total);

	return LEMON_SUCCESS;
}

int ConsoleCommand_AddObject(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int ID = getNextConsoleInt(input);
	int args[7] = {0};
	for (int i = 0; i < 7; i++)
	{
		args[i] = getNextConsoleInt(input);
	}

	AddObject(GameWorld, ID, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);

	return LEMON_SUCCESS;
}

int ConsoleCommand_Object(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	ObjectController *ObjectList = &GameWorld->ObjectList;
	char arg[USER_INPUT_MAX_LEN] = {0};

	getNextConsoleArg(input, arg);

	if (strcmp(arg, "add") == 0)
	{
		return ConsoleCommand_AddObject(input, GameWorld);
	}

	Object *object = parseArgumentToFindObject(input, ObjectList);
	if (object == NULL)
	{
		return INVALID_DATA;
	}

	char flag[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleFlag(input, flag);

	if (strcmp(arg, "info") == 0)
	{
		displayObjectInfoConsole(object);
	}
	else if (strcmp(arg, "setpos") == 0)
	{
		float x = getNextConsoleFloat(input);
		float y = getNextConsoleFloat(input);

		if (strcmp(flag, "-snaptogrid") == 0 || strcmp(flag, "-grid") == 0)
		{
			snapPositionToTileGrid(object, x, y);
		}
		else
		{
			GoTo(object, x, y);
		}
	}
	else if (strcmp(arg, "setsize") == 0)
	{
		float width = getNextConsoleInt(input);
		float height = getNextConsoleInt(input);

		setSize(object, width, height);
	}
	else if (strcmp(arg, "changexsize") == 0)
	{
		float val = getNextConsoleInt(input);

		ChangeXSizeBy(val, object, GameWorld);
	}
	else if (strcmp(arg, "changeysize") == 0)
	{
		float val = getNextConsoleInt(input);

		ChangeYSizeBy(val, object, GameWorld);
	}
	else if (strcmp(arg, "setname") == 0)
	{
		getNextConsoleArg(input, arg);
		setObjectName(object, arg);
	}
	else if (strcmp(arg, "removecomponents") == 0)
	{
		removeComponents(object, ObjectList);
	}
	else if (strcmp(arg, "delete") == 0)
	{
		object->State = TO_BE_DELETED;
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

void displayObjectInfoConsole(Object *input)
{
	if (input == NULL)
	{
		return;
	}

	putConsole("\nObject Information: \nName: '%s'\nID: %d (%s)", input->name, input->ObjectID, getObjectIDName(input->ObjectID));
	putConsole("Index: %d \nCurrent State: %d (%s)", input->index, input->State, getObjectStateName(input->State));

	if (input->Parent == NULL)
	{
		putConsole("Parent: \n    None");
	}
	else
	{
		Object *parent = input->Parent;
		putConsole("Parent: \n    Name: %s \n    ID: %d (%s)", parent->name, parent->ObjectID, getObjectIDName(parent->ObjectID));
		putConsole("    Index: %d \n    Current State: %d (%s)", parent->index, parent->State, getObjectStateName(parent->State));
	}

	Layer objLayer = getDisplayLayer(input);
	putConsole("XPos: %f  YPos: %f \nLayer: %d (%s)", input->ObjectBox->xPos, input->ObjectBox->yPos, objLayer, getLayerName(objLayer));
}

int ConsoleCommand_BackGround(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int ID = getNextConsoleInt(input);
	int set = getNextConsoleInt(input);

	switchBackGroundSprite(ID, set, &GameWorld->WorldBackground);

	return LEMON_SUCCESS;
}

int ConsoleCommand_Level(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int level = getNextConsoleInt(input);
	switchLevel(level, GameWorld);

	return LEMON_SUCCESS;
}

int ConsoleCommand_Event(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, arg);

	GameEventID eventID = getEventID(arg);

	switch(eventID)
	{
	case EVENT_SWITCH_LEVEL:
		{
			int level = getNextConsoleInt(input);
			switchLevel(level, GameWorld);
		} break;

	case EVENT_PLAY_CUTSCENE:
		{
			int scene = getNextConsoleInt(input);
			playCutscene(scene, GameWorld);
		} break;

	case EVENT_PLAY_CUTSCENE_FROM_FILE:
		{
			getNextConsoleArg(input, arg);
			playCutsceneFromFile(arg, GameWorld);
		} break;

	case EVENT_ENABLE_FULLSCREEN:
		enableFullscreen(GameWorld);
		break;

	case EVENT_ENABLE_FULLSCREEN_SCALE:
		enableFullscreenScaled(GameWorld);
		break;

	case EVENT_DISABLE_FULLSCREEN:
		disableFullscreen(GameWorld);
		break;

	case EVENT_CHANGE_SCREEN_SIZE:
		{
			int width = getNextConsoleInt(input);
			int height = getNextConsoleInt(input);
			changeScreenSize(width, height, GameWorld);
		} break;

	case EVENT_CHANGE_SCREEN_SIZE_SCALE:
		{
			int width = getNextConsoleInt(input);
			int height = getNextConsoleInt(input);
			changeScreenSizeScaled(width, height, GameWorld);
		} break;

	case EVENT_LOAD_LEVEL_PARTITION:
	{
		int partID = getNextConsoleInt(input);
		streamPartition(partID, GameWorld);
	} break;

	default:
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_List(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	ObjectController *ObjectList = &GameWorld->ObjectList;
	char arg[USER_INPUT_MAX_LEN] = {0};

	getNextConsoleArg(input, arg);

	if (strcmp(arg, "object") == 0 || strcmp(arg, "objects") == 0)
	{
		Object *cursor = ObjectList->firstObject;

		while (cursor != NULL)
		{
			putConsole("%s - Index: %d  ID: %d (%s)  State: %d (%s)", 
				cursor->name, cursor->index, cursor->ObjectID, getObjectIDName(cursor->ObjectID), cursor->State, getObjectStateName(cursor->State));
			cursor = cursor->nextObject;
		}
	}
	else if (strcmp(arg, "text") == 0)
	{
		printTextListinfo(&GameWorld->TextList, "TextList");
	}
	else if (strcmp(arg, "fonts") == 0)
	{
		FontList *list = &GameWorld->FontList;

		for (int i = 0; i < MAX_LOADED_FONTS; i++)
		{
			if (list->fonts[i] != NULL)
			{
				putConsole("Slot %d '%s'  ", i, list->names[i]);
			}
			else
			{
				putConsole("Slot %d (Empty)", i);
			}
		}
	}
	else if (strcmp(arg, "debugtext") == 0)
	{
		printTextListinfo(&TextSettings.DebugTextList, "Debug Textlist");
	}
	else if (strcmp(arg, "spritesets") == 0)
	{
		SpriteSet *set = ObjectList->spriteSets.start;
		putConsole("Spritesets loaded:");

		while (set != NULL)
		{
			putConsole("Spriteset: %d (%s)", set->setID, getObjectIDName(set->setID));
			set = set->nextSet;
		}
	}
	else if (strcmp(arg, "camviews") == 0 || strcmp(arg, "cameraviews") == 0)
	{
		printCameraViewInfo(GameWorld->views);
	}
	else if (strcmp(arg, "layers") == 0)
	{
		for (int i = BACKGROUND; i < LAYER_COUNT; i++)
		{
			putConsole("%d: %s", i, getLayerName(i));
		}
	}
	else if (strcmp(arg, "gameflags") == 0)
	{
		for (int i = 0; i < GAME_FLAG_COUNT; i++)
		{
			if (GameFlags[i].name[0] != '\0')
			{
				putConsole("(%d)\"%s\": %d", i, GameFlags[i].name, GameFlags[i].value);
			}
		}
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_CamView(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, arg);
		
	if (strcmp(arg, "clear") == 0)
	{
		removeAllCameraViews(GameWorld);
	}
	else if (strcmp(arg, "add") == 0)
	{
		char flag[USER_INPUT_MAX_LEN] = {0};
		getNextConsoleFlag(input, flag);

		float camX = getNextConsoleFloat(input);
		float camY = getNextConsoleFloat(input);				

		float screenX = getNextConsoleFloat(input);
		float screenY = getNextConsoleFloat(input);
		float width = getNextConsoleFloat(input);
		float height = getNextConsoleFloat(input);
		Layer layer = getNextConsoleInt(input);

		if (strcmp(flag, "-main") == 0)
		{
			addMainCameraView(screenX, screenY, width, height, layer, GameWorld);
		}
		else
		{
			addCameraView(camX, camY, GameWorld->MainCamera.width, GameWorld->MainCamera.height, screenX, screenY, width, height, layer, GameWorld);
		}
	}
	else if (strcmp(arg, "attach") == 0)
	{
		int index = getNextConsoleInt(input);
		Object *attach = parseArgumentToFindObject(input, &GameWorld->ObjectList);
		
		if (attach == NULL)
		{
			return INVALID_DATA;
		}

		attachCameraViewToObject(getCameraView(GameWorld, index), attach);
	}
	else if (strcmp(arg, "setrefresh") == 0)
	{
		int index = getNextConsoleInt(input);
		CameraView *camView = getCameraView(GameWorld, index);

		if (camView == NULL)
		{
			return INVALID_DATA;
		}

		camView->ticksUntilRefresh = getNextConsoleInt(input);
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_Sound(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, arg);
	stringToLower(input);

	if (strcmp(arg, "play") == 0)
	{
		char name[USER_INPUT_MAX_LEN] = {0};
		getNextConsoleArg(input, name);
		float volume = getNextConsoleFloat(input);
		if (volume < 0.01)
		{
			volume = 1.0;
		}

		ChannelName channel = getNextConsoleInt(input);

		if (PlaySound(name, volume, channel) == NULL)
		{
			putConsole("Couldn't play or find '%s'", name);
		}

	}
	else if (strcmp(arg, "cache") == 0)
	{
		putConsoleCachedSounds();
	}
	else if (strcmp(arg, "stopchannel") == 0)
	{
		int channel = getNextConsoleInt(input);

		StopAudioInChannel(channel);
	}
	else if (strcmp(arg, "stopall") == 0 || strcmp(arg, "StopAllAudio") == 0)
	{
		StopAllAudio();
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_Cutscene(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, arg);

	if (strcmp(arg, "play") == 0 || strcmp(arg, "start") == 0)
	{
		getNextConsoleArg(input, arg);

		if (inRange(arg[0], '0', '9'))
		{
			playCutscene(atoi(arg), GameWorld);
		}
		else
		{
			playCutsceneFromFile(arg, GameWorld);
		}
	}
	else if (strcmp(arg, "stop") == 0)
	{
		if (GameWorld->CurrentCutscene != NO_CUTSCENE)
		{
			GameWorld->CurrentCutscene = END_CUTSCENE;
			GameWorld->MainCamera.CameraMode = FOLLOW_PLAYER;
			if (GameWorld->TextQueue != NULL)
			{
				clearTextQueue(GameWorld);
			}
		}
	}
	else
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_Load(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, arg);

	if (strcmp(arg, "spriteset") == 0)
	{
		int ID = getNextConsoleInt(input);

		loadSpriteSet(&GameWorld->ObjectList, ID);
	}
	else if (strcmp(arg, "audio") == 0)
	{
		getNextConsoleArg(input, arg);

		loadAudio(arg);
	}
	else
	{
		return INVALID_DATA;
	}

		return LEMON_SUCCESS;
}

int ConsoleCommand_DebugText(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, arg);

	if (strcmp(arg, "info") == 0)
	{
		printTextListinfo(&TextSettings.DebugTextList, "TextList");
	}
	else 
	{
		return INVALID_DATA;
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_Pause(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	DebugSettings.PauseEngine = (DebugSettings.PauseEngine + 1) % 2;

	if (DebugSettings.PauseEngine == 1)
	{
		putConsoleTS("Engine is now paused.");
	}
	else
	{
		putConsoleTS("Engine is now unpaused.");
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_SetPos(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	float x = getNextConsoleFloat(input);
	float y = getNextConsoleFloat(input);

	GoTo(GameWorld->Player.PlayerPtr, x, y);

	return LEMON_SUCCESS;
}

int ConsoleCommand_SetCamPos(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	float x = getNextConsoleFloat(input);
	float y = getNextConsoleFloat(input);

	setCameraPos(&GameWorld->MainCamera, x, y);

	return LEMON_SUCCESS;
}

int ConsoleCommand_SetCamZoom(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	if (input[DebugSettings.argIndex] == 0)
	{
		GameWorld->MainCamera.zoomX = 1.0;
		GameWorld->MainCamera.zoomY = 1.0;
		return LEMON_SUCCESS;
	}

	float zoomX =  getNextConsoleFloat(input);
	float zoomY =  getNextConsoleFloat(input);
	GameWorld->MainCamera.zoomX = zoomX;
	GameWorld->MainCamera.zoomY = zoomY;

	return LEMON_SUCCESS;
}

int ConsoleCommand_SetTickRate(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int rate = getNextConsoleInt(input);

	setTickRate(rate);

	return LEMON_SUCCESS;
}

int ConsoleCommand_Save(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int save = getNextConsoleInt(input);
	saveGame(save, GameWorld);

	return LEMON_SUCCESS;
}

int ConsoleCommand_SaveSettings(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int save = getNextConsoleInt(input);
	saveSettings(save, GameWorld);

	return LEMON_SUCCESS;
}

int ConsoleCommand_LoadSave(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int save = getNextConsoleInt(input);
	loadSave(save, GameWorld);

	return LEMON_SUCCESS;
}

int ConsoleCommand_LoadSettings(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	loadSettings(getNextConsoleInt(input), GameWorld);

	return LEMON_SUCCESS;
}

int ConsoleCommand_AddGameFlag(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char name[USER_INPUT_MAX_LEN] = {0};

	getNextConsoleArg(input, name);

	int startVal = getNextConsoleInt(input);

	addGameFlag(name, startVal);

	return LEMON_SUCCESS;
}

int ConsoleCommand_SetGameFlag(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int index;

	if (nextConsoleArgIsNumber(input))
	{
		index = getNextConsoleInt(input);
	}
	else
	{
		char name[USER_INPUT_MAX_LEN] = {0};
		getNextConsoleArg(input, name);
		index = getGameFlag(name);
	}

	int newValue = getNextConsoleInt(input);

	if (index < 0 || index >= GAME_FLAG_COUNT)
	{
		putConsole("GameFlag does not exist");
		return LEMON_SUCCESS;
	}

	GameFlags[index].value = newValue;
	updateServerFlag(&GameFlags[index]);

	return LEMON_SUCCESS;
}

int ConsoleCommand_CheckGameFlag(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	int index;

	if (nextConsoleArgIsNumber(input))
	{
		index = getNextConsoleInt(input);
	}
	else
	{
		char name[USER_INPUT_MAX_LEN] = {0};
		getNextConsoleArg(input, name);
		index = getGameFlag(name);
	}

	if (index < 0 || index >= GAME_FLAG_COUNT)
	{
		putConsole("GameFlag does not exist");
	}
	else
	{
		putConsole("(%d)\"%s\": %d", index, GameFlags[index].name, GameFlags[index].value);
	}

	return LEMON_SUCCESS;
}


int ConsoleCommand_Help(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	char arg[USER_INPUT_MAX_LEN] = {0};
	getNextConsoleArg(input, arg);

	if (arg[0] != '\0')
	{
		ConsoleCommand *command = getConsoleCommand(arg);
		if (command != NULL)
		{
			putConsole("%s - %s", command->formatString, command->helpString);

			return LEMON_SUCCESS;
		}

		ConsoleVariable *variable = getConsoleVariable(arg);
		if (variable != NULL)
		{
			putConsole("%s - %s", variable->name, variable->helpString);

			return LEMON_SUCCESS;
		}

		putConsole("Couldn't find any command/variable with the name '%s'", arg);
	}

	ConsoleCommand *commands = DebugSettings.commands;

	// find matching command to print help string for
	for (int i = 0; i < MAX_CONSOLE_COMMANDS; i++)
	{
		if (commands[i].name[0] != '\0')
		{
			putConsole("%s - %s", commands[i].formatString, commands[i].helpString);
		}
	}

	ConsoleVariable *variables = DebugSettings.consoleVariables;

	// find matching command to print help string for
	for (int i = 0; i < MAX_CONSOLE_VARIABLES; i++)
	{
		if (variables[i].name[0] != '\0')
		{
			putConsole("%s - %s", variables[i].name, variables[i].helpString);
		}
	}

	return LEMON_SUCCESS;
}

int ConsoleCommand_DoABarrelRoll(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	Object *player = GameWorld->Player.PlayerPtr;
	if (player == NULL)
	{
		return LEMON_SUCCESS;
	}

	if (player->State == PAUSE_STATE)
	{
		if (objectPlayingThisAnimation(player, "DoABarrelRoll"))
		{
			player->State = DEFAULT_STATE;
			stopAnimation(player->ObjectDisplay);
		}
	}
	else if (player->State == DEFAULT_STATE)
	{
		player->State = PAUSE_STATE;
		PlayAnimation("BarrelRoll", 0, player->ObjectDisplay);
	}
	
	return LEMON_SUCCESS;
}

int ConsoleCommand_Noclip(char input[USER_INPUT_MAX_LEN], World *GameWorld)
{
	if (GameWorld->Player.PlayerPtr == NULL)
	{
		putConsole("No Player character found!");
		DebugSettings.noclip = false;
		return LEMON_SUCCESS;
	}

	DebugSettings.noclip = !DebugSettings.noclip;
	PhysicsBox *PlayerBox = GameWorld->Player.PlayerPtr->ObjectBox;

	if (DebugSettings.noclip == true)
	{
		putConsole("NoClip is on");

		PlayerBox->solid = UNSOLID;
		SetPhysicsGravity(GameWorld->Player.PlayerPtr, false, GameWorld);		
	}
	else
	{
		putConsole("NoClip is off");

		PlayerBox->solid = BODY;
		SetPhysicsGravity(GameWorld->Player.PlayerPtr, true, GameWorld);
	}
	
	return LEMON_SUCCESS;
}


void updateConsoleHistoryText(Text *input)
{
	Text *consoleHistory = input;

	if (consoleHistory == NULL)
	{
		consoleHistory = getDebugTextWithName("ConsoleHistory");
	}
	
	if (consoleHistory == NULL)
	{
		return;
	}

	static const int consoleLinesDisplayed = 32;
	int firstLineOffset = consoleLinesDisplayed + clamp(DebugSettings.scrollVal, 0, INPUT_HISTORY_LEN - consoleLinesDisplayed);
	int index = modulo(DebugSettings.consoleHistory.head - firstLineOffset, INPUT_HISTORY_LEN);
	char all[USER_INPUT_MAX_LEN * INPUT_HISTORY_LEN] = {0};


	for (int i = consoleLinesDisplayed; i > 0; i--)
	{
		strcat(all, DebugSettings.consoleHistory.inputs[index]);
		index = (index + 1) % INPUT_HISTORY_LEN;

		if (i > 1)
		{
			all[strlen(all)] = '\n';
		}
	}

 	updateText(consoleHistory, all);

 	// reposition
 	int height = 0;
    TTF_GetTextSize(consoleHistory->text, NULL, &height);

    consoleHistory->yPos = DebugSettings.consoleYPos + insideSpacing + (float)height;
	
	return;
}

void renderConsole(World *GameWorld, SDL_Renderer *Screen)
{
	float xCorrection = (float)(ScreenData.screenWidth >> 1);
	float yCorrection = (float)(ScreenData.screenHeight >> 1);

	static const float inputFieldHeight = 28.0;

	SDL_FRect box = {0};

	// render input field
	box.x = xCorrection + DebugSettings.consoleXPos;
	box.y = -(DebugSettings.consoleYPos - yCorrection);
	box.w = consoleWidth;
	box.h = inputFieldHeight;

	if (DebugSettings.consoleFocus)
	{
		SDL_SetRenderDrawColor(Screen, 0x2D, 0x2A, 0x2A, 0xBB);
	}
	else
	{
		SDL_SetRenderDrawColor(Screen, 0x36, 0x32, 0x32, 0xBB);
	}
	
	SDL_RenderFillRect(Screen, &box);

	// render console
	box.h = consoleHeight;
	box.y -= consoleHeight;
	SDL_SetRenderDrawColor(Screen, 0x1D, 0x1A, 0x1A, 0xBB);
	SDL_RenderFillRect(Screen, &box);


	// update text history
	static Uint64 lastUpdated = 0;
	static int lastScrollVal = 0;
	if (lastUpdated == DebugSettings.consoleHistory.inputCount && lastScrollVal == DebugSettings.scrollVal)
	{
		return;
	}

	lastUpdated = DebugSettings.consoleHistory.inputCount;
	lastScrollVal = DebugSettings.scrollVal;

	updateConsoleHistoryText(NULL);


	return;
}