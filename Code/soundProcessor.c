#include "soundProcessor.h"


static SDL_AudioDeviceID audio_device = 0;

static char lastPlayedSound[80] = {0};


int LemonPlaySound(char fileName[], char folderName[], int channel, double volume)
{
	if (strcmp(fileName, lastPlayedSound) == 0)
	{
		return EXECUTION_UNNECESSARY;
	}

	// Construct audio file path
	char path[200] = "LemonData/Sounds/";

	strcat(path, folderName);

	strcat(path, "/");

	strcat(path, fileName);

	strcat(path, ".wav");
	path[199] = 0;

	const char *pathPtr;
	pathPtr = path;


	// Load the Wav file
	if (channel < 0 || channel >= CHANNEL_COUNT)
	{
		printf("Invalid channel\n");
		return -1;
	}


	InitSound(pathPtr, channel, volume);

	strcpy(lastPlayedSound, fileName);

	return 0;
}


int InitSound(const char *pathPtr, int channel, double volume)
{
	SoundInstance* currentSound = SoundChannels[channel];

	SoundInstance *newSound = malloc(sizeof(SoundInstance));

	if (newSound == NULL)
	{
		printf("Failed to initialise new sound instance.\n");
		return LEMON_ERROR;
	}

	if (currentSound != NULL)
	{
		int k = 0;

		while (currentSound->nextSound != NULL)
		{
			currentSound = currentSound->nextSound;
			k++;

			if (k > MAX_SOUNDS_PER_CHANNEL)
			{
				printf("Out of space (%d sounds) for new sound instance in channel (%d)\n", k, channel);
				deleteSoundInstance(newSound);
				return ACTION_DISABLED;
			}
		}

		currentSound->nextSound = newSound;
		newSound->prevSound = currentSound;

	}
	else
	{
		SoundChannels[channel] = newSound;
		newSound->prevSound = NULL;
	}
	
	newSound->nextSound = NULL;
	newSound->channelID = channel;
	newSound->wav_data = 0;
	newSound->wav_data_len = 0;
	newSound->stream = 0;


	SDL_AudioSpec spec;

	if (!SDL_LoadWAV(pathPtr, &spec, &newSound->wav_data, &newSound->wav_data_len))
	{
		printf("Couldn't load audio file (%s)\n", SDL_GetError());
		deleteSoundInstance(newSound);
		return MISSING_DATA;
	}


	// Initialise an audio stream
	newSound->stream = SDL_CreateAudioStream(&spec, NULL);

	if (newSound->stream == NULL)
	{
		printf("Couldn't create audio stream %d (%s)\n", audio_device, SDL_GetError());
		deleteSoundInstance(newSound);
		return LEMON_ERROR;
	}


	if (SDL_BindAudioStream(audio_device, newSound->stream) == 0)
	{
		printf("Couldn't bind audio stream (%s)\n", SDL_GetError());
		deleteSoundInstance(newSound);
		return LEMON_ERROR;
	}


	Uint8 *audioBuffer = (Uint8 *)calloc((int)newSound->wav_data_len, 1);

	if (audioBuffer == NULL)
	{
		printf("Couldn't allocate audio buffer (%s)\n", SDL_GetError());
		deleteSoundInstance(newSound);
		return LEMON_ERROR;
	}

	SDL_MixAudio(audioBuffer, newSound->wav_data, spec.format, newSound->wav_data_len, volume);


	//SDL_PutAudioStreamData(newSound->stream, audioBuffer, (int)newSound->wav_data_len);
	

	SDL_free(audioBuffer);

	return 0;
}


int IterateAudio(void)
{
	lastPlayedSound[0] = 0;


	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		SoundInstance *currentSound;
		currentSound = SoundChannels[i];

		int k = 0;

		while (currentSound != NULL)
		{
			int streamResult = SDL_GetAudioStreamQueued(currentSound->stream);

			if (i == LOOP_CHANNEL && streamResult < ( (int)currentSound->wav_data_len) )
			{			
				SDL_PutAudioStreamData(currentSound->stream, currentSound->wav_data, (int)currentSound->wav_data_len);
			}

			if (i != LOOP_CHANNEL && streamResult <= 10)
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
		SoundChannels[inputSound->channelID] = nextSound;
	}
	

	inputSound->nextSound = NULL;
	inputSound->prevSound = NULL;

	SDL_ClearAudioStream(inputSound->stream);

	SDL_free(inputSound->wav_data);

	SDL_free(inputSound);

	return 0;
}


int StopAudioInChannel(int channel)
{
	SoundInstance *currentSound;
	currentSound = SoundChannels[channel];

	while (currentSound != NULL)
	{
		SDL_UnbindAudioStream(currentSound->stream);

		currentSound = currentSound->nextSound;
	}
	
	return 0;
}


int PauseAllAudio(void)
{
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		SoundInstance *currentSound;
		currentSound = SoundChannels[i];

		while (currentSound != NULL)
		{
			SDL_PauseAudioStreamDevice(currentSound->stream);

			currentSound = currentSound->nextSound;
		}
	}
	
	return 0;
}


int ResumeAllAudio(void)
{
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		SoundInstance *currentSound;
		currentSound = SoundChannels[i];

		while (currentSound != NULL)
		{
			SDL_ResumeAudioStreamDevice(currentSound->stream);

			currentSound = currentSound->nextSound;
		}
	}
	
	return 0;
}


int PauseChannel(int channel)
{
	SoundInstance *currentSound;
	currentSound = SoundChannels[channel];

	while (currentSound != NULL)
	{
		SDL_PauseAudioStreamDevice(currentSound->stream);

		currentSound = currentSound->nextSound;
	}

	return 0;
}


int ResumeChannel(int channel)
{
	SoundInstance *currentSound;
	currentSound = SoundChannels[channel];

	while (currentSound != NULL)
	{
		SDL_ResumeAudioStreamDevice(currentSound->stream);

		currentSound = currentSound->nextSound;
	}

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
        SoundChannels[i] = NULL;
    }

	return 0;
}


int CleanUpAudioData(void)
{
	// Close Audio devices
	for (int i = 0; i < CHANNEL_COUNT; i++) 
	{
		SoundInstance *currentSound;
		currentSound = SoundChannels[i];

		int k = 0;
		while (SoundChannels[i] != NULL && k < MAX_SOUNDS_PER_CHANNEL)
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