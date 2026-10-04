# GameEventArg

## Definition
```
typedef struct GameEventArg
{
	char name[EVENT_ARG_NAME_MAX_LEN];
	ArgType type;	
	ArgData data;
} GameEventArg;
```

## Description
GameEventArgs are designed to accept any type of argument you might want to store, such as an Int, Float,
String, etc. Different [GameEvents](Data_GameEvent.md) use different sets of arguments, so each one should
have a unique name to identify them.


## Version
Available since V0.10.

----
