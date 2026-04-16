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
	INPUT_COUNT,

	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_SIDE1,
	MOUSE_SIDE2
} LemonKeys;


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
	LAYER_COUNT = 20,
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
} CutsceneID;


typedef enum CameraState 
{
	FOLLOW_PLAYER = 0,
	MENU_CAMERA = 1,
	FREE_ROAM = 2,
	FREE_ROAM_RESTRICTED = 3,
	UNDEFINED_CAMERA_STATE
} CameraState;


typedef enum ReservedFlags
{
	RFLAG_DEFAULT 			= 0x00000000,
	RFLAG_DISABLE_PHYSICS 	= 0x00000001,
	RFLAG_PRESERVE_OBJECT	= 0x00000002,
	RFLAG_PRESERVE_ONCE		= 0x00000006 // activates PRESERVE_OBJECT as well
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

int getObjectID(char entry[]);
const char* getObjectIDName(ObjectType input);


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
	FALSE_CAMERA_BOUNDARY,
	LEVEL_TRIGGER,
	LEVEL_TRIGGER_SEAMLESS,
	SET_PLAYER_LAYER,
	START_WITH_MUSIC,
	PLAY_MUSIC_TRIGGER,
	STOP_ALL_SOUND_LOOPS,
	CACHE_TRIGGER,
	LOAD_PART_TRIGGER,
	SWITCH_TO_NEW_PART_TRIGGER,
	LOAD_PART,
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
	LOOP_INDEFINITELY = -1,
	ONE_FRAME_INDEFINITE_ANIMATION = -2
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
	char folder[MAX_LEN];
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


typedef struct animationFrame 
{
	struct animationFrame *nextFrame;

	struct sprite *frameSprite;
	float SpriteXOffset;
	float SpriteYOffset;
	float rotation;
} AnimationFrame;


// Starts counting animation IDs from 1
typedef struct animation 
{
	struct animation *nextAnimation;

	struct animationFrame *animationData;
	int frameCount;

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
	struct sprite *firstSprite;
	struct sprite *lastSprite;

	struct spriteSet *nextSet;
	struct spriteSet *prevSet;

	struct animation *Animations;

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


typedef struct TileMap
{
	unsigned int centerTileX;
	unsigned int centerTileY;
	unsigned int tileSize;
} TileMap;


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


typedef struct Polygon
{
	SDL_Vertex *vertexList;
	int vertices;
	bool quad;
	int *indicies;
} Polygon;

// downside of union approach: bad with memory use, as if any large component exists, ALL component lists will increase
// upside of union approach: much easier to work with as existing functions do not need to be copied/rewritten
typedef union ComponentType
{
	HealthComponent HealthComponent;
	TileMap TileMap;
	Timer Timer;
	StopWatch StopWatch;
	PhysicsComponent PhysicsComponent;
	Polygon Polygon;

} ComponentType;

typedef struct SparseList
{
	char componentName[COMPONENT_NAME_LENGTH];
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

	SparseList HealthComponent;
	SparseList TileMap;
	SparseList Timer;
	SparseList StopWatch;
	SparseList HurtComponent;
	SparseList PhysicsComponent;
	SparseList Polygon;

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


typedef enum GameEventID
{
	NO_EVENT = 0,
	EVENT_SWITCH_LEVEL,
	EVENT_PLAY_CUTSCENE,
	EVENT_PLAY_CUTSCENE_FROM_FILE,
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
	UNDEFINED_EVENT
} GameEventID;

const char* getEventName(GameEventID input);


typedef union GameEventData
{
	int screenDimensions[2];
	float zoomScales[2];
	int newLevelID;
	FILE *loadedFile;
	float ObjectGoTo[3];
	Object *object;
	int sceneID;
	char sceneName[CUTSCENE_FILE_NAME_MAX];
	float colourScale;
} GameEventData;

typedef struct GameEvent
{
	GameEventID EventID;
	GameEventData EventData;
	bool loadingAFile;
} GameEvent;

typedef struct GameEventManager
{
	GameEvent Events[MAX_QUEUED_GAME_EVENTS];
	int nextAvailable;
	int eventsPending;
} GameEventManager;


struct TextEventTrigger
{
	GameEvent event;
};


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


typedef struct TextBox 
{
	struct TextBox *nextText;

	Object *boxPtr;

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
	int TextSize;    	// width and height of text characters

	int textIndex;
	SDL_Color color;
} TextBox;

typedef struct Text
{
	float xPos;
	float yPos;

	TTF_Text *text;
	TextBox *textBox;

	bool CameraRelative;
	bool beingUsed;

} Text;

typedef struct TextList
{
	Text texts[MAX_TEXT_TEXTURES];
	int count;
} TextList;

typedef struct FontList
{
	int head;
	char name[MAX_LOADED_FONTS][FONT_FILE_NAME_MAX];
	TTF_Font *font[MAX_LOADED_FONTS];
} FontList;

typedef struct InputHistory
{
	int head;
	int searchIndex;
	int entries;
	char inputs[USER_INPUT_HISTORY_LEN][CONSOLE_STRING_LENGTH];
} InputHistory;


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
	int width;
	int height;
	int zoomedWidth;
	int zoomedHeight;
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

	Object *attachedObj;
	Layer layer;

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
	struct TextBox *sceneText;
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
	GameEvent TriggerEvent;
};

typedef enum SceneActionID
{
	SCENE_END,
	SCENE_DISABLE_PLAYER,
	SCENE_ENABLE_PLAYER,
	SCENE_WAIT,
	SCENE_TRIGGER_GAME_EVENT,
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
	SCENE_CHANGE_CHANNEL_VOL,
	SCENE_SET_CAMERA_POS,
	SCENE_MOVE_CAMERA,
	SCENE_MOVE_CAMERA_SMOOTH,
	SCENE_SET_SCREEN_ZOOM,
	SCENE_CHANGE_SCREEN_ZOOM,
	UNDEFINED_SCENE_ACTION
} SceneActionID;

typedef struct SceneAction
{
	SceneActionID ActionID;
	bool parallelAction;
	int repeatTimes;

	Object *ActorObject;
	union SceneActionArguments ActionData;

	struct SceneAction *nextSceneAction;
	struct SceneAction *prevSceneAction;
} SceneAction;


typedef struct World
{
	Camera MainCamera;
	CameraView views[VIEW_COUNT];

	BackgroundData WorldBackground;

	PlayerData Player;
	ObjectController *ObjectList;

	int GamePaused;
	int level;
	LemonGameState GameState;
	GameEventManager GameEvents;

	TextBox *TextQueue;
	
	CutsceneID CurrentCutscene;
	SceneAction *SceneActionQueue;
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

	struct sprite *DefaultTexture;
	DisplayData DefaultDisplay;
} EngineConfig;


typedef struct RenderConfig
{
	int drawSprites;
	int drawBackGround;
	int drawObjects;
	int drawPlayer;
	int drawHUD;
	int drawParticles;
	int drawCamViews;

	int drawHitboxes;
	int HitboxOutlineThickness;

	int maxObjects;
	int maxParticles;
	int maxUIElements;

	int drawnObjects;
	int drawnParticles;
	int drawnHudElements;

	int RendersPerSecond;
	Uint64 RenderDelta;

	bool vSync;
} RenderConfig;


typedef struct TextConfig
{
	int portraitSize;
	int defaultTextSize;
	char defaultFont[FONT_FILE_NAME_MAX];

	TextList TextList;
	FontList FontList;
} TextConfig;


typedef struct DebugConfig
{
	ConsoleTextSetting ConsoleTextEnabled;
	char ConsoleString[CONSOLE_STRING_LENGTH];
	InputHistory consoleHistory;
	float consoleXPos;
	float consoleYPos;
	int scrollVal;
	bool TypingInConsole;
	bool consoleFocus;

	char userInputString[USER_INPUT_MAX_LEN];
	InputHistory userInputHistory;
	int userInputIndex;
	int argIndex;
	float cursorXPos;

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

	SDL_Color DebugTextColour;
	float DebugTextPointSize;
	TTF_Font *DebugFont;
	TextList DebugTexts;
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

EXPORT extern int keyboard[INPUT_COUNT];

EXPORT extern int GameFlags[GAME_FLAG_COUNT];

extern EngineConfig EngineSettings;

extern RenderConfig RenderSettings;

extern TextConfig TextSettings;

extern DebugConfig DebugSettings;


