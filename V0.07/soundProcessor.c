#include "soundProcessor.h"


static SDL_AudioDeviceID audio_device = 0;

static char lastPlayedSound[MAX_LEN] = {0};


int LemonPlaySound(const char fileName[], const char folderName[], ChannelName channel, float volume)
{
	if (channel < 0 || channel >= CHANNEL_COUNT || fileName == NULL || folderName == NULL)
	{
		return INVALID_DATA;
	}

	if (SoundChannels[channel].soundCount >= EngineSettings.MaxSoundsPerChannel || SoundChannels[channel].Pause == 1 || strcmp(fileName, lastPlayedSound) == 0)
	{
		return ACTION_DISABLED;
	}

	if (strlen(fileName) > MAX_LEN || strlen(folderName) > MAX_LEN)
	{
		return INVALID_DATA;
	}

	// Construct audio file path
	char path[(MAX_LEN << 1) + strlen(SOUND_ROOT) + 5];

	strcpy(path, SOUND_ROOT);

	strcat(path, folderName);

	strcat(path, "/");

	strcat(path, fileName);

	strcat(path, ".wav");

	const char *pathPtr = path;


	// Load the Wav file
	SoundInstance *newSnd = InitSound(pathPtr, channel, volume);

	if (newSnd == NULL)
	{
		return FILE_NOT_FOUND;
	}
	

	strcpy(lastPlayedSound, fileName);
	strcpy(newSnd->name, fileName);


	return LEMON_SUCCESS;
}


SoundInstance* InitSound(const char *pathPtr, ChannelName channel, float volume)
{
	if (SoundChannels[channel].soundCount >= MaxSoundsPerChannel)
	{
		return NULL;
	}

	SoundInstance *newSound = createEmptySoundInstance(channel);

	if (newSound == NULL)
	{
		putDebugString("\nFailed to initialise new sound instance.");
		return NULL;
	}
	

	SDL_AudioSpec spec;


	if (!SDL_LoadWAV(pathPtr, &spec, &newSound->wav_data, &newSound->wav_data_len))
	{
		putDebugStrStr("Couldn't load audio file: ", SDL_GetError());
		deleteSoundInstance(newSound);
		return NULL;
	}


	// Initialise an audio stream
	newSound->stream = SDL_CreateAudioStream(&spec, NULL);

	if (newSound->stream == NULL)
	{
		putDebugStrIntStr("Couldn't create audio stream ", audio_device, ": ");
		putDebugString(SDL_GetError());
		deleteSoundInstance(newSound);
		return NULL;
	}


	if (SDL_BindAudioStream(audio_device, newSound->stream) == 0)
	{
		putDebugStrIntStr("Couldn't bind audio stream ", audio_device, ": ");
		putDebugString(SDL_GetError());
		deleteSoundInstance(newSound);
		return NULL;
	}

	newSound->format = spec.format;
	newSound->volume = volume;


	SDL_SetAudioStreamGain(newSound->stream, newSound->volume * SoundChannels[channel].channelVolume);

	SDL_PutAudioStreamData(newSound->stream, newSound->wav_data, (int)newSound->wav_data_len);


	return newSound;
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

	int i = 1;
	while (currentSound != NULL)
	{
		if (strcmp(currentSound->name, soundName) == 0)
		{
			return currentSound;
		}

		currentSound = currentSound->nextSound;
	}

	return NULL;
}


int IterateAudio(void)
{
	lastPlayedSound[0] = 0;

	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		SoundInstance *currentSound;
		currentSound = SoundChannels[i].firstSound;
		
		int k = 0;

		while (currentSound != NULL && k < EngineSettings.MaxSoundsPerChannel)
		{
			k++;

			int streamResult = SDL_GetAudioStreamAvailable(currentSound->stream);

			if (i != LOOP_CHANNEL && streamResult < 1)
			{
				SoundInstance *deleteSound = currentSound;
				currentSound = currentSound->nextSound;
				deleteSoundInstance(deleteSound);
				continue;
			}
			

			if (i == LOOP_CHANNEL && streamResult < ( (int)currentSound->wav_data_len) )
			{		
				SDL_PutAudioStreamData(currentSound->stream, currentSound->wav_data, (int)currentSound->wav_data_len);
			}

			currentSound = currentSound->nextSound;
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

	while (currentSound != NULL)
	{
		SDL_SetAudioStreamGain(currentSound->stream, 0.0);

		currentSound = currentSound->nextSound;
	}
	
	return LEMON_SUCCESS;
}


int UnmuteChannel(ChannelName channel)
{
	if (channel >= CHANNEL_COUNT || channel < 0)
	{
		return INVALID_DATA;
	}

	SoundInstance *currentSound;
	currentSound = SoundChannels[channel].firstSound;

	while (currentSound != NULL)
	{
		SDL_SetAudioStreamGain(currentSound->stream, currentSound->volume * SoundChannels[channel].channelVolume);

		currentSound = currentSound->nextSound;
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

	UnmuteChannel(channel);
	
	return LEMON_SUCCESS;
}


int MuteAllAudio(void)
{
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		SoundInstance *currentSound;
		currentSound = SoundChannels[i].firstSound;

		while (currentSound != NULL)
		{
			SDL_SetAudioStreamGain(currentSound->stream, 0.0);


			currentSound = currentSound->nextSound;
		}
	}

	return LEMON_SUCCESS;
}


int UnmuteAllAudio(void)
{
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		SoundInstance *currentSound;
		currentSound = SoundChannels[i].firstSound;

		while (currentSound != NULL)
		{
			SDL_SetAudioStreamGain(currentSound->stream, currentSound->volume * SoundChannels[i].channelVolume);

			currentSound = currentSound->nextSound;
		}
	}

	return LEMON_SUCCESS;
}


int StopAudioInChannel(ChannelName channel)
{
	if (channel >= CHANNEL_COUNT || channel < 0)
	{
		return INVALID_DATA;
	}

	SoundInstance *currentSound;
	SoundInstance *deleteSound;
	currentSound = SoundChannels[channel].firstSound;

	while (currentSound != NULL)
	{
		deleteSound = currentSound;

		currentSound = currentSound->nextSound;

		deleteSoundInstance(deleteSound);
	}
	
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

	while (currentSound != NULL)
	{
		SDL_PauseAudioStreamDevice(currentSound->stream);

		currentSound = currentSound->nextSound;
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

	while (currentSound != NULL)
	{
		SDL_ResumeAudioStreamDevice(currentSound->stream);

		currentSound = currentSound->nextSound;
	}

	SoundChannels[channel].Pause = 0;

	return LEMON_SUCCESS;
}


int initialiseAudio(void)
{
	audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

	if (audio_device == 0)
	{
		putDebugString("\nCouldn't open audio device");
		return LEMON_ERROR;
	}

	for (int i = 0; i < CHANNEL_COUNT; i++) 
	{
        SoundChannels[i].firstSound = NULL;
		SoundChannels[i].soundCount = 0;
		SoundChannels[i].Pause = 0;
		SoundChannels[i].channelVolume = 1.0;
    }

	return LEMON_SUCCESS;
}


SoundInstance* createEmptySoundInstance(ChannelName Channel)
{
	SoundInstance *newSound = malloc(sizeof(SoundInstance));

	if (newSound == NULL)
	{
		return NULL;
	}

	newSound->nextSound = NULL;
	newSound->volume = 1.0;
	newSound->wav_data = NULL;
	newSound->wav_data_len = 0;
	newSound->stream = NULL;
	newSound->format = 0;
	memset(newSound->name, 0, MAX_LEN * sizeof(char));

	newSound->channelID = Channel;

	int count = 0;

	// Place new instance into sound channel linked list
	SoundInstance *currentSound = SoundChannels[Channel].firstSound; 

	if (currentSound != NULL)
	{
		count++;

		while (currentSound->nextSound != NULL)
		{
			currentSound = currentSound->nextSound;
			count++;
		}

		currentSound->nextSound = newSound;
		newSound->prevSound = currentSound;
	}
	else
	{
		SoundChannels[Channel].firstSound = newSound;
		newSound->prevSound = NULL;
	}

	count++;

	SoundChannels[Channel].soundCount = count;

	return newSound;
}


int cleanUpAudioData(void)
{
	// Close Audio devices
	for (int i = 0; i < CHANNEL_COUNT; i++) 
	{
		SoundInstance *currentSound;
		currentSound = SoundChannels[i].firstSound;

		int k = 0;
		while (SoundChannels[i].firstSound != NULL)
		{
			SoundInstance *deleteSound;
			deleteSound = currentSound;
			currentSound = currentSound->nextSound;
			deleteSoundInstance(deleteSound);
		}

    }

	SDL_CloseAudioDevice(audio_device);

	return LEMON_SUCCESS;
}


int deleteSoundInstance(SoundInstance *inputSound)
{
	if (inputSound == NULL)
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
		SoundChannels[inputSound->channelID].firstSound = nextSound;
	}
	
	if (SoundChannels[inputSound->channelID].firstSound == NULL)
	{
		SoundChannels[inputSound->channelID].soundCount = 0;
	}
	else
	{
		SoundChannels[inputSound->channelID].soundCount--;
	}


	inputSound->nextSound = NULL;
	inputSound->prevSound = NULL;

	if (inputSound->stream != NULL)
	{
		SDL_DestroyAudioStream(inputSound->stream);
		inputSound->stream = NULL;
	}

	if (inputSound->wav_data != NULL)
	{
		SDL_free(inputSound->wav_data);
		inputSound->wav_data = NULL;
	}

	free(inputSound);

	return LEMON_SUCCESS;
}