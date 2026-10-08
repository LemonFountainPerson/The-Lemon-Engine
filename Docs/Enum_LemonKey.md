# LemonKey

## Definition
```
typedef enum LemonKey
{
	// 0 - 31 are control charcacters and should not be used
	// 32 - 127 are reserved for ASCII keys
	ACKNOWLEDGE_INPUT = 0,
	LMN_ESCAPE = 128,
	LMN_GRAVE,
	LMN_ENTER,
	LMN_SPACE,
	LMN_TAB,
	LMN_LSHIFT,
	LMN_RSHIFT,
	LMN_BACKSPACE,
	LMN_COMMA,
	LMN_PERIOD,
	LMN_SLASH,
	LMN_UPARROW,
	LMN_DOWNARROW,
	LMN_LEFTARROW,
	LMN_RIGHTARROW,
	LMN_UP = 150,
	LMN_DOWN = 151,
	LMN_LEFT = 152,
	LMN_RIGHT = 153,
	LMN_JUMP = 154,
	LMN_INTERACT = 155,
	LMN_INTERACT2 = 156,
	LMN_INTERACT3 = 157,
	LMN_TEXT_CONFIRM = 158,
	LMN_TEXT_SKIP = 159,
	LMN_MENU_CONFIRM = 160,
	LMN_MENU_OPEN,
	LMN_TYPING_END,
	LMN_CONSOLE_OPEN,

	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_SIDE1,
	MOUSE_SIDE2,

	GAMEPAD_WEST,
	GAMEPAD_SOUTH,
	GAMEPAD_EAST,
	GAMEPAD_NORTH,
	GAMEPAD_DPAD_LEFT,
	GAMEPAD_DPAD_DOWN,
	GAMEPAD_DPAD_RIGHT,
	GAMEPAD_DPAD_UP,
	GAMEPAD_START,
	GAMEPAD_BACK,
	GAMEPAD_GUIDE,
	GAMEPAD_LEFT_SHOULDER,
	GAMEPAD_RIGHT_SHOULDER,
	GAMEPAD_LEFT_STICK,
	GAMEPAD_RIGHT_STICK,

	INPUT_COUNT
} LemonKey;
```

## Description
This enum represents all readable button inputs by the engine. These values are mapped to the global 'Buttons' array that
can be indexed to find the state of the corresponding button, using the [ButtonState](Enum_ButtonState.md) enum as its value.

## Version
Available since V0.04.


----