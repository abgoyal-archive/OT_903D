
#ifndef _ANDROID_UTILS_SYSTEM_H
#define _ANDROID_UTILS_SYSTEM_H

#include <string.h>
#include <stdint.h>


/* checked malloc, only returns NULL if size is 0 */
void*  android_alloc( size_t  size );

/* checked calloc, only returns NULL if size is 0 */
void*  android_alloc0( size_t  size );

/* checked realloc, only returns NULL if size if 0 */
void*  android_realloc( void*  block, size_t  size );

/* free memory block */
void   android_free( void*  block );

/* convenience macros */

#define  AZERO(p)             memset((char*)(p),0,sizeof(*(p)))
#define  ANEW(p)              (p = android_alloc(sizeof(*p)))
#define  ANEW0(p)             (p = android_alloc0(sizeof(*p)))
#define  AFREE(p)             android_free(p)

#define  AMEM_ZERO(dst,size)      memset((char*)(dst), 0, (size_t)(size))
#define  AMEM_COPY(dst,src,size)  memcpy((char*)(dst),(const char*)(src),(size_t)(size))
#define  AMEM_MOVE(dst,src,size)  memmove((char*)(dst),(const char*)(src),(size_t)(size))

#define  AARRAY_NEW(p,count)          ((p) = android_alloc(sizeof(*p)*(count)))
#define  AARRAY_NEW0(p,count)         ((p) = android_alloc0(sizeof(*p)*(count)))

#define  AARRAY_RENEW(p,count)        ((p) = android_realloc((p),sizeof(*(p))*(count)))

#define  AARRAY_COPY(dst,src,count)   AMEM_COPY(dst,src,(count)*sizeof((dst)[0]))
#define  AARRAY_MOVE(dst,src,count)   AMEM_MOVE(dst,src,(count)*sizeof((dst)[0]))
#define  AARRAY_ZERO(dst,count)       AMEM_ZERO(dst,(count)*sizeof((dst)[0]))

#define  AARRAY_STATIC_LEN(a)         (sizeof((a))/sizeof((a)[0]))

#define  AINLINED  static __inline__

/* unlike strdup(), this accepts NULL as valid input (and will return NULL then) */
char*   android_strdup(const char*  src);

#define  ASTRDUP(str)  android_strdup(str)

typedef int  APosixStatus;

/* used for functions that return or accept a boolean type */
typedef int  ABool;

#ifndef STRINGIFY
#define  _STRINGIFY(x)  #x
#define  STRINGIFY(x)  _STRINGIFY(x)
#endif

#ifndef GLUE
#define  _GLUE(x,y)  x##y
#define  GLUE(x,y)   _GLUE(x,y)

#define  _GLUE3(x,y,z)  x##y##z
#define  GLUE3(x,y,z)    _GLUE3(x,y,z)
#endif

#ifdef _WIN32
#  undef   strsep
#  define  strsep    win32_strsep
extern char*  win32_strsep(char**  pline, const char*  delim);
#endif

#ifdef _WIN32
#  define  strcasecmp  stricmp
#endif


#ifdef _WIN32
#  define   CHECKED(ret, call)    (ret) = (call)
#else
#  define   CHECKED(ret, call)    do { (ret) = (call); } while ((ret) < 0 && errno == EINTR)
#endif

#ifdef _WIN32
typedef struct { int  dumy; }      signal_state_t;
#else
#include <signal.h>
typedef struct { sigset_t  old; }  signal_state_t;
#endif

extern  void   disable_sigalrm( signal_state_t  *state );
extern  void   restore_sigalrm( signal_state_t  *state );

#ifdef _WIN32

#define   BEGIN_NOSIGALRM  \
    {

#define   END_NOSIGALRM  \
    }

#else /* !WIN32 */

#define   BEGIN_NOSIGALRM  \
    { signal_state_t  __sigalrm_state; \
      disable_sigalrm( &__sigalrm_state );

#define   END_NOSIGALRM  \
      restore_sigalrm( &__sigalrm_state );  \
    }

#endif /* !WIN32 */


extern  void   sleep_ms( int  timeout );

/* */

#endif /* _ANDROID_UTILS_SYSTEM_H */
