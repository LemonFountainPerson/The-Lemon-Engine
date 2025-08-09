#ifndef IS_DEFINED
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <stdbool.h>
#include <Windows.h>
#include <mmsystem.h>
#define WIN32_LEAN_AND_MEAN

#include <SDL3/SDL.h>

//								Engine constants
//-------------------------------------------------------------------------------------------------
#define MULTITHREADED_ENABLED 0

#define DEBUG_STRING_LENGTH 256
#define MAX_LEN 80
#define ENCRYPT_OFFSET 600

#define CHANNEL_COUNT 8
#define MAX_SOUNDS_PER_CHANNEL 32

// Most displayed objects are designed to adjust based on resolution; however the lowest expected resolution is 1280 x 720
// lower values may result in unintended behaviour
#define V_RESOLUTION 720
#define H_RESOLUTION 1280

#define TARGET_DELTA (1.0/60.0)
#define TICKS_PER_SECOND 60
#define TICK_DELTA (int)((1.0/60.0) * 1000)

#define MAX_OBJECTS 6000
#define RESERVED_OBJECTS 500
#define PRESERVED_SPRITESETS 5

#define MAX_OBJECTS_RENDER 128
#define MAX_PARTICLES_RENDER 64
#define MAX_HUD_ELEMENTS_RENDER 128

// Number of textInstances allowed in the TextQueue at once
#define MAX_TEXTQUEUE_LENGTH 1000
//-------------------------------------------------------------------------------------------------


//								Game constants
//-------------------------------------------------------------------------------------------------
#define PLAYERHEIGHT 50
#define PLAYERWIDTH 32

#define MAX_TEXT_LENGTH 216

// Tiles Probably wont be re-implemented, but constants are useful for stylisation
#define Y_TILESCALE 32
#define X_TILESCALE 32
//-------------------------------------------------------------------------------------------------


//								Physics values
//-------------------------------------------------------------------------------------------------
#define PUSH_VEL_TOLERANCE 3.0
#define COLLISION_CYCLES 6
#define COLLISION_DEPTH 9

#define MAX_Y_VELOCITY 50.0
#define MAX_X_VELOCITY 50.0
#define MAX_FORWARD_VELOCITY 50.0
//-------------------------------------------------------------------------------------------------


//								Useful constants
//-------------------------------------------------------------------------------------------------
#define RADIAN_15 0.26179938779
#define RADIAN_30 0.52359877559
#define RADIAN_45 0.78539816339
#define RADIAN_60 1.0471975512
#define RADIAN_75 1.308996939 
#define RADIAN_90 1.5707963268

#define NO_PORTRAIT ""
//-------------------------------------------------------------------------------------------------


#define LEMON_VERSION "V0.06"

#define IS_DEFINED 1
#endif


enum FunctionResult{
	LEMON_ERROR = -1,
	MISSING_DATA = -2,
	INVALID_DATA = -3,
	LEMON_SUCCESS = 0,
	ACTION_DISABLED = 1,
	EXECUTION_UNNECESSARY = 2,
	TASK_FAILED = 3,
	END_OF_FILE = 9
};


enum LemonKeys{
	LMN_SPACE = 0,
	LMN_ESCAPE = 1,
	LMN_ENTER = 2,
	LMN_UPARROW = 100,
	LMN_DOWNARROW = 101,
	LMN_LEFTARROW = 102,
	LMN_RIGHTARROW = 103,
	LMN_UP = 104,
	LMN_DOWN = 105,
	LMN_LEFT = 106,
	LMN_RIGHT = 107,
	LMN_JUMP = 108,
	LMN_INTERACT = 109,
	LMN_INTERACT2 = 110,
	LMN_INTERACT3 = 111,
	LMN_TEXT_CONFIRM = 112,
	LMN_TEXT_SKIP = 113
};


enum ChannelName {
	LOOP_CHANNEL = 0,
	SPEECH = 1,
	PLAYER_SFX = 2,
	OBJECT_SFX = 3,
	ENEMIES_SFX = 4,
};


enum Font {
	PIXEL_REGULAR = 0,
	PIXEL_BLACK = 1,
	UNDEFINED_FONT
};


enum TextBox {
	BASIC_BLACK = 0,
	BASIC_WHITE = 1,
	UNDEFINED_TEXTBOX
};


enum VoiceMode {
	PLAY_EACH_CHARACTER = 0,
	PLAY_ONCE
};


enum TextPreset {
	DEFAULT_BOTTOM,
	DEFAULT_TOP,
	SILENT_BOTTOM,
	SILENT_TOP,
	INVERTED_BOTTOM,
	INVERTED_TOP,
	TESTFACE_DEFAULT_BOTTOM,
	UNDEFINED_PRESET
};


// If the Object's render mode is less than 0 (the default) the sprite is rendered according to the sprite's individual render mode
// Otherwise, it is overridden to be the rendermode of the Object
// Objects essentially can either have the sprites render how they would like to be rendered, or can override it with a single rendermode

enum RenderMode {
	DO_NOT_RENDER = -2,
	DEFAULT_TO_SPRITE = -1,
	SINGLE = 0,
	SINGLE_FULL_ALPHA = 1,
	SINGLE_FAST = 2,
	TILE = 3,
	TILE_FULL_ALPHA = 4,
	TILE_FAST = 5,
	SCALE = 6,
	SCALE_FULL_ALPHA = 7,
	TILE_SCALE = 8,
	TILE_SCALE_FULL_ALPHA = 9,
	SCALE_TILE = 10,
	SCALE_TILE_FULL_ALPHA = 11,
	BG_ROW_PARALLAX,
	UNDEFINED_RENDERMODE
};


//  Order of Object list determines layering of individual Objects within layers
enum Layer {
	BACKGROUND = 0,
	MIDDLEGROUND,
	MIDDLEGROUND_2,
	FOREGROUND,
	PARTICLES,
	HUD,
	LEVELFLAGS,
	UNDEFINED_LAYER
};


enum LemonGameState {
	CLOSE_GAME = -1,
	EMPTY_GAME = 0,
	LOADING = 1,
	GAMEPLAY = 2,
	CUTSCENE = 3,
	IN_MENU = 4,
	UNDEFINED_GAME_STATE
};


enum LemonGameEvent {
	NO_EVENT = 0,
	SWITCH_LEVEL,
	CHANGE_SCREEN_SIZE,
	UNDEFINED_EVENT
};


enum CutsceneID {
	EMPTY_CUTSCENE = 0,
	UNDEFINED_CUTSCENE
};


enum CameraState {
	FOLLOW_PLAYER = 0,
	MENU_CAMERA = 1,
	FREE_ROAM = 2,
	FREE_ROAM_RESTRICTED = 3,
	UNDEFINED_CAMERA_STATE
};


enum ObjectState {
	EMPTY_OBJECT = -2,
	TO_BE_DELETED = -1,
	DEFAULT = 0,
	PAUSE_BEHAVIOUR,
	ACTOR,
	BEING_CARRIED,
	IN_INVENTORY,
	UNDEFINED_STATE
};


enum CurrentAction {
	IDLE = 0,
	DAMAGED,
	DEFEATED,
	CHASING,
	ATTACKING,
	FLEEING,
	UNDEFINED_ACTION
};


enum ParentType {
	DEFAULT_LINK 		= 0b00000000,
	POSITION_LINK 		= 0b00000001,
	VELOCITY_LINK 		= 0b00000010,
	SPRITE_LINK 		= 0b00000100,
	ANIMATION_LINK 		= 0b00001000,
	TRANSPARENCY_LINK	= 0b00010000

};


enum SolidType {
	UNSOLID = 0,
	SOLID = 1,
	FLAT_SLOPE = 2,
	JUMP_THROUGH = 3,
	ENTITY = 4,
	ENTITY_SOLID = 5,
	PUSHABLE_SOLID = 6,
	UNDEFINED_SOLID
};


enum CollideType {
	NO_COLLIDE_TYPE,
	IMPACT,
	PUSH
};


enum WorldPhysics {
	TOP_DOWN,
	PLATFORMER,
	UNDEFINED_PHYSICS
};


enum ObjectType {
	LEVEL_FLAG_OBJ = 0,
	SOLID_BLOCK = 1,
	FLAT_SLOPE_FLOOR = 2,
	JUMP_THRU_BLOCK = 3,
	PLAYER_OBJECT = 4,
	UI_ELEMENT = 5,
	UI_TEXT = 6,
	PARTICLE = 7,
	COIN = 8,
	MOVING_PLATFORM_HOR = 9,
	MOVING_PLATFORM_VER = 10,
	SPRING = 11,
	GATE_SWITCH = 12,
	GATE_SWITCH_TIMED = 13,
	VERTICAL_GATE = 14,
	HORIZONTAL_GATE = 15,
	DOOR = 16,
	LEVEL_DOOR = 17,
	PUSHABLE_BOX = 18,
	BASIC_ENEMY = 19,
	UNDEFINED_OBJECT
};


enum Flags {
	SET_BACKGROUND = 0,
	SET_BACKGROUND_TRIGGER = 1,
	START_LVL_WITH_CUTSCENE = 2,
	CUTSCENE_TRIGGER = 3,
	START_PLAYER_POSITION = 4,
	START_CAMERA_POSITION = 5,
	SET_CAMBOX = 6,
	SET_CAMBOX_TRIGGER = 7,
	SET_PLAYER_LAYER = 8,
	START_WITH_MUSIC = 9,
	PLAY_MUSIC_TRIGGER = 10,
	STOP_ALL_SOUND_LOOPS = 11,
	CACHE_TRIGGER = 12,
	UNDEFINED_FLAG
};


enum ParticleSubType {
	EMPTY_PARTICLE = 0,
	SPARKLE,
	UNDEFINED_PARTICLE
};


// UISubType is shared by all UI_xxx objects, although behaviour, 
// sprites and animation are separate - if you set a UI_ELEMENT's subtype to TEXT_BOX 
// it will result in it being treated as a BASIC GRAPHIC
enum UISubType {
	BASIC_GRAPHIC = 0,
	FADEOUT,
	OPTION_BUTTON,
	PAUSE_MENU_CONTROLLER,
	PAUSE_HEADER,
	PAUSE_BACKGROUND,
	SETTINGS_MENU_CONTROLLER,
	SETTINGS_HEADER,
	PLAYER_HUD_CONTROLLER,
	TEXT_BOX,
	TEXT_CHARACTER,
	TEXT_PORTRAIT,
	UNDEFINED_UI_ELEMENT
};


enum GateSwitch {
	SINGLE_SWITCH = 0,
	CHAIN_SWITCH = 1
};


typedef enum FunctionResult FunctionResult;
typedef enum LemonKeys LemonKeys;
typedef enum ChannelName ChannelName;
typedef enum Font Font;
typedef enum TextBox TextBox;
typedef enum VoiceMode VoiceMode;
typedef enum TextPreset TextPreset;
typedef enum RenderMode RenderMode;
typedef enum Layer Layer;
typedef enum LemonGameState LemonGameState;
typedef enum LemonGameEvent LemonGameEvent;
typedef enum CutsceneID CutsceneID;
typedef enum CameraState CameraState;
typedef enum CurrentAction CurrentAction;
typedef enum ObjectState ObjectState;
typedef enum ParentType ParentType;
typedef enum SolidType SolidType;
typedef enum CollideType CollideType;
typedef enum WorldPhysics WorldPhysics;
typedef enum ObjectType ObjectType;
typedef enum Flags Flags;
typedef enum ParticleSubType ParticleSubType;
typedef enum UISubType UISubType;
typedef enum GateSwitch GateSwitch;


// Memory allocated structs of data
struct soundInstance
{
	Uint8 *wav_data;
	Uint32 wav_data_len;
	SDL_AudioStream *stream;
	SDL_AudioFormat format;

	char name[MAX_LEN];
	float volume;

	int channelID;
	struct soundInstance *nextSound;
	struct soundInstance *prevSound;
};


struct soundChannel
{
	int soundCount;
	int Pause;
	float channelVolume;

	struct soundInstance *firstSound;
};


struct renderFrame 
{
	int width;
	int height;
	uint32_t *screen;
};


// Regular Sprites (Objects, player, particles, etc.)
struct sprite
{
	struct sprite *nextSprite;
	struct sprite *prevSprite;
	unsigned char *spriteData;

	int height;
	int width;
	RenderMode RenderMode;

	int spriteID;
	char spriteName[MAX_LEN];
};


struct animationFrame 
{
	struct animationFrame *nextFrame;

	struct sprite *frameSprite;
	short SpriteXOffset;
	short SpriteYOffset;
};


struct animation 
{
	struct animation *nextAnimation;

	int animationID;
	char name[MAX_LEN]; 
	float frameRate;

	struct animationFrame *animationData;

};


// Sprite sets that lead to linked lists of sprites corresponding to an Object type;
// only needs to be initialised once per Object type
struct spriteSet
{
	struct sprite *firstSprite;
	struct sprite *lastSprite;

	struct spriteSet *nextSet;
	struct spriteSet *prevSet;

	struct animation *Animations;

	int setID;
	int spriteCount;
};


struct displayData
{
	int currentSprite;
	struct sprite *spriteBuffer;

	int currentAnimation;
	struct animationFrame *frameBuffer;
	struct animation *animationBuffer;
	int animationTick;
	short animationLoopCount;

	struct spriteSet *spriteSetSource;

	RenderMode RenderModeOverride;
	int spriteXOffset;
	int spriteYOffset;
	int pixelXOffset;
	int pixelYOffset;
	float transparencyEffect;
	int invincibilityFrames;
};


struct physicsRect
{
	double xPos;
	double yPos;
	double xPosRight;
	double yPosTop;
	double prevXPos;
	double prevYPos;

	unsigned int xSize;
	unsigned int ySize;

	double forwardVelocity;
	double yVelocity;
	double xVelocity;

	double PhysicsXVelocity;
	double PhysicsYVelocity;

	SolidType solid;
	CollideType collideMode;
	Layer collideLayer;

	double direction;
	short xFlip;
	short yFlip;
	short crouch;

	float friction;
};


// Objects are memory-allocated instances of interactable items (tiles, enemies, etc.)
struct Object
{
	int ObjectID;
	ObjectState State;
	CurrentAction Action;

	struct Object *nextObject;
	struct Object *prevObject;

	struct Object *ParentObject;
	ParentType ParentLink;
	double ParentXOffset;
	double ParentYOffset;

	struct physicsRect *ObjectBox;
	struct displayData *ObjectDisplay;
	Layer layer;

	// Multi-purpose args
	int arg1;
	int arg2;
	int arg3;
	int arg4;
	int arg5;
};

// Memory allocated struct that contains pointers to Objects and Object count
struct ObjectController
{
	int objectCount;
	struct Object *firstObject;
	struct Object *lastObject;

	int cachedCount;
	struct Object *cachedFirstObject;
	struct Object *cachedLastObject;

	struct spriteSet *startSpriteSetPtr;
	int spriteSetCount;
};


// Controls the player character
struct playerData
{
	struct Object *PlayerPtr;

	struct physicsRect *PlayerBox;
	struct displayData *PlayerDisplay;
	struct physicsRect *InteractBox;

	// These variables can be freely modified according to your modified player controller
	int inAir;
	int jumpHeld;
	int jumpProgress;

	int coinCount;
	int HP;
};


struct TextInstance 
{
	struct TextInstance *nextText;

	struct Object *boxPtr;

	char textPhrase[MAX_TEXT_LENGTH];
	char Portrait[MAX_LEN];
	int font;
	int textBoxSprite;
	int textDelayFrames;
	int Skippable;
	char voice[MAX_LEN];
	VoiceMode voiceMode;
	int textXPosition;

	int currentChar;
	int Counter;
	int xOffset;
	int yOffset;

	int PlayOnSceneTick;
	int SceneTickOnTextEnd;
};


struct Camera 
{
	int CameraX;
	int CameraY;
	int minCameraX;
	int maxCameraX;
	int minCameraY;
	int maxCameraY;

	int CameraXBuffer;
	int CameraYBuffer;
	short CameraLatch;
	CameraState CameraMode;
};



// Memory allocated struct that controls camera, holds the Object list and level data
struct world
{
	struct Camera MainCamera;

	struct ObjectController *ObjectList;

	struct playerData *Player;

	struct TextInstance *TextQueue;

	int GamePaused;
	int level;
	LemonGameState GameState;
	LemonGameEvent GameEvent;

	CutsceneID CurrentCutscene;
	int SceneTick;
	int PlayingText;

	struct spriteSet *BackGrounds;
	struct sprite *bgSpriteBuffer;

	float bgParallax;
	double bgRowParallax;
	short ParallaxRowCutOff;
	short ParallaxRowStart;

	int drawnObjects;
	int drawnParticles;
	int drawnHudElements;
	short drawHitboxes;
	short drawSprites;
	short drawBackGround;
	short drawPlayer;
	short drawUI;
	short drawParticles;
	short drawObjects;
	char debugString[DEBUG_STRING_LENGTH];

	WorldPhysics PhysicsType;
	float GlobalGravityY;
	float GlobalGravityX;

	short depthCounter;
};


typedef struct soundInstance SoundInstance;
typedef struct soundChannel SoundChannel;
typedef struct sprite Sprite;
typedef struct spriteSet SpriteSet;
typedef struct displayData DisplayData;
typedef struct renderFrame RenderFrame;
typedef struct physicsRect PhysicsRect;
typedef struct playerData PlayerData;
typedef struct ObjectController ObjectController;
typedef struct Object Object;
typedef struct TextInstance TextInstance;
typedef struct Camera Camera;
typedef struct world World;
typedef struct animation Animation;
typedef struct animationFrame AnimationFrame;

extern SoundChannel SoundChannels[CHANNEL_COUNT];

extern int screenWidth;

extern int screenHeight;

extern int gameRunning;

extern int frameThrottle;

extern int enableDebugText;
