#ifndef _track_h_
#define _track_h_

#include <AL/al.h>

typedef struct {
    ALenum format;
    ALsizei size;
    ALvoid *data;
} track_t;

track_t load_track(int format, const char *file);

#endif
