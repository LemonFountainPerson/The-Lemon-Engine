# SceneAction

## Definition
```
typedef struct SceneAction
{
	SceneActionID ActionID;

	SceneActionArguments ActionData;

	struct SceneAction *nextSceneAction;
	struct SceneAction *prevSceneAction;
} SceneAction;
```

## Description
SceneActions are stored as a linked list created when a cutscene starts, in order to represent the actions for the
scene to perform. It can describe control flow such as loops, if statements and branches. Each SceneAction is defined
by their [ActionID](Enum_SceneActionID.md) and their arguments. The queue of these actions are allocated and so the
[EndCutscene()](Func_EndCutscene.md) function is responsible for cleaning up the data.


## Version
Available since V0.08.

----
