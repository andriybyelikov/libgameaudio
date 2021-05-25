#include "track.h"
#include "libgameaudio.h"

#include <stdlib.h>
#include <AL/al.h>
#include <opus/opusfile.h>

static
track_t load_opus(const char *file)
{
    int error = 0;
    OggOpusFile *of = op_open_file(file, &error);

    if (error) {
        fprintf(stderr, "libgameaudio: Failed to open opus file.\n");
    }

    int num_channels = op_channel_count(of, -1);
    int pcm_size = op_pcm_total(of, -1);

    ALenum format;
    if (num_channels == 1) {
        format = AL_FORMAT_MONO16;
    } else if (num_channels == 2) {
        format = AL_FORMAT_STEREO16;
    } else {
        fprintf(stderr, "libgameaudio: Unexpected number of channels in audio file.\n");
    }

    int16_t *buf = malloc(sizeof(int16_t) * pcm_size * num_channels);

    int samples_read = 0;
    while (samples_read < pcm_size) {
        int ns = op_read(of, buf + samples_read * num_channels,
            pcm_size * num_channels, 0);
        samples_read += ns;
    }

    op_free(of);

    return (track_t){ format, samples_read * num_channels * 2, buf };
}

track_t load_track(int format, const char *file)
{
    switch (format) {
    case LGA_OPUS:
        return load_opus(file);
    default:
        fprintf(stderr, "libgameaudio: Invalid audio format.\n");
    }
}
