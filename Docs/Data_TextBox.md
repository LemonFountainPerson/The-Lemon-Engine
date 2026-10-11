# TextBox

## Definition
```
typedef struct TextBox 
{
	Object *boxPtr;

	char textPhrase[MAX_TEXT_LENGTH];
	char font[FONT_FILE_NAME_MAX];
	
	char voice[MAX_LEN];
	VoiceMode voiceMode;

	char Portrait[MAX_LEN];
	PortraitPos PortraitPosition;

	TextType textTypeSetting;
	TextTypeData textTypeData;

	TextPreset preset;

	// ...

} TextBox;
```

## Description
This is a partially opaque data structure that is used by [SceneActions](Data_SceneAction.md) to coordinate RPG-style text boxes that
can appear during cutscenes.

## Version
Available since V0.10.

----
