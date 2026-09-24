# SceneAction

## Definition
```
typedef struct SceneAction
{
	SceneActionID ActionID;
	bool parallelAction;

	Object *ActorObject;
	union SceneActionArguments ActionData;

	struct SceneAction *nextSceneAction;
	struct SceneAction *prevSceneAction;
} SceneAction;
```

## Description


## Version
Available since V0.08.

----
