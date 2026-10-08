# RotationMode

## Definition
```
typedef enum RotationMode 
{
	NORMAL_ROTATION = 0,
	LEFT_RIGHT_ROTATION,
	DONT_ROTATE
} RotationMode;
```

## Description
Used by [DisplayDatas](Data_DisplayData.md) to determine how to calculate the direction of the rendered [Sprite](Data_Sprite.md).
For example, LEFT_RIGHT_ROTATION flips the rotation if the [Object](Data_Object.md) the sprite is attached to is flipped on the x-axis.

## Version
Available since V0.09.


----