#ifndef IS_DEFINED
#include "data.h"
#endif


int LemonPlaySound(const char fileName[], const char folderName[], ChannelName channel, float volume);

SoundInstance* InitSound(const char *pathPtr, ChannelName channel, float volume);

SoundInstance* getSoundInstance(const char soundName[], ChannelName channel);

int IterateAudio(void);


int initialiseAudio(void);

int deleteSoundInstance(SoundInstance *inputSound);

int StopAudioInChannel(ChannelName channel);

int cleanUpAudioData(void);


int SetChannelVolume(ChannelName channel, float newVolume);

int MuteChannel(ChannelName channel);

int UnmuteChannel(ChannelName channel);

int MuteAllAudio(void);

int UnmuteAllAudio(void);

int ToggleAllAudio(void);

int PauseAllAudio(void);

int ResumeAllAudio(void);

int ToggleChannel(ChannelName channel);

int PauseChannel(ChannelName channel);

int ResumeChannel(ChannelName channel);