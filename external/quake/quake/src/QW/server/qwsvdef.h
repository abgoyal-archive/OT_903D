// quakedef.h -- primary header for server

#define	QUAKE_GAME			// as opposed to utilities

//define	PARANOID			// speed sapping error checking

#ifdef _WIN32
#pragma warning( disable : 4244 4127 4201 4214 4514 4305 4115 4018)
#endif

#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <ctype.h>

#include "bothdefs.h"

#include "common.h"
#include "bspfile.h"
#include "sys.h"
#include "zone.h"
#include "mathlib.h"

#include "cvar.h"
#include "net.h"
#include "protocol.h"
#include "cmd.h"
#include "model.h"
#include "crc.h"
#include "progs.h"

#include "server.h"
#include "world.h"
#include "pmove.h"

//=============================================================================

// the host system specifies the base of the directory tree, the
// command line parms passed to the program, and the amount of memory
// available for the program to use

typedef struct
{
	char	*basedir;
	char	*cachedir;		// for development over ISDN lines
	int		argc;
	char	**argv;
	void	*membase;
	int		memsize;
} quakeparms_t;


//=============================================================================

//
// host
//
extern	quakeparms_t host_parms;

extern	cvar_t		sys_nostdout;
extern	cvar_t		developer;

extern	qboolean	host_initialized;		// true if into command execution
extern	double		host_frametime;
extern	double		realtime;			// not bounded in any way, changed at
										// start of every frame, never reset

void SV_Error (char *error, ...);
void SV_Init (quakeparms_t *parms);

void Con_Printf (char *fmt, ...);
void Con_DPrintf (char *fmt, ...);

