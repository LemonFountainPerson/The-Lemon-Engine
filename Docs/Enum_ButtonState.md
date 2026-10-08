# ButtonState

## Definition
```
typedef enum ButtonState
{
	BUTTON_RELEASED = 0,
	BUTTON_PRESSED = 1,
	BUTTON_HELD = 2
} ButtonState;
```

## Description
This enum is used to represent the state of a button (a controller element that is either pressed or not pressed).
'BUTTON_RELEASED' and 'BUTTON_PRESSED' correspond to the button being not pressed and pressed respectively, but the
'BUTTON_HELD' state is when a button has been pressed for longer than 1 GameTick. 

When a button is first pressed, its entry in the Buttons array is set to 'BUTTON_PRESSED', but after a Tick passes all 
buttons with this state get set to 'BUTTON_HELD'. Functions like [ButtonPressed()](Func_ButtonPressed.md) check whether 
the button has just been pressed, where as the function [ButtonHeld()](Func_ButtonHeld.md) checks whether it is pressed 
or in the held down state. Conveniently, checking if Buttons[KEY] is true is equivalent to checking if the button is 
being held down.

## Version
Available since V0.10.


----
