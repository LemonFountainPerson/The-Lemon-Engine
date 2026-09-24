# Object

## Definition
```
typedef struct Object
{
	char name[OBJECT_NAME_LENGTH];	// unique identifier for Object 		
	int ObjectID;
	ObjectState State;
	CurrentAction Action;
	ReservedFlags reserved;
	int instanceNumber;

	struct Object *Parent;
	ParentType ParentLink;

	PhysicsBox * const ObjectBox;
	DisplayData * const ObjectDisplay;

	struct Object *nextObject;
	struct Object *prevObject;	
	const int index;

	// Multi-purpose args
	int arg1;
	int arg2;
	int arg3;
	int arg4;
} Object;
```

## Description
The Object struct is used to represent a distinct instance of an [Object Type](Enum_ObjectType.md) within the GameWorld. 
They will always have an attached [PhysicsBox](Data_PhysicsBox.md) and a [DisplayData](Data_DisplayData.md), and can have additional components added on to expand its functionality.


## Version
Available since V0.04.

----
