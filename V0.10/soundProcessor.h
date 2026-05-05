SoundInstance* PlaySound(const char fileName[], ChannelName channel, float volume);

SoundInstance* PlaySoundFadeIn(const char fileName[], ChannelName channel, float secondsToFade);

SoundInstance* PlaySoundRepeat(const char fileName[], ChannelName channel, float volume, int repeatTimes);

SoundInstance* PlaySoundPositional(const char fileName[], float xPos, float yPos, Camera positionCam);


SoundInstance* RepeatSound(SoundInstance *input, int repeatTimes);

SoundInstance* PositionSound(SoundInstance *input, float xPos, float yPos, Camera positionCam);

SoundInstance* SetSoundSpeed(SoundInstance *inputSound, float newSpeed);

SoundInstance* fadeOutSound(SoundInstance *input, float secondsToFade);

SoundInstance* SetLRPan(SoundInstance *input, float pan);

SoundInstance* ChangeLRPan(SoundInstance *input, float pan);

int setPanLevels(SoundInstance *input, float left, float right);



SoundInstance* getNewSound(ChannelName channel);

long getFileSize(const char path[]);

void findSoundFile(const char fileName[], char path[MAX_LEN * 2]);

MIX_Audio* loadAudio(const char fileName[]);

void startSound(SoundInstance *sound, MIX_Audio *audio);

SoundInstance* getSoundInstance(const char soundName[], ChannelName channel);

int asyncAudioLoad(void *data);

int IterateAudio(Camera positionCam);

int applyAudioPositionEffect(SoundInstance *input, Camera positionCam);


int initialiseAudio(void);

SoundInstance* createEmptySoundInstance(ChannelName Channel);

int deleteSoundInstance(SoundInstance *inputSound, ChannelName channel);

int cleanUpAudioData(void);


int UpdateChannelGain(ChannelName channel);

int SetChannelVolume(ChannelName channel, float newVolume);

int ChangeChannelVolume(ChannelName channel, float changeVolume);

void SetAllVolume(float newVolume);

void ChangeAllVolume(float changeVolume);

int SetChannelSpeed(ChannelName channel, float newSpeed);

int MuteChannel(ChannelName channel);

int UnmuteChannel(ChannelName channel);

void MuteAllAudio(void);

void UnmuteAllAudio(void);

void StopAllAudio(void);

void ToggleAllAudio(void);

void PauseAllAudio(void);

void ResumeAllAudio(void);

int StopAudioInChannel(ChannelName channel);

int ToggleChannel(ChannelName channel);

int PauseChannel(ChannelName channel);

int ResumeChannel(ChannelName channel);


int DisplaySoundChannelDebugInfo(ChannelName channel);

void putConsoleCachedSounds(void);