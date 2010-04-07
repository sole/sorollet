
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <cstdio>
#include <cmath>

#include "SorolletPlayer.h"
#include "data/song.h"
#include "../sys/MathUtils.h"

#define WIDTH 640
#define HEIGHT 480

bool running;
SorolletPlayer* sorolletPlayer;
float* bufferL;
float* bufferR;
int bufferSize;
static bool finished = false;

void draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glViewport(0, 0, WIDTH, HEIGHT);
    int xhalf = WIDTH / 2;
    int yhalf = HEIGHT / 2;
    glOrtho(-xhalf, xhalf, -yhalf, yhalf, 0.0f, 1.0f);

    float xpos = -xhalf;
    float xinc = WIDTH * 1.0f / bufferSize;

    glColor3f(1, 0, 0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < bufferSize; i++)
    {
        glVertex3f(xpos, bufferL[i] * yhalf + 10, 0.0f);
        xpos += xinc;
    }
    glEnd();

    glColor3f(0, 1, 0);
    glBegin(GL_LINE_STRIP);
    xpos = -xhalf;
    for (int i = 0; i < bufferSize; i++)
    {
        glVertex3f(xpos, bufferR[i] * yhalf - 10, 0.0f);
        xpos += xinc;
    }
    glEnd();
    glPopMatrix();
}

void play(void *userdata, Uint8 *stream, int len)
{
    static long position = 0;
    float value;
    int i, j = 0;
    int num_samples = len >> 1;
    Sint16 *dst_buf = (Sint16*) stream;
    
    int halfNumSamples = num_samples >> 1;

    finished = !sorolletPlayer->getBuffer(bufferL, bufferR, halfNumSamples);

    for (i = 0; i < halfNumSamples; ++i)
    {
        value = MathUtils::clipf(bufferL[i], -1.0f, 1.0f);
        dst_buf[j] = (Sint16) (32767.0f * value);
        j++;

        value = MathUtils::clipf(bufferR[i], -1.0f, 1.0f);
        dst_buf[j] = (Sint16) (32767.0f * value);
        j++;
    }
    position += num_samples;

    
}

void cleanup()
{
	SDL_PauseAudio(1);
    SDL_CloseAudio();
    SDL_Quit();

    delete sorolletPlayer;
    delete []bufferL;
    delete []bufferR;
}

int main(int argc, char** argv)
{
    SDL_AudioSpec audio_spec;
    
    audio_spec.freq = 44100;
    audio_spec.format = AUDIO_S16SYS;
    audio_spec.channels = 2;
    audio_spec.samples = 1512;
    audio_spec.callback = play;
    audio_spec.userdata = NULL;

    atexit(cleanup);

    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER);

    SDL_OpenAudio(&audio_spec, NULL);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    bufferL = new float[audio_spec.samples];
    bufferR = new float[audio_spec.samples];
    bufferSize = audio_spec.samples;

    sorolletPlayer = new SorolletPlayer(audio_spec.freq, audio_spec.samples);
    sorolletPlayer->loadSongFromArray(song);

    SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_OPENGL);

    running = true;
    double lastTicks = SDL_GetTicks();
    long frameCount = 0;
    SDL_PauseAudio(0);
    
    while (running && !finished)
    {
        // FPS
        double currTicks = SDL_GetTicks();
        double diffTicks = currTicks - lastTicks;
        double fps = 1000.0f / diffTicks;

		lastTicks = currTicks;
        char windowTitle[255];

        sprintf(windowTitle, "Sorollet Player %f | frame %ld (last %lf) FPS %lf", currTicks * 0.001f, frameCount, diffTicks * 0.001f, fps);
        SDL_WM_SetCaption(windowTitle, NULL);
        frameCount++;

        SDL_Event event;
        SDL_PollEvent(&event);
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
        {
            running = false;
            exit(0);
        }

        draw();
        SDL_GL_SwapBuffers();

    }
}
