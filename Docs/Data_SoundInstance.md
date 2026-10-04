# SoundInstance

## Definition
```
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
```

## Description
This struct represents a sound being played by the engine. Sounds can be loaded asynchronously, so
before attempting to write, delete, or otherwise modify one you should ensure that the ['state'](Enum_SoundState.md) 
variable is not set to 'SOUND_LOADING'. (Failing to do so will result in a race condition.)

Sounds can be played by calling [PlaySound()](Func_PlaySound.md).

Panning can be manually adjusted with [SetLRPan()](Func_SetLRPan.md).

Sounds are controlled on a channel-basis, with each sound having their own volume and each channel having 
a master volume. The volume output of a sound are these two values multiplied. Channels are primarily a 
way to organise sounds and to be able to set volume levels for different categories or types of sound.

Sounds with the 'positional' boolean set to true will adjust the volume in real-time as if it were originating
from its 'xPos' and 'yPos' position values. This only affects panning and volume and does not simulate 
spatial audio.


## Version
Available since V0.04.


----