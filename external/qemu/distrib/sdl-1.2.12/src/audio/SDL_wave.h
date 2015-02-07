
#include "SDL_config.h"

/* WAVE files are little-endian */

/*******************************************/
/* Define values for Microsoft WAVE format */
/*******************************************/
#define RIFF		0x46464952		/* "RIFF" */
#define WAVE		0x45564157		/* "WAVE" */
#define FACT		0x74636166		/* "fact" */
#define LIST		0x5453494c		/* "LIST" */
#define FMT		0x20746D66		/* "fmt " */
#define DATA		0x61746164		/* "data" */
#define PCM_CODE	0x0001
#define MS_ADPCM_CODE	0x0002
#define IMA_ADPCM_CODE	0x0011
#define MP3_CODE	0x0055
#define WAVE_MONO	1
#define WAVE_STEREO	2

/* Normally, these three chunks come consecutively in a WAVE file */
typedef struct WaveFMT {
	Uint16	encoding;	
	Uint16	channels;		/* 1 = mono, 2 = stereo */
	Uint32	frequency;		/* One of 11025, 22050, or 44100 Hz */
	Uint32	byterate;		/* Average bytes per second */
	Uint16	blockalign;		/* Bytes per sample block */
	Uint16	bitspersample;		/* One of 8, 12, 16, or 4 for ADPCM */
} WaveFMT;

/* The general chunk found in the WAVE file */
typedef struct Chunk {
	Uint32 magic;
	Uint32 length;
	Uint8 *data;
} Chunk;

