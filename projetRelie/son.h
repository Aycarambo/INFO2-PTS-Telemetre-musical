#ifndef SON_H
#define SON_H

#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include <stdint.h>
#include <unistd.h> // for usleep()
#include "variables.h"

static int patestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData );

void buzzer_set_freq(int frequency);

void buzzer_beep(int frequency, int msecs);

int buzzer_start(void);

int buzzer_stop();

void msleep(int d);


#endif // SON_H
