# SpriteSet

## Definition
```
typedef struct SpriteSet
{
	Sprite *firstSprite;
	Sprite *lastSprite;

	struct SpriteSet *nextSet;
	struct SpriteSet *prevSet;

	Animation *Animations;

	int setID;
	int spriteCount;

	int copyCount;
	int *copies;
} SpriteSet;
```

## Description
Used to store and manage [Sprites](Data_Sprite.md) and [Animations](Data_Animation.md). A SpriteSet has a 'setID' which 
corresponds to an associated [Object Type](Enum_ObjectType.md), as each type has their own SpriteSet.

SpriteSets can be shared among different Object types if a set is a 'copy' of another set. If a set is a 'copy' of 
another, you can still add to this set as if it is a seperate one, but internally a copy is actually just a reference 
to the set being copied so adding to the copy actually just adds to the copied set. 


## Version
Available since V0.04.


----