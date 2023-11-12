#include "pch.h"

#ifndef SOUND_H
#define SOUND_H

/*Plays the specified audio file on the specified channel. Returns true if successful. */
__declspec(dllexport) bool PlayAudio(const char* filepath, int channel);

#endif