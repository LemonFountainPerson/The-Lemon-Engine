#ifndef IS_DEFINED
#include "data.h"
#endif


int LemonPlaySound(const char fileName[], const char folderName[], int channel, float volume);

int InitSound(const char *pathPtr, int channel, float volume);

int IterateAudio(void);

int deleteSoundInstance(SoundInstance *inputSound);

int initialiseAudio(void);

int cleanUpAudioData(void);


int StopAudioInChannel(int channel);

int PauseAllAudio(void);

int ResumeAllAudio(void);

int PauseChannel(int channel);

int ResumeChannel(int channel);