# GameEvent

## Definition
```
typedef struct GameEvent
{
	GameEventID EventID;
	GameEventArg args[EVENT_VAR_COUNT];

	int clientID;
} GameEvent;
```

## Description
GameEvents are used to trigger actions in the engine, and can be triggered by [Objects](Data_Object.md),
[SceneActions](Data_SceneAction.md), servers, etc.

The '[EventID](Enum_GameEventID.md)' value defines the type of event to be executed.  

The '[args](Data_GameEventArg.md)' array is a collection of possible arguments to be used by the GameEvent.


## Version
Available since V0.08.

----
