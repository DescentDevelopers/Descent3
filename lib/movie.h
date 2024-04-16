#ifndef MOVIE_H__
#define MOVIE_H__

#include "renderer.h"

#define MVELIB_NOERROR 0
#define MVELIB_FILE_ERROR -1
#define MVELIB_NOTINIT_ERROR -2
#define MVELIB_DDGR_ERROR -3
#define MVELIB_INIT_ERROR -4
#define MVELIB_PLAYBACK_ERROR -5
#define MVELIB_PLAYBACK_ABORTED -6

class oeApplication;

int mve_Init(const char *dir, const char *sndcard);

// simply plays a movie.
int mve_PlayMovie(const char *mvename, oeApplication *app);

// used to copy movie data to a pointer, looping will loop, fhandle will be a pointer to a file handle to be returned
// handle to movie sequence is returned by function.
intptr_t mve_SequenceStart(const char *mvename, int *fhandle, oeApplication *app, bool looping = false);

//	frames movies started with SequenceStart.  Optionally, pass a pointer to a variable which will receive
//	bitmap handle containing data, nx and ny will contain adjusted x and y coordinates if needed.
//	passing -1 to x and y centers the frame on that axis
//	handle is the movie handle returned by mve_Sequence start
//	returned value is the handle passed into mve_SequenceFrame or new one if movie looped.
intptr_t mve_SequenceFrame(intptr_t handle, int fhandle, bool sequence, int *bm_handle);

bool mve_SequenceClose(intptr_t handle, int fhandle);

//	sets render frame boundaries.
void mve_SetRenderProperties(short x, short y, short w, short h, renderer_type type, bool hicolor);

// called every frame.
typedef void (*MovieFrameCallback_fp)(int x, int y, int movieFrameNum);
void mve_SetCallback(MovieFrameCallback_fp callBack);

// call to print out text.
void mve_Puts(short x, short y, ddgr_color col, const char *txt);

void mve_ClearRect(short x1, short y1, short x2, short y2);

#endif
