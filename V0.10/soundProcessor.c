#include "LemonEngine.h"

#define AUDIO_DISTANCE_SCALE 1000.0
#define AUDIO_FALLOFF_RATE 6.0
#define ASYNC_AUDIO_CLOSED -1


static MIX_Mixer *audioMixer = NULL;

static SDL_PropertiesID propertiesContainer = 0;

static const char channelNames[CHANNEL_COUNT][CHANNEL_NAME_LENGTH] = {"Music", "Speech", "PlayerSfx", "ObjectSfx"};

static SoundChannel SoundChannels[CHANNEL_COUNT];

static CachedSoundList storedSounds = {0};

SDL_Mutex *scheduleLock = NULL;
SDL_Mutex *threadLock = NULL;
static int scheduledSounds = ASYNC_AUDIO_CLOSED;
static bool closeAllThreads = false;


SoundInstance* PlaySound(const char fileName[], const char folderName[], ChannelName channel, float volume)
{
	if (channel < 0 || channel >= CHANNEL_COUNT || fileName == NULL)
	{
		return NULL;
	}

	if (SoundChannels[channel].Pause == 1)
	{
		return NULL;
	}

	// find sound slot
	SoundInstance *newSound = getNewSound(channel);
	
	if (newSound == NULL)
	{
		putConsoleError("Failed to initialise new sound instance.");
		return NULL;
	}

	strcpy(newSound->name, fileName);
	strcpy(newSound->folder, folderName);
	newSound->volume = volume;
	newSound->channel = channel;

	// try to find sound if already loaded
	CachedSound *list = storedSounds.list;
	int index = 0;

	while (index < MAX_CACHED_SOUNDS && list[index].data != NULL)
	{
		if (strcmp(fileName, list[index].name) == 0)
		{
			startSound(newSound, list[index].data);

			return newSound;
		}

		index++;
	}

	// check file
	char path[MAX_LEN * 3] = {0};
	findSoundFile(fileName, folderName, path);

	if (path[0] == '\0')
	{
		return NULL;
	}


	if (ASYNC_AUDIO_LOADER && getFileSize(path) > ASYNC_AUDIO_SIZE_THRESHOLD)
	{
		// data hasn't been loaded yet; schedule it
		while (!SDL_TryLockMutex(scheduleLock)) {}

		if (scheduledSounds == ASYNC_AUDIO_CLOSED)
		{
			scheduledSounds = 1;
			SDL_DetachThread(SDL_CreateThread(&asyncAudioLoad, "Audio Loader", NULL));
		}
		else
		{
			scheduledSounds++;
		}

		newSound->state = SOUND_LOADING;

		SDL_UnlockMutex(scheduleLock);
	}
	else
	{
		// load sound
		MIX_Audio *loadedAudio = loadAudio(newSound->name, newSound->folder);
		if (loadedAudio == NULL)
		{
			putConsoleError("Couldn't find audio file '%s'", fileName);
			return NULL;
		}

		startSound(newSound, loadedAudio);
	}


	return newSound;
}


SoundInstance* getNewSound(ChannelName channel)
{
	SoundInstance *current = SoundChannels[channel].firstSound;
	int count = 0;

	while (current != NULL && current->state != SOUND_INACTIVE)
	{
		current = current->nextSound;
		count++;
	}

	if (current == NULL && count < EngineSettings.MaxSoundsPerChannel)
	{
		return createEmptySoundInstance(channel);
	}

	if (current == NULL)
	{
		return NULL;
	}

	current->volume = 1.0;
	current->repeats = 1;
	current->name[0] = '\0';
	current->folder[0] = '\0';
	current->panLevels.left = 1.0;
	current->panLevels.right = 1.0;
	current->positional = false;
	MIX_SetTrackFrequencyRatio(current->audio, 1.0);

	return current;
}


long getFileSize(const char path[])
{
	FILE *file = fopen(path, "rb");
	long size = -1;
	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		fclose(file);
	}
	
	return size;
}


void findSoundFile(const char fileName[], const char folderName[], char path[MAX_LEN * 3])
{
	if (fileName == NULL || strlen(fileName) > MAX_LEN || (folderName != NULL && strlen(folderName) > MAX_LEN) )
	{
		return;
	}	

	// Must load audio - Construct audio file path
	strcpy(path, SOUND_ROOT);

	if (folderName != NULL)
	{
		strcat(path, folderName);
		strcat(path, "/");
	}

	strcat(path, fileName);

	int pathLength = strlen(path);

	char extensions[][10] = {".wav", ".mp3", ".ogg", ""};		// changing the order of this list modifies its priority

	for (int attempt = 0; attempt < 4; attempt++)
	{
		strcpy(path + pathLength, extensions[attempt]);

		// Check if file exists
	 	if (access(path, F_OK) != -1)
	 	{
	 		return;
	 	}
	}

	path[0] = '\0';

	return;
}

MIX_Audio* loadAudio(const char fileName[], const char folderName[])
{	
	// Must load audio - Construct audio file path
	char path[MAX_LEN * 3] = {0};
	findSoundFile(fileName, folderName, path);

	if (path[0] == '\0')
	{
		return NULL;
	}

	// Load audio from found file
	MIX_Audio *loadedAudio = MIX_LoadAudio(audioMixer, path, true);
	if (loadedAudio == NULL)
	{
		return NULL;
	}

	// Store this sound at the next slot in 'storedSounds', to be used later without having to re-load this data
	CachedSound *list = storedSounds.list;
	int index = storedSounds.head;
	storedSounds.head = (storedSounds.head + 1) % MAX_CACHED_SOUNDS;
	if (list[index].data != NULL)	// overwrite previous data if circled around to beginning; this is a ring buffer
	{
		MIX_DestroyAudio(list[index].data);
	}

	strncpy(list[index].name, fileName, MAX_LEN - 1);
	list[index].data = loadedAudio;

	return loadedAudio;
}


void startSound(SoundInstance *sound, MIX_Audio *audio)
{
	if (!MIX_SetTrackAudio(sound->audio, audio))
	{
		putConsoleError("Couldn't play audio '%s': %s", sound->name, SDL_GetError());
		return;
	}

	MIX_SetTrackGain(sound->audio, sound->volume * SoundChannels[sound->channel].channelVolume);

	MIX_SetTrackStereo(sound->audio, &sound->panLevels);

	MIX_TagTrack(sound->audio, channelNames[sound->channel]);

	if (SoundChannels[sound->channel].Pause == 0)
	{
		MIX_PlayTrack(sound->audio, propertiesContainer);
		MIX_SetTrackLoops(sound->audio, sound->repeats - 1);
	}

	sound->state = SOUND_PLAYING;

	return;
}


SoundInstance* PlaySoundFadeIn(const char fileName[], const char folderName[], ChannelName channel, float secondsToFade)
{
	Sint64 milis = (Sint64)(1000.0 * secondsToFade);
	SDL_SetNumberProperty(propertiesContainer, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, milis);
	SoundInstance *sound = PlaySound(fileName, folderName, channel, 1.0);
	SDL_SetNumberProperty(propertiesContainer, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, 0);

	return sound;
}


SoundInstance* PlaySoundSpeed(const char fileName[], const char folderName[], ChannelName channel, float volume, float speed)
{
	return SetSoundSpeed(PlaySound(fileName, folderName, channel, volume), speed);
}


SoundInstance* PlaySoundRepeat(const char fileName[], const char folderName[], ChannelName channel, float volume, int repeatTimes)
{
	return RepeatSound(PlaySound(fileName, folderName, channel, volume), repeatTimes);
}


SoundInstance* SetSoundSpeed(SoundInstance *inputSound, float newSpeed)
{
	if (inputSound == NULL)
	{
		return NULL;
	}

	newSpeed = fClamp(newSpeed, 0.1, 10.0);
	MIX_SetTrackFrequencyRatio(inputSound->audio, newSpeed);

	return inputSound;
}

SoundInstance* RepeatSound(SoundInstance *input, int repeatTimes)
{
	if (input == NULL)
	{
		return NULL;
	}

	// 0 indicates to loop forever
	if (repeatTimes < 0 || repeatTimes > 32000)
	{
		return NULL;
	}

	input->repeats = repeatTimes;
	MIX_SetTrackLoops(input->audio, repeatTimes - 1);

	return input;
}


SoundInstance* PlaySoundPositional(const char fileName[], const char folderName[], float xPos, float yPos, Camera positionCam)
{
	return PositionSound(PlaySound(fileName, folderName, OBJECT_SFX, 1.0), xPos, yPos, positionCam);
}

SoundInstance* PlayPositionalObjectSound(const char fileName[], float xPos, float yPos, Camera positionCam)
{
	return PositionSound(PlaySound(fileName, "Objects", OBJECT_SFX, 1.0), xPos, yPos, positionCam);
}

SoundInstance* updateSoundPosition(SoundInstance *input, Camera positionCam)
{
	if (input == NULL)
	{
		return NULL;
	}

	float x = (input->xPos - positionCam.CameraX) / AUDIO_FALLOFF_RATE;
	float y = (input->yPos - positionCam.CameraY) / AUDIO_FALLOFF_RATE;

	float volume = 1.0 - fClamp(sqrt((x * x) + (y * y)), 0.0, 1.0);

	input->panLevels.left = 1.0 - fClamp(x / AUDIO_FALLOFF_RATE, 0.0, 1.0);
	input->panLevels.right = 1.0 + fClamp(x / AUDIO_FALLOFF_RATE, -1.0, 0.0);

	float left = volume * input->panLevels.left;
	float right = volume * input->panLevels.right;

	setPanLevels(input, left, right);
	//printf("\n left: %f right: %f volume; %f   xDist: %f\n", left, right, volume, x);

	return input;
}

SoundInstance* PositionSound(SoundInstance *input, float xPos, float yPos, Camera positionCam)
{
	if (input == NULL)
	{
		return NULL;
	}

	input->xPos = xPos / AUDIO_DISTANCE_SCALE;
	input->yPos = yPos / AUDIO_DISTANCE_SCALE;

	input->positional = true;

	updateSoundPosition(input, positionCam);

	return input;
}

SoundInstance* SetLRPan(SoundInstance *input, float pan)
{
	if (input == NULL)
	{
		return NULL;
	}

	input->panLevels.left = 1.0 - fClamp(pan, 0.0, 1.0);
	input->panLevels.right = 1.0 + fClamp(pan, -1.0, 0.0);

	MIX_SetTrackStereo(input->audio, &input->panLevels);

	return input;
}

SoundInstance* ChangeLRPan(SoundInstance *input, float pan)
{
	if (input == NULL)
	{
		return NULL;
	}

	float newPan = (input->panLevels.right - input->panLevels.left) + pan;

	SetLRPan(input, newPan);

	return input;
}

int setPanLevels(SoundInstance *input, float left, float right)
{
	if (input == NULL)
	{
		return MISSING_DATA;
	}

	input->panLevels.left = left;
	input->panLevels.right = right;

	MIX_SetTrackStereo(input->audio, &input->panLevels);

	return LEMON_SUCCESS;
}

int fadeOutSound(SoundInstance *input, float secondsToFade)
{
	if (input == NULL || secondsToFade < 0.0)
	{
		return MISSING_DATA;
	}

	Sint64 milis = (Sint64)(secondsToFade * 1000.0);
	MIX_StopTrack(input->audio, MIX_TrackMSToFrames(input->audio, milis));

	return LEMON_SUCCESS;
}

int fadeOutChannel(ChannelName channel, float secondsToFade)
{
	if (channel >= CHANNEL_COUNT || channel < 0)
	{
		return INVALID_DATA;
	}


	SoundInstance *current = SoundChannels[channel].firstSound;
	int i = 0;

	if (secondsToFade < 0.001)
	{
		while (current != NULL && i < EngineSettings.MaxSoundsPerChannel)
		{
			MIX_StopTrack(current->audio, 0);

			current = current->nextSound;
			i++;
		}
	}
	else
	{
		Sint64 milis = (Sint64)(secondsToFade * 1000.0);

		while (current != NULL && i < EngineSettings.MaxSoundsPerChannel)
		{
			MIX_StopTrack(current->audio, MIX_TrackMSToFrames(current->audio, milis));

			current = current->nextSound;
			i++;
		}
	}
	

	return LEMON_SUCCESS;
}

SoundInstance* getSoundInstance(const char soundName[], ChannelName channel)
{
	if (channel >= CHANNEL_COUNT || channel < 0)
	{
		return NULL;
	}

	if (strlen(soundName) > MAX_LEN)
	{
		return NULL;
	}

	SoundInstance *currentSound = SoundChannels[channel].firstSound;

	if (currentSound == NULL)
	{
		return NULL;
	}

	int i = 0;
	while (currentSound->nextSound != NULL && i < EngineSettings.MaxSoundsPerChannel)
	{
		currentSound = currentSound->nextSound;
		i++;
	}

	i = SoundChannels[channel].soundCount;

	while (currentSound != NULL && i > 0)
	{
		if (strcmp(currentSound->name, soundName) == 0)
		{
			return currentSound;
		}

		currentSound = currentSound->prevSound;
		i--;
	}

	return NULL;
}


int asyncAudioLoad(void *data)
{
	SoundInstance *sound = NULL;

	putConsoleString("\nOpening async loader...");

	if (!SDL_TryLockMutex(threadLock))	// another thread already exists!
	{
		return LEMON_SUCCESS;
	}

	int count = 1;
	while(closeAllThreads == false && count > 0)
	{
		// attempt to find/load the data
		for (int channel = 0; channel < CHANNEL_COUNT; channel++)
		{
			sound = SoundChannels[channel].firstSound;
			
			int k = 0;

			while (sound != NULL && k < EngineSettings.MaxSoundsPerChannel)
			{
				k++;

				if (sound->state == SOUND_LOADING)	
				{
					while (!SDL_TryLockMutex(scheduleLock)) { }
					if (scheduledSounds > 0)
					{
						scheduledSounds--;
					}
					count = scheduledSounds;
					SDL_UnlockMutex(scheduleLock);

					MIX_Audio *loadedAudio = loadAudio(sound->name, sound->folder);
					if (loadedAudio != NULL)
					{
						startSound(sound, loadedAudio);
					}
					else
					{
						sound->state = SOUND_INACTIVE;
					}
				}

				sound = sound->nextSound;
			}
		}

		// if none left, begin timer to close thread
		Uint64 start = SDL_GetTicks();
		while (count == 0 && SDL_GetTicks() - start < ASYNC_AUDIO_LOADER_PERSIST_MS && !closeAllThreads)
		{
			if (SDL_TryLockMutex(scheduleLock))
			{
				count = scheduledSounds;
				SDL_UnlockMutex(scheduleLock);
			}
		}
	}


	// IMPORTANT: this code is the only place the scheduledSounds variable should be set to 'ASYNC_AUDIO_CLOSED'
	while (!SDL_TryLockMutex(scheduleLock)) { }

	scheduledSounds = ASYNC_AUDIO_CLOSED;

	SDL_UnlockMutex(scheduleLock);
	SDL_UnlockMutex(threadLock);

	putConsoleString("Closing async loader...");

	return LEMON_SUCCESS;
}

int IterateAudio(Camera positionCam)
{
	SoundInstance *sound;

	positionCam.CameraX /= AUDIO_DISTANCE_SCALE;
	positionCam.CameraY /= AUDIO_DISTANCE_SCALE;

	for (int channel = 0; channel < CHANNEL_COUNT; channel++)
	{
		if (SoundChannels[channel].Pause == 1)
		{
			continue;
		}

		int k = 0;
		sound = SoundChannels[channel].firstSound;

		while (sound != NULL && k < EngineSettings.MaxSoundsPerChannel)
		{
			k++;


			if (sound->state == SOUND_PLAYING)
			{
				if (!MIX_TrackPlaying(sound->audio) && MIX_GetTrackLoops(sound->audio) == 0)
				{
					sound->state = SOUND_INACTIVE;
				}
				else if (sound->positional)
				{
					updateSoundPosition(sound, positionCam);
				}
			}

			sound = sound->nextSound;
		}
	}

	return LEMON_SUCCESS;
}


int MuteChannel(ChannelName channel)
{
	if (channel >= CHANNEL_COUNT || channel < 0)
	{
		return INVALID_DATA;
	}

	SoundInstance *currentSound;
	currentSound = SoundChannels[channel].firstSound;
	int i = 0;

	while (currentSound != NULL && i < EngineSettings.MaxSoundsPerChannel)
	{
		MIX_SetTrackGain(currentSound->audio, 0.0);

		currentSound = currentSound->nextSound;
		i++;
	}
	
	return LEMON_SUCCESS;
}


int UnmuteChannel(ChannelName channel)
{
	return UpdateChannelGain(channel);
}


int UpdateChannelGain(ChannelName channel)
{
	if (channel >= CHANNEL_COUNT || channel < 0)
	{
		return INVALID_DATA;
	}

	SoundInstance *currentSound;
	currentSound = SoundChannels[channel].firstSound;
	int i = 0;

	while (currentSound != NULL && i < EngineSettings.MaxSoundsPerChannel)
	{
		MIX_SetTrackGain(currentSound->audio, currentSound->volume * SoundChannels[channel].channelVolume);
	
		currentSound = currentSound->nextSound;
		i++;
	}

	return LEMON_SUCCESS;
}


int SetChannelVolume(ChannelName channel, float newVolume)
{
	if (channel >= CHANNEL_COUNT || channel < 0)
	{
		return INVALID_DATA;
	}

	SoundChannels[channel].channelVolume = newVolume;

	UpdateChannelGain(channel);
	
	return LEMON_SUCCESS;
}


int ChangeChannelVolume(ChannelName channel, float changeVolume)
{
	if (channel >= CHANNEL_COUNT || channel < 0)
	{
		return INVALID_DATA;
	}

	SoundChannels[channel].channelVolume = SoundChannels[channel].channelVolume + changeVolume;

	UpdateChannelGain(channel);
	
	return LEMON_SUCCESS;
}


int SetAllVolume(float newVolume)
{
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		SetChannelVolume(i, newVolume);
	}

	
	return LEMON_SUCCESS;
}


int ChangeAllVolume(float changeVolume)
{
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		ChangeChannelVolume(i, changeVolume);
	}

	
	return LEMON_SUCCESS;
}


int MuteAllAudio(void)
{
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		MuteChannel(i);
	}

	return LEMON_SUCCESS;
}


int UnmuteAllAudio(void)
{
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		UnmuteChannel(i);
	}

	return LEMON_SUCCESS;
}


int StopAudioInChannel(ChannelName channel)
{
	if (channel >= CHANNEL_COUNT || channel < 0)
	{
		return INVALID_DATA;
	}

	MIX_StopTag(audioMixer, channelNames[channel], 0);

	return LEMON_SUCCESS;
}


int ToggleAllAudio(void)
{
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		if (SoundChannels[i].Pause == 0)
		{
			PauseChannel(i);
		}
		else
		{
			ResumeChannel(i);
		}
	}

	return LEMON_SUCCESS;
}


int PauseAllAudio(void)
{
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		PauseChannel(i);
	}

	return LEMON_SUCCESS;
}


int ResumeAllAudio(void)
{
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		ResumeChannel(i);
	}

	return LEMON_SUCCESS;
}


int ToggleChannel(ChannelName channel)
{
	if (channel >= CHANNEL_COUNT || channel < 0)
	{
		return INVALID_DATA;
	}

	if (SoundChannels[channel].Pause == 0)
	{
		PauseChannel(channel);
	}
	else
	{
		ResumeChannel(channel);
	}

	return LEMON_SUCCESS;
}


int PauseChannel(ChannelName channel)
{
	if (channel >= CHANNEL_COUNT || channel < 0)
	{
		return INVALID_DATA;
	}

	SoundInstance *currentSound;
	currentSound = SoundChannels[channel].firstSound;
	int i = 0;

	while (currentSound != NULL && i < EngineSettings.MaxSoundsPerChannel)
	{
		MIX_PauseTrack(currentSound->audio);

		currentSound = currentSound->nextSound;
		i++;
	}

	SoundChannels[channel].Pause = 1;

	return LEMON_SUCCESS;
}


int ResumeChannel(ChannelName channel)
{
	if (channel >= CHANNEL_COUNT || channel < 0)
	{
		return INVALID_DATA;
	}

	SoundInstance *currentSound;
	currentSound = SoundChannels[channel].firstSound;
	int i = 0;

	while (currentSound != NULL && i < EngineSettings.MaxSoundsPerChannel)
	{
		MIX_ResumeTrack(currentSound->audio);

		currentSound = currentSound->nextSound;
		i++;
	}

	SoundChannels[channel].Pause = 0;

	return LEMON_SUCCESS;
}

SoundInstance* createEmptySoundInstance(ChannelName Channel)
{
	SoundInstance *newSound = SoundChannels[Channel].firstSound;

	newSound = malloc(sizeof(SoundInstance));

	if (newSound == NULL)
	{
		return NULL;
	}

	newSound->state = SOUND_INACTIVE;
	newSound->volume = 1.0;
	newSound->audio = MIX_CreateTrack(audioMixer);
	memset(newSound->name, 0, MAX_LEN * sizeof(char));
	memset(newSound->folder, 0, MAX_LEN * sizeof(char));

	newSound->channel = Channel;
	newSound->positional = false;
	newSound->panLevels.left = 1.0;
	newSound->panLevels.right = 1.0;
	newSound->xPos = 0.0;
	newSound->yPos = 0.0;

	int count = 1;

	// Place new instance into sound channel linked list
	SoundInstance *currentSound = SoundChannels[Channel].firstSound; 

	if (currentSound != NULL)
	{
		while (currentSound->nextSound != NULL)
		{
			currentSound = currentSound->nextSound;
			count++;
		}
		count++;

		currentSound->nextSound = newSound;
		newSound->prevSound = currentSound;
	}
	else
	{
		SoundChannels[Channel].firstSound = newSound;
		newSound->prevSound = NULL;
	}

	SoundChannels[Channel].soundCount = count;
	newSound->nextSound = NULL;

	return newSound;
}


int initialiseAudio(void)
{
	MIX_Init();
	audioMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

	if (audioMixer == NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to initialise the audio device! \nEnsure all audio drivers are compatible/up to date.", NULL);
		return LEMON_ERROR;
	}

	propertiesContainer = SDL_CreateProperties();
	EngineSettings.MaxSoundsPerChannel = MAX_SOUNDS_PER_CHANNEL;

	for (int i = 0; i < CHANNEL_COUNT; i++) 
	{
        SoundChannels[i].firstSound = NULL;
		SoundChannels[i].soundCount = 0;
		SoundChannels[i].Pause = 0;
		SoundChannels[i].channelVolume = 1.0;
		SoundChannels[i].fadeVal = 0.0;

		for (int k = 0; k < EngineSettings.MaxSoundsPerChannel; k++)
		{
			createEmptySoundInstance(i);
		}
    }

    storedSounds.head = 0;
    CachedSound *list = storedSounds.list;
    for (int i = 0; i < MAX_CACHED_SOUNDS; i++)
    {
    	list[i].data = NULL;
    	memset(list[i].name, 0, MAX_LEN);
    }

    if (ASYNC_AUDIO_LOADER)
	{
	    scheduleLock = SDL_CreateMutex();
	    threadLock = SDL_CreateMutex();
	    closeAllThreads = false;
	}

	return LEMON_SUCCESS;
}

int cleanUpAudioData(void)
{
	if (ASYNC_AUDIO_LOADER)
	{
		closeAllThreads = true;

		// loop until thread is closed
		while (!SDL_TryLockMutex(threadLock)) { }
		
		SDL_DestroyMutex(scheduleLock);
		scheduleLock = NULL;
		SDL_DestroyMutex(threadLock);
		threadLock = NULL;
	}

	for (int i = 0; i < CHANNEL_COUNT; i++) 
	{
		while (SoundChannels[i].firstSound != NULL)
		{
			deleteSoundInstance(SoundChannels[i].firstSound, i);
		}
    }

    storedSounds.head = 0;
    CachedSound *list = storedSounds.list;
    for (int i = 0; i < MAX_CACHED_SOUNDS; i++)
    {
    	if (list[i].data != NULL)
    	{
    		MIX_DestroyAudio(list[i].data);
    		list[i].data = NULL;
    	}

    	memset(list[i].name, 0, MAX_LEN);
    }

    SDL_DestroyProperties(propertiesContainer);
    propertiesContainer = 0;
   	MIX_DestroyMixer(audioMixer);
    MIX_Quit();

	return LEMON_SUCCESS;
}


int deleteSoundInstance(SoundInstance *inputSound, ChannelName channel)	// somewhat unsafe as it assumes the channel value is correct
{
	if (inputSound == NULL || channel >= CHANNEL_COUNT || channel < 0)
	{
		return INVALID_DATA;
	}

	SoundInstance *previousSound = inputSound->prevSound;
	SoundInstance *nextSound = inputSound->nextSound;


	if (nextSound != NULL)
	{
		nextSound->prevSound = previousSound;
	}

	if (previousSound != NULL)
	{
		previousSound->nextSound = nextSound;
	}
	else
	{
		SoundChannels[channel].firstSound = nextSound;
	}
	
	if (SoundChannels[channel].firstSound == NULL)
	{
		SoundChannels[channel].soundCount = 0;
	}
	else
	{
		SoundChannels[channel].soundCount--;
	}


	inputSound->nextSound = NULL;
	inputSound->prevSound = NULL;

	if (inputSound->audio != NULL)
	{
		MIX_DestroyTrack(inputSound->audio);
	}

	free(inputSound);

	return LEMON_SUCCESS;
}



int DisplaySoundChannelDebugInfo(ChannelName channel)
{
	if (channel < 0 || channel >= CHANNEL_COUNT)
	{
		return INVALID_DATA;
	}

	SoundInstance *currentSound = SoundChannels[channel].firstSound;
	int i = 0;
	char buffer[DEBUG_TEXT_MAX_LENGTH] = {0};

	snprintf(buffer, DEBUG_TEXT_MAX_LENGTH, "Channel ID: %d  Channel vol: %.2f  \nSound Count: %d/%d", 
		channel, SoundChannels[channel].channelVolume, SoundChannels[channel].soundCount, EngineSettings.MaxSoundsPerChannel);
	AddDebugText(buffer, (ScreenData.screenWidth >> 1) - 330, i, 0, DTFORMAT_LIST_SOUND);

	i += 2;

	while (currentSound != NULL && i < EngineSettings.MaxSoundsPerChannel + 2)
	{
		if (currentSound->state != SOUND_INACTIVE)
		{
			snprintf(buffer, DEBUG_TEXT_MAX_LENGTH, "%s  Volume: %.2f", currentSound->name, currentSound->volume);
		}
		else
		{
			snprintf(buffer, DEBUG_TEXT_MAX_LENGTH, "inactive sound slot");
		}

		AddDebugText(buffer, (ScreenData.screenWidth >> 1) - (12 * strlen(buffer)), i, 0, DTFORMAT_LIST_SOUND);

		currentSound = currentSound->nextSound;
		i++;
	}

	return LEMON_SUCCESS;
}