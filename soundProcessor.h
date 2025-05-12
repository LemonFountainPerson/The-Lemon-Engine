#ifndef IS_DEFINED
#include "data.h"
#endif


int LemonPlaySound(char fileName[], char folderName[], int channel, double volume);

int InitSound(const char *pathPtr, SoundInstance *SoundChannel, double volume);

int IterateAudio(void);

int initialiseAudio(void);

int CleanUpAudioData(void);


int StopAudioInChannel(int channel);

int PauseAllAudio(void);

int ResumeAllAudio(void);

int PauseChannel(int channel);

int ResumeChannel(int channel);