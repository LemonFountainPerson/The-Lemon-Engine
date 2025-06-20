#ifndef IS_DEFINED
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <stdbool.h>
#include <Windows.h>
#include <mmsystem.h>
#define WIN32_LEAN_AND_MEAN

#include <SDL3/SDL.h>
#define CHANNEL_COUNT 8
#define MAX_SOUNDS_PER_CHANNEL 32

#define V_RESOLUTION 720
#define H_RESOLUTION 1280

#define TARGET_DELTA (1.0/60.0)
#define TICKS_PER_SECOND 60
#define TICK_DELTA (int)((1.0/60.0) * 1000)

#define MAX_OBJECTS 4096
#define MAX_OBJECTS_RENDER 128
#define MAX_PARTICLES_RENDER 64
#define MAX_HUD_ELEMENTS_RENDER 64

// Tiles Probably wont be re-implemented, but constants are useful for stylisation
#define Y_TILESCALE 32
#define X_TILESCALE 32

#define RADIAN_90 1.5707963268
#define PLAYERHEIGHT 50
#define PLAYERWIDTH 32

#define MAX_LEN 80
#define ENCRYPT_OFFSET 600

#define LEMON_VERSION "V0.05"

#define IS_DEFINED 1
#endif


enum FunctionResult{
	LEMON_ERROR = -1,
	MISSING_DATA = -2,
	INVALID_DATA = -3,
	LEMON_SUCCESS = 0,
	ACTION_DISABLED = 1,
	EXECUTION_UNNECESSARY = 2,
	END_OF_FILE = 9
};


enum LemonKeys{
	LMN_SPACE = 0,
	LMN_ESCAPE = 1,
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
	LMN_INTERACT3 = 111
};


enum ChannelNames {
	LOOP_CHANNEL = 0,
	SPEECH = 1,
	PLAYER_SFX = 2,
	OBJECT_SFX = 3,
	ENEMIES_SFX = 4,
};


// If the object's render mode is less than 0 (the default) the sprite is rendered according to the sprite's individual render mode
// Otherwise, it is overridden to be the rendermode of the object
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
	BG_ROW_PARALLAX
};


//  Order of object list determines layering of individual objects within layers
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
	TO_BE_DELETED = -1,
	DEFAULT = 0,
	PAUSE_BEHAVIOUR = 1,
	CUTSCENE_ACTOR = 2,
	DEFEATED = 3,
	BEING_CARRIED = 4,
	IN_INVENTORY = 5,
	UNDEFINED_STATE
};


enum SolidType {
	UNSOLID = 0,
	SOLID = 1,
	FLAT_SLOPE = 2,
	JUMP_THROUGH = 3,
	ENTITY = 4,
	UNDEFINED_SOLID
};


enum ObjectType {
	LEVEL_FLAG_OBJ = 0,
	SOLID_BLOCK = 1,
	FLAT_SLOPE_FLOOR = 2,
	JUMP_THRU = 3,
	UI_ELEMENT = 4,
	PARTICLE = 5,
	COIN = 6,
	MOVING_PLATFORM_HOR = 7,
	MOVING_PLATFORM_VER = 8,
	SPRING = 9,
	GATE_SWITCH = 10,
	GATE_SWITCH_TIMED = 11,
	VERTICAL_GATE = 12,
	HORIZONTAL_GATE = 13,
	DOOR = 14,
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
	UNDEFINED_FLAG
};


enum ParticleSubType {
	EMPTY_PARTICLE = 0,
	SPARKLE,
	UNDEFINED_PARTICLE
};


enum UISubType {
	BASIC_GRAPHIC = 0,
	OPTION_BUTTON,
	TEXT_BOX,
	TEXT_CHARACTER,
	PAUSE_MENU_CONTROLLER,
	PAUSE_HEADER,
	PAUSE_BACKGROUND,
	SETTINGS_MENU_CONTROLLER,
	SETTINGS_HEADER,
	PLAYER_HUD_CONTROLLER,
	UNDEFINED_UI_ELEMENT
};


enum GateSwitch {
	SINGLE_SWITCH = 0,
	CHAIN_SWITCH = 1
};


typedef enum FunctionResult FunctionResult;
typedef enum LemonKeys LemonKeys;
typedef enum ChannelNames ChannelNames;
typedef enum RenderMode RenderMode;
typedef enum Layer Layer;
typedef enum LemonGameState LemonGameState;
typedef enum CutsceneID CutsceneID;
typedef enum CameraState CameraState;
typedef enum ObjectState ObjectState;
typedef enum SolidType SolidType;
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
	float volume;

	int channelID;
	struct soundInstance *nextSound;
	struct soundInstance *prevSound;
};


struct soundChannel
{
	int soundCount;

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


// Sprite sets that lead to linked lists of sprites corresponding to an object type;
// only needs to be initialised once per object type
struct spriteSet
{
	struct sprite *firstSprite;
	struct sprite *lastSprite;

	struct spriteSet *nextSet;
	struct spriteSet *prevSet;

	int setID;
	int spriteCount;
};


struct displayData
{
	RenderMode RenderModeOverride;

	int currentSprite;
	struct sprite *spriteBuffer;

	struct spriteSet *spriteSetSource;
};


struct physicsRect
{
	double xPos;
	double yPos;
	double xPosRight;
	double yPosTop;

	int xSize;
	int ySize;

	double yVelocity;
	double xVelocity;
	double PhysicsXVelocity;
	double PhysicsYVelocity;

	SolidType solid;
	int crouch;

	int xFlip;
	int yFlip;
	double direction;

	int spriteXOffset;
	int spriteYOffset;
};


// Objects are memory-allocated instances of interactable items (tiles, enemies, etc.)
struct object
{
	int ObjectID;
	ObjectState State;
	Layer layer;
	struct object *nextObject;
	struct object *prevObject;
	struct object *ParentObject;

	struct physicsRect *ObjectBox;
	struct displayData *ObjectDisplay;
	int currentAnimation;
	int animationTick;

	// Multi-purpose args
	int arg1;
	int arg2;
	int arg3;
	int arg4;
	int arg5;
};

// Memory allocated struct that contains pointers to objects and object count
struct objectController
{
	int objectCount;
	struct object *firstObject;
	struct object *lastObject;

	struct spriteSet *startSpriteSetPtr;
	int spriteSetCount;
};


// Controls the player character
struct playerData
{
	struct physicsRect *PlayerBox;
	struct physicsRect *InteractBox;
	struct displayData *PlayerDisplay;

	Layer PlayerLayer;
	ObjectState PlayerState;

	// These variables can be freely modified according to your modified player controller
	double maxYVel;
	double maxXVel;

	int inAir;
	int jumpHeld;
	int jumpProgress;

	int coinCount;
	int HP;
};


// Memory allocated struct that controls camera, holds the object list and level data
struct world
{
	struct objectController *ObjectList;
	int drawnObjects;
	int drawnParticles;
	int drawnHudElements;

	struct playerData *Player;

	struct spriteSet *BackGrounds;
	struct sprite *bgSpriteBuffer;

	float bgParallax;
	int bgParallaxChunkSize;
	double bgChunkParallax;

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

	int drawHitboxes;
	int drawSprites;
	int drawBackGround;
	int drawPlayer;
	int drawUI;
	int drawParticles;
	int drawObjects;

	LemonGameState GameState;
	int level;
	CutsceneID CurrentCutscene;
	int SceneTick;

	float Gravity;
	int GamePaused;
};


typedef struct soundInstance SoundInstance;
typedef struct soundChannel SoundChannel;
typedef struct sprite Sprite;
typedef struct spriteSet SpriteSet;
typedef struct displayData DisplayData;
typedef struct renderFrame RenderFrame;
typedef struct physicsRect PhysicsRect;
typedef struct playerData PlayerData;
typedef struct objectController ObjectController;
typedef struct object Object;
typedef struct world World;


static SoundChannel SoundChannels[CHANNEL_COUNT];

extern int screenWidth;

extern int screenHeight;

extern int gameRunning;

extern int frameThrottle;

