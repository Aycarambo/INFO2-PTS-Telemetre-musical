#include "son.h"

#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)

int counter = 0;

bool montee = true;

typedef struct
{
    uint32_t total_count;
    uint32_t up_count;

    uint32_t counter;
    uint32_t prev_freq;
    uint32_t freq;
} paTestData;

//volatile int freq = 0;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int patestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    paTestData *data = (paTestData*)userData;
    uint8_t *out = (uint8_t*)outputBuffer;
    unsigned long i;
    uint32_t freq = data->freq;

    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) inputBuffer;

    int ecart = 127 + VOLUME;

    if (ecart == 0)
    {
        REVERB = 0;
    }
    else if(ecart < 120)
    {
        REVERB = (REVERB/ecart)*120;
    }

    if(montee)
    {
        if(REVERB >= 80)
        {
            if(counter >= REVERB)
            {
                montee = false;
            }
            else
            {
                counter += 2;
            }
        }
        else
        {
            if(counter >= REVERB)
            {
                montee = false;
            }
            else
            {
                counter += 1;
            }
        }
    }
    else
    {
        if(REVERB >= 80)
        {
            if(counter <= 0)
            {
                montee = true;
            }
            else
            {
                counter -= 2;
            }
        }
        else
        {
            if(counter <= 0)
            {
                montee = true;
            }
            else
            {
                counter -= 1;
            }
        }
    }

    for( i=0; i<framesPerBuffer; i++ )
    {
        if(data->up_count > 0 && data->total_count == data->up_count) {
            *out++ = 0;
            continue;
        }
        data->total_count++;

        if(freq != data->prev_freq) {
            data->counter = 0;
        }

        if(freq) {
            int overflow_max = SAMPLE_RATE / freq;
            uint32_t data_cnt = data->counter % overflow_max;
            if(int(data_cnt) > overflow_max/2)
                *out++ = -127 + counter;
            else {
                *out++ = 0 - VOLUME;
            }
            data->counter++;
        }
        else {
            data->counter = 0;
            *out++ = 0;
        }
        data->prev_freq = freq;
    }

    return paContinue;
}


static PaStream *stream;
static paTestData data;


void buzzer_set_freq(int frequency)
{
    data.up_count = 0; // do not stop!
    data.freq = frequency ;
}

void buzzer_beep(int frequency, int msecs)
{
    data.total_count = 0;
    data.up_count = SAMPLE_RATE * msecs / 1000;
    data.freq = frequency;
}

int buzzer_start(void)
{
    PaStreamParameters outputParameters;

    PaError err;

    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    outputParameters.channelCount = 1;       /* stereo output */
    outputParameters.sampleFormat = paUInt8; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
        &stream,
        NULL, /* no input */
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paClipOff,      /* we won't output out of range samples so don't bother clipping them */
        patestCallback,
        &data );
    if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;

    return err;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;

}

int buzzer_stop()
{
    PaError err = 0;
    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;

    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;

    Pa_Terminate();

    return err;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}
void msleep(int d){
    usleep(d*1000);
}


