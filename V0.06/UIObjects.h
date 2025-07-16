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


int InitialiseUIElement(World *GameWorld, Object *UIElement);


int LoadUISprites(SpriteSet *newSet);


int UpdateUIElement(World *GameWorld, Object *UIElement, int keyboard[256]);


int UpdateTextBox(Object *textBox, int keyboard[256]);


int PauseMenu(Object *MenuController, World *GameWorld, int keyboard[256]);


int SettingsMenu(Object *MenuController, World *GameWorld, int keyboard[256]);


int UpdateOptionButton(Object *Button);



TextInstance* CreateText(const char inputPhrase[], const char Portrait[], const char Voice[], VoiceMode voiceMode, int font, int textBox, int textDelay, int skipState, int xPos, int yPos, World *GameWorld);


// Helper function to allow for more control but automates x position and textbox
TextInstance* CreateTextBasic(const char inputPhrase[], const char Portrait[], const char Voice[], VoiceMode voiceMode, int font, int textDelay, int skipState, int yPos, World *GameWorld);


TextInstance* SayText(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld);


TextInstance* CreateTextCutscene(const char inputPhrase[], const char Portrait[], const char Voice[], VoiceMode voiceMode, int font, int textBox, int textDelay, int skipState, int xPos, int yPos, World *GameWorld, int PlayOnSceneTick, int SceneTickOnTextEnd);


TextInstance* SayTextCutscene(const char inputPhrase[], const char Portrait[], TextPreset inputPreset, World *GameWorld, int PlayOnSceneTick, int SceneTickOnTextEnd);



int updateText(World *GameWorld, int keyboard[256]);


int displayNextCharacter(TextInstance *inputText, World *GameWorld);


int mapTextToCharacter(Object *inputText);


int getCharacterSpacing(char inputCharacter);


int endTextInstance(World *GameWorld);