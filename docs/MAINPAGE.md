# Documentation

@author Andriy Byelikov
@date 2021-05-25

See API here: @ref libgameaudio.h


Example code:
```
...
lgaInit(1, 2, 1, 1, 1, 2);
// set bgm loop start section in samples so that intro part is not played again
lgaSetParami(LGA_BGM_LOOP_START, 1228384);
lgaLoad(LGA_BGM, LGA_OPUS, "../data/audio/bgm/shadows.opus");
// reset parameters to not affect the rest of bgm tracks
lgaSetParami(LGA_BGM_LOOP_START, 0);

lgaLoad(LGA_BGS, LGA_OPUS, "../data/audio/bgs/rev_w.opus");
lgaLoad(LGA_BGS, LGA_OPUS, "../data/audio/bgs/rev_s.opus");
lgaLoad(LGA_ME, LGA_OPUS, "../data/audio/me/chime.opus");
lgaLoad(LGA_SE, LGA_OPUS, "../data/audio/se/fall.opus");

// set bgm to play at half volume
lgaSetParamf(LGA_GAIN, 0.5f);
lgaPlay(LGA_BGM, 0, 0);
// reset parameter
lgaSetParamf(LGA_GAIN, 1.0f);
...
main game loop {
    ...
    lgaProc();
    ...
    if car hit floor
        lgaPlay(LGA_SE, 0, 0);
    ...
}
lgaTerminate();
```

