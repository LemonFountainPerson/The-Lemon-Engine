#include "LemonEngine.h"

#define LOOP_SOUND -1
#define AUDIO_DISTANCE_SCALE 1000.0
#define AUDIO_FALLOFF_RATE 6.0


static MIX_Mixer *audioMixer = NULL;

static char lastPlayedSound[MAX_LEN] = {0};

static SDL_PropertiesID propertiesContainer = 0;

const char channelNames[][CHANNEL_NAME_LENGTH] = {"Music", "Speech", "PlayerSfx", "ObjectSfx"};



SoundInstance* PlaySound(const char fileName[], const char folderName[], ChannelName channel, float volume)
{
	if (channel < 0 || channel >= CHANNEL_COUNT || fileName == NULL)
	{
		return NULL;
	}

	if (SoundChannels[channel].Pause == 1 || strcmp(fileName, lastPlayedSound) == 0)
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

	// Load the file
	SoundInstance *newSnd = InitSound(path, channel, volume);

	if (newSnd == NULL)
	{
		strcat(path, DEFAULT_SOUND_EXTENSION);
		newSnd = InitSound(path, channel, volume);

		if (newSnd == NULL)
		{
			goto FailedToLoadSound;
		}
	}

	char tag[CHANNEL_NAME_LENGTH] = {0};
	snprintf(tag, CHANNEL_NAME_LENGTH, "%s", channelNames[channel]);
	MIX_TagTrack(newSnd->audio, tag);
	
	strcpy(lastPlayedSound, fileName);
	strcpy(newSnd->name, fileName);


	FailedToLoadSound:

	return newSnd;
}


SoundInstance* InitSound(const char *pathPtr, ChannelName channel, float volume)
{
	if (access(pathPtr, F_OK | R_OK) == -1)
	{
		return NULL;
	}

	SoundInstance *newSound = createEmptySoundInstance(channel);
	
	if (newSound == NULL)
	{
		putConsoleString("\nFailed to initialise new sound instance.");
		return NULL;
	}
	

	MIX_Audio *loadedAudio = MIX_LoadAudio(audioMixer, pathPtr, true);

	if (loadedAudio == NULL)
	{
		putConsoleStrStr("\nCouldn't load audio file: ", SDL_GetError());
		goto LoadAudio_Error;
	}

	newSound->audio = MIX_CreateTrack(audioMixer);

	if (!MIX_SetTrackAudio(newSound->audio, loadedAudio))
	{
		putConsoleStrStr("\nCouldn't set audio file: ", SDL_GetError());
		goto LoadAudio_Error;
	}

	MIX_DestroyAudio(loadedAudio);

	MIX_SetTrackGain(newSound->audio, volume * SoundChannels[channel].channelVolume);


	MIX_PlayTrack(newSound->audio, propertiesContainer);

	return newSound;


	LoadAudio_Error:

	deleteSoundInstance(newSound, channel);
	return NULL;
}

SoundInstance* PlaySoundFadeIn(const char fileName[], const char folderName[], ChannelName channel, float secondsToFade)
{
	Sint64 milis = (Sint64)(1000.0 * secondsToFade);
	SDL_SetNumberProperty(propertiesContainer, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, milis);
	SoundInstance *sound = PlaySound(fileName, folderName, channel, 1.0);
	SDL_SetNumberProperty(propertiesContainer, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, 0);

	return sound;
}


/*
SoundInstance* InitSound_SDL(const char *pathPtr, ChannelName channel, float volume)
{
	if (access(pathPtr, F_OK | R_OK) == -1)
	{
		return NULL;
	}

	SoundInstance *newSound = createEmptySoundInstance(channel);
	
	if (newSound == NULL)
	{
		putConsoleString("\nFailed to initialise new sound instance.");
		return NULL;
	}
	

	SDL_AudioSpec spec;
	Uint8 *wav_data = NULL;

	if (!SDL_LoadWAV(pathPtr, &spec, &wav_data, &newSound->wav_data_len))
	{
		putConsoleStrStr("\nCouldn't load audio file: ", SDL_GetError());
		deleteSoundInstance(newSound, channel);
		return NULL;
	}


	// Initialise an audio stream
	newSound->stream = SDL_CreateAudioStream(&spec, NULL);

	if (newSound->stream == NULL)
	{
		putConsoleStrIntStr("\nCouldn't create audio stream ", audioDevice, ": ");
		putConsoleString(SDL_GetError());
		deleteSoundInstance(newSound, channel);
		return NULL;
	}


	if (SDL_BindAudioStream(audioDevice, newSound->stream) == 0)
	{
		putConsoleStrIntStr("\nCouldn't bind audio stream ", audioDevice, ": ");
		putConsoleString(SDL_GetError());
		deleteSoundInstance(newSound, channel);
		return NULL;
	}

	newSound->volume = volume;


	SDL_SetAudioStreamGain(newSound->stream, newSound->volume * SoundChannels[channel].channelVolume);

	SDL_PutAudioStreamData(newSound->stream, wav_data, (int)newSound->wav_data_len);

	SDL_free(wav_data);

	return newSound;
}
*/

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

	// -1 indicates to loop forever
	if (repeatTimes < -1 || repeatTimes > 32000)
	{
		return NULL;
	}

	MIX_SetTrackLoops(input->audio, repeatTimes);

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


int IterateAudio(Camera positionCam)
{
	lastPlayedSound[0] = 0;
	SoundInstance *currentSound;
	SoundInstance *updateSound;

	positionCam.CameraX /= AUDIO_DISTANCE_SCALE;
	positionCam.CameraY /= AUDIO_DISTANCE_SCALE;

	for (int channel = 0; channel < CHANNEL_COUNT; channel++)
	{
		currentSound = SoundChannels[channel].firstSound;
		
		int k = 0;

		if (SoundChannels[channel].Pause == 1)
		{
			continue;
		}

		while (currentSound != NULL && k < EngineSettings.MaxSoundsPerChannel)
		{
			k++;
			updateSound = currentSound;
			currentSound = currentSound->nextSound;
	
			if (!MIX_TrackPlaying(updateSound->audio) && MIX_GetTrackLoops(updateSound->audio) == 0)
			{
				deleteSoundInstance(updateSound, channel);
			}
			else if (updateSound->positional)
			{
				updateSoundPosition(updateSound, positionCam);
			}
		}
	}

	return LEMON_SUCCESS;
}

/*
int IterateAudio_SDL(Camera positionCam)
{
	lastPlayedSound[0] = 0;
	SoundInstance *currentSound;
	SoundInstance *updateSound;

	for (int channel = 0; channel < CHANNEL_COUNT; channel++)
	{
		currentSound = SoundChannels[channel].firstSound;
		
		int k = 0;

		if (SoundChannels[channel].Pause == 1)
		{
			continue;
		}

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

			deleteSoundInstance(updateSound, channel);
		}
	}
	

	return LEMON_SUCCESS;
}
*/

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

	if (SoundChannels[Channel].soundCount >= EngineSettings.MaxSoundsPerChannel && newSound != NULL)
	{
		MIX_DestroyTrack(newSound->audio);

		return newSound;
	}

	newSound = malloc(sizeof(SoundInstance));

	if (newSound == NULL)
	{
		return NULL;
	}

	newSound->nextSound = NULL;
	newSound->volume = 1.0;
	newSound->audio = NULL;
	memset(newSound->name, 0, MAX_LEN * sizeof(char));

	newSound->channel = Channel;
	newSound->positional = false;
	newSound->panLevels.left = 1.0;
	newSound->panLevels.right = 1.0;
	newSound->xPos = 0.0;
	newSound->yPos = 0.0;

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

	for (int i = 0; i < CHANNEL_COUNT; i++) 
	{
        SoundChannels[i].firstSound = NULL;
		SoundChannels[i].soundCount = 0;
		SoundChannels[i].Pause = 0;
		SoundChannels[i].channelVolume = 1.0;
		SoundChannels[i].fadeVal = 0.0;
    }

	return LEMON_SUCCESS;
}

int cleanUpAudioData(void)
{
	for (int i = 0; i < CHANNEL_COUNT; i++) 
	{
		while (SoundChannels[i].firstSound != NULL)
		{
			deleteSoundInstance(SoundChannels[i].firstSound, i);
		}
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

	MIX_DestroyTrack(inputSound->audio);

	free(inputSound);

	return LEMON_SUCCESS;
}
