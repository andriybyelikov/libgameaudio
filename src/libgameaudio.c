#include "libgameaudio.h"
#include "track.h"

#include <stdio.h>
#include <stdlib.h>
#include <AL/al.h>
#include <AL/alc.h>

int   param_bgm_loop_start;
float param_gain;

typedef struct {
    int free;
    int size;
    track_t *table;
} track_table_t;

static track_table_t tt[4];
static ALsizei *bgm_loop_start_table;

static ALuint bgm_source, me_source;

typedef struct {
    int free;
    int size;
    ALuint *array;
} sources_array_t;

static sources_array_t bgs_sources, se_sources;

static ALuint bgm_buffer, bgm_intro_buffer;
static ALuint me_buffer;

void lgaInit(int nbgmt, int nbgst, int nmet, int nset, int nbgss, int nses)
{
    ALCdevice *device = alcOpenDevice(NULL);
    ALCcontext *context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);
    if (context == NULL) {
        fprintf(stderr, "libgameaudio: Failed to make OpenAL context current.\n");
    }

    param_bgm_loop_start = 0;
    param_gain = 1;

    int ts[4] = { nbgmt, nbgst, nmet, nset };
    for (int i = 0; i < 4; i++) {
        tt[i].free = 0;
        tt[i].size = ts[i];
        tt[i].table = malloc(ts[i] * sizeof(track_t));
    }

    bgm_loop_start_table = malloc(nbgmt * sizeof(ALsizei));

    alGenSources(1, &bgm_source);
    alGenSources(1, &me_source);

    bgs_sources.free = 0;
    bgs_sources.size = nbgss;
    bgs_sources.array = malloc(nbgss * sizeof(sources_array_t));
    alGenSources(nbgss, bgs_sources.array);
    for (int i = 0; i < nbgss; i++) {
        alSourcei(bgs_sources.array[i], AL_LOOPING, AL_TRUE);
    }

    se_sources.free = 0;
    se_sources.size = nses;
    se_sources.array = malloc(nses * sizeof(sources_array_t));
    alGenSources(nses, se_sources.array);

    bgm_buffer = AL_NONE;
    bgm_intro_buffer = AL_NONE;
    me_buffer = AL_NONE;
}

void lgaProc(void)
{
    // BGM intro done

    ALuint bproc, bqueu;
    alGetSourcei(bgm_source, AL_BUFFERS_PROCESSED, &bproc);
    alGetSourcei(bgm_source, AL_BUFFERS_QUEUED, &bqueu);
    
    if (bproc == 1) {
        int buf;
        alSourceUnqueueBuffers(bgm_source, 1, &buf);
        alDeleteBuffers(1, &buf);
        alSourcei(bgm_source, AL_LOOPING, AL_TRUE);
    }


    // ME done playing

    {
        ALint state;
        alGetSourcei(me_source, AL_SOURCE_STATE, &state);
        if (state == AL_STOPPED) {
            ALuint buf;
            alGetSourcei(me_source, AL_BUFFER, &buf);
            alDeleteBuffers(1, &buf);
        }
    }

    // SE done playing

    for (int i = 0; i < se_sources.size; i++) {
        ALint state;
        alGetSourcei(se_sources.array[(se_sources.free + i) % se_sources.size],
            AL_SOURCE_STATE, &state);
        if (state == AL_STOPPED) {
            ALuint buf;
            alGetSourcei(
                se_sources.array[(se_sources.free + i) % se_sources.size],
                AL_BUFFER, &buf);
            alDeleteBuffers(1, &buf);
            if ((se_sources.free + i) % se_sources.size == 0)
                se_sources.free = 0;
        } else {
            break;
        }
    }
}

void lgaSetParami(int id, int value)
{
    switch (id) {
    case LGA_BGM_LOOP_START:
        param_bgm_loop_start = value * 2;
        break;
    case LGA_GAIN:
        param_gain = value;
        break;
    }
}

void lgaSetParamf(int id, float value)
{
    switch (id) {
    case LGA_BGM_LOOP_START:
        param_bgm_loop_start = value * 2;
        break;
    case LGA_GAIN:
        param_gain = value;
        break;
    }
}

void lgaLoad(int audio_type, int format, const char *file)
{
    const char *at2str[4] = { "BGM", "BGS", "ME", "SE" };

    if (tt[audio_type].free == tt[audio_type].size) {
        fprintf(stderr, "libgameaudio: No more room for %s tracks!\n",
            at2str[audio_type]);
        return;
    }

    tt[audio_type].table[tt[audio_type].free] = load_track(format, file);

    if (audio_type == LGA_BGM) {
        bgm_loop_start_table[tt[audio_type].free] = param_bgm_loop_start;
    }

    tt[audio_type].free++;
}

void lgaStop(int qt, int audio_type, int id)
{
    ALuint *sources_indirection[4] = {
        &bgm_source,
        &bgs_sources.array[0],
        &me_source,
        &se_sources.array[0]
    };
    int num_sources_ind[4] = { 1, bgs_sources.size, 1, se_sources.size };

    ALuint source = sources_indirection[audio_type][id];

    int i, j;
    if (qt == LGA_U_QT) {
        i = 0;
        j = num_sources_ind[audio_type];
    } else { // qt == LGA_E_QT
        i = id;
        j = i + 1;
    }

    for (; i < j; i++) {
        ALint state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state != AL_STOPPED) {
            alSourceStop(source);
        }
    }
}

void lgaTerminate(void)
{
    // stop all sources
    for (int i = 0; i < 4; i++) {
        lgaStop(LGA_U_QT, i, 0);
    }

    // unqueue all bgm buffers if any
    /*{
        int bqueu;
        alGetSourcei(bgm_source, AL_BUFFERS_QUEUED, &bqueu);
        while (bqueu > 0) {
            ALuint buf;
            alSourceUnqueueBuffers(bgm_source, 1, &buf);
            alGetSourcei(bgm_source, AL_BUFFERS_QUEUED, &bqueu);
        }
    }*/

    // delete all buffers
    if (bgm_buffer != AL_NONE) {
        alDeleteBuffers(1, &bgm_buffer);
    }
    if (bgm_intro_buffer != AL_NONE) {
        alDeleteBuffers(1, &bgm_intro_buffer);
    }
    if (me_buffer != AL_NONE) {
        alDeleteBuffers(1, &me_buffer);
    }
    for (int i = 0; i < bgs_sources.size; i++) {
        ALuint buf;
        alGetSourcei(bgs_sources.array[i], AL_BUFFER, &buf);
        alDeleteBuffers(1, &buf);
    }
    for (int i = 0; i < se_sources.size; i++) {
        ALuint buf;
        alGetSourcei(se_sources.array[i], AL_BUFFER, &buf);
        alDeleteBuffers(1, &buf);
    }
    

    // delete all sources
    alDeleteSources(1, &bgm_source);
    alDeleteSources(bgs_sources.size, bgs_sources.array);
    alDeleteSources(1, &me_source);
    alDeleteSources(se_sources.size, se_sources.array);
    free(bgs_sources.array);
    free(se_sources.array);

    // delete all tracks
    free(bgm_loop_start_table);
    for (int i = 0; i < 4; i++) {
        free(tt[i].table);
    }
}

void lgaPlay(int audio_type, int tid, int sid)
{
    track_t track = tt[audio_type].table[tid];

    switch (audio_type) {
    case LGA_BGM:
        lgaStop(LGA_E_QT, LGA_BGM, 0);
        if (bgm_buffer != AL_NONE) {
            alDeleteBuffers(1, &bgm_buffer);
        }
        if (bgm_intro_buffer != AL_NONE) {
            alDeleteBuffers(1, &bgm_intro_buffer);
        }
        if (bgm_loop_start_table[tid] > 0) {
            alGenBuffers(1, &bgm_buffer);
            alBufferData(bgm_buffer, track.format, track.data,
                bgm_loop_start_table[tid], 48000);
            alGenBuffers(1, &bgm_intro_buffer);
            alBufferData(bgm_buffer, track.format, track.data,
                track.size - bgm_loop_start_table[tid], 48000);

            alSourcei(bgm_source, AL_LOOPING, AL_FALSE);
            alSourceQueueBuffers(bgm_source, 1, &bgm_intro_buffer);
            alSourceQueueBuffers(bgm_source, 1, &bgm_buffer);
        } else {
            alGenBuffers(1, &bgm_buffer);
            alBufferData(bgm_buffer, track.format, track.data, track.size,
                48000);

            alSourcei(bgm_source, AL_LOOPING, AL_TRUE);
            alSourcei(bgm_source, AL_BUFFER, bgm_buffer);
        }
        alSourcef(bgm_source, AL_GAIN, param_gain);
        alSourcePlay(bgm_source);
        break;
    case LGA_BGS:
        lgaStop(LGA_E_QT, LGA_BGS, sid);
        {
            ALuint buf;
            alGetSourcei(bgs_sources.array[sid], AL_BUFFER, &buf);
            if (buf != AL_NONE) {
                alDeleteBuffers(1, &buf);
            }

            alGenBuffers(1, &buf);
            alBufferData(buf, track.format, track.data, track.size, 48000);

            alSourcei(bgs_sources.array[sid], AL_BUFFER, buf);
        }
        alSourcef(bgs_sources.array[sid], AL_GAIN, param_gain);
        alSourcePlay(bgs_sources.array[sid]);
        break;
    case LGA_ME:
        lgaStop(LGA_E_QT, LGA_ME, 0);
        if (me_buffer != AL_NONE) {
            alDeleteBuffers(1, &me_buffer);
        }
        alGenBuffers(1, &me_buffer);
        alBufferData(me_buffer, track.format, track.data, track.size, 48000);

        alSourcei(me_source, AL_BUFFER, me_buffer);
        alSourcef(me_source, AL_GAIN, param_gain);
        alSourcePlay(me_source);
        break;
    case LGA_SE:
        // if no sfx sources available then skip request
        if (se_sources.free < se_sources.size) {
            ALuint buf;
            alGenBuffers(1, &buf);
            alBufferData(buf, track.format, track.data, track.size, 48000);
            alSourcei(se_sources.array[se_sources.free], AL_BUFFER, buf);
            alSourcef(se_sources.array[se_sources.free], AL_GAIN, param_gain);
            alSourcePlay(se_sources.array[se_sources.free]);
            se_sources.free++;
        }
        break;
    }
}
