#include "soundProcessor.h"


static SDL_AudioDeviceID audio_device = 0;

static char lastPlayedSound[MAX_LEN] = {0};


SoundInstance* Lemon_PlaySound(const char fileName[], const char folderName[], ChannelName channel, float volume)
{
	if (channel < 0 || channel >= CHANNEL_COUNT || fileName == NULL)
	{
		return NULL;
	}

	if (SoundChannels[channel].soundCount >= EngineSettings.MaxSoundsPerChannel || SoundChannels[channel].Pause == 1 || strcmp(fileName, lastPlayedSound) == 0)
	{
		return NULL;
	}

	if (strlen(fileName) > MAX_LEN || (folderName != NULL && strlen(folderName) > MAX_LEN) )
	{
		return NULL;
	}

	// Construct audio file path
	char path[(MAX_LEN << 1) + strlen(SOUND_ROOT) + 5];

	strcpy(path, SOUND_ROOT);

	if (folderName != NULL)
	{
		strcat(path, folderName);
		strcat(path, "/");
	}

	strcat(path, fileName);
	strcat(path, ".wav");


	// Load the Wav file
	SoundInstance *newSnd = InitSound(path, channel, volume);

	if (newSnd == NULL)
	{
		return NULL;
	}
	

	strcpy(lastPlayedSound, fileName);
	strcpy(newSnd->name, fileName);


	return newSnd;
}


SoundInstance* InitSound(const char *pathPtr, ChannelName channel, float volume)
{
	SoundInstance *newSound = createEmptySoundInstance(channel);

	if (newSound == NULL)
	{
		putConsoleString("\nFailed to initialise new sound instance.");
		return NULL;
	}
	

	SDL_AudioSpec spec;


	if (!SDL_LoadWAV(pathPtr, &spec, &newSound->wav_data, &newSound->wav_data_len))
	{
		putConsoleStrStr("\nCouldn't load audio file: ", SDL_GetError());
		deleteSoundInstance(newSound, channel);
		return NULL;
	}


	// Initialise an audio stream
	newSound->stream = SDL_CreateAudioStream(&spec, NULL);

	if (newSound->stream == NULL)
	{
		putConsoleStrIntStr("\nCouldn't create audio stream ", audio_device, ": ");
		putConsoleString(SDL_GetError());
		deleteSoundInstance(newSound, channel);
		return NULL;
	}


	if (SDL_BindAudioStream(audio_device, newSound->stream) == 0)
	{
		putConsoleStrIntStr("\nCouldn't bind audio stream ", audio_device, ": ");
		putConsoleString(SDL_GetError());
		deleteSoundInstance(newSound, channel);
		return NULL;
	}

	newSound->format = spec.format;
	newSound->volume = volume;


	SDL_SetAudioStreamGain(newSound->stream, newSound->volume * SoundChannels[channel].channelVolume);

	SDL_PutAudioStreamData(newSound->stream, newSound->wav_data, (int)newSound->wav_data_len);

	if (channel != LOOP_CHANNEL && newSound->repeatTimes < 2)
	{
		SDL_free(newSound->wav_data);
		newSound->wav_data = NULL;
	}

	return newSound;
}


int Lemon_PlaySoundSpeed(const char fileName[], const char folderName[], ChannelName channel, float volume, float speed)
{
	SetSoundSpeed(Lemon_PlaySound(fileName, folderName, channel, volume), speed);

	return LEMON_SUCCESS;
}


int Lemon_PlaySoundRepeat(const char fileName[], const char folderName[], ChannelName channel, float volume, int repeatTimes)
{
	RepeatSound(Lemon_PlaySound(fileName, folderName, channel, volume), repeatTimes);

	return LEMON_SUCCESS;
}


SoundInstance* RepeatSound(SoundInstance *input, int repeatTimes)
{
	if (input == NULL)
	{
		return NULL;
	}

	if (repeatTimes < 1 || repeatTimes > 32000)
	{
		return NULL;
	}

	input->repeatTimes = repeatTimes;

	return input;
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


int IterateAudio(void)
{
	lastPlayedSound[0] = 0;
	SoundInstance *currentSound;
	SoundInstance *updateSound;

	for (int channel = 0; channel < CHANNEL_COUNT; channel++)
	{
		currentSound = SoundChannels[channel].firstSound;
		
		int k = 0;

		while (currentSound != NULL && k < EngineSettings.MaxSoundsPerChannel)
		{
			k++;
			updateSound = currentSound;
			currentSound = currentSound->nextSound;

			int streamResult = SDL_GetAudioStreamAvailable(updateSound->stream);

			if (streamResult > 0)
			{
				continue;
			}

			updateSound->repeatTimes--;

			if (updateSound->repeatTimes < 1 && channel != LOOP_CHANNEL)
			{
				deleteSoundInstance(updateSound, channel);
			}
			else
			{
				SDL_PutAudioStreamData(updateSound->stream, updateSound->wav_data, (int)updateSound->wav_data_len);
			}
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
		SDL_SetAudioStreamGain(currentSound->stream, 0.0);

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
		SDL_SetAudioStreamGain(currentSound->stream, currentSound->volume * SoundChannels[channel].channelVolume);

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


SoundInstance* SetSoundSpeed(SoundInstance *inputSound, float newSpeed)
{
	if (inputSound == NULL)
	{
		return NULL;
	}

	newSpeed = fClamp(newSpeed, 0.1, 16.0);

	SDL_SetAudioStreamFrequencyRatio(inputSound->stream, newSpeed);

	return inputSound;
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

	int i = 0;

	while (SoundChannels[channel].firstSound != NULL && i < EngineSettings.MaxSoundsPerChannel)
	{
		deleteSoundInstance(SoundChannels[channel].firstSound, channel); 
		i++;
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
	int i = 0;

	while (currentSound != NULL && i < EngineSettings.MaxSoundsPerChannel)
	{
		SDL_PauseAudioStreamDevice(currentSound->stream);

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
		SDL_ResumeAudioStreamDevice(currentSound->stream);

		currentSound = currentSound->nextSound;
		i++;
	}

	SoundChannels[channel].Pause = 0;

	return LEMON_SUCCESS;
}


int initialiseAudio(void)
{
	audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

	if (audio_device == 0)
	{
		putConsoleString("\nCouldn't open audio device");
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
	newSound->repeatTimes = 1;

	int count = 0;

	// Place new instance into sound channel linked list
	SoundInstance *currentSound = SoundChannels[Channel].firstSound; 

	if (currentSound != NULL)
	{
		count++;

		while (currentSound->nextSound != NULL && count < EngineSettings.MaxSoundsPerChannel)
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
		while (SoundChannels[i].firstSound != NULL)
		{
			deleteSoundInstance(SoundChannels[i].firstSound,i);
		}
    }

	SDL_CloseAudioDevice(audio_device);

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
