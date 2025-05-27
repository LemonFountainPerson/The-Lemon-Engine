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
#define CHANNEL_COUNT 12
#define LOOP_CHANNELS 4

#define V_RESOLUTION 720
#define H_RESOLUTION 1280

// Tiles Probably wont be re-implemented, but constants are useful for stylisation
#define Y_TILESCALE 32
#define X_TILESCALE 32

#define MAX_OBJECTS 128
#define MAX_PARTICLES 64
#define MAX_HUD_ELEMENTS 64

#define PLAYERHEIGHT 50
#define PLAYERWIDTH 32

#define MAX_LEN 80

#define IS_DEFINED 1
#endif


enum FunctionResult {
	LEMON_ERROR = -1,
	MISSING_DATA = -2,
	INVALID_DATA = -3,
	LEMON_SUCCESS = 0,
	ACTION_DISABLED = 1,
	EXECUTION_UNNECESSARY = 2,
	END_OF_FILE = 9
};


enum LemonKeys {
	LMN_ESCAPE = VK_ESCAPE,
	LMN_SPACE = VK_SPACE,
	LMN_UP = VK_UP,
	LMN_DOWN = VK_DOWN,
	LMN_RIGHT = VK_RIGHT,
	LMN_LEFT = VK_LEFT
};


enum lemonGameState {
	EMPTY_GAME = 0,
	LOADING = 1,
	GAMEPLAY = 2,
	CUTSCENE = 3,
	IN_MENU = 4,
	UNDEFINED_GAME_STATE
};

enum ChannelNames {
	MUSIC_CHANNEL = 0,
	PLAYER_SFX = 4,
	OBJECT_SFX = 6,
	ENEMIES_SFX = 7,
	SPEECH = 10
};


enum objectType {
	LEVEL_FLAG_OBJ = 0,
	SOLID_BLOCK = 1,
	RIGHT_SLOPE = 2,
	LEFT_SLOPE = 3,
	JUMP_THRU = 4,
	COIN = 5,
	MOVING_PLATFORM_HOR = 6,
	MOVING_PLATFORM_VER = 7,
	SPRING = 8,
	GATE_SWITCH = 9,
	GATE_SWITCH_TIMED = 10,
	VERTICAL_GATE = 11,
	HORIZONTAL_GATE = 12,
	PARTICLE = 13,
	UNDEFINED_OBJECT
};


//  Order of object list determines layering of individual objects within layers
enum Layer {
	LEVELFLAGS = -1,
	BACKGROUND = 0,
	MIDDLEGROUND = 1,
	FOREGROUND = 2,
	PARTICLES = 3,
	HUD = 4
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
	SCALE_TILE_FULL_ALPHA = 11
};


enum Flags {
	BACKGROUND_SET = 0,
	BACKGROUND_SET_TRIGGER = 1,
	TRIGGER_CUTSCENE = 2
};


enum ObjectState {
	TO_BE_DELETED = -1,
	DEFAULT = 0,
	PAUSE_BEHAVIOUR = 1,
	UNDEFINED_STATE
};


enum ParticleSubType {
	EMPTY_PARTICLE = 0,
	SPARKLE = 1,
	UNDEFINED_PARTICLE
};


enum GateSwitch {
	SINGLE_SWITCH = 0,
	CHAIN_SWITCH = 1
};


// Memory allocated structs of data

// Regular Sprites (Objects, player, particles, etc.)
struct sprite
{
	struct sprite *nextSprite;
	struct sprite *prevSprite;
	unsigned char *spriteData;

	int height;
	int width;
	enum RenderMode RenderMode;

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


// Objects are memory-allocated instances of interactable items (tiles, enemies, etc.)
struct object
{
	struct object *nextObject;
	struct object *prevObject;
	int objectID;
	enum ObjectState State;

	double xPos;
	double yPos;
	double xPosRight;
	double yPosTop;
	int xSize;
	int ySize;
	double xVel;
	double yVel;
	int solid;

	enum Layer layer;

	int currentAnimation;
	int animationTick;

	struct sprite *spriteBuffer;
	int currentSprite;
	int xFlip;
	int yFlip;

	enum RenderMode objectRenderMode;

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
	// Do not modify these variables to prevent game logic breaking
	double xPos;
	double yPos;
	double xPosRight;
	double yPosTop;
	double yVelocity;
	double xVelocity;
	double maxYVel;
	double maxXVel;

	int xFlip;
	int currentSprite;
	struct sprite *spriteBuffer;

	int spriteCount;
	struct spriteSet *spriteSetPtr;

	enum Layer playerLayer;

	// These variables can be freely modified according to your modified player controller
	double PhysicsXVelocity;
	double PhysicsYVelocity;
	double direction;

	int inAir;
	int jumpHeld;
	int jumpProgress;
	int crouch;

	int coinCount;
};


// Memory allocated struct that controls camera, holds the object list and level data
struct world
{
	struct objectController *objectList;
	int drawnObjects;
	int drawnParticles;
	int drawnHudElements;

	struct playerData *Player;

	struct spriteSet *BackGrounds;
	struct sprite *bgSpriteBuffer;

	double bgParallax;
	int bgTileVertically;
	int bgParallaxChunkSize;
	double bgChunkParallax;

	int cameraX;
	int cameraY;
	int level;
	double Gravity;

	int drawHitboxes;
	int drawSprites;
	int drawBackGround;
	int drawPlayer;
	int drawHud;
	int drawParticles;
	int drawObjects;
	int playBgMusic;

	int GamePaused;
	enum lemonGameState GameState;
};


typedef struct
{
	Uint8 *wav_data;
	Uint32 wav_data_len;
	SDL_AudioStream *stream;
} SoundInstance;

typedef struct {
	int width;
	int height;
	uint32_t *screen;
} RenderFrame;



typedef struct sprite Sprite;
typedef struct spriteSet SpriteSet;
typedef struct bgsprite BGSprite;
typedef struct bgSpriteSet BGSpriteSet;
typedef struct playerData PlayerData;
typedef struct objectController ObjectController;
typedef struct object Object;
typedef struct world World;

typedef enum lemonGameState LemonGameState;
typedef enum FunctionResult FunctionResult;
typedef enum LemonKeys LemonKeys;
typedef enum Layer Layer;
typedef enum RenderMode RenderMode;
typedef enum Flags Flags;
typedef enum ParticleSubType ParticleSubType;
typedef enum GateSwitch GateSwitch;


static SoundInstance AllSounds[CHANNEL_COUNT];

static int gameRunning = 1;

static double deltaTime = 1.0;




