# World

## Definition
```
typedef struct World
{
	Camera MainCamera;
	CameraView views[VIEW_COUNT];

	PlayerData Player;
	ObjectController ObjectList;

	BackgroundData WorldBackground;

	TextList TextList;
	FontList FontList;

	int GamePaused;
	int level;
	LemonGameState GameState;
	GameEventManager GameEvents;
	
	CutsceneID CurrentCutscene;
	bool TextBox;
	SceneAction *SceneActionQueue;
	SceneAction *nextSceneAction;
	int SceneActionCount;

	WorldPhysics PhysicsType;
	float GlobalGravityY;
	float GlobalGravityX;
} World;
```

## Description
The Backbone of the entire Lemon Engine, this struct represents an entire world with [Objects](Data_Object.md), [GameEvents](Data_GameEvent.md), [SceneActions](Data_SceneAction.md), etc. 
A World represents the game's entire state and switching to another is equivalent to switching the type of game being played.
The state of the application window, [Developer Console](Doc_DeveloperConsole.md) and settings are not contained within the World.


## Version
Available since V0.04.

----
