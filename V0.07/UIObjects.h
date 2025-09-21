#ifndef IS_DEFINED
#include "data.h"
#include "gameObjects.h"
#include "animations.h"
#include "spriteLoader.h"
#include "soundProcessor.h"
#include "playerController.h"
#include "eventManager.h"
#include "drawScreen.h"
#include "LemonMain.h"
#endif



int SpawnHUD(World *GameWorld);


int InitialiseUIElement(Object *UIElement, World *GameWorld);


int InitialiseUIText(Object *UIText, World *GameWorld);


int LoadUISprites(SpriteSet *newSet);


int LoadUITextSprites(SpriteSet *newSet);


int UpdateUIElement(World *GameWorld, Object *UIElement);


int UpdateCursor(Object *Cursor, World *GameWorld);


int UpdateTextBox(Object *textBox);


int PauseMenu(Object *MenuController, World *GameWorld);


int SettingsMenu(Object *MenuController, World *GameWorld);


int UpdateOptionButton(Object *Button);



TextInstance* CreateText(const char inputPhrase[], World *GameWorld);


TextInstance* SayText(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld);


TextInstance* SayTextCutscene(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld, int SceneTickOnTextEnd);


int ApplyTextPresets(TextInstance *inputText, const char Portrait[], TextPreset inputPreset);


int updateText(World *GameWorld);


int TextInteraction(World *GameWorld);


int displayNextCharacter(TextInstance *inputText, World *GameWorld);


Object* createTextCharacter(TextInstance *inputText, World *GameWorld);


Object* createTextBox(TextInstance *inputText, World *GameWorld);


Object* createTextBoxPortrait(TextInstance *inputText, World *GameWorld);


int mapTextToCharacter(Object *inputText);


int getCharacterSpacing(char inputCharacter);


int endTextInstance(World *GameWorld);


int clearTextQueue(World *GameWorld);