#ifndef IS_DEFINED
#define IS_DEFINED 1


//								Engine Settings (Can be modified during runtime via variables)
//-------------------------------------------------------------------------------------------------

// Most displayed objects are designed to adjust positioning based on resolution; 
// however the lowest expected resolution is 1280 x 720, lower values may result in unintended behaviour
#define V_RESOLUTION 720
#define H_RESOLUTION 1280

#define MAX_OBJECTS_RENDER 256
#define MAX_PARTICLES_RENDER 64
#define MAX_HUD_ELEMENTS_RENDER 64

#define MAX_SOUNDS_PER_CHANNEL 16
#define MAX_TEXTQUEUE_LENGTH 200			// Number of textInstances allowed in the TextQueue at once
#define MAX_SCENEACTIONS 200 				// Number of scene actions allowed in the queue at once

#define RESERVED_OBJECTS 500
#define PRESERVED_SPRITESETS 2

#define X_WORLD_BOUND 100000.0
#define Y_WORLD_BOUND 100000.0

#define MULTITHREADED_ENABLED 	false 		// UNIMPLEMENTED		

#define DEFAULT_TEXTURE "Missing.png"

#define TICKS_PER_SECOND 60
#define RENDERS_PER_SECOND 20000	

#define VSYNC_DEFAULT true

//-------------------------------------------------------------------------------------------------


//								Engine constants (Cannot be modified during runtime)
//-------------------------------------------------------------------------------------------------

#define DEBUG_MODE true
#define LEMON_COLLISION_PHYSICS true 		// turn this to false to disable all physics and collision if you want to implement your own scheme

#define RANDOM_SEED 0

// Avoid editing these values, as it may cause issues/performance loss
#define MAX_OBJECTS 15000
#define OBJECT_NAME_LENGTH 16
#define DEFAULT_SCALEMODE SDL_SCALEMODE_NEAREST

#define MAX_QUEUED_GAME_EVENTS 64
#define GAME_FLAG_COUNT 100  

#define CHANNEL_NAME_LENGTH 20
#define DEFAULT_SOUND_EXTENSION ".wav"

#define USER_INPUT_MAX_LEN 100

#define MAX_LEN 80
#define MAX_COMMENT_LENGTH 200
#define CUTSCENE_FILE_NAME_MAX 50

#define MAX_TEXT_LENGTH 200
#define MAX_TEXT_OPTIONS 4
#define OPTION_TEXT_MAX_LEN 50

#define CONSOLE_STRING_LENGTH 512
#define MAX_DEBUG_TEXTS 32
#define DEBUG_TEXT_MAX_LENGTH 400

#define MAX_SPRITE_SIZE 4000

#define MINIMUM_SCREEN_WIDTH 144
#define MINIMUM_SCREEN_HEIGHT 144
#define MINIMUM_ZOOM 0.4  		 // Do not set this value to 0.1 or below

// Do not change
#define TICK_RESOLUTION 			1000	// values closer to 1 means higher resolution, while further means lower.
#define FRAMERATE_UPDATE_RATE		2		
#define TICK_RESOLUTION_INVERSE		(1000000000 / TICK_RESOLUTION)	
#define INTERP_FRAMERATE(x)			(x << (FRAMERATE_UPDATE_RATE - 1))
#define FRAMERATE_UPDATE_TICK 		(TICK_RESOLUTION_INVERSE >> (FRAMERATE_UPDATE_RATE - 1))


//-------------------------------------------------------------------------------------------------


//								Physics values
//-------------------------------------------------------------------------------------------------

#define COLLISION_CYCLES 5
#define COLLISION_DEPTH 16

#define FAST_COLLISION_THRESHOLD 999	// If objectCount is over this, it will switch to the faster but slightly less accurate collision detection

//-------------------------------------------------------------------------------------------------


//								Useful constants (For convenience)
//-------------------------------------------------------------------------------------------------

#define RADIAN_15 0.26179938779
#define RADIAN_30 0.52359877559
#define RADIAN_45 0.78539816339
#define RADIAN_60 1.0471975512
#define RADIAN_75 1.308996939 
#define RADIAN_90 1.5707963268
#define RADIAN_180 3.14159265359
#define RADIAN_360 6.2831853072
#define DEGREE_TO_RADIAN_PI 0.01745329251 
#define RADIAN_TO_DEGREE_PI 57.2957795131
#define DEFAULT_DIRECTION 90.0

#define INT_MAX_LEN 20

#define NO_PORTRAIT ""
#define USE_CURRENT_SPRITESET 0
#define SWITCH_TO_MISSING(x) switchSprite(-1, 0, x->ObjectDisplay); 
#define BACKGROUND_SETID 1
#define NO_ACTION NULL

// Tiles Probably wont be re-implemented, but constants are useful for stylisation
#define Y_TILESCALE 32
#define X_TILESCALE 32

//-------------------------------------------------------------------------------------------------


//									Root Folder locations
//-------------------------------------------------------------------------------------------------

#define SOUND_ROOT 		"LemonData/Sounds/"
#define SPRITE_ROOT 	"LemonData/Sprites/"
#define ANIMATION_ROOT 	"LemonData/AnimationData/"
#define LEVELDATA_ROOT 	"LemonData/LevelData/"
#define CUTSCENE_ROOT	"LemonData/CutsceneData/"
#define SAVEDATA_ROOT	"LemonData/SaveData/"

//-------------------------------------------------------------------------------------------------



#define LEMON_VERSION "V0.09"




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
	FPS_COUNTER_ONLY,
	CONSOLE_TEXT_SETTING_COUNT
} ConsoleTextSetting;


typedef enum LemonKeys 
{
	// 0 - 31 are control charcacters and should not be used
	// 32 - 127 are reserved for ASCII keys
	ACKNOWLEDGE_INPUT = 0,
	LMN_ESCAPE = 128,
	LMN_ENTER,
	LMN_SPACE,
	LMN_BACKSPACE,
	LMN_UPARROW,
	LMN_DOWNARROW,
	LMN_LEFTARROW,
	LMN_RIGHTARROW,
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
	MUSIC_CHANNEL = 0,
	SPEECH = 1,
	PLAYER_SFX = 2,
	OBJECT_SFX = 3,
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
	SINGLE,
	TILE,
	TILE_FAST,
	SCALE,
	STATIC_BACKGROUND,
	SINGLE_BACKGROUND,
	TILEPLANE_BACKGROUND,
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
    ENCOUNTERED_FATAL_ERROR,
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
	DELETE_ENVIRONMENT_OBJECTS,
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
	END_CUTSCENE = -1,
	NO_CUTSCENE = 0,
	TEST_SCENE = 1,
	TEST_SCENE_2 = 2,
	TEST_SCENE_2_AGAIN = 3,
	TEST_SCENE_2_CORRECT,
	TEST_SCENE_2_WRONG,
	FILE_TEST = 9,
	UNDEFINED_CUTSCENE,
	CUTSCENE_FROM_FILE,
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
	PHYSICS_DISABLED 		= 0b00000000,
	AWAITING_UPDATE			= 0b00000001,	
	AWAITING_MAGNETISATION	= 0b00000010,
	AWAITING_RESOLUTION		= 0b00000011,
} PhysicsFlags;


typedef enum ObjectState 
{
	EMPTY_OBJECT = -2,
	TO_BE_DELETED = -1,
	DEFAULT = 0,
	RESERVED_STATE,
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
	REFRESH,
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
	PUSHABLE_SOLID = 6,
	UNDEFINED_SOLID
} SolidType;

typedef enum SolidFlag
{
	NORMAL_SOLID,
	ENTITY_SOLID,
	IGNORE_SOLID,
	UNDEFINED_SOLIDFLAG
} SolidFlag;


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
	LOAD_PART_TRIGGER,
	SWITCH_TO_NEW_PART_TRIGGER,
	LOAD_PART,
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
	SAVE_OPTIONS_CONTROLLER,
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


typedef enum RotationMode 
{
	NORMAL_ROTATION = 0,
	LEFT_RIGHT_ROTATION,
	DONT_ROTATE
} RotationMode;


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
	MIX_Track *audio;
	bool positional;
	float xPos;
	float yPos;

	ChannelName channel;
	char name[MAX_LEN];
	float volume;
	MIX_StereoGains panLevels;

	struct soundInstance *nextSound;
	struct soundInstance *prevSound;
};


struct soundChannel
{
	int soundCount;
	int Pause;
	float channelVolume;

	float fadeVal;

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
	Uint64 FrameTimer;
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
	char name[MAX_LEN];
};


struct animationFrame 
{
	struct animationFrame *nextFrame;

	struct sprite *frameSprite;
	float SpriteXOffset;
	float SpriteYOffset;
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

typedef struct displayData
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

	Layer layer;
	RenderMode RenderModeOverride;
	float size;
	RotationMode rotateMode;
	float spriteXOffset;
	float spriteYOffset;
	unsigned int pixelXOffset;
	unsigned int pixelYOffset;

	float transparency;		// 0.0 is no transparency - 1.0 is full transparency (invisible) -- SDL uses 0-255 where 255 is no transparency and 0 is fully transparent
	bool hidden;
} DisplayData;


typedef struct PhysicsBox
{
	float xPos;
	float yPos;
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
	struct PhysicsBox *GroundBox;

	SolidType solid;
	SolidFlag flag;
	CollideType collideMode;
	Layer collideLayer;

	double direction;
	short xFlip;
	short yFlip;
	bool crouch;
} PhysicsBox;


typedef struct Object
{
	char name[OBJECT_NAME_LENGTH];	// unique identifier for Object 		
	int ObjectID;
	ObjectState State;
	CurrentAction Action;
	CurrentInterrupt Interrupt;	
	PhysicsFlags reserved;
	int index;

	PhysicsBox *ObjectBox;
	DisplayData *ObjectDisplay;

	struct Object *ParentObject;
	ParentType ParentLink;

	struct Object *nextObject;
	struct Object *prevObject;

	// Multi-purpose args
	int arg1;
	int arg2;
	int arg3;
	int arg4;
} Object;

typedef struct World World;
typedef int (*TriggerableFunction)(Object*, struct World*);

struct FrameUpdateFunction
{
	struct FrameUpdateFunction *nextFunction;

	Object *inputObject;
	TriggerableFunction FunctionPointer;
};

#define MAX_COMPONENT_SLOTS 256


typedef struct HealthComponent
{
	int health;
	int maxHealth;
	int (*TakeDamageCallBack)(int damage, Object*, struct HealthComponent*);
} HealthComponent;


typedef struct TileMap
{
	unsigned int centerTileX;
	unsigned int centerTileY;
	unsigned int tileSize;
} TileMap;


// downside of union approach: bad with memory use, as if any large component exists, ALL component lists will increase
// upside of union approach: much easier to work with as existing functions do not need to be copied/rewritten
typedef union ComponentType
{
	HealthComponent HealthComponent;
	TileMap TileMap;

} ComponentType;

typedef struct SparseList
{
	int storedComponents;
	
	int sparse[MAX_OBJECTS];
	int denseID[MAX_COMPONENT_SLOTS];
	ComponentType dense[MAX_COMPONENT_SLOTS];
} SparseList;


typedef struct ComponentData
{
	Object Objects[MAX_OBJECTS];
	PhysicsBox PhysicsBoxes[MAX_OBJECTS];
	DisplayData Displays[MAX_OBJECTS];

	SparseList HealthComponents;
	SparseList TileMaps;

} ComponentData;

typedef struct IntSparseList
{
	int storedElements;
	
	int sparse[MAX_OBJECTS];
	int dense[MAX_OBJECTS];
} IntSparseList;


#define STACKARRAY_LENGTH MAX_OBJECTS
typedef struct StackArray
{
	int storedElements;
	int list[STACKARRAY_LENGTH];
} StackArray;


struct ObjectController
{
	int objectCount;
	Object *firstObject;
	Object *lastObject;

	Object *availableSlots;

	int cachedCount;
	Object *cachedFirstObject;
	Object *cachedLastObject;

	struct spriteSet *startSpriteSetPtr;

	struct FrameUpdateFunction *FrameUpdates;

	ComponentData objectComponents;

	StackArray solidList;
};


// Controls the player character
struct playerData
{
	Object *PlayerPtr;

	PhysicsBox *PlayerBox;
	DisplayData *PlayerDisplay;
	PhysicsBox InteractBox;

	// These variables can be freely modified according to your modified player controller
	int jumpHeld;
	int jumpProgress;

	int coinCount;
	int HP;
};


union TextEventTriggerData
{
	CutsceneID cutscene;
	int level;
	Object *TriggerObject;
};

struct TextEventTrigger
{
	union TextEventTriggerData FunctionArguments;
	TriggerableFunction FunctionPointer;
};


struct TextOptionPrompt
{
	int SelectedOption;
	int numberOfOptions;
	int optionBeingPrinted;

	float OptionYPositions[MAX_TEXT_OPTIONS];

	char optionNames[MAX_TEXT_OPTIONS][OPTION_TEXT_MAX_LEN];

	struct TextEventTrigger optionTriggers[MAX_TEXT_OPTIONS];
};


union TextTypeData
{
	struct TextEventTrigger TriggerEvent;
	struct TextOptionPrompt OptionPrompt; 
};


struct TextInstance 
{
	struct TextInstance *nextText;

	Object *boxPtr;

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

	int boxOffsetX;		// x position within the text box
	int boxOffsetY;		// y position within the text box
	int textLengthSize;	// Maximum amount of pixels from left-most side to render text within before going to next line
	int LineSpacing;	// space between lines 
	int TextSize;    	// width and height of text characters

	int currentIndex;
	int Counter;
	int currentXPos;
	int currentYPos;
};


typedef struct Camera 
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
} Camera;


typedef struct TilePlane
{
	Uint8 *tiles;
	int GridWidth;
	int GridHeight;
	int tileWidth;
	int tileHeight;
	float xPos;
	float yPos;
} TilePlane;

typedef struct BackgroundData
{
	struct spriteSet *BackgroundSpriteSet;
	struct sprite *BackgroundSpriteBuffer;

	float bgParallax;

	RenderMode BackgroundRenderMode;

	TilePlane tileBG;
} BackgroundData;


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


typedef struct sceneBranchData
{
	int variableIndex;
	int comparisonValue;

	bool elseBranchPresent;
	CutsceneID ifTrue;
	CutsceneID ifFalse;

	char ifTrueString[CUTSCENE_FILE_NAME_MAX];
	char ifFalseString[CUTSCENE_FILE_NAME_MAX];
} SceneBranchData;


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
	SceneBranchData branchData;
	int variableArgs[2];
	int invisWall[4];
};

typedef enum SceneActionID
{
	SCENE_END,
	SCENE_DISABLE_PLAYER,
	SCENE_ENABLE_PLAYER,
	SCENE_WAIT,
	SCENE_CHANGE_VARIABLE_BY,
	SCENE_SET_VARIABLE_TO,
	SCENE_IF_EQUALS,
	SCENE_IF_LESS_THAN,
	SCENE_IF_GREATER_THAN,
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
	SCENE_SET_ACTOR_LAYER,
	SCENE_CREATE_ACTOR,
	SCENE_RELEASE_ACTOR,
	SCENE_PLACE_INVISIBLE_WALL,
	SCENE_PLAY_SOUND,
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

	Object *ActorObject;
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
	float ObjectGoTo[3];
};

typedef struct GameEvent
{
	GameEventID EventID;
	union GameEventData EventData;
	bool canDelete;
	bool loadingAFile;
} GameEvent;

typedef struct GameEventManager
{
	GameEvent Events[MAX_QUEUED_GAME_EVENTS];
	int nextAvailable;
	int eventsPending;
} GameEventManager;


struct World
{
	struct Camera MainCamera;

	BackgroundData WorldBackground;

	struct playerData Player;
	struct ObjectController *ObjectList;

	int GamePaused;
	int level;
	LemonGameState GameState;
	GameEventManager GameEvents;

	struct TextInstance *TextQueue;
	int PlayingText;
	
	CutsceneID CurrentCutscene;
	struct SceneAction *SceneActionQueue;
	int SceneActionCount;

	WorldPhysics PhysicsType;
	float GlobalGravityY;
	float GlobalGravityX;
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

	const int MaxGameEvents;
	int MaxSoundsPerChannel;
	int MaxTextQueueLength;
	int MaxSceneActions;

	const int MaxObjects;
	int MaxParticles;
	int ReservedObjects;
	int PreservedSpriteSets;

	float WorldBoundX;
	float WorldBoundY;

	int GameTicksPerSecond;
	Uint64 TickDelta;

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
	Uint64 RenderDelta;

	bool vSync;
};


struct DebugData
{
	ConsoleTextSetting ConsoleTextEnabled;
	char ConsoleString[CONSOLE_STRING_LENGTH];

	int DisplayPlayerData;

	int HitboxOutlineThickness;

	int PauseEngine;

	int DebugTextDisplayMode;
	int DebugTextInfoPreset;
	int DebugOverlay;
	int FPSCounter;
	int CameraInfo;
	int SoundInfo;

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

typedef struct ObjectController ObjectController;
typedef struct FrameUpdateFunction FrameUpdateFunction;

typedef struct TextInstance TextInstance;

typedef struct playerData PlayerData;

typedef struct MouseData MouseData;

typedef struct TTF_DebugText DebugText;

typedef struct EngineData EngineData;
typedef struct RenderData RenderData;
typedef struct DebugData DebugData;

typedef union GameEventData GameEventData;
typedef struct SceneAction SceneAction;
typedef union SceneActionArguments SceneActionArguments;


typedef struct saveData
{
	int flags[GAME_FLAG_COUNT];
} SaveData;

#endif


//Global variables/data
extern SoundChannel SoundChannels[CHANNEL_COUNT];

extern RenderFrame ScreenData;

extern int screenWidth;

extern int screenHeight;

extern MouseData MouseInput;

extern int keyboard[256];

extern char userInputString[USER_INPUT_MAX_LEN];

extern EngineData EngineSettings;

extern RenderData RenderSettings;

extern DebugData DebugSettings;

extern int GameFlags[GAME_FLAG_COUNT];


extern unsigned char tiny_ttf[];

extern unsigned int tiny_ttf_len;