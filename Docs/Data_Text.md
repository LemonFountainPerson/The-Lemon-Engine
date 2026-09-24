# Text

## Definition
```
typedef struct Text
{
	float xPos;
	float yPos;

	bool CameraRelative;

	TTF_Text *text;
	Font *usedFont;

	Object *attachedObj;
	int recordedInstance;

	char name[TEXT_NAME_MAX_LEN];
} Text;
```

## Description
This struct represents a renderable string of text in the GameWorld, and can be created with [AddText()](AddText.md).
If attached to an Object via [AttachTextToObject()](AttachTextToObject.md), then deleting the Object will delete the Text as well.
Otherwise, you must call [RemoveText()](RemoveText.md) or [RemoveTextFromList()](RemoveTextFromList.md) to dispose of it.


## Version
Available since V0.10.

----
