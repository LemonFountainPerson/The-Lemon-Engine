# ObjectType

## Definition
```
typedef enum ObjectType 
{
	LEVEL_FLAG_OBJ = 0,
	SOLID_BLOCK,	
	FLAT_SLOPE_FLOOR,
	JUMP_THRU_BLOCK,
	PLAYER_OBJECT,
	UI_ELEMENT,
	UI_TEXT,
	PARTICLE,
	COIN,
	SPRING,
	MOVING_PLATFORM_HOR,
	MOVING_PLATFORM_VER,
	GATE_SWITCH,
	GATE_SWITCH_TIMED,
	VERTICAL_GATE,
	HORIZONTAL_GATE,
	DOOR,
	LEVEL_DOOR,
	PUSHABLE_BOX,
	PROJECTILE,
	BASIC_ENEMY,
	OBJECT_TYPE_COUNT,
	UNDEFINED_OBJECT
} ObjectType;
```

## Description
This enum represents the type of an [Object](Data_Object.md), defining its animations and behaviour.
The type is more of an organisational tool, as it essentially is used to decide what function to call when the Object is updated during a Game Tick.

## Version
Available since V0.04.


----