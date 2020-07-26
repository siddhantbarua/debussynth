#ifndef AUDIO_H
#define AUDIO_H

#include<stdlib.h>
#include<stdio.h>
#include<soundio/soundio.h>
#include<pthread.h>
#include<math.h>

static struct SoundIo* soundio;
static struct SoundIoDevice* device;
static struct SoundIoOutStream* outstream;

struct NoteData{
        char note;
        float freq;
};

void *thread_function(void *arg);
static void write_callback(struct SoundIoOutStream *outstream, int frame_count_min, int frame_count_max); 

int init_audio();
void destroy_audio();

#endif
