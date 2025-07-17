#ifndef IS_DEFINED
#include "data.h"
#endif


int LemonPlaySound(const char fileName[], const char folderName[], ChannelName channel, float volume);

int InitSound(const char *pathPtr, ChannelName channel, float volume);

int IterateAudio(void);

int MuteChannel(ChannelName channel);

int UnmuteChannel(ChannelName channel);

int MuteAllAudio(void);

int UnmuteAllAudio(void);

int deleteSoundInstance(SoundInstance *inputSound);

int initialiseAudio(void);

int cleanUpAudioData(void);


int StopAudioInChannel(ChannelName channel);

int ToggleAllAudio(void);

int PauseAllAudio(void);

int ResumeAllAudio(void);

int ToggleChannel(ChannelName channel);

int PauseChannel(ChannelName channel);

int ResumeChannel(ChannelName channel);