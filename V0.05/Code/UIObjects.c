#include "UIObjects.h"


int SpawnHUD(World *GameWorld)
{
	if (GameWorld == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	

	return 0;
}


int InitialiseUIElement(World *GameWorld, Object *UIElement)
{
	if (GameWorld == NULL || UIElement == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}

	UIElement->layer = HUD;
	UIElement->ObjectBox->solid = UNSOLID;


	switch(UIElement->arg1)
	{
		case PAUSE_BACKGROUND:
		switchSpriteByName("PauseBackground", 0, UIElement->ObjectDisplay);
		break;


		case PAUSE_HEADER:
		switchSpriteByName("PauseHeader", 0, UIElement->ObjectDisplay);
		break;


		case SETTINGS_HEADER:
		switchSpriteByName("SettingsHeader", 0, UIElement->ObjectDisplay);
		break;


		case PAUSE_MENU_CONTROLLER:
		{
			UIElement->ObjectBox->xPos = 3 * X_TILESCALE;
			UIElement->ObjectBox->yPos = 13 * Y_TILESCALE;
			UIElement->arg2 = 0;

			Object *createdOption = NULL;


			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 5 * X_TILESCALE, screenHeight - (Y_TILESCALE * 4), 200, 40, PAUSE_HEADER, 0, 0, 0, 0);

			
			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 6 * X_TILESCALE, 13 * Y_TILESCALE, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 0, 0, 0, 0);

			switchObjectSpriteName("ResumeGame", createdOption, GameWorld->ObjectList);
			

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 6 * X_TILESCALE, 9 * Y_TILESCALE, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 1, 0, 0, 0);

			switchObjectSpriteName("Settings", createdOption, GameWorld->ObjectList);
			

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 6 * X_TILESCALE, 5 * Y_TILESCALE, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 2, 0, 0, 0);

			switchObjectSpriteName("QuitGame", createdOption, GameWorld->ObjectList);
			
			
			switchObjectSpriteName("OptionCursor", UIElement, GameWorld->ObjectList);
			SetDrawPriorityToFront(GameWorld->ObjectList, UIElement);
		} break;


		case SETTINGS_MENU_CONTROLLER:
		{
			// In order to have particles be visible in the pause menu while hiding normal particles, the camera is moved elsewhere
			// and is restored to its previous position when unpaused  (There should not be any level geometry before X pos 0)
			// When game is resumed, even if x pos is not reset to original value, it will be corrected to 0 by WorldCameraControl
			UIElement->ObjectBox->xPos = 3 * X_TILESCALE;
			UIElement->ObjectBox->yPos = 17 * Y_TILESCALE;
			UIElement->arg2 = 0;

			Object *createdOption = NULL;

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 0, 0, 0, 0, PAUSE_BACKGROUND, 0, 0, 0, 0);

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 4 * X_TILESCALE, screenHeight - (Y_TILESCALE * 2), 100, 40, SETTINGS_HEADER, 0, 0, 0, 0);
			

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 6 * X_TILESCALE, 17 * Y_TILESCALE, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 0, 0, 0, 0);

			switchObjectSpriteName("RenderQuality_Option", createdOption, GameWorld->ObjectList);
			

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 6 * X_TILESCALE, 13 * Y_TILESCALE, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 1, 0, 0, 0);

			switchObjectSpriteName("Volume_Option", createdOption, GameWorld->ObjectList);
			

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 6 * X_TILESCALE, 9 * Y_TILESCALE, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 1, 0, 0, 0);

			switchObjectSpriteName("Test_Option", createdOption, GameWorld->ObjectList);
			

			createdOption = AddObjectWithParent(GameWorld, UIElement, UI_ELEMENT, 6 * X_TILESCALE, 5 * Y_TILESCALE, X_TILESCALE, Y_TILESCALE, OPTION_BUTTON, 2, 0, 0, 0);

			switchObjectSpriteName("BackButton", createdOption, GameWorld->ObjectList);
			
			
			switchObjectSpriteName("OptionCursor", UIElement, GameWorld->ObjectList);
			SetDrawPriorityToFront(GameWorld->ObjectList, UIElement);
		} break;


		case TEXT_BOX:
		switchSpriteByName("TextBox", 0, UIElement->ObjectDisplay);
		break;


		default:
		break;
	}


	return 0;
}


int LoadUISprites(SpriteSet *newSet)
{
	loadObjectSprite("OBJ_Missing", newSet, TILE);
	loadObjectSprite("OptionCursor", newSet, SINGLE);
	loadObjectSprite("BackButton", newSet, SINGLE);

	loadObjectSprite("GamePaused_Background", newSet, SINGLE);
	loadObjectSprite("GamePaused_Header", newSet, SINGLE);
	loadObjectSprite("ResumeGame", newSet, SINGLE);
	loadObjectSprite("ResumeGame_Highlighted", newSet, SINGLE);
	loadObjectSprite("Settings", newSet, SINGLE);
	loadObjectSprite("Settings_Highlighted", newSet, SINGLE);
	loadObjectSprite("QuitGame", newSet, SINGLE);
	loadObjectSprite("QuitGame_Highlighted", newSet, SINGLE);

	loadObjectSprite("Settings_Background", newSet, SINGLE);
	loadObjectSprite("Settings_Header", newSet, SINGLE);
	loadObjectSprite("RenderQuality_Option", newSet, SINGLE);
	loadObjectSprite("Volume_Option", newSet, SINGLE);
	loadObjectSprite("Test_Option", newSet, SINGLE);

	return 0;
}



int UpdateUIElement(World *GameWorld, Object *UIElement, int keyboard[256])
{
	if (GameWorld == NULL || UIElement == NULL || GameWorld->ObjectList == NULL)
	{
		return MISSING_DATA;
	}


	switch (UIElement->arg1)
	{
		case PAUSE_MENU_CONTROLLER:
		PauseMenu(UIElement, GameWorld, keyboard);
		break;

		case SETTINGS_MENU_CONTROLLER:
		SettingsMenu(UIElement, GameWorld, keyboard);
		break;

		case OPTION_BUTTON:
		UpdateOptionButton(UIElement);
		break;

		case TEXT_BOX:
		UpdateTextBox(UIElement, keyboard);
		break;

		default:
		break;
	}


	return 0;
}


int UpdateTextBox(Object *textBox, int keyboard[256])
{


	return 0;
}


int PauseMenu(Object *MenuController, World *GameWorld, int keyboard[256])
{
	if (MenuController == NULL || GameWorld == NULL || MenuController->ObjectID != UI_ELEMENT)
	{
		return MISSING_DATA;
	}


	if (keyboard[LMN_DOWN])
	{
		keyboard[LMN_DOWN] = 0;

		MenuController->arg2++;
		MenuController->ObjectBox->yPos -= 4 * Y_TILESCALE;
	}

	if (keyboard[LMN_UP])
	{
		keyboard[LMN_UP] = 0;

		MenuController->arg2--;
		MenuController->ObjectBox->yPos += 4 * Y_TILESCALE;
	}

	if (MenuController->arg2 > 2)
	{
		MenuController->arg2 = 0;
		MenuController->ObjectBox->yPos = 13 * Y_TILESCALE;
	}
	else if (MenuController->arg2 < 0)
	{
		MenuController->arg2 = 2;
		MenuController->ObjectBox->yPos = 5 * Y_TILESCALE;
	}


	if (keyboard[LMN_INTERACT] == 1 || keyboard[LMN_JUMP] == 1)
	{
		keyboard[LMN_INTERACT] = 0;
		keyboard[LMN_JUMP] = 0;

		switch (MenuController->arg2)
		{
			case 1:
				if (OpenSettings(GameWorld, keyboard) == 0)
				{
					MarkObjectForDeletion(MenuController, GameWorld->ObjectList);
				}
				break;

			case 2:
				GameWorld->GameState = CLOSE_GAME;
				break;

			default:
				ResumeGame(GameWorld);
				break;
		}
	}


	if (GameWorld->GamePaused == 0 && GameWorld->GameState != IN_MENU)
	{
		MarkObjectForDeletion(MenuController, GameWorld->ObjectList);
	}

	return 0;
}



int SettingsMenu(Object *MenuController, World *GameWorld, int keyboard[256])
{
	if (MenuController == NULL || GameWorld == NULL || MenuController->ObjectID != UI_ELEMENT)
	{
		return MISSING_DATA;
	}


	if (keyboard[LMN_DOWN])
	{
		keyboard[LMN_DOWN] = 0;

		MenuController->arg2++;
		MenuController->ObjectBox->yPos -= 4 * Y_TILESCALE;
	}

	if (keyboard[LMN_UP])
	{
		keyboard[LMN_UP] = 0;

		MenuController->arg2--;
		MenuController->ObjectBox->yPos += 4 * Y_TILESCALE;
	}

	if (MenuController->arg2 > 3)
	{
		MenuController->arg2 = 0;
		MenuController->ObjectBox->yPos = 17 * Y_TILESCALE;
	}
	else if (MenuController->arg2 < 0)
	{
		MenuController->arg2 = 3;
		MenuController->ObjectBox->yPos = 5 * Y_TILESCALE;
	}


	if (keyboard[LMN_INTERACT] == 1 || keyboard[LMN_JUMP] == 1)
	{
		keyboard[LMN_INTERACT] = 0;
		keyboard[LMN_JUMP] = 0;

		switch (MenuController->arg2)
		{
			case 0:
				break;

			case 1:
				break;

			case 2:
				break;


			default:
				PauseGame(GameWorld, keyboard);
				MarkObjectForDeletion(MenuController, GameWorld->ObjectList);
				break;
		}
	}

	if (GameWorld->GamePaused == 0 && GameWorld->GameState != IN_MENU)
	{
		MarkObjectForDeletion(MenuController, GameWorld->ObjectList);
	}

	return 0;
}


int UpdateOptionButton(Object *Button)
{
	if (Button == NULL || Button->ParentObject == NULL || Button->ObjectDisplay == NULL)
	{
		return MISSING_DATA;
	}

	if (Button->ParentObject->arg2 == Button->arg2 && Button->arg3 == 0)
	{
		Button->ObjectDisplay->currentSprite++;
		Button->arg3 = 1;
	}
	else if (Button->ParentObject->arg2 != Button->arg2 && Button->arg3 == 1)
	{
		Button->ObjectDisplay->currentSprite--;
		Button->arg3 = 0;
	}

	return 0;
}