#pragma once

namespace Sound
{

const int MAX_WAV_BUFFERS = 32;
typedef struct WavFile
{
    short format;
    short channels;
    int sample_rate;
    int byte_rate;
    short bits_per_sample;
    float duration;
    int size;
    bool in_use;
} WavData;

// returns buffer id.  make sure to free *buffer after binding to an ALbuffer
int load_wav_file(char *fn, unsigned char** buffer);

void release_wav_data(int buffer_id);
WavData* get_loaded_wav_data(int buffer_id);

void init_wav_buffers();
void teardown_wav_buffers();


}
