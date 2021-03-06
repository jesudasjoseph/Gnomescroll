/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "wav.hpp"

#if DC_SERVER
# error Do not include this file in the server
#endif

#include <string.h>

/*
 *  References:
 *  http://enigma-dev.org/forums/index.php?topic=730.0
 *  http://home.roadrunner.com/~jgglatt/tech/wave.htm
 *  https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
 */


namespace Sound
{

typedef union WavDataElement
{
    char buffer[5];
    int16_t two_bytes;
    int32_t four_bytes;
} WavDataElement;

static WavData* wav_buffers = NULL;
static const char base_path[] = MEDIA_PATH "sound/wav/";

void print_wav_data(WavData* data)
{
    printf(
        "WavData:\n"
        "format: %hd\n"
        "channels: %hd\n"
        "sample rate: %d\n"
        "byte rate: %d\n"
        "bits per sample: %hd\n"
        "duration (seconds): %0.2f\n"
        "size (bytes):  %lu\n"
        "in use: %d\n",
        data->format, data->channels, data->sample_rate, data->byte_rate,
        data->bits_per_sample, data->duration, (long unsigned)data->size,
        data->in_use);
}

ALenum get_openal_wav_format(WavData* data)
{
    if (data->channels == 1)
    {
        if (data->bits_per_sample == 8)
            return AL_FORMAT_MONO8;
        else
            return AL_FORMAT_MONO16;
    }
    else
    {
        if (data->bits_per_sample == 8)
            return AL_FORMAT_STEREO8;
        else
            return AL_FORMAT_STEREO16;
    }

}

int get_free_wav_data(WavData** data)
{
    *data = NULL;
    int i;
    for (i=0; i<MAX_WAV_BUFFERS; i++)
        if (!wav_buffers[i].in_use)
        {
            *data = &wav_buffers[i];
            break;
        }
    GS_ASSERT(i < MAX_WAV_BUFFERS);
    return i;
}

bool is_valid_wav(FILE* f)
{
    WavDataElement metadata;
    memset(metadata.buffer, 0, sizeof(char)*5);

    if (fread(metadata.buffer, sizeof(char), 4, f) != 4 || strcmp(metadata.buffer, "RIFF") != 0)
        return false;

    if (fread(metadata.buffer, sizeof(char), 4, f) != 4)    // pcm chunk size, we dont care (openal does)
        return false;

    if (fread(metadata.buffer, sizeof(char), 4, f) != 4 || strcmp(metadata.buffer, "WAVE") != 0)
        return false;

    return true;
}

int skip_extra(FILE* f, char* buffer, size_t buffer_size, int skip_amount)
{
    int skipped = 0;
    int read_amt = 0;
    int remaining = 0;
    size_t read = 0;
    while (skipped < skip_amount)
    {
        remaining = skip_amount - skipped;
        read_amt = (remaining < int(buffer_size)) ? remaining : buffer_size;
        GS_ASSERT(remaining > 0);
        read = fread(buffer, sizeof(char), read_amt, f);
        skipped += read;
        if (read != buffer_size) break;
    }
    return (skip_amount - skipped);
}

bool read_wav_fmt_subchunk(FILE* f, WavData* data)
{
    WavDataElement metadata;
    memset(metadata.buffer, 0, sizeof(char)*5);
    size_t read = 0;
    int total_read = 0;

    read = fread(metadata.buffer, sizeof(char), 4, f);
    if (read != 4)
        return false;
    if (strcmp(metadata.buffer, "fmt ") != 0)
        return false;

    read = fread(metadata.buffer, sizeof(char), 4, f);
    if (read != 4)
        return false;
    int chunk_size = metadata.four_bytes;

    read = fread(metadata.buffer, sizeof(char), 2, f);
    if (read != 2)
        return false;
    total_read += read;
    data->format = metadata.two_bytes;

    read = fread(metadata.buffer, sizeof(char), 2, f);
    if (read != 2)
        return false;
    total_read += read;
    data->channels = metadata.two_bytes;

    read = fread(metadata.buffer, sizeof(char), 4, f);
    if (read != 4)
        return false;
    total_read += read;
    data->sample_rate = metadata.four_bytes;

    read = fread(metadata.buffer, sizeof(char), 4, f);
    if (read != 4)
        return false;
    total_read += read;
    data->byte_rate = metadata.four_bytes;

    read = fread(metadata.buffer, sizeof(char), 2, f);
    if (read != 2)
        return false;
    total_read += read;
    // blockAlignment, dont care

    read = fread(metadata.buffer, sizeof(char), 2, f);
    if (read != 2)
        return false;
    total_read += read;
    data->bits_per_sample = metadata.two_bytes;

    int extra = chunk_size - total_read;
    int remaining = skip_extra(f, metadata.buffer, 5, extra);
    if (remaining)
        return false;

    return true;
}

static void print_read_error(FILE* f)
{
    if (feof(f))
        printf("Reached the end of the file\n");
    if (ferror(f))
        printf("File had error: %s\n", strerror(errno));
}

bool read_wav_data(FILE* f, WavData* data, unsigned char** buffer)
{
    *buffer = NULL;
    data->size = 0;
    data->duration = 0;

    WavDataElement metadata;
    memset(metadata.buffer, 0, sizeof(metadata.buffer));
    size_t read;

    // read chunk labels until we find the "data" chunk
    // skip any chunks that are not "data"
    while (1)
    {   // read subchunk label
        clearerr(f);

        read = fread(metadata.buffer, sizeof(char), 4, f);
        if (read != 4)
        {
            printf("Not enough data for the subchunk label\n");
            print_read_error(f);
            return false;
        }

        //printf("subchunk label= %s\n", metadata.buffer);

        metadata.buffer[4] = '\0';
        if (strcmp(metadata.buffer, "data") == 0)
            break;

        // read subchunk size
        read = fread(metadata.buffer, sizeof(char), 4, f);
        if (read != 4)
        {
            printf("Not enough data for the subchunk size\n");
            return false;
        }

        int remaining = skip_extra(f, metadata.buffer, 5, metadata.four_bytes);
        if (remaining < 0)
        {
            printf("WARNING -- skipped %d bytes past wav subchunk size\n", -int(remaining));
            return false;
        }
        if (remaining != 0)
        {
            printf("Remaning subchunk data was not 0 (was %d instead)\n", remaining);
            return false;
        }
    }

    // get size of wav data
    read = fread(metadata.buffer, sizeof(char), 4, f);
    if (read != 4)
    {
        printf("Not enough data for the wav data size\n");
        return false;
    }
    data->size = metadata.four_bytes;
    data->duration = float(double(data->size) / data->byte_rate);

    if (data->size == 0)
    {
        printf("PCM data size is 0\n");
        return false;
    }

    // load pcm data to buffer
    *buffer = (unsigned char*)malloc(sizeof(unsigned char) * data->size);
    read = fread(*buffer, sizeof(char), data->size, f);
    if (read != data->size)
    {
        printf("Failed to read PCM data of size %lu\n", (long unsigned)data->size);
        return false;
    }

    return true;
}


// returns buffer id.  make sure to free *buffer after binding to an ALbuffer
int load_wav_file(const char* filename, unsigned char** buffer)
{
    *buffer = NULL;
    WavData* data = NULL;
    int id = get_free_wav_data(&data);
    if (data == NULL)
    {
        printf("No free wav data slots found.\n");
        return -1;
    }

    // open file
    char* fullpath = (char*)malloc(sizeof(char) * (strlen(base_path) + strlen(filename) + 1));
    sprintf(fullpath, "%s%s", base_path, filename);
    FILE* f = fopen(fullpath, "rb");
    free(fullpath);
    if (f == NULL)
    {
        printf("Error opening file: %s\n", fullpath);
        return -1;
    }

    // check header
    if (!is_valid_wav(f))
    {
        printf("%s is not a WAV file\n", filename);
        fclose(f);
        return -1;
    }

    // read metadata
    if (!read_wav_fmt_subchunk(f, data))
    {
        printf("%s has an invalid format chunk\n", filename);
        fclose(f);
        return -1;
    }

    // read wav data
    if (!read_wav_data(f, data, buffer))
    {
        printf("Failed to read data subchunk of %s\n", filename);
        return -1;
    }
    if (*buffer == NULL)
    {
        printf("WAV data read failure: buffer is NULL\n");
        return -1;
    }

    data->in_use = true;
    fclose(f);

    return id;
}

void release_wav_data(int buffer_id)
{
    IF_ASSERT(buffer_id < 0 || buffer_id >= MAX_WAV_BUFFERS)
        return;
    wav_buffers[buffer_id].in_use = false;
}

WavData* get_loaded_wav_data(int buffer_id)
{
    IF_ASSERT(buffer_id < 0 || buffer_id >= MAX_WAV_BUFFERS)
        return NULL;
    return &wav_buffers[buffer_id];
}

void init_wav_buffers()
{
    GS_ASSERT(wav_buffers == NULL);
    wav_buffers = (WavData*)calloc(MAX_WAV_BUFFERS, sizeof(WavData));
}

void teardown_wav_buffers()
{
    if (wav_buffers != NULL)
        for (int i=0; i<MAX_WAV_BUFFERS; i++)
            if (!wav_buffers[i].in_use)
                release_wav_data(i);
    free(wav_buffers);
    wav_buffers = NULL;
}

}   // Sound
