#include"audio.h"

static const float PI = 3.1415926535f;
static float seconds_offset = 0.0f;

void input_to_note(char input, struct NoteData *data){
        switch(input){
                case 'z':
                        data->note = 'c';
                        data->freq = 261.626;
                        break;
                case 's':
                        data->note = 'C';
                        data->freq = 277.183;
                        break;
                case 'x':
                        data->note = 'd';
                        data->freq = 293.665;
                        break;
                case 'd':
                        data->note = 'D';
                        data->freq = 311.127;
                        break;
                case 'c':
                        data->note = 'e';
                        data->freq = 329.628;
                        break;
                case 'v':
                        data->note = 'f';
                        data->freq = 349.228;
                        break;
                case 'g':
                        data->note = 'F';
                        data->freq = 369.994;
                        break;
                case 'b':
                        data->note = 'g';
                        data->freq = 391.995;
                        break;
                case 'h':
                        data->note = 'G';
                        data->freq = 415.305;
                        break;
                case 'n':
                        data->note = 'a';
                        data->freq = 440.0;
                        break;
                case 'j':
                        data->note = 'A';
                        data->freq = 466.164;
                        break;
                case 'm':
                        data->note = 'b';
                        data->freq = 493.883;
                        break;
                case ',':
                        data->note = 'c';
                        data->freq = 523.251;
                        break;
        }
}

void* thread_function(void *arg){
        struct NoteData *data = (struct NoteData*) arg;
        char input;
        while(1){
                scanf("%c", &input);
                input_to_note(input, data);        
        }
}

static void write_callback(struct SoundIoOutStream *outstream, int frame_count_min, int frame_count_max){
        struct NoteData *data = (struct NoteData*) outstream->userdata;
        const struct SoundIoChannelLayout *layout = &outstream->layout;
        float float_sample_rate = outstream->sample_rate;
        float seconds_per_frame = 1.0f / float_sample_rate;
        struct SoundIoChannelArea *areas;
        int frames_left = frame_count_max;
        int err;
        
        while (frames_left > 0) {  
                //printf("%c", data->variable);
                int frame_count = frames_left;

                if ((err = soundio_outstream_begin_write(outstream, &areas, &frame_count))) {
                        fprintf(stderr, "%s\n", soundio_strerror(err));
                        exit(1);
                }

                if (!frame_count){
                        break;
                }

                float pitch = data->freq; 
                float radians_per_second = pitch * 2.0f * PI;

                for (int frame = 0; frame < frame_count; frame += 1) {
                        float sample = sinf((seconds_offset + frame * seconds_per_frame)*radians_per_second); 
                        for (int channel = 0; channel < layout->channel_count; channel += 1){
                                float *ptr = (float*)(areas[channel].ptr + areas[channel].step * frame);
                                *ptr = sample;
                        }
                }
                
                seconds_offset = fmodf(seconds_offset + seconds_per_frame * frame_count, 1.0f);

                if ((err = soundio_outstream_end_write(outstream))){
                        fprintf(stderr, "%s\n", soundio_strerror(err));
                        exit(1);
                }

                frames_left -= frame_count;
        }
}


int init_audio(){
        int err;

        soundio = soundio_create();

        if (!soundio){
                fprintf(stderr, "out of memory\n");
                return 1;
        }
        
        if ((err = soundio_connect(soundio))){
                fprintf(stderr, "error connecting: %s", soundio_strerror(err));
                return 1;
        }
        
        soundio_flush_events(soundio);
        
        int default_out_device_index = soundio_default_output_device_index(soundio);
        if (default_out_device_index < 0){
                fprintf(stderr, "no output device found");
                return 1;
        }
        
        device = soundio_get_output_device(soundio, default_out_device_index);
        if (!device){
                fprintf(stderr, "out of memory");
                return 1;
        }
        
        fprintf(stderr, "Output device: %s\n", device->name);
        outstream = soundio_outstream_create(device);

        struct NoteData *data = malloc(sizeof(struct NoteData));
        data->note = 'c';
        data->freq = 261.626;

        pthread_t thread;
        pthread_create(&thread, NULL, thread_function, (void*) data);

        outstream->format = SoundIoFormatFloat32NE;
        outstream->write_callback = write_callback;
        outstream->userdata = data;

        if ((err = soundio_outstream_open(outstream))){
                fprintf(stderr, "unable to open device: %s", soundio_strerror(err));
                return 1;
        }
        
        if (outstream->layout_error){
                fprintf(stderr, "unable to set channel layout: %s\n", soundio_strerror(outstream->layout_error));
        }

        if ((err = soundio_outstream_start(outstream))){
                fprintf(stderr, "unable to start device: %s", soundio_strerror(err));
                return 1;
        }

        for (;;){
                soundio_wait_events(soundio);        
        }
        
        pthread_exit(NULL);
        return 0;
}
        
void destroy_audio(){
        soundio_outstream_destroy(outstream);
        soundio_device_unref(device);
        soundio_destroy(soundio);
}
        
