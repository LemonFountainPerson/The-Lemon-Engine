# PlaySound

## File Location

Defined in [soundProcessor.h](../V0.11/soundProcessor.h).

## Syntax
```
SoundInstance* PlaySound(const char fileName[], float volume, ChannelName channel)
```

## Description

Play a sound from start to finish. After this function is called, playback and cleanup will happen automatically.

Larger sound files will not begin playback immediately even if this function returns a [SoundInstance](Data_SoundInstance.md), 
as this may result in the data being loaded on a seperate thread.

## Inputs

|                  Type                          |    Name     |      Description      |
| ---------------------------------------------- | ----------- | --------------------- |
| const char[] | **fileName** | The path to the sound file to play from 'LemonData/Sounds/'. File extention is not needed if the sound is an mp3, ogg or wav. |
| float | **volume** | The volume level to play the sound at, 1.0 is full volume. |
| [ChannelName](Enum_ChannelName.md) | **channel** | The channel to play the sound on. |



## Return Value

Returns a pointer to a newly-created [SoundInstance](Data_SoundInstance.md), or NULL on failure.


## Version

Available since V0.09.

-------------------------------