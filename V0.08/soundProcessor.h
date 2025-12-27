#ifndef IS_DEFINED
#include "data.h"
#include "LemonMain.h"
#endif


SoundInstance* Lemon_PlaySound(const char fileName[], const char folderName[], ChannelName channel, float volume);

int Lemon_PlaySoundSpeed(const char fileName[], const char folderName[], ChannelName channel, float volume, float speed);

int Lemon_PlaySoundRepeat(const char fileName[], const char folderName[], ChannelName channel, float volume, int repeatTimes);

SoundInstance* RepeatSound(SoundInstance *input, int repeatTimes);

SoundInstance* InitSound(const char *pathPtr, ChannelName channel, float volume);

SoundInstance* getSoundInstance(const char soundName[], ChannelName channel);

int IterateAudio(void);


int initialiseAudio(void);

SoundInstance* createEmptySoundInstance(ChannelName Channel);

int deleteSoundInstance(SoundInstance *inputSound, ChannelName channel);

int StopAudioInChannel(ChannelName channel);

int cleanUpAudioData(void);


int UpdateChannelGain(ChannelName channel);

int SetChannelVolume(ChannelName channel, float newVolume);

int ChangeChannelVolume(ChannelName channel, float changeVolume);

int SetAllVolume(float newVolume);

int ChangeAllVolume(float changeVolume);

SoundInstance* SetSoundSpeed(SoundInstance *inputSound, float newSpeed);

int SetChannelSpeed(ChannelName channel, float newSpeed);

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
