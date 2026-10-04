# ChannelName

## Definition
```
typedef enum ChannelName 
{
	MUSIC_CHANNEL = 0,
	SPEECH = 1,
	PLAYER_SFX = 2,
	OBJECT_SFX = 3,
	CHANNEL_COUNT 	// Simultaniously used as 'last', undefined channel and channel count
} ChannelName;
```

## Description
This enum represents all sound channels. 'CHANNEL_COUNT' is used to allocate the amount of channels required,
so when adding new channels, enter them before this value. New channels can be added to suit your game's 
requirements.

## Version
Available since V0.04.


----