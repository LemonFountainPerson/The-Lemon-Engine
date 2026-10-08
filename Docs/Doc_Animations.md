# Animations
_________________________________________________


The animation system is operated through the spriteSets and the DisplayData. They are stored with [AnimationFrame](Data_AnimationFrame.md) structs as an array stored in 
an [Animation](Data_Animation.md) struct representing each animation. These Animation structs are stored as a linked list from the animations pointer located
in the [SpriteSet](Data_SpriteSet.md).


The currently playing animation is referenced by the DisplayData in the 'animationBuffer' pointer, and by the 'currentAnimation' variable which refers to the animation's ID. 
'CurrentAnimation' is set to the ID value of the currently playing animation and is set to 0 when no animation is playing. The 'currentFrame' and 'frameBuffer' variables
store the frame number and a reference to that frame respectively. 
To play an animation, the [PlayAnimation](Func_PlayAnimation.md) function is called, with the number of repititions being the second arguement. (0 for repeating infinitely.)

The 'animationBuffer' and 'frameBuffer' pointers should not and don't ever have to be modified other than by the engine itself. If you want to manually control which animation
or which frame is playing, you can simply set the 'currentAnimation' and 'currentFrame' variables, and it will assign the correct data automatically.


