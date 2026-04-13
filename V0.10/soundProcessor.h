SoundInstance* PlaySound(const char fileName[], const char folderName[], ChannelName channel, float volume);

SoundInstance* PlaySoundFadeIn(const char fileName[], const char folderName[], ChannelName channel, float secondsToFade);

SoundInstance* PlaySoundRepeat(const char fileName[], const char folderName[], ChannelName channel, float volume, int repeatTimes);

SoundInstance* PlaySoundPositional(const char fileName[], const char folderName[], float xPos, float yPos, Camera positionCam);

SoundInstance* PlayPositionalObjectSound(const char fileName[], float xPos, float yPos, Camera positionCam);

SoundInstance* SetSoundSpeed(SoundInstance *inputSound, float newSpeed);

SoundInstance* RepeatSound(SoundInstance *input, int repeatTimes);

SoundInstance* PositionSound(SoundInstance *input, float xPos, float yPos, Camera positionCam);

SoundInstance* SetLRPan(SoundInstance *input, float pan);

SoundInstance* ChangeLRPan(SoundInstance *input, float pan);

int setPanLevels(SoundInstance *input, float left, float right);

int fadeOutSound(SoundInstance *input, float secondsToFade);


SoundInstance* getNewSound(ChannelName channel);

long getFileSize(const char path[]);

void findSoundFile(const char fileName[], const char folderName[], char path[MAX_LEN * 3]);

MIX_Audio* loadAudio(const char fileName[], const char folderName[]);

void startSound(SoundInstance *sound, MIX_Audio *audio);

SoundInstance* getSoundInstance(const char soundName[], ChannelName channel);

int asyncAudioLoad(void *data);

int IterateAudio(Camera positionCam);

int applyAudioPositionEffect(SoundInstance *input, Camera positionCam);


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


int DisplaySoundChannelDebugInfo(ChannelName channel);