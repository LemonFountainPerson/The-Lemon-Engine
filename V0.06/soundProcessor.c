#include "soundProcessor.h"


static SDL_AudioDeviceID audio_device = 0;

static char lastPlayedSound[80] = {0};


int LemonPlaySound(const char fileName[], const char folderName[], ChannelName channel, float volume)
{
	if (strcmp(fileName, lastPlayedSound) == 0 || SoundChannels[channel].Pause == 1)
	{
		return ACTION_DISABLED;
	}

	if (strlen(fileName) > MAX_LEN || strlen(folderName) > MAX_LEN)
	{
		return INVALID_DATA;
	}

	if (channel < 0 || channel >= CHANNEL_COUNT)
	{
		return INVALID_DATA;
	}

	// Construct audio file path
	char path[(MAX_LEN << 1) + 22] = "LemonData/Sounds/";

	strcat(path, folderName);

	strcat(path, "/");

	strcat(path, fileName);

	strcat(path, ".wav");

	const char *pathPtr = path;


	// Load the Wav file
	strcpy(lastPlayedSound, fileName);

	SoundInstance *newSnd = InitSound(pathPtr, channel, volume);

	if (newSnd == NULL)
	{
		return LEMON_ERROR;
	}
	else
	{
		strcpy(newSnd->name, fileName);
	}

	return LEMON_SUCCESS;
}


SoundInstance* InitSound(const char *pathPtr, ChannelName channel, float volume)
{
	if (SoundChannels[channel].soundCount >= MAX_SOUNDS_PER_CHANNEL)
	{
		return NULL;
	}

	SoundInstance* currentSound = SoundChannels[channel].firstSound;

	SoundInstance *newSound = malloc(sizeof(SoundInstance));

	if (newSound == NULL)
	{
		printf("Failed to initialise new sound instance.\n");
		return NULL;
	}


	if (currentSound != NULL)
	{
		while (currentSound->nextSound != NULL)
		{
			currentSound = currentSound->nextSound;
		}

		currentSound->nextSound = newSound;
		newSound->prevSound = currentSound;

	}
	else
	{
		SoundChannels[channel].firstSound = newSound;
		newSound->prevSound = NULL;
	}
	
	newSound->nextSound = NULL;
	newSound->channelID = channel;
	newSound->volume = volume;
	newSound->wav_data = 0;
	newSound->wav_data_len = 0;
	newSound->stream = 0;
	memset(newSound->name, 0, MAX_LEN * sizeof(char));


	SDL_AudioSpec spec;

	if (!SDL_LoadWAV(pathPtr, &spec, &newSound->wav_data, &newSound->wav_data_len))
	{
		printf("Couldn't load audio file (%s)\n", SDL_GetError());
		deleteSoundInstance(newSound);
		return NULL;
	}


	// Initialise an audio stream
	newSound->stream = SDL_CreateAudioStream(&spec, NULL);

	if (newSound->stream == NULL)
	{
		printf("Couldn't create audio stream %d (%s)\n", audio_device, SDL_GetError());
		deleteSoundInstance(newSound);
		return NULL;
	}

	newSound->format = spec.format;


	if (SDL_BindAudioStream(audio_device, newSound->stream) == 0)
	{
		printf("Couldn't bind audio stream (%s)\n", SDL_GetError());
		deleteSoundInstance(newSound);
		return NULL;
	}


	SDL_SetAudioStreamGain(newSound->stream, newSound->volume * SoundChannels[channel].channelVolume);

	SDL_PutAudioStreamData(newSound->stream, newSound->wav_data, (int)newSound->wav_data_len);
	

	SoundChannels[channel].soundCount++;

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
	while (currentSound != NULL && i < MAX_SOUNDS_PER_CHANNEL)
	{
		if (strcmp(currentSound->name, soundName) == 0)
		{
			return currentSound;
		}

		currentSound = currentSound->nextSound;
		i++;
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
		int deleteSound = 0;

		while (currentSound != NULL)
		{
			deleteSound = 0;
			int streamResult = SDL_GetAudioStreamAvailable(currentSound->stream);

			if (i != LOOP_CHANNEL && streamResult < 1)
			{
				deleteSound = 1;
			}
			

			if (deleteSound == 0 && i == LOOP_CHANNEL && streamResult < ( (int)currentSound->wav_data_len) )
			{		
				SDL_PutAudioStreamData(currentSound->stream, currentSound->wav_data, (int)currentSound->wav_data_len);
			}

			if (deleteSound == 1)
			{
				SoundInstance *deleteSound;
				deleteSound = currentSound;
				currentSound = currentSound->nextSound;
				deleteSoundInstance(deleteSound);
			}
			else
			{
				currentSound = currentSound->nextSound;
			}
			

			k++;
		}
	}
	

	return 0;
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
	}

	if (inputSound->wav_data != NULL)
	{
		SDL_free(inputSound->wav_data);
	}

	SDL_free(inputSound);

	return 0;
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

		deleteSoundInstance(deleteSound);

		currentSound = currentSound->nextSound;
	}
	
	return 0;
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

	return 0;
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

	return 0;
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

	return 0;
}


int initialiseAudio(void)
{
	audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

	if (audio_device == 0)
	{
		printf("Couldn't open audio device\n");
		return -1;
	}

	for (int i = 0; i < CHANNEL_COUNT; i++) 
	{
        SoundChannels[i].firstSound = NULL;
		SoundChannels[i].soundCount = 0;
		SoundChannels[i].Pause = 0;
		SoundChannels[i].channelVolume = 1.0;
    }

	return 0;
}


int cleanUpAudioData(void)
{
	// Close Audio devices
	for (int i = 0; i < CHANNEL_COUNT; i++) 
	{
		SoundInstance *currentSound;
		currentSound = SoundChannels[i].firstSound;

		int k = 0;
		while (SoundChannels[i].firstSound != NULL && k < MAX_SOUNDS_PER_CHANNEL)
		{
			SoundInstance *deleteSound;
			deleteSound = currentSound;
			currentSound = currentSound->nextSound;
			deleteSoundInstance(deleteSound);
			k++;
		}

    }

	SDL_CloseAudioDevice(audio_device);

	return 0;
}