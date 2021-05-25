#include "libgameaudio.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    lgaInit(1, 0, 0, 0, 0, 0);
    lgaLoad(LGA_BGM, LGA_OPUS, argv[1]);
    lgaPlay(LGA_BGM, 0, 0);
    while (1) {
        lgaProc();
    }
    lgaTerminate();
}
