# ConsoleFlag

## Definition
```
typedef enum ConsoleFlag
{
	CONFLAG_NONE		= 0x0000,
	CONFLAG_CHEAT 		= 0x0001,
	CONFLAG_SERVER_SIDE = 0x0002,
	CONFLAG_NOTIFY		= 0x0004,
	CONFLAG_PROTECTED 	= 0x0008,
	CONFLAG_SVR_AND_PRO	= CONFLAG_PROTECTED | CONFLAG_SERVER_SIDE
} ConsoleFlag;
```

## Description
This enum defines any additional attributes for a [ConsoleCommand](Data_ConsoleCommand.md) or [ConsoleVariable](Data_ConsoleVariable.md) 
to follow. 

|                  Name                          |    Affects Commands     |      Description      |
| ---------------------------------------------- | ----------------------- | --------------------- |
| CONFLAG_CHEAT | Yes | This flag means the command can only be run if cheats are enabled. |
| CONFLAG_SERVER_SIDE | Yes | This flag means the command is intended to be controlled by the server host, and so has no effect when not online. As a server host, this means the command can be broadcasted to all clients. As a client, it means you cannot run this command unless the server asks you to. |
| CONFLAG_NOTIFY | No | This flag will cause a message to be sent to the chat whenever the variable is modified. |
| CONFLAG_PROTECTED | Yes | This flag means the command will never be broadcasted across the network or have its information shown to the user, even if it also holds the 'CONFLAG_NOTIFY' flag. |
| CONFLAG_SVR_AND_PRO | Yes | This is not a real flag, and is simply a convinience combination of 'CONFLAG_SERVER_SIDE' and 'CONFLAG_PROTECTED'. |



## Version
Available since V0.11.


----