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
#include <windowsx.h>
#define WIN32_LEAN_AND_MEAN

#include <SDL3/SDL.h>



//								Engine Settings (Can be modified via variables)
//-------------------------------------------------------------------------------------------------
// Most displayed objects are designed to adjust based on resolution; however the lowest expected resolution is 1280 x 720
// lower values may result in unintended behaviour
#define V_RESOLUTION 720
#define H_RESOLUTION 1280

#define TICKS_PER_SECOND 60
#define TICK_TARGET_DELTA (1.0/(double)TICKS_PER_SECOND)
#define TICK_DELTA (int)((TICK_TARGET_DELTA) * 10000)

#define RENDERS_PER_SECOND 60
#define RENDER_TARGET_DELTA (1.0/(double)RENDERS_PER_SECOND)
#define RENDER_DELTA (int)((RENDER_TARGET_DELTA) * 10000)

#define MAX_OBJECTS_RENDER 128
#define MAX_PARTICLES_RENDER 64
#define MAX_HUD_ELEMENTS_RENDER 128

#define MAX_SOUNDS_PER_CHANNEL 32

#define MAX_OBJECTS 6000
#define RESERVED_OBJECTS 500
#define PRESERVED_SPRITESETS 5
#define OBJECT_PREALLOCATION 0

#define X_WORLD_BOUND 100000.0
#define Y_WORLD_BOUND 100000.0

#define MULTITHREADED_ENABLED 0

//-------------------------------------------------------------------------------------------------


//									Root Folder locations
//-------------------------------------------------------------------------------------------------
#define SOUND_ROOT "LemonData/Sounds/"
#define SPRITE_ROOT "LemonData/Sprites/"
#define ANIMATION_ROOT "LemonData/Animations/"
#define LEVELDATA_ROOT "LemonData/LevelData/"

//-------------------------------------------------------------------------------------------------


//								Engine constants
//-------------------------------------------------------------------------------------------------
#define DEBUG_STRING_LENGTH 512
#define MAX_LEN 80
#define ENCRYPT_OFFSET 600

// Number of textInstances allowed in the TextQueue at once
#define MAX_TEXTQUEUE_LENGTH 1000

#define MAX_SPRITE_SIZE 3000

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
#define COLLISION_CYCLES 6
#define COLLISION_DEPTH 9

#define MAX_Y_VELOCITY 64.0
#define MAX_X_VELOCITY 64.0
#define MAX_FORWARD_VELOCITY 64.0

//-------------------------------------------------------------------------------------------------


//								Useful constants
//-------------------------------------------------------------------------------------------------
#define RADIAN_15 0.26179938779
#define RADIAN_30 0.52359877559
#define RADIAN_45 0.78539816339
#define RADIAN_60 1.0471975512
#define RADIAN_75 1.308996939 
#define RADIAN_90 1.5707963268
#define RADIAN_360 6.2831853072
#define DEGREE_TO_RADIAN_PI 0.01745329251 
#define RADIAN_TO_DEGREE_PI 57.2957795131

#define NO_PORTRAIT ""
#define USE_CURRENT_SPRITESET 0

//-------------------------------------------------------------------------------------------------


#define LEMON_VERSION "V0.07"

#define IS_DEFINED 1
#endif


enum FunctionResult{
	LEMON_ERROR = -1,
	MISSING_DATA = -2,
	INVALID_DATA = -3,
	TASK_FAILED = -4,
	LEMON_SUCCESS = 0,
	ACTION_DISABLED = 1,
	EXECUTION_UNNECESSARY = 2,
	AT_FULL_CAPACITY = 3,
	FILE_NOT_FOUND = 8,
	END_OF_FILE = 9
};


enum DebugTextSetting {
	DEBUG_TEXT_DISABLED = 0,
	ONLY_ERRORS = 1,
	ALL_EVENTS = 2
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
	CHANNEL_COUNT = 8 	// Simultaniously used as 'last', undefined channel and channel count
};


enum VoiceMode {
	PLAY_EACH_CHARACTER = 0,
	PLAY_ONCE
};


enum PortraitPos {
	INSIDE_BOX_LEFT,
	INSIDE_BOX_RIGHT,
	ON_TOP_BOX_LEFT,
	ON_TOP_BOX_RIGHT
};


enum TextPreset {
	WB_BOTTOM = 0,
	WB_TOP,
	WB_BOTTOM_FLIP,
	WB_TOP_FLIP,
	SILENT_BOTTOM,
	SILENT_TOP,
	BW_BOTTOM,
	BW_TOP,
	PLAINTEXT_BOTTOM,
	PLAINTEXT_TOP,
	UNDEFINED_PRESET
};


enum TextType {
	REGULAR_TEXT,
	OPTION_PROMPT
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

	/*	UNIMPLEMENTED - May or may not implement in future, as it is a very situational render mode
	TILE_SCALE = 8,
	TILE_SCALE_FULL_ALPHA = 9,
	SCALE_TILE = 10,
	SCALE_TILE_FULL_ALPHA = 11,
	*/

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


enum WorldPhysics {
	TOP_DOWN,
	PLATFORMER,
	UNDEFINED_PHYSICS
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
	STATIC,
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
	TRANSPARENCY_LINK	= 0b00010000,
	DISPLAYDATA_LINK	= 0b00100000

};


enum SolidType {
	UNSOLID = 0,
	SOLID = 1,
	CIRCLE = 2,
	FLAT_SLOPE = 3,
	JUMP_THROUGH = 4,
	ENTITY = 5,
	ENTITY_SOLID = 6,
	IGNORE_SOLID = 7,
	PUSHABLE_SOLID = 8,
	UNDEFINED_SOLID
};


enum CollideType {
	NO_COLLIDE_TYPE,
	IMPACT,
	PUSH
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
	PROJECTILE = 20,
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
	MOUSE_CURSOR,
	TEXT_BOX,
	TEXT_CHARACTER,
	TEXT_PORTRAIT,
	UNDEFINED_UI_ELEMENT
};


enum GateSwitch {
	SINGLE_SWITCH = 0,
	CHAIN_SWITCH = 1
};


enum RotateMode {
	ROTATE_ALL = 0,
	ROTATE_SPRITE,
	ROTATE_BOX
};


typedef enum FunctionResult FunctionResult;
typedef enum LemonKeys LemonKeys;
typedef enum ChannelName ChannelName;
typedef enum TextBox TextBox;
typedef enum VoiceMode VoiceMode;
typedef enum PortraitPos PortraitPos;
typedef enum TextPreset TextPreset;
typedef enum TextType TextType;
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
typedef enum RotateMode RotateMode;


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

	unsigned int height;
	unsigned int width;
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
	float rotation;
};


// Starts counting animation IDs from 1
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
// Starts counting sprite IDs from 1
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

	float animationTick;
	int currentAnimation;
	short animationLoopCount;
	struct animationFrame *frameBuffer;
	struct animation *animationBuffer;

	struct spriteSet *spriteSetSource;

	RenderMode RenderModeOverride;
	double direction;
	int spriteXOffset;
	int spriteYOffset;
	int pixelXOffset;
	int pixelYOffset;

	float transparencyEffect;
	int invincibilityFrames;
};


struct PhysicsRect
{
	double xPos;
	double yPos;
	double xPosRight;
	double yPosTop;
	double prevXPos;
	double prevYPos;

	int xSize;
	int ySize;

	double forwardVelocity;
	double yVelocity;
	double xVelocity;

	int inAir;
	double PhysicsXVelocity;
	double PhysicsYVelocity;
	struct PhysicsRect *GroundBox;

	SolidType solid;
	CollideType collideMode;
	Layer collideLayer;

	double direction;
	short xFlip;
	short yFlip;
	int crouch;
};


// Objects are memory-allocated instances of interactable items (tiles, enemies, etc.)
struct Object
{
	int ObjectID;
	ObjectState State;
	CurrentAction Action;

	struct PhysicsRect *ObjectBox;
	struct displayData *ObjectDisplay;
	Layer layer;

	struct Object *ParentObject;
	ParentType ParentLink;

	struct Object *nextObject;
	struct Object *prevObject;

	// Multi-purpose args
	int arg1;
	int arg2;
	int arg3;
	int arg4;
	int arg5;
};


struct AABB_BoundingBox
{
	double xPos;
	double yPos;

	int Width;
	int Height;
};


struct QuadTree
{
	struct QuadTree *Children[4];

	struct Object* ObjectList[4];

	struct AABB_BoundingBox BoundingBox;
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

	void *FrameUpdates;

	struct QuadTree *QuadTreeRoot;

	int depthCounter;
};


// Controls the player character
struct playerData
{
	struct Object *PlayerPtr;

	struct PhysicsRect *PlayerBox;
	struct displayData *PlayerDisplay;
	struct PhysicsRect *InteractBox;

	// These variables can be freely modified according to your modified player controller
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
	char font[MAX_LEN];
	char textBoxSprite[MAX_LEN];
	
	char voice[MAX_LEN];
	VoiceMode voiceMode;

	char Portrait[MAX_LEN];
	PortraitPos PortraitPosition;

	TextType textTypeSetting;

	int textDelayFrames;
	int Skippable;
	int SceneTickOnTextEnd;

	int textXPos;
	int textEndXPos;
	int textYPos;

	int currentChar;
	int Counter;
	int currentXPos;
	int currentYPos;
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
	int CameraLatch;
	CameraState CameraMode;
};


struct BackgroundData
{
	struct spriteSet *BackgroundSpriteSet;
	struct sprite *BackgroundSpriteBuffer;

	float bgParallax;
	float bgRowParallax;
	int ParallaxRowCutOff;
	int ParallaxRowStart;
};



// Memory allocated struct that controls camera, holds the Object list and level data
struct World
{
	struct Camera MainCamera;

	struct BackgroundData WorldBackground;

	struct ObjectController *ObjectList;

	struct playerData *Player;

	struct TextInstance *TextQueue;
	int PlayingText;

	LemonGameState GameState;
	LemonGameEvent GameEvent;
	int GamePaused;
	int level;
	CutsceneID CurrentCutscene;
	int SceneTick;

	int drawnObjects;
	int drawnParticles;
	int drawnHudElements;

	WorldPhysics PhysicsType;
	double GlobalGravityY;
	double GlobalGravityX;
};


struct FrameUpdateFunction
{
	struct FrameUpdateFunction *nextFunction;

	struct Object *inputObject;
	int (*FunctionPointer)(struct Object*, struct World *GameWorld);
};


struct MouseData
{
	int LeftButton;
	int RightButton;
	int MiddleButton;
	int SideButton1;
	int SideButton2;

	float xPos;
	float yPos;
};


struct EngineData
{
	// Multi threading still not implemented!
	int MultiThreadingEnabled;

	int ObjectPreAllocationEnabled;

	int MaxObjects;
	int ReservedObjects;
	int PreservedSpriteSets;

	double WorldBoundX;
	double WorldBoundY;

	int MaxSoundsPerChannel;
};


struct RenderData
{
	int drawHitboxes;
	int drawSprites;
	int drawBackGround;
	int drawObjects;
	int drawPlayer;
	int drawUI;
	int drawParticles;

	int maxObjects;
	int maxParticles;
	int maxUIElements;
};


struct DebugData
{
	int DebugTextEnabled;
	char DebugString[DEBUG_STRING_LENGTH];

	int DisplayPlayerData;

	int HitboxOutlineThickness;
};


typedef struct soundInstance SoundInstance;
typedef struct soundChannel SoundChannel;

typedef struct sprite Sprite;
typedef struct spriteSet SpriteSet;
typedef struct animation Animation;
typedef struct animationFrame AnimationFrame;
typedef struct renderFrame RenderFrame;

typedef struct PhysicsRect PhysicsRect;
typedef struct displayData DisplayData;
typedef struct playerData PlayerData;

typedef struct ObjectController ObjectController;
typedef struct FrameUpdateFunction FrameUpdateFunction;
typedef struct Object Object;

typedef struct TextInstance TextInstance;
typedef struct Camera Camera;
typedef struct BackgroundData BackgroundData;
typedef struct World World;

typedef struct MouseData MouseData;

typedef struct EngineData EngineData;
typedef struct RenderData RenderData;
typedef struct DebugData DebugData;


//Global variables/data
extern int MaxSoundsPerChannel;

extern SoundChannel SoundChannels[CHANNEL_COUNT];

extern int GameTicksPerSecond;

extern int TickDelta;

extern int RendersPerSecond;

extern int RenderDelta;

extern int screenWidth;

extern int screenHeight;

extern uint32_t *screenBuffer;

extern unsigned char RotateRenderBuffer[MAX_SPRITE_SIZE * MAX_SPRITE_SIZE * 4];

extern MouseData MouseInput;

extern int keyboard[256];

extern EngineData EngineSettings;

extern RenderData RenderSettings;

extern DebugData DebugSettings;


extern int Running_In_Windows_Mode;		// Windows causes a lot shennanigans on the technical side; should be depreciated later