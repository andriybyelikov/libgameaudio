#ifndef _libgameaudio_h_
#define _libgameaudio_h_

/**
 * @file libgameaudio.h
 */


/** @defgroup quantifiers Quantifiers
 * @{ */

#define LGA_U_QT 0
#define LGA_E_QT 1

/** @} */


/** @defgroup audio_formats Audio formats
 * @{ */

#define LGA_OPUS 0

/** @} */


/** @defgroup audio_types Audio types
 * @{ */

#define LGA_BGM 0
#define LGA_BGS 1
#define LGA_ME  2
#define LGA_SE  3

/** @} */


/** @defgroup parameters Parameters
 * @{ */

/**
 * @brief Specifies the start of the loop section in the BGM track in samples.
 */
#define LGA_BGM_LOOP_START 0

/**
 * @brief Specifies the gain ("volume") with which a source should play.
 */
#define LGA_GAIN 1

/** @} */


/**
 * @brief Initializes libgameaudio.
 * 
 * Allocates memory for the specified number of tracks of each audio type and
 * sources for each sound audio type.
 * 
 * @param nbgmt Number of BGM tracks
 * @param nbgst Number of BGS tracks
 * @param nmet  Number of ME tracks
 * @param nset  Number of SE tracks
 * @param nbgss Number of BGS sources
 * @param nses  Number of SE sources
 */
void lgaInit(int nbgmt, int nbgst, int nmet, int nset, int nbgss, int nses);

/**
 * @brief Processes libgameaudio state.
 * 
 * This function mainly performs resource management, for example, given the
 * limited amount of sound effect sources it frees up those sources that have
 * already played their sound effect to play the next sound effect requests. It
 * also does state management, such as making sure that the intro section of a
 * BGM track is not played again.
 * 
 * @attention This function must be called periodically, preferably every
 * frame.
 */
void lgaProc(void);

/**
 * @brief Terminates libgameaudio.
 * 
 * Stops all audio playback and deallocates all tracks, sources, and buffers.
 */
void lgaTerminate(void);

/**
 * @brief Sets library parameters that modify how certain functions are
 * performed.
 * 
 * @param id    Parameter identifier. See @ref parameters
 * @param value Parameter value
 */
void lgaSetParami(int id, int value);

/**
 * @brief Sets library parameters that modify how certain functions are
 * performed.
 * 
 * @param id    Parameter identifier. See @ref parameters
 * @param value Parameter value
 */
void lgaSetParamf(int id, float value);

/**
 * @brief Loads an audio track from an audio file.
 * 
 * @param audio_type Play type of the audio file. See @ref audio_types
 * @param format     Format of the audio file. See @ref audio_formats
 * @param file       Path to audio file
 */
void lgaLoad(int audio_type, int format, const char *file);

/**
 * @brief Plays the the i-th track of an audio type.
 * 
 * Playing a BGM/BGS/ME track will stop the previously playing BGM/BGS/ME
 * track, if there was any. Requests to play a SE will be ignored if there are
 * no more available sources.
 * 
 * @param audio_type Play type of the audio file. See @ref audio_types
 * @param tid        Track identifier. A track identifier is the order in which
 *                   it was loaded with @ref lgaLoad, e.g. if a track was
 *                   loaded first its id is 0, if second 1 and so on.
 * @param sid        Source identifier. Used to keep track on which source a
 *                   BGS track is/should be played. Must be 0 in case of
 *                   BGM/ME/SE
 */
void lgaPlay(int audio_type, int tid, int sid);

/**
 * @brief Stops the playing BGM/ME track, if there is any.
 * 
 * @param qt         First-order logic quantifier
 * @param audio_type Play type of the audio file. See @ref audio_types
 * @param id         Source identifier. Must be 0 in case of BGM/ME/SE
 */
void lgaStop(int qt, int audio_type, int id);

#endif
