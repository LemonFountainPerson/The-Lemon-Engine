#include "LemonEngine.h"

void InitialiseObjectController(ObjectController *newController)
{
	if (newController == NULL)
	{
		return;
	}

	newController->lastObject = NULL;
	newController->firstObject = NULL;
	InitialiseSpriteSetList(&newController->spriteSets);
	newController->availableSlots = NULL;
	newController->FrameUpdates = NULL;

	InitialiseComponents(newController);
	
	ComponentData *newArena = &newController->objectComponents;

	int i = EngineSettings.MaxObjects - 1;
	Object *newObject = NULL;

	while (i >= 0)
	{
		newObject = &newArena->Objects[i];
		clearObjectData(newObject);

		// This pointer nonsense is used to circumvent the const modifier; this should not be used elsewhere as these values should not change
		// pointers to the objectbox, objectdisplay and index values of the object are cast to regular values without const, before being dereferenced to be assigned with new values
		(*(PhysicsBox * *)&newObject->ObjectBox) = &newArena->PhysicsBoxes[i];
		(*(DisplayData * *)&newObject->ObjectDisplay) = &newArena->Displays[i];
		*((int *)&newObject->index) = i;

		if (newController->availableSlots != NULL)
		{
			newController->availableSlots->prevObject = newObject;
		}

		newObject->nextObject = newController->availableSlots;
		newController->availableSlots = newObject;
		i--;
	}


	InitialiseBSPTree(&newController->staticGeometry);

	return;
}

void ClearObjectController(ObjectController *ObjectList)
{
	if (ObjectList == NULL)
	{
		return;
	}

	// texts might be attached to objects
	RemoveObjectDebugTexts();

	ClearBSPTree(&ObjectList->staticGeometry);
	deleteAllObjects(ObjectList);	

	deleteAllSpriteSets(&ObjectList->spriteSets);

	return;
}

void InitialiseBSPTree(BSPTree *input)
{
	input->nodeCount = 1;
	input->maxHeight = 1;
	input->root = NULL;

	return;
}

int ClearBSPTree(BSPTree *input)
{
	if (input == NULL)
	{
		return MISSING_DATA;
	}

	DeleteBSPNode(input->root);
	input->root = NULL;

	input->nodeCount = 0;
	input->maxHeight = 0;

	return LEMON_SUCCESS;
}

void DeleteBSPNode(BSPNode *node)
{
	if (node == NULL)
	{
		return;
	}

	if (node->left != NULL)
	{
		DeleteBSPNode(node->left);
	}

	if (node->right != NULL)
	{
		DeleteBSPNode(node->right);
	}

	if (node->objects != NULL)
	{
		free(node->objects);
	}
	
	free(node);

	return;
}


int BSPComparison(PhysicsBox *inputBox, BSPNode *node)
{
	float position;
	float size;

	if (node->axis == 'x')
	{
		position = inputBox->xPos;
		size = inputBox->xSize;
	}
	else
	{
		position = inputBox->yPos;
		size = inputBox->ySize;
	}

	if (position > node->edgePos)
	{
		return 1;
	}
	else if (position + size < node->edgePos)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

// unused; this generates a BSP from given list of objects all at once
int BuildBSPTree(BSPTree *input, Object *firstObject)
{
	if (input == NULL || firstObject == NULL)
	{
		return MISSING_DATA;
	}

	float xBound = GetConVarAsFloat("bsp_rangex");
	float yBound = GetConVarAsFloat("bsp_rangey");

	PhysicsBox area = {0};
	area.xPos = -xBound / 2.0;
	area.yPos = -yBound / 2.0;
	area.xSize = xBound;
	area.ySize = yBound;

	int objectsInArea = 0;
	Object *current = firstObject;
	while (current != NULL)
	{
		if (CheckBoxOverlapsBoxBroad(current->ObjectBox, &area) && CheckBoxOverlapsBox(current->ObjectBox, &area))
		{
			objectsInArea++;
		}
		current = current->nextObject;
	}
	
	SDL_FRect nodeArea;
	nodeArea.x = -xBound / 2.0;
	nodeArea.y = -yBound / 2.0;
	nodeArea.w = xBound;
	nodeArea.h = yBound;

	input->root = BuildSubBSPTree(nodeArea, 0, 'x', objectsInArea, firstObject);

	return LEMON_SUCCESS;
}

BSPNode* BuildSubBSPTree(SDL_FRect nodeArea, int height, char axis, int totalObjects, Object *firstObject)
{
	if (nodeArea.w < MINIMUM_BSP_SIZE || nodeArea.h < MINIMUM_BSP_SIZE || height > BSPNODE_MAX_DEPTH)
	{
		return NULL;
	}

	PhysicsBox area = {0};
	area.xPos = nodeArea.x;
	area.yPos = nodeArea.y;
	area.xSize = (int)nodeArea.w;
	area.ySize = (int)nodeArea.h;

	int objectsInArea = 0;
	Object *current = firstObject;
	while (current != NULL)
	{
		if (current->State != STATIC_STATE || current->ObjectBox->solid == UNSOLID)
		{
			current = current->nextObject;
			continue;
		}

		if (CheckBoxOverlapsBoxBroad(current->ObjectBox, &area) && CheckBoxOverlapsBox(current->ObjectBox, &area))
		{
			objectsInArea++;
		}

		current = current->nextObject;
	}

	if (objectsInArea < 1)
	{
		return NULL;
	}

	BSPNode *newNode = malloc(sizeof(BSPNode));
	if (newNode == NULL)
	{
		return NULL;
	}

	newNode->left = NULL;
	newNode->right = NULL;
	newNode->objectCount = 0;
	newNode->objects = NULL;
	newNode->xPos = nodeArea.x;
	newNode->yPos = nodeArea.y;
	newNode->xSize = nodeArea.w;
	newNode->ySize = nodeArea.h;
	newNode->height = height;
	newNode->axis = axis;

	if (axis == 'x')
	{	
		newNode->edgePos = nodeArea.x + (nodeArea.w / 2.0);
	}
	else
	{
		newNode->edgePos = nodeArea.y + (nodeArea.h / 2.0);
	}

	if (objectsInArea > GetConVarAsInt("bsp_threshold"))
	{
		SDL_FRect left, right;
		left = nodeArea;
		right = nodeArea;

		// recursively construct a smaller node, only if a smaller subset is possible
		if (axis == 'x')
		{	
			left.w = (nodeArea.w / 2.0);
			right.x += left.w;
			right.w = left.w;

			newNode->left = BuildSubBSPTree(left, height + 1, 'y', objectsInArea, firstObject);
			newNode->right = BuildSubBSPTree(right, height + 1, 'y', objectsInArea, firstObject);	
		}
		else
		{
			left.h = (nodeArea.h / 2.0);
			right.y += left.h;
			right.h = left.h;

			newNode->left = BuildSubBSPTree(left, height + 1, 'x', objectsInArea, firstObject);
			newNode->right = BuildSubBSPTree(right, height + 1, 'x', objectsInArea, firstObject);
		}

		if (newNode->left != NULL || newNode->right != NULL)
		{
			return newNode;
		}
	}

	// create object references
	newNode->objects = malloc(sizeof(Object *) * objectsInArea);
	if (newNode->objects == NULL)
	{
		free(newNode);
		return NULL;
	}

	memset(newNode->objects, 0, sizeof(Object *) * objectsInArea);

	current = firstObject;
	int i = 0;
	while (current != NULL && i < objectsInArea)
	{
		if (current->State != STATIC_STATE || current->ObjectBox->solid == UNSOLID)
		{
			current = current->nextObject;
			continue;
		}

		if (CheckBoxOverlapsBoxBroad(current->ObjectBox, &area) && CheckBoxOverlapsBox(current->ObjectBox, &area))
		{
			newNode->objects[i] = current;
			newNode->objectCount++;
			i++;
		}

		current = current->nextObject;
	}

	return newNode;
}


BSPNode* MakeChildNodeWithObjects(BSPNode *parent, char side, Object **list, int count)
{
	BSPNode *child = malloc(sizeof(BSPNode));
	if (child == NULL)
	{
		return NULL;
	}

	if (list != NULL && count > 0)
	{
		child->objects = malloc(sizeof(Object *) * count);
		if (child->objects == NULL)
		{
			free(child);
			return NULL;
		}

		
		memcpy(child->objects, list, sizeof(Object *) * count);
		child->objectCount = count;
	}
	else
	{
		child->objects = NULL;
		child->objectCount = 0;
	}
	
	child->left = NULL;
	child->right = NULL;
	child->height = parent->height + 1;
	child->xPos = parent->xPos;
	child->yPos = parent->yPos;

	if (parent->axis == 'x')
	{
		child->axis = 'y';
		child->xSize = parent->xSize / 2.0;
		child->ySize = parent->ySize;

		if (side == 'r')
		{
			child->xPos += child->xSize;
		}

		child->edgePos = child->yPos + (child->ySize / 2.0);
	}
	else
	{
		child->axis = 'x';
		child->xSize = parent->xSize;
		child->ySize = parent->ySize / 2.0;
		
		if (side == 'r')
		{
			child->yPos += child->ySize;
		}

		child->edgePos = child->xPos + (child->xSize / 2.0);
	}
 
	return child;
}

int AddObjectToBSP(Object *input, BSPNode *node)
{
	if (input == NULL || node == NULL)
	{
		return MISSING_DATA;
	}

	
	// found a leaf node
	if (node->left == NULL && node->right == NULL)
	{
		Object **objList = node->objects;
		int objectCount = node->objectCount;

		float newSize;
		if (node->axis == 'x')
		{
			newSize = node->ySize / 2.0;
		}
		else
		{
			newSize = node->xSize / 2.0;
		}

		if (newSize < MINIMUM_BSP_SIZE || node->height >= BSPNODE_MAX_DEPTH || objectCount < GetConVarAsInt("bsp_threshold"))
		{
			// if cannot/shouldn't split, then add object to this node
			Object **newList = malloc(sizeof(Object *) * (objectCount + 1));
			if (newList == NULL)
			{
				return LEMON_ERROR;
			}

			if (objList != NULL)
			{
				memcpy(newList, objList, sizeof(Object *) * objectCount);
				free(objList);
			}
			
			newList[objectCount] = input;

			node->objects = newList;
			node->objectCount = objectCount + 1;

			return LEMON_SUCCESS;
		}
		else
		{
			// if can split, split existing node's objects among children
			int i = 0;
			int leftIndex = 0;
			int rightIndex = 0;
			Object* leftList[objectCount];
			Object* rightList[objectCount];

			while (i < objectCount)
			{
				int comparison = BSPComparison(objList[i]->ObjectBox, node);
				if (comparison <= 0)
				{
					leftList[leftIndex] = objList[i];
					leftIndex++;
				}

				if (comparison >= 0)
				{
					rightList[rightIndex] = objList[i];
					rightIndex++;
				}

				i++;
			}

			if (leftIndex > 0)
			{
				node->left = MakeChildNodeWithObjects(node, 'l', leftList, leftIndex);
			}

			if (rightIndex > 0)
			{
				node->right = MakeChildNodeWithObjects(node, 'r', rightList, rightIndex);
			}

			free(node->objects);
			node->objects = NULL;
			node->objectCount = 0;
		}
	}

	// try to add object to child node(s)
	int comparison = BSPComparison(input->ObjectBox, node);
	if (comparison >= 0)
	{
		if (node->right == NULL)
		{
			node->right = MakeChildNodeWithObjects(node, 'r', &input, 1);
		}
		else
		{
			AddObjectToBSP(input, node->right);
		}
	}

	if (comparison <= 0)
	{
		if (node->left == NULL)
		{
			node->left = MakeChildNodeWithObjects(node, 'l', &input, 1);
		}
		else
		{
			AddObjectToBSP(input, node->left);
		}
	}

	return LEMON_SUCCESS;
}

int AddObjectToBSPTree(Object *input, BSPTree *tree)
{
	if (tree == NULL || input == NULL)
	{
		return MISSING_DATA;
	}

		// should only be the case if we are placing object into root node
	if (tree->root == NULL)
	{
		BSPNode *newNode = malloc(sizeof(BSPNode));
		if (newNode == NULL)
		{
			return LEMON_ERROR;
		}

		tree->root = newNode;

		newNode->left = NULL;
		newNode->right = NULL;
		newNode->height = 0;
		newNode->axis = 'x';

		float xBound = GetConVarAsFloat("bsp_rangex");
		float yBound = GetConVarAsFloat("bsp_rangey");

		newNode->xPos = -xBound / 2.0;
		newNode->xSize = xBound;
		newNode->yPos = -yBound / 2.0;
		newNode->ySize = yBound;
		newNode->edgePos = 0.0;

		newNode->objects = malloc(sizeof(Object *));
		if (newNode->objects != NULL)
		{
			newNode->objects[0] = input;
			newNode->objectCount = 1;
		}
		else
		{
			newNode->objectCount = 0;
		}

		return LEMON_SUCCESS;
	}
	else
	{
		return AddObjectToBSP(input, tree->root);
	}
}


BSPNode* RemoveObjectFromLeafNode(Object *input, BSPNode *node)
{
	Object **objects = node->objects;

	if (node->objects == NULL || (node->objectCount <= 1 && objects[0] == input))
	{
		if (objects != NULL)
		{
			free(objects);
		}
		
		free(node);
		return NULL;
	}

	int i = 0;
	while (i < node->objectCount && objects[i] != input)
	{
		i++;
	}

	if (i >= node->objectCount)
	{
		return node;
	}

	Object **newList = malloc(sizeof(Object *) * (node->objectCount - 1));
	if (newList == NULL)
	{
		return node;
	}

	int newListIndex = 0;
	int oldListIndex = 0;
	while (oldListIndex < node->objectCount)
	{
		if (oldListIndex != i)
		{
			newList[newListIndex] = objects[oldListIndex];

			newListIndex++;
		}

		oldListIndex++;
	}

	free(objects);
	node->objects = newList;
	node->objectCount--;

	return node;
}

void CombineLeafNodes(BSPNode *parent)
{
	BSPNode *left = parent->left;
	BSPNode *right = parent->right;

	int totalCount = left->objectCount + right->objectCount;
	if (left->objects == NULL || right->objects == NULL || totalCount > GetConVarAsInt("bsp_threshold"))
	{
		return;
	}

	parent->objects = malloc(sizeof(Object *) * totalCount);
	if (parent->objects == NULL)
	{
		return;
	}
	memset(parent->objects, 0, sizeof(Object *) * totalCount);

	parent->objectCount = totalCount;
	int parentIndex = 0;
	for (int leftIndex = 0; leftIndex < left->objectCount; leftIndex++, parentIndex++)
	{
		parent->objects[parentIndex] = left->objects[leftIndex];
	}

	for (int rightIndex = 0; rightIndex < left->objectCount; rightIndex++, parentIndex++)
	{
		parent->objects[parentIndex] = right->objects[rightIndex];
	}

	free(left->objects);
	free(left);
	parent->left = NULL;

	free(right->objects);
	free(right);
	parent->right = NULL;

	return;
}

void MoveLeafNodeUp(BSPNode *node)
{
	BSPNode *left = node->left;
	BSPNode *right = node->right;

	if (left != NULL && left->objectCount > 0)
	{
		// move left's list up to parent node
		node->objectCount = right->objectCount;
		node->objects = right->objects;
		free(left);
		node->left = NULL;
	}
	else if (right != NULL && right->objectCount > 0)
	{
		// move right's list up to parent node
		node->objectCount = right->objectCount;
		node->objects = right->objects;
		free(right);
		node->right = NULL;
	}

	return;
}

BSPNode* RemoveObjectFromBSP(Object *input, BSPNode *node)
{
	if (input == NULL || node == NULL)
	{
		return node;
	}

	PhysicsBox *box = input->ObjectBox;
	if (node->xPos > box->xPos + box->xSize || node->xPos + node->xSize < box->xPos || node->yPos > box->yPos + box->ySize || node->yPos + node->ySize < box->yPos)
	{
		return node;
	}

	if (node->left == NULL && node->right == NULL)
	{
		return RemoveObjectFromLeafNode(input, node);
	}

	int comparison = BSPComparison(input->ObjectBox, node);

	if (comparison == 1)
	{
		node->right = RemoveObjectFromBSP(input, node->right);
	}
	else if (comparison == -1)
	{
		node->left = RemoveObjectFromBSP(input, node->left);
	}
	else
	{
		node->right = RemoveObjectFromBSP(input, node->right);
		node->left = RemoveObjectFromBSP(input, node->left);
	}

	// if recursive steps above deleted both branches, delete this node too
	if (node->left == NULL && node->right == NULL)
	{
		free(node);
		return NULL;
	}
	
	if (node->left != NULL && node->right != NULL)
	{
		CombineLeafNodes(node);
	}
	else
	{
		// if here, then either left or right is null but not both, and the child is a leaf node
		MoveLeafNodeUp(node);
	}

	return node;
}

int RemoveObjectFromBSPTree(Object *input, BSPTree *tree)
{
	if (tree == NULL || tree->root == NULL || input == NULL)
	{
		return MISSING_DATA;
	}

	if (input->State != STATIC_STATE)
	{
		return ACTION_DISABLED;
	}

	tree->root = RemoveObjectFromBSP(input, tree->root);

	return LEMON_SUCCESS;
}

void renderBSPNode(BSPNode *input, Camera inputCamera, SDL_Renderer *Screen)
{
	if (input == NULL)
	{
		return;
	}

	PhysicsBox node = {0};
	node.xPos = input->xPos;
	node.xSize = input->xSize;
	node.yPos = input->yPos;
	node.ySize = input->ySize;
	node.solid = 7;
	renderHitbox(inputCamera, &node, Screen);

	// if leaf node, render all objects referenced in its list
	if (input->left == NULL && input->right == NULL)
	{
		int i = 0; 
		while (i < input->objectCount)
		{
			if (input->objects[i] != NULL)
			{
				renderHitbox(inputCamera, input->objects[i]->ObjectBox, Screen);
			}
			
			i++;
		}
	}
	else
	{
		renderBSPNode(input->left, inputCamera, Screen);
		renderBSPNode(input->right, inputCamera, Screen);
	}
	
	return;
}


Object* GetCollidingObjectFromBSPSubTree(PhysicsBox *inputBox, BSPNode *current)
{
	while (current != NULL && (current->left != NULL || current->right != NULL))
	{
		int comparison = BSPComparison(inputBox, current);

		if (comparison == 1)
		{
			current = current->right;
		}
		else if (comparison == -1)
		{
			current = current->left;
		}
		else
		{
			// when intersecting both?
			Object *otherPath = GetCollidingObjectFromBSPSubTree(inputBox, current->left);
			if (otherPath != NULL)
			{
				return otherPath;
			}

			current = current->right;
		}
	}

	if (current == NULL || current->objects == NULL)
	{
		return NULL;
	}

	int i = 0;
	Object **objects = current->objects;

	while (i < current->objectCount)
	{
		if (objects[i] != NULL && CheckBoxOverlapsBoxBroad(inputBox, objects[i]->ObjectBox))
		{
			if (CheckBoxCollidesBox(inputBox, objects[i]->ObjectBox))
			{
				return objects[i];
			}	
		}

		i++;
	}

	return NULL;
}

Object* GetCollidingObjectFromBSP(PhysicsBox *inputBox, ObjectController *ObjectList)
{
	return GetCollidingObjectFromBSPSubTree(inputBox, ObjectList->staticGeometry.root);
}

Object* GetCollidingObject(PhysicsBox *inputBox, ObjectController *ObjectList)
{
	if (inputBox == NULL || inputBox->solid == UNSOLID || ObjectList == NULL)
	{
		return NULL;
	}

	// check static objects
	Object *staticCheck = GetCollidingObjectFromBSP(inputBox, ObjectList);

	if (staticCheck != NULL)
	{
		return staticCheck;
	}


	// check dynamic objects
	int i = -1;
	StackArray *list = &ObjectList->solidList;
	Object *objects = ObjectList->objectComponents.Objects;
	PhysicsBox *boxes = ObjectList->objectComponents.PhysicsBoxes;
	int index = 0;

	while (i < list->storedElements)
	{
		i++;
		index = list->array[i];

		if (boxes[index].solid == UNSOLID || CheckBoxOverlapsBoxBroad(inputBox, &boxes[index]) == false)
		{
			continue;
		}

		if (CheckBoxCollidesBox(inputBox, &boxes[index]))
		{
			return &objects[index];
		}
	}

	return NULL;
}


// returns pointer of object overlapping, NULL if no object is detected; has n^2 complexity, not great!
Object* GetCollidingObjectFull(PhysicsBox *inputBox, ObjectController *ObjectList)
{
	if (inputBox == NULL || inputBox->solid == UNSOLID || ObjectList == NULL)
	{
		return NULL;
	}

	int i = ObjectList->objectCount;

	if (i > FAST_COLLISION_THRESHOLD)
	{
		return GetCollidingObject(inputBox, ObjectList);
	}

	Object *currentObject = ObjectList->firstObject;

	while (currentObject != NULL && i > 0)
	{
		i--;

		if (currentObject->ObjectBox->solid == UNSOLID || !CheckBoxOverlapsBoxBroad(inputBox, currentObject->ObjectBox))
		{
			currentObject = currentObject->nextObject;
			continue;
		}

		if (CheckBoxCollidesBox(inputBox, currentObject->ObjectBox))
		{
			return currentObject;
		}

		currentObject = currentObject->nextObject;
	}

	return NULL;
}


Object* GetOverlappingObject(PhysicsBox *inputBox, ObjectController *ObjectList)
{
	if (inputBox == NULL || ObjectList == NULL)
	{
		return NULL;
	}

	Object *currentObject = ObjectList->firstObject;

	int i = ObjectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		i--;

		if (CheckBoxOverlapsBoxBroad(inputBox, currentObject->ObjectBox) == false)
		{
			currentObject = currentObject->nextObject;
			continue;
		}
		
		if (CheckBoxOverlapsBox(inputBox, currentObject->ObjectBox))
		{
			return currentObject;
		}

		currentObject = currentObject->nextObject;
	}

	return NULL;
}


Object* GetOverlappingObjectType(PhysicsBox *inputBox, int overlapObjectID, ObjectController *ObjectList)
{
	if (inputBox == NULL || ObjectList == NULL)
	{
		return NULL;
	}

	Object *currentObject = ObjectList->firstObject;

	int i = ObjectList->objectCount;

	while(currentObject != NULL && i > 0)
	{
		if (CheckBoxOverlapsBoxBroad(inputBox, currentObject->ObjectBox) == false)
		{
			currentObject = currentObject->nextObject;
			continue;
		}

		
		if (currentObject->ObjectID == overlapObjectID && CheckBoxOverlapsBox(inputBox, currentObject->ObjectBox))
		{
			return currentObject;
		}


		currentObject = currentObject->nextObject;

		i--;
	}

	return NULL;
}


Object* GetOverlappingSolid(PhysicsBox *inputBox, int solidID, ObjectController *ObjectList)
{
	if (inputBox == NULL || inputBox->solid == UNSOLID || ObjectList == NULL)
	{
		return NULL;
	}

	int i = -1;
	StackArray *list = &ObjectList->solidList;
	Object *objects = ObjectList->objectComponents.Objects;
	PhysicsBox *boxes = ObjectList->objectComponents.PhysicsBoxes;
	int index = 0;

	while (i < list->storedElements)
	{
		i++;
		index = list->array[i];
		if (boxes[index].solid == UNSOLID || CheckBoxOverlapsBoxBroad(inputBox, &boxes[index]) == false)
		{
			continue;
		}

		if (CheckBoxOverlapsBox(inputBox, &boxes[index]) && (solidID == UNDEFINED_SOLID || solidID == boxes[index].solid))
		{
			return &objects[index];
		}
	}

	return NULL;
}

Object* GetOverlappingObjectAllSolids(PhysicsBox *inputBox, ObjectController *ObjectList)
{
	if (ObjectList == NULL || inputBox == NULL)
	{
		return NULL;
	}

	int i = ObjectList->objectCount;

	if (i > FAST_COLLISION_THRESHOLD)
	{
		return GetOverlappingSolid(inputBox, UNDEFINED_SOLID, ObjectList);
	}

	Object *currentObject = ObjectList->firstObject;


	while(currentObject != NULL && i > 0)
	{
		i--;

		if (CheckBoxOverlapsBoxBroad(inputBox, currentObject->ObjectBox) == false)
		{
			currentObject = currentObject->nextObject;
			continue;
		}

		
		if (currentObject->ObjectBox->solid != UNSOLID && CheckBoxOverlapsBox(inputBox, currentObject->ObjectBox))
		{
			return currentObject;
		}
		

		currentObject = currentObject->nextObject;
	}

	return NULL;
}


// **READ THIS Before adding new components**
// Because C does not have templates, you must write some boilerplate before adding a new component
// This involves creating the struct for the component itself, and the wrapper to contain an array of them alongside the SparseSet
// you must also create the associated add/remove/get functions, and put the initialisation into the initialiseComponents function
// it's recommended to basically just copy and paste as it should copy the functionality of the existing components

// These macros can simplify the process of adding new components
#define initComponentType(x) 		InitialiseSparseList(&ObjectList->objectComponents.x, #x)
#define removeComponentType(x, y) 	removeComponent(x, &GameWorld->ObjectList.objectComponents.y)
#define addComponentType(x, y) 		(y *)addComponent(x, &GameWorld->ObjectList.objectComponents.y)
#define getComponentType(x, y) 		(y *)getComponent(x, &GameWorld->ObjectList.objectComponents.y)
#define hasComponentType(x, y)		(GameWorld->ObjectList.objectComponents.y.sparse[x->index] >= 0)


int InitialiseComponents(ObjectController *ObjectList)
{
	if (ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	// initialise new components here
	initComponentType(HealthComponent);
	initComponentType(BulletComponent);
	initComponentType(TileMap);
	initComponentType(Timer);
	initComponentType(StopWatch);
	initComponentType(PhysicsComponent);
	initComponentType(Polygon);
	initComponentType(ObjectEvent);

	return LEMON_SUCCESS;
}

int removeComponents(Object *input, ObjectController *ObjectList)
{
	if (ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	// remove new components here
	removeComponent(input, &ObjectList->objectComponents.HealthComponent);
	removeComponent(input, &ObjectList->objectComponents.BulletComponent);
	removeComponent(input, &ObjectList->objectComponents.TileMap);
	removeComponent(input, &ObjectList->objectComponents.Timer);
	removeComponent(input, &ObjectList->objectComponents.StopWatch);
	removeComponent(input, &ObjectList->objectComponents.PhysicsComponent);
	removeComponent(input, &ObjectList->objectComponents.Polygon);
	removeComponent(input, &ObjectList->objectComponents.ObjectEvent);

	return LEMON_SUCCESS;
}


void InitialiseSparseList(SparseList *input, const char name[])
{
	strcpy(input->name, name);

	// -1 is tombstone value (empty slot)
	for (int i = 0; i < EngineSettings.MaxObjects; i++)
	{
		input->sparse[i] = -1;
	}

	input->storedComponents = 0;

	memset(input->dense, 0, sizeof(ComponentType) * MAX_COMPONENT_SLOTS);
	memset(input->denseID, 0, sizeof(int) * MAX_COMPONENT_SLOTS);

	return;
}


ComponentType* addComponent(Object *input, SparseList *List)
{
	if (input == NULL || List == NULL)
	{
		return NULL;
	}

	int denseIndex = List->sparse[input->index];
	if (denseIndex >= 0)
	{
		return &List->dense[denseIndex];
	}

	if (List->storedComponents >= MAX_COMPONENT_SLOTS)
	{
		return NULL;
	}

	ComponentType *newSlot = &List->dense[List->storedComponents];
	List->sparse[input->index] = List->storedComponents;
	List->denseID[List->storedComponents] = input->index;
	List->storedComponents++;

	return newSlot;
}

int removeComponent(Object *input, SparseList *List)
{
	if (input == NULL || List == NULL)
	{
		return INVALID_DATA;
	}

	int denseIndex = List->sparse[input->index];
	if (denseIndex < 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	// set to -1 to indicate its empty, deletion of data is optional
	ComponentType *denseList = List->dense;
	int lastIndex = List->storedComponents - 1;

	if (strcmp(List->name, "Polygon") == 0)
	{
		Polygon *poly = &denseList[denseIndex].Polygon;
		
		if (poly->vertexList != NULL)
		{
			free(poly->vertexList);
			poly->vertexList = NULL;
		}

		if (poly->indicies != NULL)
		{
			free(poly->indicies);
			poly->indicies = NULL;
		}
	} 
	else if (strcmp(List->name, "ObjectEvent") == 0)
	{
		ObjectEvent *event = &denseList[denseIndex].ObjectEvent;
		if (event->event != NULL)
		{
			free(event->event);
			event->event = NULL;
		}
	} 

	
	// swap last and component to delete
	if (denseIndex != lastIndex)
	{
		List->sparse[List->denseID[lastIndex]] = denseIndex;
		denseList[denseIndex] = denseList[lastIndex];
		List->denseID[denseIndex] = List->denseID[lastIndex];
	}

	List->sparse[input->index] = -1;
	List->storedComponents--;

	return LEMON_SUCCESS;
}

ComponentType* getComponentWithIndex(int index, SparseList *List)
{
	if (index < 0 || List->sparse[index] < 0)
	{
		return NULL;
	}

	return &List->dense[List->sparse[index]];
}

ComponentType* getComponent(Object *input, SparseList *List)
{
	if (input == NULL || List->sparse[input->index] < 0)
	{
		return NULL;
	}

	return &List->dense[List->sparse[input->index]];
}


int updateComponents(World *GameWorld)
{
	updatePhysicsComponents(GameWorld);

	return LEMON_SUCCESS;
}


GameEvent* addObjectEvent(Object *input, bool triggerOnce, World *GameWorld)
{
	ObjectEvent *newEvent = getComponentType(input, ObjectEvent);

	if (newEvent != NULL)
	{
		newEvent->triggerOnce = triggerOnce;
		return newEvent->event;
	}

	newEvent = addComponentType(input, ObjectEvent);

	if (newEvent == NULL)
	{
		return NULL;
	}

	newEvent->event = malloc(sizeof(GameEvent));
	if (newEvent->event == NULL)
	{
		removeComponentType(input, ObjectEvent);
		return NULL;
	}

	memset(newEvent->event, 0, sizeof(GameEvent));
	newEvent->triggerOnce = triggerOnce;

	return newEvent->event;
}

GameEvent* getObjectEvent(Object *input, World *GameWorld)
{
	ObjectEvent *newEvent = getComponentType(input, ObjectEvent);

	if (newEvent == NULL)
	{
		return NULL;
	}

	return newEvent->event;
}

bool hasObjectEvent(Object *input, World *GameWorld)
{
	return hasComponentType(input, ObjectEvent);
}

void triggerObjectEvent(Object *input, World *GameWorld)
{
	ObjectEvent *event = getComponentType(input, ObjectEvent);

	if (event == NULL || event->event == NULL)
	{
		return;
	}

	// this client has triggered the event, so put your own clientID here
	event->event->clientID = Networking.clientID;	
	triggerGameEvent(event->event, GameWorld);

	if (event->triggerOnce)
	{	
		removeComponentType(input, ObjectEvent);
	}

	return;
}


PhysicsComponent* addPhysics(Object *input, bool gravity, World *GameWorld)
{
	PhysicsComponent *newPhys = addComponentType(input, PhysicsComponent);

	if (newPhys == NULL)
	{
		return NULL;
	}

	newPhys->object = input;
	newPhys->gravity = gravity;

	return newPhys;
}

PhysicsComponent* addPhysicsDefault(Object *input, World *GameWorld)
{
	PhysicsComponent *newPhys = addComponentType(input, PhysicsComponent);
	if (newPhys == NULL)
	{
		return NULL;
	}

	newPhys->object = input;
	newPhys->gravity = true;

	return newPhys;
}

PhysicsComponent* getPhysicsComponent(Object *input, World *GameWorld)
{
	return getComponentType(input, PhysicsComponent);
}

bool HasPhysics(Object *input, World *GameWorld)
{
	if (input == NULL || (input->reserved & RFLAG_DISABLE_PHYSICS) != 0)
	{
		return false;
	}

	return hasComponentType(input, PhysicsComponent);
}

bool HasGravity(Object *input, World *GameWorld)
{
	PhysicsComponent *myPhys = getComponentType(input, PhysicsComponent);

	if (myPhys)
	{
		return myPhys->gravity;
	}

	return false;
}

void SetPhysicsGravity(Object *input, bool gravity, World *GameWorld)
{
	PhysicsComponent *myPhys = getComponentType(input, PhysicsComponent);

	if (myPhys)
	{
		myPhys->gravity = gravity;
	}

	return;
}

void updatePhysicsComponents(World *GameWorld)
{
	if (!LEMON_COLLISION_PHYSICS || GameWorld->PhysicsType != PLATFORMER)
	{
		return;
	}

	SparseList *List = &GameWorld->ObjectList.objectComponents.PhysicsComponent;
	ComponentType *denseList = List->dense;
	PhysicsComponent *phys;

	for (int i = List->storedComponents - 1; i >= 0; i--)
	{
		phys = &denseList[i].PhysicsComponent;

		if (phys->gravity && (phys->object->reserved & RFLAG_DISABLE_PHYSICS) == 0)
		{
			ApplyGravity(phys->object, GameWorld);
		}
	}

	return;
}


Polygon* addPolygon(Object *input, World *GameWorld, int numOfVertices, ...)
{
	Polygon *newPolygon = getComponentType(input, Polygon);

	if (newPolygon != NULL)		// necessary because vertex list is allocated on heap
	{
		free(newPolygon->vertexList);
		newPolygon->vertexList = NULL;
	}
	else
	{
		newPolygon = addComponentType(input, Polygon);

		if (newPolygon == NULL)
		{
			return NULL;
		}
	}

	SDL_Vertex *vertexList = malloc(sizeof(SDL_Vertex) * numOfVertices);
	if (vertexList == NULL)
	{
		return NULL;
	}
	memset(vertexList, 0, sizeof(SDL_Vertex) * numOfVertices);

	va_list args;
	va_start(args, numOfVertices);

	for (int i = 0; i < numOfVertices; i++)
	{
		vertexList[i].position.x = (float)va_arg(args, double);
		vertexList[i].position.y = (float)va_arg(args, double);
		vertexList[i].tex_coord.x = (float)va_arg(args, double);
		vertexList[i].tex_coord.y = (float)va_arg(args, double);
		vertexList[i].color.r = vertexList[i].color.b = vertexList[i].color.g = vertexList[i].color.a = 1.0;
	}

	va_end(args);

	newPolygon->vertexList = vertexList;
	newPolygon->vertices = numOfVertices;
	newPolygon->quad = false;
	newPolygon->indicies = NULL;

	return newPolygon;
}

Polygon* addQuad(Object *input, World *GameWorld)		
{
	float x = (float)input->ObjectBox->xSize;
	float y = (float)input->ObjectBox->ySize;
	// add a polygon that is a box surrounding the sprite, that without modification appears identically to regular sprite rendering, albeit without rotations
	Polygon *new = addPolygon(input, GameWorld, 4, 
		0.0, 0.0, 0.0, 0.0,	
		0.0, y, 0.0, 1.0,
		x, 0.0, 1.0, 0.0,
		x, y, 1.0, 1.0);

	if (new != NULL)
	{
		new->quad = true;

		if (new->indicies != NULL)
		{
			free(new->indicies);
		}

		new->indicies = malloc(6 * sizeof(int));
		new->indicies[0] = 0;
		new->indicies[1] = 1;
		new->indicies[2] = 2;
		new->indicies[3] = 1;
		new->indicies[4] = 2;
		new->indicies[5] = 3;
	}

	return new;
}


Polygon* getPolygon(Object *input, World *GameWorld)
{
	return (Polygon *)getComponent(input, &GameWorld->ObjectList.objectComponents.Polygon);
}

void movePolygonVertex(Object *input, int vertex, float newX, float newY, World *GameWorld)
{
	Polygon *poly = getPolygon(input, GameWorld);

	if (poly == NULL || vertex < 0 || vertex >= poly->vertices)
	{
		return;
	}

	// kinda messy but allows you to treat the quad as a polygon with 4 points
	if (poly->quad)
	{
		if (vertex > 3)
		{
			return;
		}

		switch(vertex)
		{
		case 1:
			poly->vertexList[3].position.x = newX;
			poly->vertexList[3].position.y = newY;
			break;

		case 2:
			poly->vertexList[4].position.x = newX;
			poly->vertexList[4].position.y = newY;
			break;

		case 3:
			vertex = 5;
			break;

		default:
			break;
		}
	}

	poly->vertexList[vertex].position.x = newX;
	poly->vertexList[vertex].position.y = newY;

	return;
}

SDL_Vertex* getPolygonVertex(Object *input, int vertex, World *GameWorld)
{
	Polygon *poly = getPolygon(input, GameWorld);

	if (poly == NULL || vertex < 0 || vertex >= poly->vertices)
	{
		return NULL;
	}

	return &poly->vertexList[vertex];
}

HealthComponent* addHealthComponent(Object *input, int Health, GroupType group, World *GameWorld)
{
	HealthComponent *newHp = addComponentType(input, HealthComponent);

	if (newHp == NULL)
	{
		return NULL;
	}

	newHp->health = Health;
	newHp->maxHealth = Health;
	newHp->hurtDuration = 0;
	newHp->hurtTick = 0;
	newHp->group = group;

	return newHp;
}

HealthComponent* getHealthComponent(Object *input, World *GameWorld)
{
	return getComponentType(input, HealthComponent);
}

GroupType getGroupAffiliation(Object *input, World *GameWorld)
{
	HealthComponent *health = getComponentType(input, HealthComponent);

	if (health == NULL)
	{
		return NO_GROUP;
	}

	return health->group;
}

int inflictDamage(int damage, Object *input, World *GameWorld)
{
	HealthComponent *targetHp = getHealthComponent(input, GameWorld);

	if (targetHp == NULL)
	{
		return MISSING_DATA;
	}

	targetHp->health -= damage;
	if (targetHp->health < 1)
	{
		MarkObjectForDeletion(input);
	}

	targetHp->hurtTick = TickNumber();
	targetHp->hurtDuration = 5;
	PlayObjectAnimation("Hurt", 1, input);

	return LEMON_SUCCESS;
}

bool isHurt(Object *input, World *GameWorld)
{
	HealthComponent *health = getHealthComponent(input, GameWorld);

	if (health == NULL)
	{		
		return false;
	}

	return (TickNumber() < health->hurtTick + health->hurtDuration);
}


BulletComponent* addBulletComponent(Object *input, Object *owner, int damage, ParticleSubType particleType, World *GameWorld)
{
	centerOnObject(input, owner);
	input->ObjectBox->shape = CIRCLE;
	input->ObjectBox->solid = SOLID;
	input->ObjectBox->flag = GET_IGNORED;

	BulletComponent *newBullet = addComponentType(input, BulletComponent);

	if (newBullet == NULL)
	{
		return NULL;
	}

	newBullet->damage = damage;
	newBullet->owner = owner;
	newBullet->particleType = particleType;
	newBullet->particleLifeTime = 0;
	newBullet->bulletCollide = true;
	newBullet->bulletLifeTime = 200;
	newBullet->group = getGroupAffiliation(owner, GameWorld);
	
	return newBullet;
}


bool isBullet(Object *input, World *GameWorld)
{
	if (input == NULL)
	{		
		return false;
	}

	return hasComponentType(input, BulletComponent);
}


void bulletCollision(Object *bulletObject, World *GameWorld)
{
	BulletComponent *bulletInfo = getComponentType(bulletObject, BulletComponent);
	if (bulletInfo == NULL)
	{
		return;
	}

	bulletInfo->bulletLifeTime--;

	if (bulletInfo->bulletLifeTime < 1)
	{
		MarkObjectForDeletion(bulletObject);
		return;
	}

	// search for collisions with any objects that have health attached first
	SparseList *healthData = &GameWorld->ObjectList.objectComponents.HealthComponent;
	HealthComponent *healthList = (HealthComponent *)healthData->dense;

	PhysicsBox *boxes = GameWorld->ObjectList.objectComponents.PhysicsBoxes;
	Object *objects = GameWorld->ObjectList.objectComponents.Objects;
	int index = 0;

	for (int i = 0; i < healthData->storedComponents; i++)
	{
		// If the bullet's affiliation differs from the hit object, or if the bullet has no affiliation, it should deal damage
		if (healthList[i].group == bulletInfo->group && bulletInfo->group != NO_GROUP)
		{
			continue;
		}

		index = healthData->denseID[i];

		// one last safety check to ensure bullet is not hitting the object who spawned it, although you may remove this if you want that functionality
		if (&objects[index] != bulletInfo->owner && CheckBoxOverlapsBox(bulletObject->ObjectBox, &boxes[index]))
		{
			MarkObjectForDeletion(bulletObject);

			centerOnObject(AddParticle(GameWorld, bulletInfo->particleType, 0, 0, 1, bulletInfo->particleLifeTime), bulletObject);

			inflictDamage(bulletInfo->damage, &objects[index], GameWorld);
		}
	}

	// If bullet is supposed to be destroyed on hitting geometry, check for it here
	// If bullet object has physics attached, it is assumed you want the physics system to take over
	if (!bulletInfo->bulletCollide || HasPhysics(bulletObject, GameWorld))
	{
		return;
	}

	Object *hitObject = GetCollidingObject(bulletObject->ObjectBox, &GameWorld->ObjectList);

	if (hitObject == NULL || hitObject == bulletInfo->owner)
	{
		return;
	}	

	MarkObjectForDeletion(bulletObject);

	centerOnObject(AddParticle(GameWorld, bulletInfo->particleType, 0, 0, 1, bulletInfo->particleLifeTime), bulletObject);

	return;
}


int addTileMap(Object *input, int centerTileX, int centerTileY, int tileSize, World *GameWorld)
{
	TileMap *newMap = addComponentType(input, TileMap);

	if (newMap == NULL)
	{
		return MISSING_DATA;
	}

	newMap->centerTileX = (float)clamp(centerTileX, 0, centerTileX);
	newMap->centerTileY = (float)clamp(centerTileY, 0, centerTileY);
	newMap->tileSize = (float)clamp(tileSize, 1, tileSize);

	return LEMON_SUCCESS;
}


TileMap* getTileMap(Object *input, World *GameWorld)
{
	return (TileMap *)getComponent(input, &GameWorld->ObjectList.objectComponents.TileMap);
}


int startTimer(int ticks, Object *input, World *GameWorld)
{
	if (ticks < 1)
	{
		return EXECUTION_UNNECESSARY;
	}

	Timer *newTimer = addComponentType(input, Timer);

	if (newTimer == NULL)
	{
		return MISSING_DATA;
	}

	newTimer->pause = false;
	newTimer->pauseTick = 0;
	newTimer->timerLength = ticks;
	newTimer->startTick = TickNumber();

	return LEMON_SUCCESS;
}

int startTimerSeconds(float seconds, Object *input, World *GameWorld)
{
	return startTimer((int)(seconds * EngineSettings.GameTicksPerSecond), input, GameWorld);
}

bool timerExpired(Object *input, World *GameWorld)
{
	return (checkTimer(input, GameWorld) == 0);
}

// returns time remaining
Uint64 checkTimer(Object *input, World *GameWorld)
{
	const Timer *timer = getComponentType(input, Timer);

	if (timer == NULL)
	{
		return 0;
	}

	Uint64 elapsed = (timer->pause ? timer->pauseTick : TickNumber()) - timer->startTick;

	if (elapsed >= timer->timerLength)
	{
		removeComponentType(input, Timer);
		return 0;
	}

	return timer->timerLength - elapsed;
}

Timer* getTimer(Object *input, World *GameWorld)
{
	return getComponentType(input, Timer);
}

int endTimer(Object *input, World *GameWorld)
{
	return removeComponentType(input, Timer);
}

void pauseTimer(Object *input, World *GameWorld)
{
	Timer *timer = getComponentType(input, Timer);

	if (timer == NULL || timer->pause)
	{
		return;
	}

	timer->pause = true;
	timer->pauseTick = TickNumber();

	return;
}

#define resumeTimer(x) unpauseTimer(x)
void unpauseTimer(Object *input, World *GameWorld)
{
	Timer *timer = getComponentType(input, Timer);

	if (timer == NULL || !timer->pause)
	{
		return;
	}

	timer->pause = false;
	timer->startTick += TickNumber() - timer->pauseTick;

	return;
}

int startStopWatch(Object *input, World *GameWorld)
{
	StopWatch *newStopWatch = addComponentType(input, StopWatch);

	if (newStopWatch == NULL)
	{
		return MISSING_DATA;
	}

	newStopWatch->startTimeStamp = SDL_GetTicks();
	newStopWatch->pause = false;
	newStopWatch->pauseTimeStamp = 0;

	return LEMON_SUCCESS;
}

float checkStopWatch(Object *input, World *GameWorld)
{
	const StopWatch *watch = getComponentType(input, StopWatch);

	if (watch == NULL)
	{
		return 0.0;
	}

	if (watch->pause)
	{
		return (float)(watch->pauseTimeStamp - watch->startTimeStamp) / 1000.0;
	}
	else
	{
		return (float)(SDL_GetTicks() - watch->startTimeStamp) / 1000.0;
	}
}

void pauseStopWatch(Object *input, World *GameWorld)
{
	StopWatch *watch = getComponentType(input, StopWatch);

	if (watch == NULL || watch->pause)
	{
		return;
	}

	watch->pause = true;
	watch->pauseTimeStamp = SDL_GetTicks();

	return;
}

#define resumeStopWatch(x) unpauseStopWatch(x)
void unpauseStopWatch(Object *input, World *GameWorld)
{
	StopWatch *watch = getComponentType(input, StopWatch);

	if (watch == NULL || !watch->pause)
	{
		return;
	}

	watch->pause = false;
	watch->startTimeStamp += SDL_GetTicks() - watch->pauseTimeStamp;

	return;
}

float endStopWatch(Object *input, World *GameWorld)
{
	float time = checkStopWatch(input, GameWorld);

	removeComponentType(input, StopWatch);

	return time;
}
