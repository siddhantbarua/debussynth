#include<soundio/soundio.h>
#include"term.h" 
#include<math.h> 
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

struct UserData {
        char variable;
};


void* thread_function(void *arg) {
        struct UserData *data = (struct UserData*) arg;
        while(1){
                scanf("%c", &data->variable);
        }
}

static const float PI = 3.1415926535f;
static float seconds_offset = 0.0f;

static void write_callback(struct SoundIoOutStream *outstream, int frame_count_min, int frame_count_max) {
        struct UserData *data = (struct UserData*) outstream->userdata;
        const struct SoundIoChannelLayout *layout = &outstream->layout;
        float float_sample_rate = outstream->sample_rate;
        float seconds_per_frame = 1.0f / float_sample_rate;
        struct SoundIoChannelArea *areas;
        int frames_left = frame_count_max;
        int err;
        
        
        while (frames_left > 0) {  
                //printf("%c", data->variable);
                
                int frame_count = frames_left;
                soundio_outstream_begin_write(outstream, &areas, &frame_count);


                float pitch = (float) data->variable * 5;
                float radians_per_second = pitch * 2.0f * PI;
                for (int frame = 0; frame < frame_count; frame += 1) {
                        //float sample = sinf((seconds_offset + frame * seconds_per_frame) * radians_per_second);
                        float sample = 0.5*sin((seconds_offset + frame * seconds_per_frame) * radians_per_second) > 0 ? 1.0 : -1.0 + 0*sinf((seconds_offset + frame * seconds_per_frame) * radians_per_second) + (2.0 / PI) * (pitch * PI * fmod(seconds_offset + frame*seconds_per_frame, 1.0 / pitch) - (PI / 2.0));
                        for (int channel = 0; channel < layout->channel_count; channel += 1) {
                                float *ptr = (float*)(areas[channel].ptr + areas[channel].step * frame);
                                *ptr = sample;
                        }
                }
                
                seconds_offset = fmodf(seconds_offset + seconds_per_frame * frame_count, 1.0f);

                soundio_outstream_end_write(outstream);
                frames_left -= frame_count;
        }
        //fflush(stdout);
}


int main(){
        enableRawMode();
        
        struct SoundIo *soundio = soundio_create();
        soundio_connect(soundio);

        soundio_flush_events(soundio);

        int default_out_device_index = soundio_default_output_device_index(soundio);

        struct SoundIoDevice *device = soundio_get_output_device(soundio, default_out_device_index);

        printf("DEBUSSYNTH\n");
        struct SoundIoOutStream *outstream = soundio_outstream_create(device);

        struct UserData *data = malloc(sizeof(struct UserData));
        data->variable = 'c';
        
        pthread_t tid;
        pthread_create(&tid, NULL, thread_function, (void*) data);
        
        outstream->format = SoundIoFormatFloat32NE;
        outstream->write_callback = write_callback;
        outstream->userdata = data;

        soundio_outstream_open(outstream);
        soundio_outstream_start(outstream);

        for (;;) {
                soundio_wait_events(soundio);
        }

        soundio_outstream_destroy(outstream);
        soundio_device_unref(device);
        soundio_destroy(soundio);
        
        pthread_exit(NULL);

        return 0;
}

