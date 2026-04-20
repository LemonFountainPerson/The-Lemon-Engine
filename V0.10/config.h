#ifndef CONFIG_DEFINED
#define CONFIG_DEFINED


//								Default Engine Settings (Can be modified during runtime via variables)
//-------------------------------------------------------------------------------------------------

// Most displayed objects are designed to adjust positioning based on resolution; 
// however the lowest expected resolution is 1280 x 720, lower values may result in unintended behaviour
#define V_RESOLUTION 720
#define H_RESOLUTION 1280

#define DEFAULT_TEXTURE "Missing.png"
#define DEFAULT_FONT "PTSansBold"

#define MAX_OBJECTS_RENDER 256
#define MAX_PARTICLES_RENDER 64
#define MAX_HUD_ELEMENTS_RENDER 128

#define MAX_SOUNDS_PER_CHANNEL 16
#define MAX_TEXTQUEUE_LENGTH 128			// Number of textBoxess allowed in the TextQueue at once
#define MAX_SCENEACTIONS 200 				// Number of scene actions allowed in the queue at once

#define RESERVED_OBJECTS 500
#define PRESERVED_SPRITESETS 2

#define X_WORLD_BOUND 100000.0
#define Y_WORLD_BOUND 100000.0		

#define TICKS_PER_SECOND 60
#define RENDERS_PER_SECOND 20000	

#define VSYNC_DEFAULT true
#define CONTIGUOUS_ANIMATION_ALLOCATION true

//-------------------------------------------------------------------------------------------------


//								Engine constants (Cannot be modified during runtime)
//-------------------------------------------------------------------------------------------------

#define DEBUG_MODE true
#define LEMON_COLLISION_PHYSICS true 		// turn this to false to disable all physics and collision if you want to implement your own scheme

#define RANDOM_SEED time(NULL)

#define GAME_FLAG_COUNT 64  

#define MAX_OBJECTS 5000
#define OBJECT_NAME_LENGTH 24
#define ANIMATION_NAME_LENGTH 80
#define COMPONENT_NAME_LENGTH 32
#define DEFAULT_SCALEMODE SDL_SCALEMODE_LINEAR

#define MAX_QUEUED_GAME_EVENTS 64

#define CHANNEL_NAME_LENGTH 24
#define MAX_CACHED_SOUNDS 32
#define ASYNC_AUDIO_LOADER true
#define ASYNC_AUDIO_LOADER_PERSIST_MS 8000
#define ASYNC_AUDIO_SIZE_THRESHOLD 1000000

#define USER_INPUT_MAX_LEN 200
#define USER_INPUT_HISTORY_LEN 80
#define CONSOLE_STRING_LENGTH 256
#define HELP_STRING_MAX 200
#define MAX_CONSOLE_COMMANDS 40
 
#define MAX_LEN 80
#define FILE_POSITION_HISTORY_LENGTH 8
#define CUTSCENE_FILE_NAME_MAX 50
#define FONT_FILE_NAME_MAX 50

#define MAX_TEXT_LENGTH 200
#define MAX_TEXT_OPTIONS 4
#define OPTION_TEXT_MAX_LEN 50

#define MAX_TEXT_TEXTURES 16 		
#define MAX_LOADED_FONTS 16

#define DEBUG_TEXT_MAX_LENGTH 400
#define EXPERIMENTAL_TEXT true		// disable to revert text rendering to old method

#define VIEW_COUNT 8
#define DEFAULT_VIEW_REFRESH_RATE 30

#define MINIMUM_SCREEN_WIDTH 144
#define MINIMUM_SCREEN_HEIGHT 144
#define MINIMUM_ZOOM 0.4  		 // Do not set this value to 0.1 or below

// Do not change
#define FRAMERATE_UPDATE_RATE		3	
#define NS_PER_SECOND				1000000000	
#define INTERP_FRAMERATE(x)			(x << (FRAMERATE_UPDATE_RATE - 1))
#define FRAMERATE_UPDATE_TICK 		(NS_PER_SECOND >> (FRAMERATE_UPDATE_RATE - 1))



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
#define SWITCH_TO_MISSING(x) switchSprite(-1, 0, getDisplay(x)); 
#define BACKGROUND_SETID 1
#define NO_ACTION NULL
#define LOOP_SOUND 0

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
#define FONT_ROOT		"LemonData/Fonts/"

//-------------------------------------------------------------------------------------------------


#define EXPORT __declspec( dllexport )


#define LEMON_ENGINE_INFO "Lemon Engine - by Liam Fuentes Pessoa"

#define FILE_READER_VERSION "FRV<1>"
#define LEMON_VERSION "V0.10"

#endif