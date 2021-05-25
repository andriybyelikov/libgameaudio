# libgameaudio

## Introduction

libgameaudio is an open source, multi-platform library that provides an
implementation of a video game audio system.

It currently only implements a 2D model like the one seen in the RPG Maker.


## Usage

### Audio types

| Type                   | Play | Sources | Features |
| :--------------------- | :--- | :-----: | :------- |
| Background Music (BGM) | Loop | 1       | Intro    |
| Background Sound (BGS) | Loop | *       |          |
| Music Effect (ME)      | Once | 1       |          |
| Sound Effect (SE)      | Once | *       |          |

### Supported formats

- opus

## Dependencies

- OpenAL Soft
- libogg
- libopus
- libopusfile

## Distribution

### Windows

```
oal_soft.dll => OpenAL32.dll
libogg-0.dll
libopus-0.dll
libopusfile-0.dll
```

### Linux

```
libopenal.so.1
libogg.so.0
libopus.so.0
libopusfile.so.0
```

## Documentation

[Click here to read](https://andriybyelikov.github.io/libgameaudio/docs/html/index.html)
