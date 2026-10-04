# SoundState

## Definition
```
typedef enum SoundState
{
	SOUND_INACTIVE,
	SOUND_LOADING,
	SOUND_PLAYING
} SoundState;
```

## Description
This enum defines the current state of a [SoundInstance](Data_SoundInstance.md), whether it
is yet to be used, has data being loaded or is currently playing. 

If set to 'SOUND_LOADING' this instance has data being loaded on a seperate thread and should 
not be modified until this instance is set to 'SOUND_PLAYING'.

## Version
Available since V0.10.


----