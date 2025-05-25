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

	// This is to stop looping sounds playing over each other, as StopAudio only works on most recently made sound
	if (channel < LOOP_CHANNELS)
	{
		StopAudioInChannel(channel);
	}

	InitSound(pathPtr, &AllSounds[channel], volume);

	strcpy(lastPlayedSound, fileName);

	return 0;
}


int InitSound(const char *pathPtr, SoundInstance *SoundChannel, double volume)
{
	if (SoundChannel == NULL)
	{
		printf("Failed to initialise Sound channel pointer.\n");
		return -1;
	}

	SDL_AudioSpec spec;

	if (!SDL_LoadWAV(pathPtr, &spec, &SoundChannel->wav_data, &SoundChannel->wav_data_len))
	{
		printf("Couldn't load audio file (%s)\n", SDL_GetError());
		return -1;
	}


	// Initialise an audio stream
	SoundChannel->stream = SDL_CreateAudioStream(&spec, NULL);

	if (SoundChannel->stream == NULL)
	{
		printf("Couldn't create audio stream %d (%s)\n", audio_device, SDL_GetError());
		return -1;
	}


	if (SDL_BindAudioStream(audio_device, SoundChannel->stream) == 0)
	{
		printf("Couldn't bind audio stream (%s)\n", SDL_GetError());
		return -1;
	}


	Uint8 *audioBuffer = (Uint8 *)calloc((int)SoundChannel->wav_data_len, 1);

	if (audioBuffer == NULL)
	{
		printf("Couldn't allocate audio buffer (%s)\n", SDL_GetError());
		return -1;
	}

	SDL_MixAudio(audioBuffer, SoundChannel->wav_data, spec.format, SoundChannel->wav_data_len, volume);


	SDL_PutAudioStreamData(SoundChannel->stream, audioBuffer, (int)SoundChannel->wav_data_len);
	
	SDL_free(audioBuffer);

	return 0;
}


int IterateAudio(void)
{
	lastPlayedSound[0] = 0;

	int i = 0;

	while (i < SDL_arraysize(AllSounds))
	{
		int streamResult = SDL_GetAudioStreamQueued(AllSounds[i].stream);

		if (i < LOOP_CHANNELS && streamResult < ( (int)AllSounds[i].wav_data_len) )
		{			
			SDL_PutAudioStreamData(AllSounds[i].stream, AllSounds[i].wav_data, (int)AllSounds[i].wav_data_len);
		}

		i++;
	}


	return 0;
}


int StopAudioInChannel(int channel)
{
	SDL_UnbindAudioStream(AllSounds[channel].stream);

	return 0;
}


int PauseAllAudio(void)
{
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		SDL_PauseAudioStreamDevice(AllSounds[i].stream);
	}
	
	return 0;
}


int ResumeAllAudio(void)
{
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		SDL_ResumeAudioStreamDevice(AllSounds[i].stream);
	}
	
	return 0;
}


int PauseChannel(int channel)
{
	SDL_PauseAudioStreamDevice(AllSounds[channel].stream);
	
	return 0;
}


int ResumeChannel(int channel)
{
	SDL_ResumeAudioStreamDevice(AllSounds[channel].stream);
	
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

	return 0;
}


int CleanUpAudioData(void)
{
	// Close Audio devices
	for (int i = 0; i < SDL_arraysize(AllSounds); i++) 
	{
        if (AllSounds[i].stream) 
		{
            SDL_DestroyAudioStream(AllSounds[i].stream);
        }

        SDL_free(AllSounds[i].wav_data);
    }

	return 0;
}