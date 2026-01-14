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
#include <stdarg.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>




//								Engine Settings (Can be modified via variables)
//-------------------------------------------------------------------------------------------------
// Most displayed objects are designed to adjust positioning based on resolution; 
// however the lowest expected resolution is 1280 x 720, lower values may result in unintended behaviour
#define V_RESOLUTION 720
#define H_RESOLUTION 1280

#define TICKS_PER_SECOND 60
#define RENDERS_PER_SECOND 120

#define MAX_OBJECTS_RENDER 200
#define MAX_PARTICLES_RENDER 64
#define MAX_HUD_ELEMENTS_RENDER 128

#define MAX_SOUNDS_PER_CHANNEL 16
#define MAX_TEXTQUEUE_LENGTH 200			// Number of textInstances allowed in the TextQueue at once
#define MAX_QUEUED_GAME_EVENTS 16

#define MAX_OBJECTS 15000
#define MAX_PARTICLES 1000
#define RESERVED_OBJECTS 500
#define PRESERVED_SPRITESETS 2

#define X_WORLD_BOUND 100000.0
#define Y_WORLD_BOUND 100000.0

#define MULTITHREADED_ENABLED 	false 		// UNIMPLEMENTED
#define OBJECT_PREALLOCATION 	true  		

#define DEFAULT_TEXTURE "Missing.png"

//-------------------------------------------------------------------------------------------------


//									Root Folder locations
//-------------------------------------------------------------------------------------------------
#define SOUND_ROOT 		"LemonData/Sounds/"
#define SPRITE_ROOT 	"LemonData/Sprites/"
#define ANIMATION_ROOT 	"LemonData/AnimationData/"
#define LEVELDATA_ROOT 	"LemonData/LevelData/"

//-------------------------------------------------------------------------------------------------


//								Engine constants
//-------------------------------------------------------------------------------------------------
// Avoid editing these values, as it may cause issues/performance loss
#define OBJECT_NAME_LENGTH 15
#define MAX_SPRITE_SIZE 3000

#define CONSOLE_STRING_LENGTH 512
#define MAX_DEBUG_TEXTS 32
#define DEBUG_TEXT_MAX_LENGTH 400

#define MAX_LEN 80
#define INT_MAX_LEN 20

#define ENCRYPT_OFFSET 600	

#define MINIMUM_SCREEN_WIDTH 144
#define MINIMUM_SCREEN_HEIGHT 144
#define MINIMUM_ZOOM 0.4  		 // Do not set this value to 0.1 or below

//-------------------------------------------------------------------------------------------------


//								Game constants
//-------------------------------------------------------------------------------------------------
#define MAX_TEXT_LENGTH 200

#define MAX_OPTIONS 4
#define OPTION_TEXT_MAX_LEN 50

// Tiles Probably wont be re-implemented, but constants are useful for stylisation
#define Y_TILESCALE 32
#define X_TILESCALE 32

//-------------------------------------------------------------------------------------------------


//								Physics values
//-------------------------------------------------------------------------------------------------
#define COLLISION_CYCLES 5
#define COLLISION_DEPTH 16

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
#define BACKGROUND_SETID 1

//-------------------------------------------------------------------------------------------------


#define LEMON_VERSION "V0.08"

#define IS_DEFINED 1
#endif


typedef enum FunctionResult 
{
	LEMON_ERROR = -1,
	MISSING_DATA = -2,
	INVALID_DATA = -3,
	TASK_FAILED = -4,
	LEMON_SUCCESS = 0,
	ACTION_DISABLED = 1,
	EXECUTION_UNNECESSARY = 2,
	AT_FULL_CAPACITY = 3,
	FILE_NOT_FOUND = 4,
	END_OF_FILE = 5,
	DATA_CLEARED = 6
} FuncResult;


typedef enum ConsoleTextSetting 
{
	CONSOLE_TEXT_DISABLED = 0,
	ONLY_ERRORS = 1,
	ALL_EVENTS = 2,
	CONSOLE_TEXT_SETTING_COUNT
} ConsoleTextSetting;


typedef enum LemonKeys 
{
	// 0 - 31 are control charcacters and should not be used
	// 32 - 127 are reserved for ASCII keys
	ACKNOWLEDGE_INPUT = 0,
	LMN_SPACE = 128,
	LMN_ESCAPE = 129,
	LMN_ENTER = 130,
	LMN_UPARROW = 131,
	LMN_DOWNARROW = 132,
	LMN_LEFTARROW = 133,
	LMN_RIGHTARROW = 134,
	LMN_UP = 150,
	LMN_DOWN = 151,
	LMN_LEFT = 152,
	LMN_RIGHT = 153,
	LMN_JUMP = 154,
	LMN_INTERACT = 155,
	LMN_INTERACT2 = 156,
	LMN_INTERACT3 = 157,
	LMN_TEXT_CONFIRM = 158,
	LMN_TEXT_SKIP = 159,
	LMN_MENU_CONFIRM = 160,
	INPUT_COUNT,

	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE
} LemonKeys;


typedef enum ChannelName 
{
	LOOP_CHANNEL = 0,
	SPEECH = 1,
	PLAYER_SFX = 2,
	OBJECT_SFX = 3,
	ENEMIES_SFX = 4,
	CHANNEL_COUNT 	// Simultaniously used as 'last', undefined channel and channel count
} ChannelName;


typedef enum VoiceMode 
{
	PLAY_EACH_CHARACTER = 0,
	PLAY_ONCE
} VoiceMode;


typedef enum PortraitPos 
{
	INSIDE_BOX_LEFT,
	INSIDE_BOX_RIGHT,
	ON_TOP_BOX_LEFT,
	ON_TOP_BOX_RIGHT
} PortraitPos;


typedef enum TextPreset 
{
	BASIC_TEXT = 0,
	BASIC_TOP,
	BASIC_FADE,
	BASIC_TOP_FADE,
	BASIC_FAST,
	BASIC_FLIP,
	BASIC_TOP_FLIP,
	WHITE_TEXT,
	WHITE_TOP,
	SILENT_TEXT,
	SILENT_TOP,
	PLAINTEXT_BOTTOM,
	PLAINTEXT_TOP,
	UNDEFINED_PRESET
} TextPreset;


typedef enum TextType 
{
	REGULAR_TEXT,
	OPTION_PROMPT,
	TRIGGER_EVENT
} TextType;


// If the Object's render mode is less than 0 (the default) the sprite is rendered according to the sprite's individual render mode
// Otherwise, it is overridden to be the rendermode of the Object
// Objects essentially can either have the sprites render how they would like to be rendered, or can override it with a single rendermode

typedef enum RenderMode 
{
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
	STATIC_BACKGROUND,
	SINGLE_BACKGROUND,
	TILE_BACKGROUND,
	UNDEFINED_RENDERMODE
} RenderMode;


//  Order of Object list determines layering of individual Objects within layers
typedef enum Layer 
{
	BACKGROUND = 0,
	MIDDLEGROUND,
	MIDDLEGROUND_2,
	FOREGROUND,
	PARTICLES,
	HUD,
	FRONT_LAYER,
	LAYER_COUNT,
	UNDEFINED_LAYER
} Layer;


typedef enum LemonGameState 
{
	CLOSE_GAME = -1,
	EMPTY_GAME = 0,
	LOADING = 1,
	GAMEPLAY = 2,
	CUTSCENE = 3,
	IN_MENU = 4,
	UNDEFINED_GAME_STATE
} LemonGameState;


typedef enum GameEventID
{
	NO_EVENT = 0,
	SWITCH_LEVEL,
	CHANGE_SCREEN_SIZE,
	SET_SCREEN_AND_RENDERER_SIZE,
	ENABLE_FULLSCREEN,
	DISABLE_FULLSCREEN,
	CHANGE_SCREEN_SIZE_SCALE,
	ENABLE_FULLSCREEN_SCALE,
	SET_CAMERA_ZOOM,
	CHANGE_CAMERA_ZOOM,
	STREAM_LEVEL_PARTITION,
	UNDEFINED_EVENT
} GameEventID;


typedef enum WorldPhysics 
{
	TOP_DOWN,
	PLATFORMER,
	POINT_AND_CLICK,
	UNDEFINED_PHYSICS
} WorldPhysics;


typedef enum CutsceneID 
{
	END_CUTSCENE = -2,
	PLAYING_CUTSCENE = -1,
	NO_CUTSCENE = 0,
	TEST_SCENE = 1,
	TEST_SCENE_2 = 2,
	TEST_SCENE_2_AGAIN = 3,
	TEST_SCENE_2_CORRECT,
	TEST_SCENE_2_WRONG,
	UNDEFINED_CUTSCENE
} CutsceneID;


typedef enum CameraState 
{
	FOLLOW_PLAYER = 0,
	MENU_CAMERA = 1,
	FREE_ROAM = 2,
	FREE_ROAM_RESTRICTED = 3,
	UNDEFINED_CAMERA_STATE
} CameraState;


// Communication with physics execution
typedef enum PhysicsFlags 
{
	PHYSICS_DISABLED,
	AWAITING_UPDATE,
	AWAITING_MAGNETISATION,
	AWAITING_RESOLUTION
} PhysicsFlags;


typedef enum ObjectState 
{
	EMPTY_OBJECT = -2,
	TO_BE_DELETED = -1,
	DEFAULT = 0,
	STATIC,
	PAUSE_BEHAVIOUR,
	ACTOR,
	BEING_CARRIED,
	IN_INVENTORY,
	UNDEFINED_STATE
} ObjectState;

 
typedef enum CurrentAction 
{
	IDLE = 0,
	DEFEATED,
	CHASING,
	ATTACKING,
	FLEEING,
	UNDEFINED_ACTION
} CurrentAction;


typedef enum CurrentInterrupt
{
	NO_INTERRUPT,
	INTERACTION_INTERRUPT,
	MOUSEDOWN_INTERRUPT,
	MOUSECLICK_INTERRUPT,
	UNDEFINED_INTERRUPT
} CurrentInterrupt;


typedef enum ParentType 
{
	DEFAULT_LINK 		= 			0b00000000,
	POSITION_LINK 		= 			0b00000001,
	VELOCITY_LINK 		= 			0b00000010,
	SPRITE_LINK 		= 			0b00000100,
	ANIMATION_LINK 		= 			0b00001000,
	TRANSPARENCY_LINK	= 			0b00010000,
	DISPLAYDATA_LINK	= 			0b00100000,
	FINAL_LINK			= 			0b10000000
} ParentType;


typedef enum SolidType 
{
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
} SolidType;


typedef enum CollideType 
{
	NO_COLLIDE_TYPE,
	IMPACT,
	PUSH
} CollideType;


typedef enum ObjectType 
{
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
	OBJECT_TYPE_COUNT,
	UNDEFINED_OBJECT
} ObjectType;


typedef enum Flags 
{
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
	STREAM_PARTITION_TRIGGER,
	UNDEFINED_FLAG
} Flags;


typedef enum ParticleSubType 
{
	EMPTY_PARTICLE = 0,
	SPARKLE,
	UNDEFINED_PARTICLE
} ParticleSubType;


// UISubType is shared by all UI_xxx objects, although behaviour, 
// sprites and animation are separate - if you set a UI_ELEMENT's subtype to TEXT_BOX 
// it will result in it being treated as a BASIC GRAPHIC
typedef enum UISubType 
{
	BASIC_GRAPHIC = 0,
	FADEOUT,
	OPTION_BUTTON,
	PAUSE_MENU_CONTROLLER,
	PAUSE_HEADER,
	PAUSE_BACKGROUND,
	SETTINGS_MENU_CONTROLLER,
	SETTINGS_HEADER,
	VIDEO_SETTINGS_CONTROLLER,
	SOUND_SETTINGS_CONTROLLER,
	PLAYER_HUD_CONTROLLER,
	MOUSE_CURSOR,
	TEXT_BOX,
	TEXT_CHARACTER,
	TEXT_PORTRAIT,
	TEXTOPTION_CURSOR,
	UNDEFINED_UI_ELEMENT
} UISubType;


typedef enum GateSwitch 
{
	SINGLE_SWITCH = 0,
	CHAIN_SWITCH = 1
} GateSwitch;


typedef enum RotateMode 
{
	ROTATE_ALL = 0,
	ROTATE_SPRITE,
	ROTATE_BOX
} RotateMode;


typedef enum AnimationLoopState
{
	LOOP_INDEFINITELY = -1,
	ONE_FRAME_INDEFINITE_ANIMATION = -2
} AnimationLoopState;


typedef enum DebugTextRenderMode
{
	DEBUG_TEXT_DISABLED = 0,
	DEBUG_TEXT_ENABLED,
	ONLY_PLAYER_INFO,
	ONLY_NONSTATIC_OBJECT_INFO,
	ALL_OBJECT_INFO,
	DEBUG_TEXT_MODE_COUNT
} DebugTextMode;


typedef enum DebugTextFormatting
{
	CAMERA_RELATIVE,
	SCREEN_RELATIVE,
	SCREEN_LIST_FORMAT,
	SCREEN_SOUND_FORMAT
} DebugTextFormatting;


// Memory allocated structs of data
struct soundInstance
{
	Uint8 *wav_data;
	Uint32 wav_data_len;
	SDL_AudioStream *stream;
	SDL_AudioFormat format;
	bool mono;

	char name[MAX_LEN];
	float volume;
	int repeatTimes;

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


struct RenderFrame 
{
	SDL_Window *Window;
	int windowWidth;
	int windowHeight;
	SDL_Surface *Screen;
	SDL_Renderer *Renderer;

	bool Fullscreen;

	int FramesElapsed;
	clock_t lastSecond;
};


struct DrawData
{
	int xDraw;
	int xDraw2;
	int yDraw;
	int yDraw2;

	int xOffset;
	int yOffset;
};


// Regular Sprites (Objects, player, particles, etc.)
struct sprite
{
	struct sprite *nextSprite;
	struct sprite *prevSprite;

	SDL_Texture *texture;
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
	int SpriteXOffset;
	int SpriteYOffset;
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

	int currentAnimation;
	float animationTick;
	int animationLoopCount;
	float animationSpeed;
	struct animationFrame *frameBuffer;
	struct animation *animationBuffer;

	struct spriteSet *spriteSetSource;

	RenderMode RenderModeOverride;
	float size;
	double direction;
	int spriteXOffset;
	int spriteYOffset;
	unsigned int pixelXOffset;
	unsigned int pixelYOffset;

	float transparencyEffect;
	bool hidden;
};


struct PhysicsRect
{
	float xPos;
	float yPos;
	float xPosRight;
	float yPosTop;
	float prevXPos;
	float prevYPos;

	int xSize;
	int ySize;

	float forwardVelocity;
	float yVelocity;
	float xVelocity;

	int inAir;
	float PhysicsXVelocity;
	float PhysicsYVelocity;
	struct PhysicsRect *GroundBox;

	SolidType solid;
	CollideType collideMode;
	Layer collideLayer;

	double direction;
	int xFlip;
	int yFlip;
	bool crouch;
};


// Objects are memory-allocated instances of interactable items (tiles, enemies, etc.)
struct Object
{
	char name[OBJECT_NAME_LENGTH + 1];	// unique identifier for Object 		
	int ObjectID;
	ObjectState State;
	CurrentAction Action;
	CurrentInterrupt Interrupt;	
	Layer layer;
	PhysicsFlags reserved;

	struct PhysicsRect *ObjectBox;
	struct displayData *ObjectDisplay;

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


// UNIMPLEMENTED
struct AABB_BoundingBox
{
	float xPos;
	float yPos;

	int Width;
	int Height;
};


// UNIMPLEMENTED
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

	struct Object *availableSlots;

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


union TextEventTriggerData
{
	CutsceneID cutscene;
	struct Object *TriggerObject;
};

typedef enum TriggerableFunctionID
{
	NO_ACTION,
	START_CUTSCENE,
	MOVE_PLAYER_TO_EXIT_DOOR
} TriggerableFunctionID;

struct TextEventTrigger
{
	union TextEventTriggerData FunctionArguments;
	TriggerableFunctionID TriggerFunction;
};


struct TextOptionPrompt
{
	int SelectedOption;
	int numberOfOptions;
	int optionBeingPrinted;

	float OptionYPositions[MAX_OPTIONS];

	char optionNames[MAX_OPTIONS][OPTION_TEXT_MAX_LEN];

	struct TextEventTrigger optionTriggers[MAX_OPTIONS];
};


union TextTypeData
{
	struct TextEventTrigger TriggerEvent;
	struct TextOptionPrompt OptionPrompt; 
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
	union TextTypeData textTypeData;

	int textDelayFrames;
	bool Skippable;

	int boxOffsetX;
	int boxOffsetY;
	int textLengthSize;

	int currentChar;
	int Counter;
	int currentXPos;
	int currentYPos;
};


struct Camera 
{
	float CameraX;
	float CameraY;
	float minCameraX;
	float maxCameraX;
	float minCameraY;
	float maxCameraY;

	float CameraXBuffer;
	float CameraYBuffer;
	bool CameraLatch;
	CameraState CameraMode;

	float zoomX;
	float zoomY;
	int zoomedWidth;
	int zoomedHeight;
};


struct BackgroundData
{
	struct spriteSet *BackgroundSpriteSet;
	struct sprite *BackgroundSpriteBuffer;

	float bgParallax;

	RenderMode BackgroundRenderMode;
};


struct SoundMeta
{
	char soundName[MAX_LEN];
	char folderName[MAX_LEN];
	float volume;
	ChannelName channel;
};

struct ObjectMeta
{
	char name[OBJECT_NAME_LENGTH + 1];
	ObjectType objectID;
	int xPos;
	int yPos;
};


struct ifIntData
{
	int *variable;
	int comparisonValue;

	bool elseBranchPresent;
	CutsceneID ifTrue;
	CutsceneID ifFalse;
};

struct ifFloatData
{
	float *variable;
	float comparisonValue;

	bool elseBranchPresent;
	CutsceneID ifTrue;
	CutsceneID ifFalse;
};

union SceneActionArguments
{
	float zoomScales[2];
	struct TextInstance *sceneText;
	int animationDetails[2];
	float positions[2];
	struct SoundMeta soundData;
	struct ObjectMeta objectInfo;
	bool hidden;
	float CameraData[3];
	Layer layer;
	struct ifIntData branchDataInt;
	struct ifFloatData branchDataFloat;
};

typedef enum SceneActionID
{
	SCENE_END,
	SCENE_WAIT,
	SCENE_IF_EQUALS,
	SCENE_IF_NOT_EQUALS,
	SCENE_IF_LESS_THAN,
	SCENE_IF_GREATER_THAN,
	SCENE_FLOAT_IF_EQUALS,
	SCENE_CHANGE_VARIABLE_BY,
	SCENE_SET_VARIABLE_TO,
	SCENE_SAY_TEXT,
	SCENE_ANIMATE_ACTOR,
	SCENE_SET_ACTOR_SPRITE,
	SCENE_SET_ACTOR_POS,
	SCENE_MOVE_ACTOR,
	SCENE_MOVE_ACTOR_X,
	SCENE_MOVE_ACTOR_Y,
	SCENE_MOVE_ACTOR_TO,
	SCENE_ROTATE_ACTOR,
	SCENE_SET_ACTOR_DIRECTION,
	SCENE_HIDE_ACTOR,
	SCENE_SHOW_ACTOR,
	SCENE_CREATE_ACTOR,
	SCENE_SET_ACTOR_LAYER,
	SCENE_PLAY_SOUND,
	SCENE_PLAY_SOUND_REPEAT,
	SCENE_SET_CHANNEL_VOL,
	SCENE_FADE_CHANNEL_VOL,
	SCENE_SET_CAMERA_POS,
	SCENE_MOVE_CAMERA,
	SCENE_MOVE_CAMERA_SMOOTH,
	SCENE_SET_SCREEN_ZOOM,
	SCENE_CHANGE_SCREEN_ZOOM,
	UNDEFINED_SCENE_ACTION
} SceneActionID;

struct SceneAction
{
	SceneActionID ActionID;
	bool parallelAction;
	int repeatTimes;

	struct Object *ActorObject;
	union SceneActionArguments ActionData;

	struct SceneAction *nextSceneAction;
	struct SceneAction *prevSceneAction;
};


union GameEventData
{
	int screenDimensions[2];
	float zoomScales[2];
	int newLevelID;
	FILE *loadedFile;
};

typedef struct GameEvent
{
	GameEventID EventID;
	union GameEventData EventData;
	bool canDelete;

	struct GameEvent *additionalEvent;
} GameEvent;

struct World
{
	struct Camera MainCamera;

	struct BackgroundData WorldBackground;

	struct playerData Player;
	struct ObjectController *ObjectList;

	int GamePaused;
	int level;
	LemonGameState GameState;
	GameEvent GameEvents;

	struct TextInstance *TextQueue;
	int PlayingText;
	
	CutsceneID CurrentCutscene;
	struct SceneAction *SceneActionQueue;

	WorldPhysics PhysicsType;
	float GlobalGravityY;
	float GlobalGravityX;
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


struct TTF_DebugText
{
	SDL_Texture *Text;
	SDL_FRect Location;

	bool CameraRelative;

	int deletionTick;
};


struct EngineData
{
	// Multi threading still not implemented!
	bool MultiThreadingEnabled;

	bool ObjectPreAllocationEnabled;

	int MaxObjects;
	int MaxParticles;
	int ReservedObjects;
	int PreservedSpriteSets;

	float WorldBoundX;
	float WorldBoundY;

	int MaxSoundsPerChannel;
	int MaxTextQueueLength;
	int MaxGameEvents;

	int GameTicksPerSecond;
	int TickDelta;

	struct sprite *DefaultTexture;
};


struct RenderData
{
	int drawSprites;
	int drawBackGround;
	int drawObjects;
	int drawPlayer;
	int drawUI;
	int drawParticles;

	int drawHitboxes;

	int maxObjects;
	int maxParticles;
	int maxUIElements;

	int drawnObjects;
	int drawnParticles;
	int drawnHudElements;

	int RendersPerSecond;
	int RenderDelta;
};


struct DebugData
{
	int ConsoleTextEnabled;
	char ConsoleString[CONSOLE_STRING_LENGTH];

	int DisplayPlayerData;

	int HitboxOutlineThickness;

	int PauseEngine;

	int DebugTextDisplayMode;
	int DebugTextInfoPreset;
	int ObjectCount;
	int FPSCounter;
	int PauseStatus;
	int CameraInfo;
	int SoundInfo;
	int CutsceneInfo;

	SDL_Color DebugTextColour;
	struct TTF_DebugText DebugTexts[MAX_DEBUG_TEXTS];
};


typedef struct string
{
	char *stringChars;
	int length;
} String;


typedef struct RenderFrame  RenderFrame;
typedef struct DrawData DrawData;

typedef struct soundInstance SoundInstance;
typedef struct soundChannel SoundChannel;

typedef struct sprite Sprite;
typedef struct spriteSet SpriteSet;
typedef struct animation Animation;
typedef struct animationFrame AnimationFrame;

typedef struct PhysicsRect PhysicsRect;
typedef struct displayData DisplayData;
typedef struct Object Object;

typedef struct ObjectController ObjectController;
typedef struct FrameUpdateFunction FrameUpdateFunction;

typedef struct TextInstance TextInstance;

typedef struct Camera Camera;
typedef struct BackgroundData BackgroundData;
typedef struct World World;

typedef struct playerData PlayerData;

typedef struct MouseData MouseData;

typedef struct TTF_DebugText DebugText;

typedef struct EngineData EngineData;
typedef struct RenderData RenderData;
typedef struct DebugData DebugData;

typedef union GameEventData GameEventData;
typedef struct SceneAction SceneAction;
typedef union SceneActionArguments SceneActionArguments;


//Global variables/data
extern SoundChannel SoundChannels[CHANNEL_COUNT];

extern RenderFrame ScreenData;

extern int screenWidth;

extern int screenHeight;

extern MouseData MouseInput;

extern int keyboard[256];

extern EngineData EngineSettings;

extern RenderData RenderSettings;

extern DebugData DebugSettings;

extern unsigned char tiny_ttf[];

extern unsigned int tiny_ttf_len;


extern int Running_In_Windows_Mode;		// Windows causes a lot shennanigans on the technical side; should be depreciated later
