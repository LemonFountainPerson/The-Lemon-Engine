#ifndef DATA_DEFINED
#define DATA_DEFINED


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

typedef enum LemonKeys 
{
	// 0 - 31 are control charcacters and should not be used
	// 32 - 127 are reserved for ASCII keys
	ACKNOWLEDGE_INPUT = 0,
	LMN_ESCAPE = 128,
	LMN_GRAVE,
	LMN_ENTER,
	LMN_SPACE,
	LMN_TAB,
	LMN_LSHIFT,
	LMN_RSHIFT,
	LMN_BACKSPACE,
	LMN_COMMA,
	LMN_PERIOD,
	LMN_SLASH,
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
	LMN_MENU_OPEN,
	LMN_TYPING_END,
	LMN_CONSOLE_OPEN,

	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_SIDE1,
	MOUSE_SIDE2,

	GAMEPAD_WEST,
	GAMEPAD_SOUTH,
	GAMEPAD_EAST,
	GAMEPAD_NORTH,
	GAMEPAD_DPAD_LEFT,
	GAMEPAD_DPAD_DOWN,
	GAMEPAD_DPAD_RIGHT,
	GAMEPAD_DPAD_UP,
	GAMEPAD_START,
	GAMEPAD_BACK,
	GAMEPAD_GUIDE,
	GAMEPAD_LEFT_SHOULDER,
	GAMEPAD_RIGHT_SHOULDER,
	GAMEPAD_LEFT_STICK,
	GAMEPAD_RIGHT_STICK,

	INPUT_COUNT
} LemonKeys;

typedef enum ButtonState
{
	BUTTON_RELEASED = 0,
	BUTTON_PRESSED = 1,
	BUTTON_HELD = 2
} ButtonState;


typedef enum ConsoleTextSetting 
{
	CONSOLE_TEXT_DISABLED = 0,
	CONSOLE_ONLY_ERRORS = 1,
	CONSOLE_ALL_EVENTS = 2,
	CONSOLE_TEXT_SETTING_COUNT
} ConsoleTextSetting;


typedef enum ChannelName 
{
	MUSIC_CHANNEL = 0,
	SPEECH = 1,
	PLAYER_SFX = 2,
	OBJECT_SFX = 3,
	CHANNEL_COUNT 	// Simultaniously used as 'last', undefined channel and channel count
} ChannelName;

typedef enum SoundState
{
	SOUND_INACTIVE,
	SOUND_LOADING,
	SOUND_PLAYING
} SoundState;


typedef enum VoiceMode 
{
	VOICE_EACH_CHARACTER = 0,
	VOICE_ONCE
} VoiceMode;


typedef enum PortraitPos 
{
	PORTRAIT_INSIDE_BOX_LEFT,
	PORTRAIT_INSIDE_BOX_RIGHT,
	PORTRAIT_ON_TOP_BOX_LEFT,
	PORTRAIT_ON_TOP_BOX_RIGHT
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
	BLACK_TEXT,
	BLACK_TOP,
	SILENT_TEXT,
	SILENT_TOP,
	PLAINTEXT_BOTTOM,
	PLAINTEXT_TOP,
	COMIC_TEXT,
	COMIC_TOP,
	UNDEFINED_PRESET
} TextPreset;


typedef enum TextType 
{
	TEXTBOX_REGULAR_TEXT,
	TEXTBOX_OPTION_PROMPT,
	TEXTBOX_TRIGGER_EVENT
} TextType;

typedef enum TextPosition
{
	TEXT_BOTTOM_LEFT,
	TEXT_CENTERED
} TextPosition;

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

RenderMode convertStringToRenderMode(char string[]);
const char* getRenderModeName(RenderMode input);


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

const char* getLayerName(Layer input);


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
	NON_STATIC_SCENE
} CutsceneID;


typedef enum CameraState 
{
	FOLLOW_PLAYER = 0,
	FREE_ROAM = 1,
	FREE_ROAM_RESTRICTED = 2,
	MENU_CAMERA = 3,
	UNDEFINED_CAMERA_STATE
} CameraState;


typedef enum ReservedFlags
{
	RFLAG_DEFAULT 			= 0x00000000,	// 0000
	RFLAG_DISABLE_PHYSICS 	= 0x00000001,	// 0001
	RFLAG_PRESERVE_OBJECT	= 0x00000002,	// 0010
	RFLAG_PRESERVE_ONCE		= 0x00000006, 	// 0110 - activates PRESERVE_OBJECT as well
	RFLAG_CUTSCENE_IMMUNITY	= 0x00000008,	// 1000
	RFLAG_GROUND_SET		= 0x00000010	// 0001_0000
} ReservedFlags;


typedef enum ObjectState 
{
	EMPTY_OBJECT = -2,
	TO_BE_DELETED = -1,
	DEFAULT_STATE = 0,
	STATIC_STATE,
	PAUSE_STATE,
	ACTOR_STATE,
	BEING_CARRIED_STATE,
	UNDEFINED_STATE
} ObjectState;

const char* getObjectStateName(ObjectState input);

 
typedef enum CurrentAction 
{
	IDLE = 0,
	DEFEATED,
	CHASING,
	ATTACKING,
	FLEEING,
	INTERACTION_INTERRUPT,
	MOUSECLICK_INTERRUPT,
	UNDEFINED_ACTION
} CurrentAction;


typedef enum ParentType 
{
	DEFAULT_LINK 		= 	0x00000000,
	FINAL_LINK			= 	0x00000001,
	MOTION_LINK 		= 	0x00000002,
	POSITION_LINK 		= 	0x00000004,
	SPRITE_LINK 		= 	0x00000008,
	ANIMATION_LINK		= 	0x00000010,
	TRANSPARENCY_LINK	= 	0x00000020,
	PHYSICSBOX_LINK		= 	0x00000040,
	DISPLAYDATA_LINK	=	0x00000080,
	PARENTLINK_CONFIRM	=	0x80000000,
	PARENTLINK_MASK		=	0x7FFFFFFF
} ParentType;


typedef enum SolidType 
{
	UNSOLID = 0,
	SOLID = 1,
	CIRCLE = 2,
	FLAT_SLOPE = 3,
	JUMP_THROUGH = 4,
	BODY = 5,
	PUSHABLE_SOLID = 6,
	UNDEFINED_SOLID
} SolidType;

const char* getSolidTypeName(SolidType input);


typedef enum SolidFlag
{
	DEFAULT_SOLIDFLAG,
	IMPACT_COLLISION,
	PUSH_COLLISION,
	IGNORE_SOLID,
	IGNORE_SELF,
	ONLY_BODIES,
	GET_IGNORED,
	UNDEFINED_SOLIDFLAG
} SolidFlag;

const char* getSolidFlagName(SolidFlag input);


typedef enum ObjectType 
{
	LEVEL_FLAG_OBJ = 0,
	SOLID_BLOCK,	
	FLAT_SLOPE_FLOOR,
	JUMP_THRU_BLOCK,
	PLAYER_OBJECT,
	UI_ELEMENT,
	UI_TEXT,
	PARTICLE,
	COIN,
	SPRING,
	MOVING_PLATFORM_HOR,
	MOVING_PLATFORM_VER,
	GATE_SWITCH,
	GATE_SWITCH_TIMED,
	VERTICAL_GATE,
	HORIZONTAL_GATE,
	DOOR,
	LEVEL_DOOR,
	PUSHABLE_BOX,
	PROJECTILE,
	BASIC_ENEMY,
	OBJECT_TYPE_COUNT,
	UNDEFINED_OBJECT
} ObjectType;

int getObjectID(const char entry[]);
const char* getObjectIDName(ObjectType input);

typedef enum Orientation
{
	ROW,
	COLUMN
} Orientation;

typedef enum Flags 
{
	SET_BACKGROUND = 0,
	SET_BACKGROUND_TRIGGER,
	START_LVL_WITH_CUTSCENE,
	CUTSCENE_TRIGGER,
	DELETE_OBJECT_TRIGGER,
	DELETE_BODY_TRIGGER,
	START_PLAYER_POSITION,
	START_CAMERA_POSITION,
	SET_CAMBOX,
	CAMERA_BOUNDARY,
	FALSE_CAMERA_BOUNDARY,
	LEVEL_TRIGGER,
	LEVEL_TRIGGER_SEAMLESS,
	SET_PLAYER_LAYER,
	START_WITH_MUSIC,
	PLAY_SOUND_TRIGGER,
	CACHE_TRIGGER,
	LOAD_PART_TRIGGER,
	SWITCH_TO_NEW_PART_TRIGGER,
	LOAD_PART,
	ADD_GAME_FLAG,
	SET_GAME_FLAG,
	INCREMENT_GAME_FLAG,
	DECREMENT_GAME_FLAG,
	UNDEFINED_FLAG
} Flags;


typedef enum ParticleSubType 
{
	EMPTY_PARTICLE = 0,
	SPARKLE,
	STATIC,
	UNDEFINED_PARTICLE
} ParticleSubType;


// UISubType is shared by all UI_xxx objects, although behaviour, 
// sprites and animation are separate - if you set a UI_ELEMENT's subtype to TEXT_BOX 
// it will result in it being treated as a BASIC GRAPHIC
typedef enum UISubType 
{
	BASIC_GRAPHIC = 0,
	LEVEL_FADE,
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
	LOOP_INDEFINITELY = -1
} AnimationLoopState;


typedef enum DebugTextRenderMode
{
	DEBUG_TEXT_DISABLED = 0,
	DEBUG_TEXT_ENABLED,
	ONLY_NONSTATIC_OBJECT_INFO,
	ALL_OBJECT_INFO,
	DEBUG_TEXT_MODE_COUNT
} DebugTextMode;


typedef enum DebugTextFormatting
{
	DTFORMAT_SCREEN_RELATIVE,
	DTFORMAT_CAMERA_RELATIVE,
	DTFORMAT_JUSTIFY_TOP,
	DTFORMAT_LIST_SOUND
} DebugTextFormatting;


typedef enum EnginePauseStatus
{
	ENGINE_UNPAUSED,
	ENGINE_PAUSED,
	ENGINE_SINGLE_TICK
} EnginePauseStatus;



// Memory allocated structs of data
typedef struct soundInstance
{
	SoundState state;
	MIX_Track *audio;

	bool positional;
	float xPos;
	float yPos;

	ChannelName channel;
	char name[MAX_LEN];
	float volume;
	int repeats;
	MIX_StereoGains panLevels;

	struct soundInstance *nextSound;
	struct soundInstance *prevSound;
} SoundInstance;

typedef struct CachedSound
{
	char name[MAX_LEN];
	MIX_Audio *data;
} CachedSound;

typedef struct CachedSoundList
{
	int head;
	CachedSound list[MAX_CACHED_SOUNDS];
} CachedSoundList;


typedef struct soundChannel
{
	int soundCount;
	int Pause;
	float channelVolume;

	float fadeVal;

	struct soundInstance *firstSound;
} SoundChannel;

typedef struct SoundMeta
{
	char soundName[MAX_LEN];
	float volume;
	ChannelName channel;
} SoundMeta;


// Regular Sprites (Objects, player, particles, etc.)
typedef struct sprite
{
	struct sprite *nextSprite;
	struct sprite *prevSprite;

	SDL_Texture *texture;
	unsigned int height;
	unsigned int width;
	RenderMode RenderMode;

	int spriteID;
	char name[MAX_LEN];
} Sprite;


typedef struct AnimationFrame 
{
	struct AnimationFrame *nextFrame;

	Sprite *frameSprite;
	float SpriteXOffset;
	float SpriteYOffset;
	float rotation;

	int soundIndex; // Which sound from animation's soundlist to play - '-1' means no sound
} AnimationFrame;

// Starts counting animation IDs from 1
typedef struct Animation 
{
	struct Animation *nextAnimation;

	AnimationFrame *animationData;
	int frameCount;

	SoundMeta *animationSounds;
	int soundCount;

	char name[ANIMATION_NAME_LENGTH]; 
	int animationID;
	float frameRate;
	bool contiguousFrames;
} Animation;


// Sprite sets that lead to linked lists of sprites corresponding to an Object type;
// only needs to be initialised once per Object type
// Starts counting sprite IDs from 1
typedef struct spriteSet
{
	Sprite *firstSprite;
	Sprite *lastSprite;

	struct spriteSet *nextSet;
	struct spriteSet *prevSet;

	Animation *Animations;

	int setID;
	int spriteCount;

	int copyCount;
	int *copies;
} SpriteSet;

typedef struct SpriteSetList
{
	SpriteSet *start;
} SpriteSetList;

typedef struct displayData
{
	int currentSprite;
	Sprite *spriteBuffer;

	int currentAnimation;
	float animationTick;
	int animationLoopCount;
	float animationSpeed;
	AnimationFrame *frameBuffer;
	Animation *animationBuffer;

	SpriteSet *spriteSetSource;

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
	ReservedFlags reserved;
	int instanceNumber;

	struct Object *Parent;
	ParentType ParentLink;

	const int index;
	PhysicsBox * const ObjectBox;
	DisplayData * const ObjectDisplay;

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

typedef struct FrameUpdateFunction
{
	struct FrameUpdateFunction *nextFunction;

	Object *inputObject;
	TriggerableFunction FunctionPointer;
} FrameUpdateFunction;

#define MAX_COMPONENT_SLOTS 256


typedef struct HealthComponent
{
	int health;
	int maxHealth;

	int duration;
	Uint64 startTick;
	int inflictedDamage;
} HealthComponent;


typedef struct BulletComponent
{
	Object *owner;

	ParticleSubType particleType;
	int particleRepeat;
	int particleLifeTime;

	bool bulletCollide;
	int bulletLifeTime;

	int damage;
} BulletComponent;


typedef struct TileMap
{
	unsigned int centerTileX;
	unsigned int centerTileY;
	unsigned int tileSize;
} TileMap;

typedef struct Polygon
{
	SDL_Vertex *vertexList;
	int vertices;
	bool quad;
	int *indicies;
} Polygon;

typedef union RenderMethod
{
	TileMap TileMap;
	Polygon Polygon;
} RenderMethod;

typedef enum RenderMethodType
{
	RENDERMETHOD_POLYGON,
	RENDERMETHOD_TILEMAP
} RenderMethodType;

typedef struct CustomDisplay
{
	RenderMethodType type;
	RenderMethod RenderMethod;
} CustomDisplay;


typedef struct Timer
{
	Uint64 startTick;
	Uint64 timerLength;

	bool pause;
	Uint64 pauseTick; 
} Timer;

typedef struct StopWatch
{
	Uint64 startTimeStamp;
	bool pause;
	Uint64 pauseTimeStamp; // when paused, the time value is (pauseTimeStamp - startTimeStamp) 
	// when unpausing increment startTimeStamp by (currentTimeStamp - pauseTimeStamp)
	// start: 5   paused: 23     current: 50      currentTime: 23 - 5 = 18   unpause: 5 + 50 - 23 = 5 + 27 = 32	   currentTime after unpause: 50 - 32 = 18
} StopWatch;


typedef struct PhysicsComponent
{
	Object *object;
	bool gravity;
} PhysicsComponent;


// downside of union approach: bad with memory use, as if any large component exists, ALL component lists will increase
// upside of union approach: much easier to work with as existing functions do not need to be copied/rewritten
typedef union ComponentType
{
	HealthComponent HealthComponent;
	BulletComponent BulletComponent;
	TileMap TileMap;
	Polygon Polygon;
	Timer Timer;
	StopWatch StopWatch;
	PhysicsComponent PhysicsComponent;
} ComponentType;

typedef struct SparseList
{
	char componentName[COMPONENT_NAME_LENGTH];
	int storedComponents;
	
	short sparse[MAX_OBJECTS];
	int denseID[MAX_COMPONENT_SLOTS];
	ComponentType dense[MAX_COMPONENT_SLOTS];
} SparseList;


typedef struct ComponentData
{
	Object Objects[MAX_OBJECTS];
	PhysicsBox PhysicsBoxes[MAX_OBJECTS];
	DisplayData Displays[MAX_OBJECTS];

	SparseList HealthComponent;
	SparseList BulletComponent;
	SparseList TileMap;
	SparseList Polygon;
	SparseList Timer;
	SparseList StopWatch;
	SparseList PhysicsComponent;
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

typedef struct ObjectController
{
	int objectCount;
	Object *firstObject;
	Object *lastObject;

	Object *availableSlots;

	int cachedCount;
	Object *cachedFirstObject;
	Object *cachedLastObject;

	SpriteSetList spriteSets;

	FrameUpdateFunction *FrameUpdates;

	ComponentData objectComponents;

	StackArray solidList;
} ObjectController;


// Controls the player character
typedef struct playerData
{
	Object *PlayerPtr;

	PhysicsBox *PlayerBox;
	DisplayData *PlayerDisplay;
	PhysicsBox InteractBox;

	// These variables can be freely modified according to your modified player controller
	int jumpProgress;
	bool jumpHeld;
	float jumpForce;
	int coyoteFrames;
	int jumpRange;
	int cancelRange;

	int coinCount;
} PlayerData;


typedef struct GameFlag
{
	char name[MAX_LEN];
	int nameLength;
	int value;
} GameFlag;


typedef enum GameEventID
{
	NO_EVENT = 0,
	EVENT_SWITCH_LEVEL,
	EVENT_PLAY_CUTSCENE,
	EVENT_PLAY_CUTSCENE_FROM_FILE,
	EVENT_PLAY_SOUND,
	EVENT_MOVE_PLAYER,
	EVENT_MOVE_OBJECT,
	EVENT_TELEPORT_PLAYER_TO_EXIT_DOOR,
	EVENT_SET_BRIGHTNESS,
	EVENT_CHANGE_SCREEN_SIZE,
	EVENT_CHANGE_SCREEN_SIZE_SCALE,
	EVENT_ENABLE_FULLSCREEN,
	EVENT_DISABLE_FULLSCREEN,
	EVENT_ENABLE_FULLSCREEN_SCALE,
	EVENT_STREAM_LEVEL_PARTITION,
	EVENT_DELETE_ENVIRONMENT_OBJECTS,
	EVENT_COUNT,
	UNDEFINED_EVENT
} GameEventID;

const char* getEventName(GameEventID input);
GameEventID getEventID(const char input[]);


#define GAME_EVENT_VAR_COUNT 4
typedef struct GameEventData
{
	float vars[GAME_EVENT_VAR_COUNT];
	char string[MAX_LEN];
	Object *objReference;
} GameEventData;

typedef struct GameEvent
{
	GameEventID EventID;
	GameEventData EventData;
	FILE *loadedFile;
} GameEvent;

typedef struct GameEventManager
{
	GameEvent Events[MAX_QUEUED_GAME_EVENTS];
	int nextAvailable;
	int eventsPending;
} GameEventManager;


struct TextOptionPrompt
{
	int SelectedOption;
	int numberOfOptions;
	int optionBeingPrinted;

	float OptionYPositions[MAX_TEXT_OPTIONS];

	char optionNames[MAX_TEXT_OPTIONS][OPTION_TEXT_MAX_LEN];

	GameEvent optionTriggers[MAX_TEXT_OPTIONS];
};


union TextTypeData
{
	GameEvent TriggerEvent;
	struct TextOptionPrompt OptionPrompt; 
};


typedef struct Text
{
	float xPos;
	float yPos;

	bool CameraRelative;
	bool beingUsed;

	TTF_Text *text;

	Object *attachedObj;
	int recordedInstance;
	TextPosition textPos;

	char name[TEXT_NAME_MAX_LEN];
} Text;

typedef struct TextList
{
	Text texts[MAX_TEXT_TEXTURES];
	int count;
} TextList;

typedef struct FontList
{
	int head;
	char names[MAX_LOADED_FONTS][FONT_FILE_NAME_MAX];
	TTF_Font *fonts[MAX_LOADED_FONTS];
} FontList;

typedef struct TextBox 
{
	struct TextBox *nextText;

	Object *boxPtr;
	Object *portraitObj;

	char textPhrase[MAX_TEXT_LENGTH];
	char font[FONT_FILE_NAME_MAX];
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
	int textLengthSize;	// Maximum amount of pixels from left to right to render text within before going to next line

	int currentIndex;
	int Counter;
	int currentXPos;
	int currentYPos;

	int LineSpacing;	// space between lines 
	float TextSize;    	// width and height of text characters

	Text *textReference;
	SDL_Color color;
} TextBox;


typedef struct Camera 
{
	float CameraX;
	float CameraY;
	float prevCameraX;
	float prevCameraY;

	float minCameraX;
	float maxCameraX;
	float minCameraY;
	float maxCameraY;

	float zoomX;
	float zoomY;
	int width;
	int height;
	int zoomedWidth;
	int zoomedHeight;

	float CameraXBuffer;
	float CameraYBuffer;
	bool CameraLatch;
	CameraState CameraMode;
} Camera;

typedef struct CameraView
{
	Camera cam;
	bool useMainCam;

	float viewXPos;
	float viewYPos;
	float viewWidth;
	float viewHeight;
	double direction;
	Layer layer;

	Object *attachedObj;
	int recordedInstance;
	
	SDL_Texture *target;

	Uint64 nextRender;
	Uint64 ticksUntilRefresh;

	bool active;
} CameraView;

typedef struct RenderFrame 
{
	SDL_Window *Window;
	int screenWidth;
	int screenHeight;
	SDL_Renderer *Renderer;
	TTF_TextEngine *textEngine;

	bool Fullscreen;
	bool Scaled;

	int FramesElapsed;
	Uint64 FrameTimer;
} RenderFrame;


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
	SpriteSetList bgSpriteSets;
	Sprite *BackgroundSpriteBuffer;

	float bgParallax;

	RenderMode BackgroundRenderMode;

	TilePlane tileBG;
} BackgroundData;


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
	char expression[3];

	bool elseBranchPresent;
	int branchDistanceIfFalse;	// amount of instructions to skip if false
} SceneBranchData;

typedef struct SceneLoop
{
	int repeatTimes;
	int currentLoop;
	int instructionCount;
} SceneLoop;

union SceneActionArguments
{
	int SceneID;
	int instructionsToSkip;
	int WaitTicks[2];
	struct TextBox *sceneText;
	int animationDetails[2];
	float positions[2];
	struct SoundMeta soundData;
	struct ObjectMeta objectInfo;
	bool hidden;
	float CameraData[3];
	float zoomScales[3];
	int cameraMode;
	Layer layer;
	SceneBranchData branchData;
	int variableArgs[2];
	int invisWall[4];
	GameEvent TriggerEvent;
	SceneLoop loopData;
};

typedef enum SceneActionID
{
	SCENE_END,
	SCENE_LOOP_POINT,
	SCENE_SKIP_INSTRUCTIONS,
	SCENE_SWITCH_CUTSCENE,
	SCENE_TRIGGER_GAME_EVENT,
	SCENE_DISABLE_PLAYER,
	SCENE_ENABLE_PLAYER,
	SCENE_WAIT,
	SCENE_CHANGE_VARIABLE_BY,
	SCENE_SET_VARIABLE_TO,
	SCENE_IF_STATEMENT,
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
	SCENE_CHANGE_CHANNEL_VOL,
	SCENE_SET_CAMERA_POS,
	SCENE_SET_CAMERA_MODE,
	SCENE_MOVE_CAMERA,
	SCENE_MOVE_CAMERA_TO,
	SCENE_MOVE_CAMERA_TO_OBJECT,
	SCENE_SET_CAMERA_ZOOM,
	SCENE_CHANGE_CAMERA_ZOOM,
	SCENE_CHANGE_CAMERA_ZOOM_TO,
	UNDEFINED_SCENE_ACTION
} SceneActionID;

typedef struct SceneAction
{
	SceneActionID ActionID;
	bool parallelAction;

	Object *ActorObject;
	union SceneActionArguments ActionData;

	struct SceneAction *nextSceneAction;
	struct SceneAction *prevSceneAction;
} SceneAction;


typedef int (*ConsoleCommandFunction)(char *, World *);

typedef struct ConsoleCommand
{
	char name[MAX_LEN];
	char helpString[HELP_STRING_MAX];
	char formatString[MAX_LEN];
	ConsoleCommandFunction function;
} ConsoleCommand;

typedef struct InputHistory
{
	int head;
	int searchIndex;
	int entries;
	char inputs[USER_INPUT_HISTORY_LEN][CONSOLE_STRING_LENGTH];
} InputHistory;


typedef struct Entity
{
	int EntityID;

} Entity;

typedef struct World
{
	Camera MainCamera;
	CameraView views[VIEW_COUNT];

	BackgroundData WorldBackground;

	PlayerData Player;
	ObjectController *ObjectList;

	TextList TextList;
	FontList FontList;

	int GamePaused;
	int level;
	LemonGameState GameState;
	GameEventManager GameEvents;

	TextBox *TextQueue;
	
	CutsceneID CurrentCutscene;
	SceneAction *SceneActionQueue;
	SceneAction *nextSceneAction;
	int SceneActionCount;

	WorldPhysics PhysicsType;
	float GlobalGravityY;
	float GlobalGravityX;
} World;

typedef struct MouseData
{
	int LeftButton;
	int RightButton;
	int MiddleButton;
	int SideButton1;
	int SideButton2;

	float xPos;
	float yPos;

	float wheelY;
	float wheelX;
	int wheelYDir;
	int wheelXDir;
} MouseData;

typedef struct GamePadData
{
	SDL_JoystickID ID;
	SDL_Gamepad *gamepad;

	int dPadUp;
	int dPadDown;
	int dPadLeft;
	int dPadRight;

	int northButton;
	int southButton;
	int westButton;
	int eastButton;

	int leftShoulder;
	int rightShoulder;
	float leftTrigger;
	float rightTrigger;

	int start;
	int back;
	int guide;

	int leftStick;
	int rightStick;
	float leftStickX;
	float leftStickY;
	float rightStickX;
	float rightStickY;
} GamePadData;


typedef struct EngineConfig
{
	bool ContiguousAnimAllocation;

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

	Sprite *DefaultTexture;
	DisplayData DefaultDisplay;
} EngineConfig;


typedef struct RenderConfig
{
	bool drawSprites;
	bool drawBackGround;
	bool drawHUD;
	bool drawParticles;
	bool drawCamViews;

	int drawHitboxes;
	int HitboxOutlineThickness;

	int maxObjects;
	int maxParticles;
	int maxUIElements;

	int drawnObjects;

	int RendersPerSecond;
	Uint64 RenderDelta;

	bool vSync;
} RenderConfig;


typedef struct TextConfig
{
	int portraitSize;
	float defaultTextPointSize;
	char defaultFont[FONT_FILE_NAME_MAX];

	SDL_Color DebugTextColour;
	float DebugTextPointSize;
	TTF_Font *DebugFont;
	TextList DebugTextList;

	bool Typing;
	char userInputString[USER_INPUT_MAX_LEN];
	int userInputIndex;
	float cursorXPos;
	float cursorYPos;
	Text *typingText;
} TextConfig;


typedef struct DebugConfig
{
	bool consoleOpen;
	ConsoleTextSetting ConsoleTextEnabled;
	char ConsoleString[CONSOLE_STRING_LENGTH];
	InputHistory consoleHistory;
	float consoleXPos;
	float consoleYPos;
	int scrollVal;
	bool consoleFocus;
	ConsoleCommand commands[MAX_CONSOLE_COMMANDS];
	
	InputHistory userInputHistory;
	int argIndex;

	int PauseEngine;

	int DebugTextDisplayMode;
	int DebugTextInfoPreset;
	int DebugOverlay;
	int FPSCounter;
	int CameraInfo;
	int SoundInfo;
	bool showEvents;
	bool showSceneActions;
	bool showSpriteset;
	bool showErrors;

	bool noclip;
} DebugConfig;


typedef struct String
{
	char *stringChars;
	int length;
} String;

#endif


//Global variables/data
EXPORT extern RenderFrame ScreenData;

EXPORT extern float deltaTime;

EXPORT extern MouseData MouseInput;

EXPORT extern GamePadData GamePadInput;

EXPORT extern ButtonState buttons[INPUT_COUNT];

EXPORT extern GameFlag GameFlags[GAME_FLAG_COUNT];

EXPORT extern EngineConfig EngineSettings;

EXPORT extern RenderConfig RenderSettings;

EXPORT extern TextConfig TextSettings;

EXPORT extern DebugConfig DebugSettings;
