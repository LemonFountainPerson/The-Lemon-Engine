# CameraState

## Definition
```
typedef enum CameraState 
{
	FOLLOW_PLAYER = 0,
	FREE_ROAM = 1,
	FREE_ROAM_RESTRICTED = 2,
	MENU_CAMERA = 3,
	UNDEFINED_CAMERA_STATE
} CameraState;
```

## Description
This enum defines the automatic behaviour of a [Camera](Data_Camera.md). All of these behaviours can be modified via the 
[CameraControl()](Func_CameraControl.md) function. 
- FOLLOW_PLAYER: The camera will follow the [Player](Data_PlayerData.md), if one exists.
- FREE_ROAM: The camera does not move automatically, and can be moved anywhere.
- FREE_ROAM_RESTRICTED: The camera does not move automatically, and its position isrestricted to within its min/max bounds.
etc.

## Version
Available since V0.05.


----