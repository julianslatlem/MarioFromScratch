#include "pch.h"
#include "sound.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

ma_result result;
ma_engine engine1;
ma_engine engine2;
ma_engine engine3;

bool PlayAudio(const char* filepath, int channel) {

    if (channel == 0) ma_engine_uninit(&engine1);
    if (channel == 1) ma_engine_uninit(&engine2);
    if (channel == 2) ma_engine_uninit(&engine3);
    
    if (channel == 0) result = ma_engine_init(NULL, &engine1);
    if (channel == 1) result = ma_engine_init(NULL, &engine2);
    if (channel == 2) result = ma_engine_init(NULL, &engine3);
    if (result != MA_SUCCESS) {
        return false;
    }

    if (channel == 0) ma_engine_play_sound(&engine1, filepath, NULL);
    if (channel == 1) ma_engine_play_sound(&engine2, filepath, NULL);
    if (channel == 2) ma_engine_play_sound(&engine3, filepath, NULL);

    return true;
}