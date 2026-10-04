# Camera

## Definition
```
typedef struct Camera 
{
	float CameraX;
	float CameraY;
	float prevCameraX;
	float prevCameraY;

	float minCameraX;
	float maxCameraX;
	float minCameraY;
	float maxCameraY;

	float zoomX;
	float zoomY;
	int width;
	int height;
	int zoomedWidth;
	int zoomedHeight;

	float CameraXBuffer;
	float CameraYBuffer;
	CameraState CameraMode;
} Camera;
```

## Description
The camera represents a rectangular view in a [World's](Data_World.md) space that can then be rendered to the screen.
The camera is centered on 'CameraX' and 'CameraY' and has several controls such as an X and Y zoom value (with 1.0 as the default), a 
[CameraState](Enum_CameraState.md) that can automate the behaviour of the camera (such as following the player when set to 'FOLLOW_PLAYER')
and min/max values for the X and Y axis that determine boundaries for the camera's position.

## Version
Available since V0.05.

----
