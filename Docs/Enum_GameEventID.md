# GameEventID

## Definition
```
typedef enum GameEventID
{
	NO_EVENT = 0,
	EVENT_SWITCH_LEVEL,
	EVENT_PLAY_CUTSCENE,
	EVENT_PLAY_CUTSCENE_FROM_FILE,
	EVENT_SET_GAME_FLAG,
	EVENT_CHANGE_GAME_FLAG,
	EVENT_PLAY_SOUND,
	EVENT_CHAT_MESSAGE,
	EVENT_MOVE_PLAYER,
	EVENT_TELEPORT_PLAYER_TO_EXIT_DOOR,
	EVENT_LOAD_LEVEL_PARTITION,
	EVENT_CONSOLE_COMMAND,
	EVENT_SET_TICKRATE,
	EVENT_SET_BRIGHTNESS,
	EVENT_SET_SCREEN_SIZE,
	EVENT_SET_SCREEN_SIZE_SCALE,
	EVENT_ENABLE_FULLSCREEN,
	EVENT_DISABLE_FULLSCREEN,
	EVENT_ENABLE_FULLSCREEN_SCALE,
	EVENT_COUNT,
	UNDEFINED_EVENT
} GameEventID;
```

## Description
This enum describes all possible [GameEvents](Data_GameEvent.md). The Game Event system is designed to be
easily added to with your own event types. In order to do so, start by adding a new entry to this enum before
the 'EVENT_COUNT' value.

## Version
Available since V0.06.


----