
#include "SDL_config.h"

/* This file contains portable memory management functions for SDL */

#include "SDL_stdinc.h"

#ifndef HAVE_MALLOC

#define LACKS_SYS_TYPES_H
#define LACKS_STDIO_H
#define LACKS_STRINGS_H
#define LACKS_STRING_H
#define LACKS_STDLIB_H
#define ABORT


#ifndef WIN32
#ifdef _WIN32
#define WIN32 1
#endif  /* _WIN32 */
#endif  /* WIN32 */
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define HAVE_MMAP 1
#define HAVE_MORECORE 0
#define LACKS_UNISTD_H
#define LACKS_SYS_PARAM_H
#define LACKS_SYS_MMAN_H
#define LACKS_STRING_H
#define LACKS_STRINGS_H
#define LACKS_SYS_TYPES_H
#define LACKS_ERRNO_H
#define LACKS_FCNTL_H 
#define MALLOC_FAILURE_ACTION
#define MMAP_CLEARS 0 /* WINCE and some others apparently don't clear */
#endif  /* WIN32 */

#if defined(DARWIN) || defined(_DARWIN)
/* Mac OSX docs advise not to use sbrk; it seems better to use mmap */
#ifndef HAVE_MORECORE
#define HAVE_MORECORE 0
#define HAVE_MMAP 1
#endif  /* HAVE_MORECORE */
#endif  /* DARWIN */

#ifndef LACKS_SYS_TYPES_H
#include <sys/types.h>  /* For size_t */
#endif  /* LACKS_SYS_TYPES_H */

/* The maximum possible size_t value has all bits set */
#define MAX_SIZE_T           (~(size_t)0)

#ifndef ONLY_MSPACES
#define ONLY_MSPACES 0
#endif  /* ONLY_MSPACES */
#ifndef MSPACES
#if ONLY_MSPACES
#define MSPACES 1
#else   /* ONLY_MSPACES */
#define MSPACES 0
#endif  /* ONLY_MSPACES */
#endif  /* MSPACES */
#ifndef MALLOC_ALIGNMENT
#define MALLOC_ALIGNMENT ((size_t)8U)
#endif  /* MALLOC_ALIGNMENT */
#ifndef FOOTERS
#define FOOTERS 0
#endif  /* FOOTERS */
#ifndef ABORT
#define ABORT  abort()
#endif  /* ABORT */
#ifndef ABORT_ON_ASSERT_FAILURE
#define ABORT_ON_ASSERT_FAILURE 1
#endif  /* ABORT_ON_ASSERT_FAILURE */
#ifndef PROCEED_ON_ERROR
#define PROCEED_ON_ERROR 0
#endif  /* PROCEED_ON_ERROR */
#ifndef USE_LOCKS
#define USE_LOCKS 0
#endif  /* USE_LOCKS */
#ifndef INSECURE
#define INSECURE 0
#endif  /* INSECURE */
#ifndef HAVE_MMAP
#define HAVE_MMAP 1
#endif  /* HAVE_MMAP */
#ifndef MMAP_CLEARS
#define MMAP_CLEARS 1
#endif  /* MMAP_CLEARS */
#ifndef HAVE_MREMAP
#ifdef linux
#define HAVE_MREMAP 1
#else   /* linux */
#define HAVE_MREMAP 0
#endif  /* linux */
#endif  /* HAVE_MREMAP */
#ifndef MALLOC_FAILURE_ACTION
#define MALLOC_FAILURE_ACTION  errno = ENOMEM;
#endif  /* MALLOC_FAILURE_ACTION */
#ifndef HAVE_MORECORE
#if ONLY_MSPACES
#define HAVE_MORECORE 0
#else   /* ONLY_MSPACES */
#define HAVE_MORECORE 1
#endif  /* ONLY_MSPACES */
#endif  /* HAVE_MORECORE */
#if !HAVE_MORECORE
#define MORECORE_CONTIGUOUS 0
#else   /* !HAVE_MORECORE */
#ifndef MORECORE
#define MORECORE sbrk
#endif  /* MORECORE */
#ifndef MORECORE_CONTIGUOUS
#define MORECORE_CONTIGUOUS 1
#endif  /* MORECORE_CONTIGUOUS */
#endif  /* HAVE_MORECORE */
#ifndef DEFAULT_GRANULARITY
#if MORECORE_CONTIGUOUS
#define DEFAULT_GRANULARITY (0)  /* 0 means to compute in init_mparams */
#else   /* MORECORE_CONTIGUOUS */
#define DEFAULT_GRANULARITY ((size_t)64U * (size_t)1024U)
#endif  /* MORECORE_CONTIGUOUS */
#endif  /* DEFAULT_GRANULARITY */
#ifndef DEFAULT_TRIM_THRESHOLD
#ifndef MORECORE_CANNOT_TRIM
#define DEFAULT_TRIM_THRESHOLD ((size_t)2U * (size_t)1024U * (size_t)1024U)
#else   /* MORECORE_CANNOT_TRIM */
#define DEFAULT_TRIM_THRESHOLD MAX_SIZE_T
#endif  /* MORECORE_CANNOT_TRIM */
#endif  /* DEFAULT_TRIM_THRESHOLD */
#ifndef DEFAULT_MMAP_THRESHOLD
#if HAVE_MMAP
#define DEFAULT_MMAP_THRESHOLD ((size_t)256U * (size_t)1024U)
#else   /* HAVE_MMAP */
#define DEFAULT_MMAP_THRESHOLD MAX_SIZE_T
#endif  /* HAVE_MMAP */
#endif  /* DEFAULT_MMAP_THRESHOLD */
#ifndef USE_BUILTIN_FFS
#define USE_BUILTIN_FFS 0
#endif  /* USE_BUILTIN_FFS */
#ifndef USE_DEV_RANDOM
#define USE_DEV_RANDOM 0
#endif  /* USE_DEV_RANDOM */
#ifndef NO_MALLINFO
#define NO_MALLINFO 0
#endif  /* NO_MALLINFO */
#ifndef MALLINFO_FIELD_TYPE
#define MALLINFO_FIELD_TYPE size_t
#endif  /* MALLINFO_FIELD_TYPE */

#define memset	SDL_memset
#define memcpy	SDL_memcpy
#define malloc	SDL_malloc
#define calloc	SDL_calloc
#define realloc	SDL_realloc
#define free	SDL_free


#define M_TRIM_THRESHOLD     (-1)
#define M_GRANULARITY        (-2)
#define M_MMAP_THRESHOLD     (-3)

/* ------------------------ Mallinfo declarations ------------------------ */

#if !NO_MALLINFO

/* #define HAVE_USR_INCLUDE_MALLOC_H */

#ifdef HAVE_USR_INCLUDE_MALLOC_H
#include "/usr/include/malloc.h"
#else /* HAVE_USR_INCLUDE_MALLOC_H */

struct mallinfo {
  MALLINFO_FIELD_TYPE arena;    /* non-mmapped space allocated from system */
  MALLINFO_FIELD_TYPE ordblks;  /* number of free chunks */
  MALLINFO_FIELD_TYPE smblks;   /* always 0 */
  MALLINFO_FIELD_TYPE hblks;    /* always 0 */
  MALLINFO_FIELD_TYPE hblkhd;   /* space in mmapped regions */
  MALLINFO_FIELD_TYPE usmblks;  /* maximum total allocated space */
  MALLINFO_FIELD_TYPE fsmblks;  /* always 0 */
  MALLINFO_FIELD_TYPE uordblks; /* total allocated space */
  MALLINFO_FIELD_TYPE fordblks; /* total free space */
  MALLINFO_FIELD_TYPE keepcost; /* releasable (via malloc_trim) space */
};

#endif /* HAVE_USR_INCLUDE_MALLOC_H */
#endif /* NO_MALLINFO */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if !ONLY_MSPACES

/* ------------------- Declarations of public routines ------------------- */

#ifndef USE_DL_PREFIX
#define dlcalloc               calloc
#define dlfree                 free
#define dlmalloc               malloc
#define dlmemalign             memalign
#define dlrealloc              realloc
#define dlvalloc               valloc
#define dlpvalloc              pvalloc
#define dlmallinfo             mallinfo
#define dlmallopt              mallopt
#define dlmalloc_trim          malloc_trim
#define dlmalloc_stats         malloc_stats
#define dlmalloc_usable_size   malloc_usable_size
#define dlmalloc_footprint     malloc_footprint
#define dlmalloc_max_footprint malloc_max_footprint
#define dlindependent_calloc   independent_calloc
#define dlindependent_comalloc independent_comalloc
#endif /* USE_DL_PREFIX */


void* dlmalloc(size_t);

void  dlfree(void*);

void* dlcalloc(size_t, size_t);


void* dlrealloc(void*, size_t);

void* dlmemalign(size_t, size_t);

void* dlvalloc(size_t);

int dlmallopt(int, int);

size_t dlmalloc_footprint(void);

size_t dlmalloc_max_footprint(void);

#if !NO_MALLINFO
struct mallinfo dlmallinfo(void);
#endif /* NO_MALLINFO */

void** dlindependent_calloc(size_t, size_t, void**);

void** dlindependent_comalloc(size_t, size_t*, void**);


void*  dlpvalloc(size_t);

int  dlmalloc_trim(size_t);

size_t dlmalloc_usable_size(void*);

void  dlmalloc_stats(void);

#endif /* ONLY_MSPACES */

#if MSPACES

typedef void* mspace;

mspace create_mspace(size_t capacity, int locked);

size_t destroy_mspace(mspace msp);

mspace create_mspace_with_base(void* base, size_t capacity, int locked);

void* mspace_malloc(mspace msp, size_t bytes);

void mspace_free(mspace msp, void* mem);

void* mspace_realloc(mspace msp, void* mem, size_t newsize);

void* mspace_calloc(mspace msp, size_t n_elements, size_t elem_size);

void* mspace_memalign(mspace msp, size_t alignment, size_t bytes);

void** mspace_independent_calloc(mspace msp, size_t n_elements,
                                 size_t elem_size, void* chunks[]);

void** mspace_independent_comalloc(mspace msp, size_t n_elements,
                                   size_t sizes[], void* chunks[]);

size_t mspace_footprint(mspace msp);

size_t mspace_max_footprint(mspace msp);


#if !NO_MALLINFO
struct mallinfo mspace_mallinfo(mspace msp);
#endif /* NO_MALLINFO */

void mspace_malloc_stats(mspace msp);

int mspace_trim(mspace msp, size_t pad);

int mspace_mallopt(int, int);

#endif /* MSPACES */

#ifdef __cplusplus
};  /* end of extern "C" */
#endif /* __cplusplus */


/* #include "malloc.h" */

/*------------------------------ internal #includes ---------------------- */

#ifdef _MSC_VER
#pragma warning( disable : 4146 ) /* no "unsigned" warnings */
#endif /* _MSC_VER */

#ifndef LACKS_STDIO_H
#include <stdio.h>       /* for printing in malloc_stats */
#endif

#ifndef LACKS_ERRNO_H
#include <errno.h>       /* for MALLOC_FAILURE_ACTION */
#endif /* LACKS_ERRNO_H */
#if FOOTERS
#include <time.h>        /* for magic initialization */
#endif /* FOOTERS */
#ifndef LACKS_STDLIB_H
#include <stdlib.h>      /* for abort() */
#endif /* LACKS_STDLIB_H */
#ifdef DEBUG
#if ABORT_ON_ASSERT_FAILURE
#define assert(x) if(!(x)) ABORT
#else /* ABORT_ON_ASSERT_FAILURE */
#include <assert.h>
#endif /* ABORT_ON_ASSERT_FAILURE */
#else  /* DEBUG */
#define assert(x)
#endif /* DEBUG */
#ifndef LACKS_STRING_H
#include <string.h>      /* for memset etc */
#endif  /* LACKS_STRING_H */
#if USE_BUILTIN_FFS
#ifndef LACKS_STRINGS_H
#include <strings.h>     /* for ffs */
#endif /* LACKS_STRINGS_H */
#endif /* USE_BUILTIN_FFS */
#if HAVE_MMAP
#ifndef LACKS_SYS_MMAN_H
#include <sys/mman.h>    /* for mmap */
#endif /* LACKS_SYS_MMAN_H */
#ifndef LACKS_FCNTL_H
#include <fcntl.h>
#endif /* LACKS_FCNTL_H */
#endif /* HAVE_MMAP */
#if HAVE_MORECORE
#ifndef LACKS_UNISTD_H
#include <unistd.h>     /* for sbrk */
#else /* LACKS_UNISTD_H */
#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__)
extern void*     sbrk(ptrdiff_t);
#endif /* FreeBSD etc */
#endif /* LACKS_UNISTD_H */
#endif /* HAVE_MMAP */

#ifndef WIN32
#ifndef malloc_getpagesize
#  ifdef _SC_PAGESIZE         /* some SVR4 systems omit an underscore */
#    ifndef _SC_PAGE_SIZE
#      define _SC_PAGE_SIZE _SC_PAGESIZE
#    endif
#  endif
#  ifdef _SC_PAGE_SIZE
#    define malloc_getpagesize sysconf(_SC_PAGE_SIZE)
#  else
#    if defined(BSD) || defined(DGUX) || defined(HAVE_GETPAGESIZE)
       extern size_t getpagesize();
#      define malloc_getpagesize getpagesize()
#    else
#      ifdef WIN32 /* use supplied emulation of getpagesize */
#        define malloc_getpagesize getpagesize()
#      else
#        ifndef LACKS_SYS_PARAM_H
#          include <sys/param.h>
#        endif
#        ifdef EXEC_PAGESIZE
#          define malloc_getpagesize EXEC_PAGESIZE
#        else
#          ifdef NBPG
#            ifndef CLSIZE
#              define malloc_getpagesize NBPG
#            else
#              define malloc_getpagesize (NBPG * CLSIZE)
#            endif
#          else
#            ifdef NBPC
#              define malloc_getpagesize NBPC
#            else
#              ifdef PAGESIZE
#                define malloc_getpagesize PAGESIZE
#              else /* just guess */
#                define malloc_getpagesize ((size_t)4096U)
#              endif
#            endif
#          endif
#        endif
#      endif
#    endif
#  endif
#endif
#endif

/* ------------------- size_t and alignment properties -------------------- */

/* The byte and bit size of a size_t */
#define SIZE_T_SIZE         (sizeof(size_t))
#define SIZE_T_BITSIZE      (sizeof(size_t) << 3)

/* Some constants coerced to size_t */
/* Annoying but necessary to avoid errors on some plaftorms */
#define SIZE_T_ZERO         ((size_t)0)
#define SIZE_T_ONE          ((size_t)1)
#define SIZE_T_TWO          ((size_t)2)
#define TWO_SIZE_T_SIZES    (SIZE_T_SIZE<<1)
#define FOUR_SIZE_T_SIZES   (SIZE_T_SIZE<<2)
#define SIX_SIZE_T_SIZES    (FOUR_SIZE_T_SIZES+TWO_SIZE_T_SIZES)
#define HALF_MAX_SIZE_T     (MAX_SIZE_T / 2U)

/* The bit mask value corresponding to MALLOC_ALIGNMENT */
#define CHUNK_ALIGN_MASK    (MALLOC_ALIGNMENT - SIZE_T_ONE)

/* True if address a has acceptable alignment */
#define is_aligned(A)       (((size_t)((A)) & (CHUNK_ALIGN_MASK)) == 0)

/* the number of bytes to offset an address to align it */
#define align_offset(A)\
 ((((size_t)(A) & CHUNK_ALIGN_MASK) == 0)? 0 :\
  ((MALLOC_ALIGNMENT - ((size_t)(A) & CHUNK_ALIGN_MASK)) & CHUNK_ALIGN_MASK))

/* -------------------------- MMAP preliminaries ------------------------- */



/* MORECORE and MMAP must return MFAIL on failure */
#define MFAIL                ((void*)(MAX_SIZE_T))
#define CMFAIL               ((char*)(MFAIL)) /* defined for convenience */

#if !HAVE_MMAP
#define IS_MMAPPED_BIT       (SIZE_T_ZERO)
#define USE_MMAP_BIT         (SIZE_T_ZERO)
#define CALL_MMAP(s)         MFAIL
#define CALL_MUNMAP(a, s)    (-1)
#define DIRECT_MMAP(s)       MFAIL

#else /* HAVE_MMAP */
#define IS_MMAPPED_BIT       (SIZE_T_ONE)
#define USE_MMAP_BIT         (SIZE_T_ONE)

#ifndef WIN32
#define CALL_MUNMAP(a, s)    munmap((a), (s))
#define MMAP_PROT            (PROT_READ|PROT_WRITE)
#if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#define MAP_ANONYMOUS        MAP_ANON
#endif /* MAP_ANON */
#ifdef MAP_ANONYMOUS
#define MMAP_FLAGS           (MAP_PRIVATE|MAP_ANONYMOUS)
#define CALL_MMAP(s)         mmap(0, (s), MMAP_PROT, MMAP_FLAGS, -1, 0)
#else /* MAP_ANONYMOUS */
#define MMAP_FLAGS           (MAP_PRIVATE)
static int dev_zero_fd = -1; /* Cached file descriptor for /dev/zero. */
#define CALL_MMAP(s) ((dev_zero_fd < 0) ? \
           (dev_zero_fd = open("/dev/zero", O_RDWR), \
            mmap(0, (s), MMAP_PROT, MMAP_FLAGS, dev_zero_fd, 0)) : \
            mmap(0, (s), MMAP_PROT, MMAP_FLAGS, dev_zero_fd, 0))
#endif /* MAP_ANONYMOUS */

#define DIRECT_MMAP(s)       CALL_MMAP(s)
#else /* WIN32 */

/* Win32 MMAP via VirtualAlloc */
static void* win32mmap(size_t size) {
  void* ptr = VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  return (ptr != 0)? ptr: MFAIL;
}

/* For direct MMAP, use MEM_TOP_DOWN to minimize interference */
static void* win32direct_mmap(size_t size) {
  void* ptr = VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN,
                           PAGE_READWRITE);
  return (ptr != 0)? ptr: MFAIL;
}

/* This function supports releasing coalesed segments */
static int win32munmap(void* ptr, size_t size) {
  MEMORY_BASIC_INFORMATION minfo;
  char* cptr = ptr;
  while (size) {
    if (VirtualQuery(cptr, &minfo, sizeof(minfo)) == 0)
      return -1;
    if (minfo.BaseAddress != cptr || minfo.AllocationBase != cptr ||
        minfo.State != MEM_COMMIT || minfo.RegionSize > size)
      return -1;
    if (VirtualFree(cptr, 0, MEM_RELEASE) == 0)
      return -1;
    cptr += minfo.RegionSize;
    size -= minfo.RegionSize;
  }
  return 0;
}

#define CALL_MMAP(s)         win32mmap(s)
#define CALL_MUNMAP(a, s)    win32munmap((a), (s))
#define DIRECT_MMAP(s)       win32direct_mmap(s)
#endif /* WIN32 */
#endif /* HAVE_MMAP */

#if HAVE_MMAP && HAVE_MREMAP
#define CALL_MREMAP(addr, osz, nsz, mv) mremap((addr), (osz), (nsz), (mv))
#else  /* HAVE_MMAP && HAVE_MREMAP */
#define CALL_MREMAP(addr, osz, nsz, mv) MFAIL
#endif /* HAVE_MMAP && HAVE_MREMAP */

#if HAVE_MORECORE
#define CALL_MORECORE(S)     MORECORE(S)
#else  /* HAVE_MORECORE */
#define CALL_MORECORE(S)     MFAIL
#endif /* HAVE_MORECORE */

/* mstate bit set if continguous morecore disabled or failed */
#define USE_NONCONTIGUOUS_BIT (4U)

/* segment bit set in create_mspace_with_base */
#define EXTERN_BIT            (8U)


/* --------------------------- Lock preliminaries ------------------------ */

#if USE_LOCKS


#ifndef WIN32
/* By default use posix locks */
#include <pthread.h>
#define MLOCK_T pthread_mutex_t
#define INITIAL_LOCK(l)      pthread_mutex_init(l, NULL)
#define ACQUIRE_LOCK(l)      pthread_mutex_lock(l)
#define RELEASE_LOCK(l)      pthread_mutex_unlock(l)

#if HAVE_MORECORE
static MLOCK_T morecore_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif /* HAVE_MORECORE */

static MLOCK_T magic_init_mutex = PTHREAD_MUTEX_INITIALIZER;

#else /* WIN32 */

#define MLOCK_T long
static int win32_acquire_lock (MLOCK_T *sl) {
  for (;;) {
#ifdef InterlockedCompareExchangePointer
    if (!InterlockedCompareExchange(sl, 1, 0))
      return 0;
#else  /* Use older void* version */
    if (!InterlockedCompareExchange((void**)sl, (void*)1, (void*)0))
      return 0;
#endif /* InterlockedCompareExchangePointer */
    Sleep (0);
  }
}

static void win32_release_lock (MLOCK_T *sl) {
  InterlockedExchange (sl, 0);
}

#define INITIAL_LOCK(l)      *(l)=0
#define ACQUIRE_LOCK(l)      win32_acquire_lock(l)
#define RELEASE_LOCK(l)      win32_release_lock(l)
#if HAVE_MORECORE
static MLOCK_T morecore_mutex;
#endif /* HAVE_MORECORE */
static MLOCK_T magic_init_mutex;
#endif /* WIN32 */

#define USE_LOCK_BIT               (2U)
#else  /* USE_LOCKS */
#define USE_LOCK_BIT               (0U)
#define INITIAL_LOCK(l)
#endif /* USE_LOCKS */

#if USE_LOCKS && HAVE_MORECORE
#define ACQUIRE_MORECORE_LOCK()    ACQUIRE_LOCK(&morecore_mutex);
#define RELEASE_MORECORE_LOCK()    RELEASE_LOCK(&morecore_mutex);
#else /* USE_LOCKS && HAVE_MORECORE */
#define ACQUIRE_MORECORE_LOCK()
#define RELEASE_MORECORE_LOCK()
#endif /* USE_LOCKS && HAVE_MORECORE */

#if USE_LOCKS
#define ACQUIRE_MAGIC_INIT_LOCK()  ACQUIRE_LOCK(&magic_init_mutex);
#define RELEASE_MAGIC_INIT_LOCK()  RELEASE_LOCK(&magic_init_mutex);
#else  /* USE_LOCKS */
#define ACQUIRE_MAGIC_INIT_LOCK()
#define RELEASE_MAGIC_INIT_LOCK()
#endif /* USE_LOCKS */


/* -----------------------  Chunk representations ------------------------ */


struct malloc_chunk {
  size_t               prev_foot;  /* Size of previous chunk (if free).  */
  size_t               head;       /* Size and inuse bits. */
  struct malloc_chunk* fd;         /* double links -- used only if free. */
  struct malloc_chunk* bk;
};

typedef struct malloc_chunk  mchunk;
typedef struct malloc_chunk* mchunkptr;
typedef struct malloc_chunk* sbinptr;  /* The type of bins of chunks */
typedef size_t bindex_t;               /* Described below */
typedef unsigned int binmap_t;         /* Described below */
typedef unsigned int flag_t;           /* The type of various bit flag sets */

/* ------------------- Chunks sizes and alignments ----------------------- */

#define MCHUNK_SIZE         (sizeof(mchunk))

#if FOOTERS
#define CHUNK_OVERHEAD      (TWO_SIZE_T_SIZES)
#else /* FOOTERS */
#define CHUNK_OVERHEAD      (SIZE_T_SIZE)
#endif /* FOOTERS */

/* MMapped chunks need a second word of overhead ... */
#define MMAP_CHUNK_OVERHEAD (TWO_SIZE_T_SIZES)
/* ... and additional padding for fake next-chunk at foot */
#define MMAP_FOOT_PAD       (FOUR_SIZE_T_SIZES)

/* The smallest size we can malloc is an aligned minimal chunk */
#define MIN_CHUNK_SIZE\
  ((MCHUNK_SIZE + CHUNK_ALIGN_MASK) & ~CHUNK_ALIGN_MASK)

/* conversion from malloc headers to user pointers, and back */
#define chunk2mem(p)        ((void*)((char*)(p)       + TWO_SIZE_T_SIZES))
#define mem2chunk(mem)      ((mchunkptr)((char*)(mem) - TWO_SIZE_T_SIZES))
/* chunk associated with aligned address A */
#define align_as_chunk(A)   (mchunkptr)((A) + align_offset(chunk2mem(A)))

/* Bounds on request (not chunk) sizes. */
#define MAX_REQUEST         ((-MIN_CHUNK_SIZE) << 2)
#define MIN_REQUEST         (MIN_CHUNK_SIZE - CHUNK_OVERHEAD - SIZE_T_ONE)

/* pad request bytes into a usable size */
#define pad_request(req) \
   (((req) + CHUNK_OVERHEAD + CHUNK_ALIGN_MASK) & ~CHUNK_ALIGN_MASK)

/* pad request, checking for minimum (but not maximum) */
#define request2size(req) \
  (((req) < MIN_REQUEST)? MIN_CHUNK_SIZE : pad_request(req))


/* ------------------ Operations on head and foot fields ----------------- */


#define PINUSE_BIT          (SIZE_T_ONE)
#define CINUSE_BIT          (SIZE_T_TWO)
#define INUSE_BITS          (PINUSE_BIT|CINUSE_BIT)

/* Head value for fenceposts */
#define FENCEPOST_HEAD      (INUSE_BITS|SIZE_T_SIZE)

/* extraction of fields from head words */
#define cinuse(p)           ((p)->head & CINUSE_BIT)
#define pinuse(p)           ((p)->head & PINUSE_BIT)
#define chunksize(p)        ((p)->head & ~(INUSE_BITS))

#define clear_pinuse(p)     ((p)->head &= ~PINUSE_BIT)
#define clear_cinuse(p)     ((p)->head &= ~CINUSE_BIT)

/* Treat space at ptr +/- offset as a chunk */
#define chunk_plus_offset(p, s)  ((mchunkptr)(((char*)(p)) + (s)))
#define chunk_minus_offset(p, s) ((mchunkptr)(((char*)(p)) - (s)))

/* Ptr to next or previous physical malloc_chunk. */
#define next_chunk(p) ((mchunkptr)( ((char*)(p)) + ((p)->head & ~INUSE_BITS)))
#define prev_chunk(p) ((mchunkptr)( ((char*)(p)) - ((p)->prev_foot) ))

/* extract next chunk's pinuse bit */
#define next_pinuse(p)  ((next_chunk(p)->head) & PINUSE_BIT)

/* Get/set size at footer */
#define get_foot(p, s)  (((mchunkptr)((char*)(p) + (s)))->prev_foot)
#define set_foot(p, s)  (((mchunkptr)((char*)(p) + (s)))->prev_foot = (s))

/* Set size, pinuse bit, and foot */
#define set_size_and_pinuse_of_free_chunk(p, s)\
  ((p)->head = (s|PINUSE_BIT), set_foot(p, s))

/* Set size, pinuse bit, foot, and clear next pinuse */
#define set_free_with_pinuse(p, s, n)\
  (clear_pinuse(n), set_size_and_pinuse_of_free_chunk(p, s))

#define is_mmapped(p)\
  (!((p)->head & PINUSE_BIT) && ((p)->prev_foot & IS_MMAPPED_BIT))

/* Get the internal overhead associated with chunk p */
#define overhead_for(p)\
 (is_mmapped(p)? MMAP_CHUNK_OVERHEAD : CHUNK_OVERHEAD)

/* Return true if malloced space is not necessarily cleared */
#if MMAP_CLEARS
#define calloc_must_clear(p) (!is_mmapped(p))
#else /* MMAP_CLEARS */
#define calloc_must_clear(p) (1)
#endif /* MMAP_CLEARS */

/* ---------------------- Overlaid data structures ----------------------- */


struct malloc_tree_chunk {
  /* The first four fields must be compatible with malloc_chunk */
  size_t                    prev_foot;
  size_t                    head;
  struct malloc_tree_chunk* fd;
  struct malloc_tree_chunk* bk;

  struct malloc_tree_chunk* child[2];
  struct malloc_tree_chunk* parent;
  bindex_t                  index;
};

typedef struct malloc_tree_chunk  tchunk;
typedef struct malloc_tree_chunk* tchunkptr;
typedef struct malloc_tree_chunk* tbinptr; /* The type of bins of trees */

/* A little helper macro for trees */
#define leftmost_child(t) ((t)->child[0] != 0? (t)->child[0] : (t)->child[1])

/* ----------------------------- Segments -------------------------------- */


struct malloc_segment {
  char*        base;             /* base address */
  size_t       size;             /* allocated size */
  struct malloc_segment* next;   /* ptr to next segment */
  flag_t       sflags;           /* mmap and extern flag */
};

#define is_mmapped_segment(S)  ((S)->sflags & IS_MMAPPED_BIT)
#define is_extern_segment(S)   ((S)->sflags & EXTERN_BIT)

typedef struct malloc_segment  msegment;
typedef struct malloc_segment* msegmentptr;

/* ---------------------------- malloc_state ----------------------------- */


/* Bin types, widths and sizes */
#define NSMALLBINS        (32U)
#define NTREEBINS         (32U)
#define SMALLBIN_SHIFT    (3U)
#define SMALLBIN_WIDTH    (SIZE_T_ONE << SMALLBIN_SHIFT)
#define TREEBIN_SHIFT     (8U)
#define MIN_LARGE_SIZE    (SIZE_T_ONE << TREEBIN_SHIFT)
#define MAX_SMALL_SIZE    (MIN_LARGE_SIZE - SIZE_T_ONE)
#define MAX_SMALL_REQUEST (MAX_SMALL_SIZE - CHUNK_ALIGN_MASK - CHUNK_OVERHEAD)

struct malloc_state {
  binmap_t   smallmap;
  binmap_t   treemap;
  size_t     dvsize;
  size_t     topsize;
  char*      least_addr;
  mchunkptr  dv;
  mchunkptr  top;
  size_t     trim_check;
  size_t     magic;
  mchunkptr  smallbins[(NSMALLBINS+1)*2];
  tbinptr    treebins[NTREEBINS];
  size_t     footprint;
  size_t     max_footprint;
  flag_t     mflags;
#if USE_LOCKS
  MLOCK_T    mutex;     /* locate lock among fields that rarely change */
#endif /* USE_LOCKS */
  msegment   seg;
};

typedef struct malloc_state*    mstate;

/* ------------- Global malloc_state and malloc_params ------------------- */


struct malloc_params {
  size_t magic;
  size_t page_size;
  size_t granularity;
  size_t mmap_threshold;
  size_t trim_threshold;
  flag_t default_mflags;
};

static struct malloc_params mparams;

/* The global malloc_state used for all non-"mspace" calls */
static struct malloc_state _gm_;
#define gm                 (&_gm_)
#define is_global(M)       ((M) == &_gm_)
#define is_initialized(M)  ((M)->top != 0)

/* -------------------------- system alloc setup ------------------------- */

/* Operations on mflags */

#define use_lock(M)           ((M)->mflags &   USE_LOCK_BIT)
#define enable_lock(M)        ((M)->mflags |=  USE_LOCK_BIT)
#define disable_lock(M)       ((M)->mflags &= ~USE_LOCK_BIT)

#define use_mmap(M)           ((M)->mflags &   USE_MMAP_BIT)
#define enable_mmap(M)        ((M)->mflags |=  USE_MMAP_BIT)
#define disable_mmap(M)       ((M)->mflags &= ~USE_MMAP_BIT)

#define use_noncontiguous(M)  ((M)->mflags &   USE_NONCONTIGUOUS_BIT)
#define disable_contiguous(M) ((M)->mflags |=  USE_NONCONTIGUOUS_BIT)

#define set_lock(M,L)\
 ((M)->mflags = (L)?\
  ((M)->mflags | USE_LOCK_BIT) :\
  ((M)->mflags & ~USE_LOCK_BIT))

/* page-align a size */
#define page_align(S)\
 (((S) + (mparams.page_size)) & ~(mparams.page_size - SIZE_T_ONE))

/* granularity-align a size */
#define granularity_align(S)\
  (((S) + (mparams.granularity)) & ~(mparams.granularity - SIZE_T_ONE))

#define is_page_aligned(S)\
   (((size_t)(S) & (mparams.page_size - SIZE_T_ONE)) == 0)
#define is_granularity_aligned(S)\
   (((size_t)(S) & (mparams.granularity - SIZE_T_ONE)) == 0)

/*  True if segment S holds address A */
#define segment_holds(S, A)\
  ((char*)(A) >= S->base && (char*)(A) < S->base + S->size)

/* Return segment holding given address */
static msegmentptr segment_holding(mstate m, char* addr) {
  msegmentptr sp = &m->seg;
  for (;;) {
    if (addr >= sp->base && addr < sp->base + sp->size)
      return sp;
    if ((sp = sp->next) == 0)
      return 0;
  }
}

/* Return true if segment contains a segment link */
static int has_segment_link(mstate m, msegmentptr ss) {
  msegmentptr sp = &m->seg;
  for (;;) {
    if ((char*)sp >= ss->base && (char*)sp < ss->base + ss->size)
      return 1;
    if ((sp = sp->next) == 0)
      return 0;
  }
}

#ifndef MORECORE_CANNOT_TRIM
#define should_trim(M,s)  ((s) > (M)->trim_check)
#else  /* MORECORE_CANNOT_TRIM */
#define should_trim(M,s)  (0)
#endif /* MORECORE_CANNOT_TRIM */

#define TOP_FOOT_SIZE\
  (align_offset(chunk2mem(0))+pad_request(sizeof(struct malloc_segment))+MIN_CHUNK_SIZE)


/* -------------------------------  Hooks -------------------------------- */


#if USE_LOCKS

/* Ensure locks are initialized */
#define GLOBALLY_INITIALIZE() (mparams.page_size == 0 && init_mparams())

#define PREACTION(M)  ((GLOBALLY_INITIALIZE() || use_lock(M))? ACQUIRE_LOCK(&(M)->mutex) : 0)
#define POSTACTION(M) { if (use_lock(M)) RELEASE_LOCK(&(M)->mutex); }
#else /* USE_LOCKS */

#ifndef PREACTION
#define PREACTION(M) (0)
#endif  /* PREACTION */

#ifndef POSTACTION
#define POSTACTION(M)
#endif  /* POSTACTION */

#endif /* USE_LOCKS */


#if PROCEED_ON_ERROR

/* A count of the number of corruption errors causing resets */
int malloc_corruption_error_count;

/* default corruption action */
static void reset_on_error(mstate m);

#define CORRUPTION_ERROR_ACTION(m)  reset_on_error(m)
#define USAGE_ERROR_ACTION(m, p)

#else /* PROCEED_ON_ERROR */

#ifndef CORRUPTION_ERROR_ACTION
#define CORRUPTION_ERROR_ACTION(m) ABORT
#endif /* CORRUPTION_ERROR_ACTION */

#ifndef USAGE_ERROR_ACTION
#define USAGE_ERROR_ACTION(m,p) ABORT
#endif /* USAGE_ERROR_ACTION */

#endif /* PROCEED_ON_ERROR */

/* -------------------------- Debugging setup ---------------------------- */

#if ! DEBUG

#define check_free_chunk(M,P)
#define check_inuse_chunk(M,P)
#define check_malloced_chunk(M,P,N)
#define check_mmapped_chunk(M,P)
#define check_malloc_state(M)
#define check_top_chunk(M,P)

#else /* DEBUG */
#define check_free_chunk(M,P)       do_check_free_chunk(M,P)
#define check_inuse_chunk(M,P)      do_check_inuse_chunk(M,P)
#define check_top_chunk(M,P)        do_check_top_chunk(M,P)
#define check_malloced_chunk(M,P,N) do_check_malloced_chunk(M,P,N)
#define check_mmapped_chunk(M,P)    do_check_mmapped_chunk(M,P)
#define check_malloc_state(M)       do_check_malloc_state(M)

static void   do_check_any_chunk(mstate m, mchunkptr p);
static void   do_check_top_chunk(mstate m, mchunkptr p);
static void   do_check_mmapped_chunk(mstate m, mchunkptr p);
static void   do_check_inuse_chunk(mstate m, mchunkptr p);
static void   do_check_free_chunk(mstate m, mchunkptr p);
static void   do_check_malloced_chunk(mstate m, void* mem, size_t s);
static void   do_check_tree(mstate m, tchunkptr t);
static void   do_check_treebin(mstate m, bindex_t i);
static void   do_check_smallbin(mstate m, bindex_t i);
static void   do_check_malloc_state(mstate m);
static int    bin_find(mstate m, mchunkptr x);
static size_t traverse_and_check(mstate m);
#endif /* DEBUG */

/* ---------------------------- Indexing Bins ---------------------------- */

#define is_small(s)         (((s) >> SMALLBIN_SHIFT) < NSMALLBINS)
#define small_index(s)      ((s)  >> SMALLBIN_SHIFT)
#define small_index2size(i) ((i)  << SMALLBIN_SHIFT)
#define MIN_SMALL_INDEX     (small_index(MIN_CHUNK_SIZE))

/* addressing by index. See above about smallbin repositioning */
#define smallbin_at(M, i)   ((sbinptr)((char*)&((M)->smallbins[(i)<<1])))
#define treebin_at(M,i)     (&((M)->treebins[i]))

/* assign tree index for size S to variable I */
#if defined(__GNUC__) && defined(i386)
#define compute_tree_index(S, I)\
{\
  size_t X = S >> TREEBIN_SHIFT;\
  if (X == 0)\
    I = 0;\
  else if (X > 0xFFFF)\
    I = NTREEBINS-1;\
  else {\
    unsigned int K;\
    __asm__("bsrl %1,%0\n\t" : "=r" (K) : "rm"  (X));\
    I =  (bindex_t)((K << 1) + ((S >> (K + (TREEBIN_SHIFT-1)) & 1)));\
  }\
}
#else /* GNUC */
#define compute_tree_index(S, I)\
{\
  size_t X = S >> TREEBIN_SHIFT;\
  if (X == 0)\
    I = 0;\
  else if (X > 0xFFFF)\
    I = NTREEBINS-1;\
  else {\
    unsigned int Y = (unsigned int)X;\
    unsigned int N = ((Y - 0x100) >> 16) & 8;\
    unsigned int K = (((Y <<= N) - 0x1000) >> 16) & 4;\
    N += K;\
    N += K = (((Y <<= K) - 0x4000) >> 16) & 2;\
    K = 14 - N + ((Y <<= K) >> 15);\
    I = (K << 1) + ((S >> (K + (TREEBIN_SHIFT-1)) & 1));\
  }\
}
#endif /* GNUC */

/* Bit representing maximum resolved size in a treebin at i */
#define bit_for_tree_index(i) \
   (i == NTREEBINS-1)? (SIZE_T_BITSIZE-1) : (((i) >> 1) + TREEBIN_SHIFT - 2)

/* Shift placing maximum resolved bit in a treebin at i as sign bit */
#define leftshift_for_tree_index(i) \
   ((i == NTREEBINS-1)? 0 : \
    ((SIZE_T_BITSIZE-SIZE_T_ONE) - (((i) >> 1) + TREEBIN_SHIFT - 2)))

/* The size of the smallest chunk held in bin with index i */
#define minsize_for_tree_index(i) \
   ((SIZE_T_ONE << (((i) >> 1) + TREEBIN_SHIFT)) |  \
   (((size_t)((i) & SIZE_T_ONE)) << (((i) >> 1) + TREEBIN_SHIFT - 1)))


/* ------------------------ Operations on bin maps ----------------------- */

/* bit corresponding to given index */
#define idx2bit(i)              ((binmap_t)(1) << (i))

/* Mark/Clear bits with given index */
#define mark_smallmap(M,i)      ((M)->smallmap |=  idx2bit(i))
#define clear_smallmap(M,i)     ((M)->smallmap &= ~idx2bit(i))
#define smallmap_is_marked(M,i) ((M)->smallmap &   idx2bit(i))

#define mark_treemap(M,i)       ((M)->treemap  |=  idx2bit(i))
#define clear_treemap(M,i)      ((M)->treemap  &= ~idx2bit(i))
#define treemap_is_marked(M,i)  ((M)->treemap  &   idx2bit(i))

/* index corresponding to given bit */

#if defined(__GNUC__) && defined(i386)
#define compute_bit2idx(X, I)\
{\
  unsigned int J;\
  __asm__("bsfl %1,%0\n\t" : "=r" (J) : "rm" (X));\
  I = (bindex_t)J;\
}

#else /* GNUC */
#if  USE_BUILTIN_FFS
#define compute_bit2idx(X, I) I = ffs(X)-1

#else /* USE_BUILTIN_FFS */
#define compute_bit2idx(X, I)\
{\
  unsigned int Y = X - 1;\
  unsigned int K = Y >> (16-4) & 16;\
  unsigned int N = K;        Y >>= K;\
  N += K = Y >> (8-3) &  8;  Y >>= K;\
  N += K = Y >> (4-2) &  4;  Y >>= K;\
  N += K = Y >> (2-1) &  2;  Y >>= K;\
  N += K = Y >> (1-0) &  1;  Y >>= K;\
  I = (bindex_t)(N + Y);\
}
#endif /* USE_BUILTIN_FFS */
#endif /* GNUC */

/* isolate the least set bit of a bitmap */
#define least_bit(x)         ((x) & -(x))

/* mask with all bits to left of least bit of x on */
#define left_bits(x)         ((x<<1) | -(x<<1))

/* mask with all bits to left of or equal to least bit of x on */
#define same_or_left_bits(x) ((x) | -(x))


/* ----------------------- Runtime Check Support ------------------------- */


#if !INSECURE
/* Check if address a is at least as high as any from MORECORE or MMAP */
#define ok_address(M, a) ((char*)(a) >= (M)->least_addr)
/* Check if address of next chunk n is higher than base chunk p */
#define ok_next(p, n)    ((char*)(p) < (char*)(n))
/* Check if p has its cinuse bit on */
#define ok_cinuse(p)     cinuse(p)
/* Check if p has its pinuse bit on */
#define ok_pinuse(p)     pinuse(p)

#else /* !INSECURE */
#define ok_address(M, a) (1)
#define ok_next(b, n)    (1)
#define ok_cinuse(p)     (1)
#define ok_pinuse(p)     (1)
#endif /* !INSECURE */

#if (FOOTERS && !INSECURE)
/* Check if (alleged) mstate m has expected magic field */
#define ok_magic(M)      ((M)->magic == mparams.magic)
#else  /* (FOOTERS && !INSECURE) */
#define ok_magic(M)      (1)
#endif /* (FOOTERS && !INSECURE) */


/* In gcc, use __builtin_expect to minimize impact of checks */
#if !INSECURE
#if defined(__GNUC__) && __GNUC__ >= 3
#define RTCHECK(e)  __builtin_expect(e, 1)
#else /* GNUC */
#define RTCHECK(e)  (e)
#endif /* GNUC */
#else /* !INSECURE */
#define RTCHECK(e)  (1)
#endif /* !INSECURE */

/* macros to set up inuse chunks with or without footers */

#if !FOOTERS

#define mark_inuse_foot(M,p,s)

/* Set cinuse bit and pinuse bit of next chunk */
#define set_inuse(M,p,s)\
  ((p)->head = (((p)->head & PINUSE_BIT)|s|CINUSE_BIT),\
  ((mchunkptr)(((char*)(p)) + (s)))->head |= PINUSE_BIT)

/* Set cinuse and pinuse of this chunk and pinuse of next chunk */
#define set_inuse_and_pinuse(M,p,s)\
  ((p)->head = (s|PINUSE_BIT|CINUSE_BIT),\
  ((mchunkptr)(((char*)(p)) + (s)))->head |= PINUSE_BIT)

/* Set size, cinuse and pinuse bit of this chunk */
#define set_size_and_pinuse_of_inuse_chunk(M, p, s)\
  ((p)->head = (s|PINUSE_BIT|CINUSE_BIT))

#else /* FOOTERS */

/* Set foot of inuse chunk to be xor of mstate and seed */
#define mark_inuse_foot(M,p,s)\
  (((mchunkptr)((char*)(p) + (s)))->prev_foot = ((size_t)(M) ^ mparams.magic))

#define get_mstate_for(p)\
  ((mstate)(((mchunkptr)((char*)(p) +\
    (chunksize(p))))->prev_foot ^ mparams.magic))

#define set_inuse(M,p,s)\
  ((p)->head = (((p)->head & PINUSE_BIT)|s|CINUSE_BIT),\
  (((mchunkptr)(((char*)(p)) + (s)))->head |= PINUSE_BIT), \
  mark_inuse_foot(M,p,s))

#define set_inuse_and_pinuse(M,p,s)\
  ((p)->head = (s|PINUSE_BIT|CINUSE_BIT),\
  (((mchunkptr)(((char*)(p)) + (s)))->head |= PINUSE_BIT),\
 mark_inuse_foot(M,p,s))

#define set_size_and_pinuse_of_inuse_chunk(M, p, s)\
  ((p)->head = (s|PINUSE_BIT|CINUSE_BIT),\
  mark_inuse_foot(M, p, s))

#endif /* !FOOTERS */

/* ---------------------------- setting mparams -------------------------- */

/* Initialize mparams */
static int init_mparams(void) {
  if (mparams.page_size == 0) {
    size_t s;

    mparams.mmap_threshold = DEFAULT_MMAP_THRESHOLD;
    mparams.trim_threshold = DEFAULT_TRIM_THRESHOLD;
#if MORECORE_CONTIGUOUS
    mparams.default_mflags = USE_LOCK_BIT|USE_MMAP_BIT;
#else  /* MORECORE_CONTIGUOUS */
    mparams.default_mflags = USE_LOCK_BIT|USE_MMAP_BIT|USE_NONCONTIGUOUS_BIT;
#endif /* MORECORE_CONTIGUOUS */

#if (FOOTERS && !INSECURE)
    {
#if USE_DEV_RANDOM
      int fd;
      unsigned char buf[sizeof(size_t)];
      /* Try to use /dev/urandom, else fall back on using time */
      if ((fd = open("/dev/urandom", O_RDONLY)) >= 0 &&
          read(fd, buf, sizeof(buf)) == sizeof(buf)) {
        s = *((size_t *) buf);
        close(fd);
      }
      else
#endif /* USE_DEV_RANDOM */
        s = (size_t)(time(0) ^ (size_t)0x55555555U);

      s |= (size_t)8U;    /* ensure nonzero */
      s &= ~(size_t)7U;   /* improve chances of fault for bad values */

    }
#else /* (FOOTERS && !INSECURE) */
    s = (size_t)0x58585858U;
#endif /* (FOOTERS && !INSECURE) */
    ACQUIRE_MAGIC_INIT_LOCK();
    if (mparams.magic == 0) {
      mparams.magic = s;
      /* Set up lock for main malloc area */
      INITIAL_LOCK(&gm->mutex);
      gm->mflags = mparams.default_mflags;
    }
    RELEASE_MAGIC_INIT_LOCK();

#ifndef WIN32
    mparams.page_size = malloc_getpagesize;
    mparams.granularity = ((DEFAULT_GRANULARITY != 0)?
                           DEFAULT_GRANULARITY : mparams.page_size);
#else /* WIN32 */
    {
      SYSTEM_INFO system_info;
      GetSystemInfo(&system_info);
      mparams.page_size = system_info.dwPageSize;
      mparams.granularity = system_info.dwAllocationGranularity;
    }
#endif /* WIN32 */

    /* Sanity-check configuration:
       size_t must be unsigned and as wide as pointer type.
       ints must be at least 4 bytes.
       alignment must be at least 8.
       Alignment, min chunk size, and page size must all be powers of 2.
    */
    if ((sizeof(size_t) != sizeof(char*)) ||
        (MAX_SIZE_T < MIN_CHUNK_SIZE)  ||
        (sizeof(int) < 4)  ||
        (MALLOC_ALIGNMENT < (size_t)8U) ||
        ((MALLOC_ALIGNMENT    & (MALLOC_ALIGNMENT-SIZE_T_ONE))    != 0) ||
        ((MCHUNK_SIZE         & (MCHUNK_SIZE-SIZE_T_ONE))         != 0) ||
        ((mparams.granularity & (mparams.granularity-SIZE_T_ONE)) != 0) ||
        ((mparams.page_size   & (mparams.page_size-SIZE_T_ONE))   != 0))
      ABORT;
  }
  return 0;
}

/* support for mallopt */
static int change_mparam(int param_number, int value) {
  size_t val = (size_t)value;
  init_mparams();
  switch(param_number) {
  case M_TRIM_THRESHOLD:
    mparams.trim_threshold = val;
    return 1;
  case M_GRANULARITY:
    if (val >= mparams.page_size && ((val & (val-1)) == 0)) {
      mparams.granularity = val;
      return 1;
    }
    else
      return 0;
  case M_MMAP_THRESHOLD:
    mparams.mmap_threshold = val;
    return 1;
  default:
    return 0;
  }
}

#if DEBUG
/* ------------------------- Debugging Support --------------------------- */

/* Check properties of any chunk, whether free, inuse, mmapped etc  */
static void do_check_any_chunk(mstate m, mchunkptr p) {
  assert((is_aligned(chunk2mem(p))) || (p->head == FENCEPOST_HEAD));
  assert(ok_address(m, p));
}

/* Check properties of top chunk */
static void do_check_top_chunk(mstate m, mchunkptr p) {
  msegmentptr sp = segment_holding(m, (char*)p);
  size_t  sz = chunksize(p);
  assert(sp != 0);
  assert((is_aligned(chunk2mem(p))) || (p->head == FENCEPOST_HEAD));
  assert(ok_address(m, p));
  assert(sz == m->topsize);
  assert(sz > 0);
  assert(sz == ((sp->base + sp->size) - (char*)p) - TOP_FOOT_SIZE);
  assert(pinuse(p));
  assert(!next_pinuse(p));
}

/* Check properties of (inuse) mmapped chunks */
static void do_check_mmapped_chunk(mstate m, mchunkptr p) {
  size_t  sz = chunksize(p);
  size_t len = (sz + (p->prev_foot & ~IS_MMAPPED_BIT) + MMAP_FOOT_PAD);
  assert(is_mmapped(p));
  assert(use_mmap(m));
  assert((is_aligned(chunk2mem(p))) || (p->head == FENCEPOST_HEAD));
  assert(ok_address(m, p));
  assert(!is_small(sz));
  assert((len & (mparams.page_size-SIZE_T_ONE)) == 0);
  assert(chunk_plus_offset(p, sz)->head == FENCEPOST_HEAD);
  assert(chunk_plus_offset(p, sz+SIZE_T_SIZE)->head == 0);
}

/* Check properties of inuse chunks */
static void do_check_inuse_chunk(mstate m, mchunkptr p) {
  do_check_any_chunk(m, p);
  assert(cinuse(p));
  assert(next_pinuse(p));
  /* If not pinuse and not mmapped, previous chunk has OK offset */
  assert(is_mmapped(p) || pinuse(p) || next_chunk(prev_chunk(p)) == p);
  if (is_mmapped(p))
    do_check_mmapped_chunk(m, p);
}

/* Check properties of free chunks */
static void do_check_free_chunk(mstate m, mchunkptr p) {
  size_t sz = p->head & ~(PINUSE_BIT|CINUSE_BIT);
  mchunkptr next = chunk_plus_offset(p, sz);
  do_check_any_chunk(m, p);
  assert(!cinuse(p));
  assert(!next_pinuse(p));
  assert (!is_mmapped(p));
  if (p != m->dv && p != m->top) {
    if (sz >= MIN_CHUNK_SIZE) {
      assert((sz & CHUNK_ALIGN_MASK) == 0);
      assert(is_aligned(chunk2mem(p)));
      assert(next->prev_foot == sz);
      assert(pinuse(p));
      assert (next == m->top || cinuse(next));
      assert(p->fd->bk == p);
      assert(p->bk->fd == p);
    }
    else  /* markers are always of size SIZE_T_SIZE */
      assert(sz == SIZE_T_SIZE);
  }
}

/* Check properties of malloced chunks at the point they are malloced */
static void do_check_malloced_chunk(mstate m, void* mem, size_t s) {
  if (mem != 0) {
    mchunkptr p = mem2chunk(mem);
    size_t sz = p->head & ~(PINUSE_BIT|CINUSE_BIT);
    do_check_inuse_chunk(m, p);
    assert((sz & CHUNK_ALIGN_MASK) == 0);
    assert(sz >= MIN_CHUNK_SIZE);
    assert(sz >= s);
    /* unless mmapped, size is less than MIN_CHUNK_SIZE more than request */
    assert(is_mmapped(p) || sz < (s + MIN_CHUNK_SIZE));
  }
}

/* Check a tree and its subtrees.  */
static void do_check_tree(mstate m, tchunkptr t) {
  tchunkptr head = 0;
  tchunkptr u = t;
  bindex_t tindex = t->index;
  size_t tsize = chunksize(t);
  bindex_t idx;
  compute_tree_index(tsize, idx);
  assert(tindex == idx);
  assert(tsize >= MIN_LARGE_SIZE);
  assert(tsize >= minsize_for_tree_index(idx));
  assert((idx == NTREEBINS-1) || (tsize < minsize_for_tree_index((idx+1))));

  do { /* traverse through chain of same-sized nodes */
    do_check_any_chunk(m, ((mchunkptr)u));
    assert(u->index == tindex);
    assert(chunksize(u) == tsize);
    assert(!cinuse(u));
    assert(!next_pinuse(u));
    assert(u->fd->bk == u);
    assert(u->bk->fd == u);
    if (u->parent == 0) {
      assert(u->child[0] == 0);
      assert(u->child[1] == 0);
    }
    else {
      assert(head == 0); /* only one node on chain has parent */
      head = u;
      assert(u->parent != u);
      assert (u->parent->child[0] == u ||
              u->parent->child[1] == u ||
              *((tbinptr*)(u->parent)) == u);
      if (u->child[0] != 0) {
        assert(u->child[0]->parent == u);
        assert(u->child[0] != u);
        do_check_tree(m, u->child[0]);
      }
      if (u->child[1] != 0) {
        assert(u->child[1]->parent == u);
        assert(u->child[1] != u);
        do_check_tree(m, u->child[1]);
      }
      if (u->child[0] != 0 && u->child[1] != 0) {
        assert(chunksize(u->child[0]) < chunksize(u->child[1]));
      }
    }
    u = u->fd;
  } while (u != t);
  assert(head != 0);
}

/*  Check all the chunks in a treebin.  */
static void do_check_treebin(mstate m, bindex_t i) {
  tbinptr* tb = treebin_at(m, i);
  tchunkptr t = *tb;
  int empty = (m->treemap & (1U << i)) == 0;
  if (t == 0)
    assert(empty);
  if (!empty)
    do_check_tree(m, t);
}

/*  Check all the chunks in a smallbin.  */
static void do_check_smallbin(mstate m, bindex_t i) {
  sbinptr b = smallbin_at(m, i);
  mchunkptr p = b->bk;
  unsigned int empty = (m->smallmap & (1U << i)) == 0;
  if (p == b)
    assert(empty);
  if (!empty) {
    for (; p != b; p = p->bk) {
      size_t size = chunksize(p);
      mchunkptr q;
      /* each chunk claims to be free */
      do_check_free_chunk(m, p);
      /* chunk belongs in bin */
      assert(small_index(size) == i);
      assert(p->bk == b || chunksize(p->bk) == chunksize(p));
      /* chunk is followed by an inuse chunk */
      q = next_chunk(p);
      if (q->head != FENCEPOST_HEAD)
        do_check_inuse_chunk(m, q);
    }
  }
}

/* Find x in a bin. Used in other check functions. */
static int bin_find(mstate m, mchunkptr x) {
  size_t size = chunksize(x);
  if (is_small(size)) {
    bindex_t sidx = small_index(size);
    sbinptr b = smallbin_at(m, sidx);
    if (smallmap_is_marked(m, sidx)) {
      mchunkptr p = b;
      do {
        if (p == x)
          return 1;
      } while ((p = p->fd) != b);
    }
  }
  else {
    bindex_t tidx;
    compute_tree_index(size, tidx);
    if (treemap_is_marked(m, tidx)) {
      tchunkptr t = *treebin_at(m, tidx);
      size_t sizebits = size << leftshift_for_tree_index(tidx);
      while (t != 0 && chunksize(t) != size) {
        t = t->child[(sizebits >> (SIZE_T_BITSIZE-SIZE_T_ONE)) & 1];
        sizebits <<= 1;
      }
      if (t != 0) {
        tchunkptr u = t;
        do {
          if (u == (tchunkptr)x)
            return 1;
        } while ((u = u->fd) != t);
      }
    }
  }
  return 0;
}

/* Traverse each chunk and check it; return total */
static size_t traverse_and_check(mstate m) {
  size_t sum = 0;
  if (is_initialized(m)) {
    msegmentptr s = &m->seg;
    sum += m->topsize + TOP_FOOT_SIZE;
    while (s != 0) {
      mchunkptr q = align_as_chunk(s->base);
      mchunkptr lastq = 0;
      assert(pinuse(q));
      while (segment_holds(s, q) &&
             q != m->top && q->head != FENCEPOST_HEAD) {
        sum += chunksize(q);
        if (cinuse(q)) {
          assert(!bin_find(m, q));
          do_check_inuse_chunk(m, q);
        }
        else {
          assert(q == m->dv || bin_find(m, q));
          assert(lastq == 0 || cinuse(lastq)); /* Not 2 consecutive free */
          do_check_free_chunk(m, q);
        }
        lastq = q;
        q = next_chunk(q);
      }
      s = s->next;
    }
  }
  return sum;
}

/* Check all properties of malloc_state. */
static void do_check_malloc_state(mstate m) {
  bindex_t i;
  size_t total;
  /* check bins */
  for (i = 0; i < NSMALLBINS; ++i)
    do_check_smallbin(m, i);
  for (i = 0; i < NTREEBINS; ++i)
    do_check_treebin(m, i);

  if (m->dvsize != 0) { /* check dv chunk */
    do_check_any_chunk(m, m->dv);
    assert(m->dvsize == chunksize(m->dv));
    assert(m->dvsize >= MIN_CHUNK_SIZE);
    assert(bin_find(m, m->dv) == 0);
  }

  if (m->top != 0) {   /* check top chunk */
    do_check_top_chunk(m, m->top);
    assert(m->topsize == chunksize(m->top));
    assert(m->topsize > 0);
    assert(bin_find(m, m->top) == 0);
  }

  total = traverse_and_check(m);
  assert(total <= m->footprint);
  assert(m->footprint <= m->max_footprint);
}
#endif /* DEBUG */

/* ----------------------------- statistics ------------------------------ */

#if !NO_MALLINFO
static struct mallinfo internal_mallinfo(mstate m) {
  struct mallinfo nm = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  if (!PREACTION(m)) {
    check_malloc_state(m);
    if (is_initialized(m)) {
      size_t nfree = SIZE_T_ONE; /* top always free */
      size_t mfree = m->topsize + TOP_FOOT_SIZE;
      size_t sum = mfree;
      msegmentptr s = &m->seg;
      while (s != 0) {
        mchunkptr q = align_as_chunk(s->base);
        while (segment_holds(s, q) &&
               q != m->top && q->head != FENCEPOST_HEAD) {
          size_t sz = chunksize(q);
          sum += sz;
          if (!cinuse(q)) {
            mfree += sz;
            ++nfree;
          }
          q = next_chunk(q);
        }
        s = s->next;
      }

      nm.arena    = sum;
      nm.ordblks  = nfree;
      nm.hblkhd   = m->footprint - sum;
      nm.usmblks  = m->max_footprint;
      nm.uordblks = m->footprint - mfree;
      nm.fordblks = mfree;
      nm.keepcost = m->topsize;
    }

    POSTACTION(m);
  }
  return nm;
}
#endif /* !NO_MALLINFO */

static void internal_malloc_stats(mstate m) {
  if (!PREACTION(m)) {
    size_t maxfp = 0;
    size_t fp = 0;
    size_t used = 0;
    check_malloc_state(m);
    if (is_initialized(m)) {
      msegmentptr s = &m->seg;
      maxfp = m->max_footprint;
      fp = m->footprint;
      used = fp - (m->topsize + TOP_FOOT_SIZE);

      while (s != 0) {
        mchunkptr q = align_as_chunk(s->base);
        while (segment_holds(s, q) &&
               q != m->top && q->head != FENCEPOST_HEAD) {
          if (!cinuse(q))
            used -= chunksize(q);
          q = next_chunk(q);
        }
        s = s->next;
      }
    }

#ifndef LACKS_STDIO_H
    fprintf(stderr, "max system bytes = %10lu\n", (unsigned long)(maxfp));
    fprintf(stderr, "system bytes     = %10lu\n", (unsigned long)(fp));
    fprintf(stderr, "in use bytes     = %10lu\n", (unsigned long)(used));
#endif

    POSTACTION(m);
  }
}

/* ----------------------- Operations on smallbins ----------------------- */


/* Link a free chunk into a smallbin  */
#define insert_small_chunk(M, P, S) {\
  bindex_t I  = small_index(S);\
  mchunkptr B = smallbin_at(M, I);\
  mchunkptr F = B;\
  assert(S >= MIN_CHUNK_SIZE);\
  if (!smallmap_is_marked(M, I))\
    mark_smallmap(M, I);\
  else if (RTCHECK(ok_address(M, B->fd)))\
    F = B->fd;\
  else {\
    CORRUPTION_ERROR_ACTION(M);\
  }\
  B->fd = P;\
  F->bk = P;\
  P->fd = F;\
  P->bk = B;\
}

/* Unlink a chunk from a smallbin  */
#define unlink_small_chunk(M, P, S) {\
  mchunkptr F = P->fd;\
  mchunkptr B = P->bk;\
  bindex_t I = small_index(S);\
  assert(P != B);\
  assert(P != F);\
  assert(chunksize(P) == small_index2size(I));\
  if (F == B)\
    clear_smallmap(M, I);\
  else if (RTCHECK((F == smallbin_at(M,I) || ok_address(M, F)) &&\
                   (B == smallbin_at(M,I) || ok_address(M, B)))) {\
    F->bk = B;\
    B->fd = F;\
  }\
  else {\
    CORRUPTION_ERROR_ACTION(M);\
  }\
}

/* Unlink the first chunk from a smallbin */
#define unlink_first_small_chunk(M, B, P, I) {\
  mchunkptr F = P->fd;\
  assert(P != B);\
  assert(P != F);\
  assert(chunksize(P) == small_index2size(I));\
  if (B == F)\
    clear_smallmap(M, I);\
  else if (RTCHECK(ok_address(M, F))) {\
    B->fd = F;\
    F->bk = B;\
  }\
  else {\
    CORRUPTION_ERROR_ACTION(M);\
  }\
}

/* Replace dv node, binning the old one */
/* Used only when dvsize known to be small */
#define replace_dv(M, P, S) {\
  size_t DVS = M->dvsize;\
  if (DVS != 0) {\
    mchunkptr DV = M->dv;\
    assert(is_small(DVS));\
    insert_small_chunk(M, DV, DVS);\
  }\
  M->dvsize = S;\
  M->dv = P;\
}

/* ------------------------- Operations on trees ------------------------- */

/* Insert chunk into tree */
#define insert_large_chunk(M, X, S) {\
  tbinptr* H;\
  bindex_t I;\
  compute_tree_index(S, I);\
  H = treebin_at(M, I);\
  X->index = I;\
  X->child[0] = X->child[1] = 0;\
  if (!treemap_is_marked(M, I)) {\
    mark_treemap(M, I);\
    *H = X;\
    X->parent = (tchunkptr)H;\
    X->fd = X->bk = X;\
  }\
  else {\
    tchunkptr T = *H;\
    size_t K = S << leftshift_for_tree_index(I);\
    for (;;) {\
      if (chunksize(T) != S) {\
        tchunkptr* C = &(T->child[(K >> (SIZE_T_BITSIZE-SIZE_T_ONE)) & 1]);\
        K <<= 1;\
        if (*C != 0)\
          T = *C;\
        else if (RTCHECK(ok_address(M, C))) {\
          *C = X;\
          X->parent = T;\
          X->fd = X->bk = X;\
          break;\
        }\
        else {\
          CORRUPTION_ERROR_ACTION(M);\
          break;\
        }\
      }\
      else {\
        tchunkptr F = T->fd;\
        if (RTCHECK(ok_address(M, T) && ok_address(M, F))) {\
          T->fd = F->bk = X;\
          X->fd = F;\
          X->bk = T;\
          X->parent = 0;\
          break;\
        }\
        else {\
          CORRUPTION_ERROR_ACTION(M);\
          break;\
        }\
      }\
    }\
  }\
}


#define unlink_large_chunk(M, X) {\
  tchunkptr XP = X->parent;\
  tchunkptr R;\
  if (X->bk != X) {\
    tchunkptr F = X->fd;\
    R = X->bk;\
    if (RTCHECK(ok_address(M, F))) {\
      F->bk = R;\
      R->fd = F;\
    }\
    else {\
      CORRUPTION_ERROR_ACTION(M);\
    }\
  }\
  else {\
    tchunkptr* RP;\
    if (((R = *(RP = &(X->child[1]))) != 0) ||\
        ((R = *(RP = &(X->child[0]))) != 0)) {\
      tchunkptr* CP;\
      while ((*(CP = &(R->child[1])) != 0) ||\
             (*(CP = &(R->child[0])) != 0)) {\
        R = *(RP = CP);\
      }\
      if (RTCHECK(ok_address(M, RP)))\
        *RP = 0;\
      else {\
        CORRUPTION_ERROR_ACTION(M);\
      }\
    }\
  }\
  if (XP != 0) {\
    tbinptr* H = treebin_at(M, X->index);\
    if (X == *H) {\
      if ((*H = R) == 0) \
        clear_treemap(M, X->index);\
    }\
    else if (RTCHECK(ok_address(M, XP))) {\
      if (XP->child[0] == X) \
        XP->child[0] = R;\
      else \
        XP->child[1] = R;\
    }\
    else\
      CORRUPTION_ERROR_ACTION(M);\
    if (R != 0) {\
      if (RTCHECK(ok_address(M, R))) {\
        tchunkptr C0, C1;\
        R->parent = XP;\
        if ((C0 = X->child[0]) != 0) {\
          if (RTCHECK(ok_address(M, C0))) {\
            R->child[0] = C0;\
            C0->parent = R;\
          }\
          else\
            CORRUPTION_ERROR_ACTION(M);\
        }\
        if ((C1 = X->child[1]) != 0) {\
          if (RTCHECK(ok_address(M, C1))) {\
            R->child[1] = C1;\
            C1->parent = R;\
          }\
          else\
            CORRUPTION_ERROR_ACTION(M);\
        }\
      }\
      else\
        CORRUPTION_ERROR_ACTION(M);\
    }\
  }\
}

/* Relays to large vs small bin operations */

#define insert_chunk(M, P, S)\
  if (is_small(S)) insert_small_chunk(M, P, S)\
  else { tchunkptr TP = (tchunkptr)(P); insert_large_chunk(M, TP, S); }

#define unlink_chunk(M, P, S)\
  if (is_small(S)) unlink_small_chunk(M, P, S)\
  else { tchunkptr TP = (tchunkptr)(P); unlink_large_chunk(M, TP); }


/* Relays to internal calls to malloc/free from realloc, memalign etc */

#if ONLY_MSPACES
#define internal_malloc(m, b) mspace_malloc(m, b)
#define internal_free(m, mem) mspace_free(m,mem);
#else /* ONLY_MSPACES */
#if MSPACES
#define internal_malloc(m, b)\
   (m == gm)? dlmalloc(b) : mspace_malloc(m, b)
#define internal_free(m, mem)\
   if (m == gm) dlfree(mem); else mspace_free(m,mem);
#else /* MSPACES */
#define internal_malloc(m, b) dlmalloc(b)
#define internal_free(m, mem) dlfree(mem)
#endif /* MSPACES */
#endif /* ONLY_MSPACES */

/* -----------------------  Direct-mmapping chunks ----------------------- */


/* Malloc using mmap */
static void* mmap_alloc(mstate m, size_t nb) {
  size_t mmsize = granularity_align(nb + SIX_SIZE_T_SIZES + CHUNK_ALIGN_MASK);
  if (mmsize > nb) {     /* Check for wrap around 0 */
    char* mm = (char*)(DIRECT_MMAP(mmsize));
    if (mm != CMFAIL) {
      size_t offset = align_offset(chunk2mem(mm));
      size_t psize = mmsize - offset - MMAP_FOOT_PAD;
      mchunkptr p = (mchunkptr)(mm + offset);
      p->prev_foot = offset | IS_MMAPPED_BIT;
      (p)->head = (psize|CINUSE_BIT);
      mark_inuse_foot(m, p, psize);
      chunk_plus_offset(p, psize)->head = FENCEPOST_HEAD;
      chunk_plus_offset(p, psize+SIZE_T_SIZE)->head = 0;

      if (mm < m->least_addr)
        m->least_addr = mm;
      if ((m->footprint += mmsize) > m->max_footprint)
        m->max_footprint = m->footprint;
      assert(is_aligned(chunk2mem(p)));
      check_mmapped_chunk(m, p);
      return chunk2mem(p);
    }
  }
  return 0;
}

/* Realloc using mmap */
static mchunkptr mmap_resize(mstate m, mchunkptr oldp, size_t nb) {
  size_t oldsize = chunksize(oldp);
  if (is_small(nb)) /* Can't shrink mmap regions below small size */
    return 0;
  /* Keep old chunk if big enough but not too big */
  if (oldsize >= nb + SIZE_T_SIZE &&
      (oldsize - nb) <= (mparams.granularity << 1))
    return oldp;
  else {
    size_t offset = oldp->prev_foot & ~IS_MMAPPED_BIT;
    size_t oldmmsize = oldsize + offset + MMAP_FOOT_PAD;
    size_t newmmsize = granularity_align(nb + SIX_SIZE_T_SIZES +
                                         CHUNK_ALIGN_MASK);
    char* cp = (char*)CALL_MREMAP((char*)oldp - offset,
                                  oldmmsize, newmmsize, 1);
    if (cp != CMFAIL) {
      mchunkptr newp = (mchunkptr)(cp + offset);
      size_t psize = newmmsize - offset - MMAP_FOOT_PAD;
      newp->head = (psize|CINUSE_BIT);
      mark_inuse_foot(m, newp, psize);
      chunk_plus_offset(newp, psize)->head = FENCEPOST_HEAD;
      chunk_plus_offset(newp, psize+SIZE_T_SIZE)->head = 0;

      if (cp < m->least_addr)
        m->least_addr = cp;
      if ((m->footprint += newmmsize - oldmmsize) > m->max_footprint)
        m->max_footprint = m->footprint;
      check_mmapped_chunk(m, newp);
      return newp;
    }
  }
  return 0;
}

/* -------------------------- mspace management -------------------------- */

/* Initialize top chunk and its size */
static void init_top(mstate m, mchunkptr p, size_t psize) {
  /* Ensure alignment */
  size_t offset = align_offset(chunk2mem(p));
  p = (mchunkptr)((char*)p + offset);
  psize -= offset;

  m->top = p;
  m->topsize = psize;
  p->head = psize | PINUSE_BIT;
  /* set size of fake trailing chunk holding overhead space only once */
  chunk_plus_offset(p, psize)->head = TOP_FOOT_SIZE;
  m->trim_check = mparams.trim_threshold; /* reset on each update */
}

/* Initialize bins for a new mstate that is otherwise zeroed out */
static void init_bins(mstate m) {
  /* Establish circular links for smallbins */
  bindex_t i;
  for (i = 0; i < NSMALLBINS; ++i) {
    sbinptr bin = smallbin_at(m,i);
    bin->fd = bin->bk = bin;
  }
}

#if PROCEED_ON_ERROR

/* default corruption action */
static void reset_on_error(mstate m) {
  int i;
  ++malloc_corruption_error_count;
  /* Reinitialize fields to forget about all memory */
  m->smallbins = m->treebins = 0;
  m->dvsize = m->topsize = 0;
  m->seg.base = 0;
  m->seg.size = 0;
  m->seg.next = 0;
  m->top = m->dv = 0;
  for (i = 0; i < NTREEBINS; ++i)
    *treebin_at(m, i) = 0;
  init_bins(m);
}
#endif /* PROCEED_ON_ERROR */

/* Allocate chunk and prepend remainder with chunk in successor base. */
static void* prepend_alloc(mstate m, char* newbase, char* oldbase,
                           size_t nb) {
  mchunkptr p = align_as_chunk(newbase);
  mchunkptr oldfirst = align_as_chunk(oldbase);
  size_t psize = (char*)oldfirst - (char*)p;
  mchunkptr q = chunk_plus_offset(p, nb);
  size_t qsize = psize - nb;
  set_size_and_pinuse_of_inuse_chunk(m, p, nb);

  assert((char*)oldfirst > (char*)q);
  assert(pinuse(oldfirst));
  assert(qsize >= MIN_CHUNK_SIZE);

  /* consolidate remainder with first chunk of old base */
  if (oldfirst == m->top) {
    size_t tsize = m->topsize += qsize;
    m->top = q;
    q->head = tsize | PINUSE_BIT;
    check_top_chunk(m, q);
  }
  else if (oldfirst == m->dv) {
    size_t dsize = m->dvsize += qsize;
    m->dv = q;
    set_size_and_pinuse_of_free_chunk(q, dsize);
  }
  else {
    if (!cinuse(oldfirst)) {
      size_t nsize = chunksize(oldfirst);
      unlink_chunk(m, oldfirst, nsize);
      oldfirst = chunk_plus_offset(oldfirst, nsize);
      qsize += nsize;
    }
    set_free_with_pinuse(q, qsize, oldfirst);
    insert_chunk(m, q, qsize);
    check_free_chunk(m, q);
  }

  check_malloced_chunk(m, chunk2mem(p), nb);
  return chunk2mem(p);
}


/* Add a segment to hold a new noncontiguous region */
static void add_segment(mstate m, char* tbase, size_t tsize, flag_t mmapped) {
  /* Determine locations and sizes of segment, fenceposts, old top */
  char* old_top = (char*)m->top;
  msegmentptr oldsp = segment_holding(m, old_top);
  char* old_end = oldsp->base + oldsp->size;
  size_t ssize = pad_request(sizeof(struct malloc_segment));
  char* rawsp = old_end - (ssize + FOUR_SIZE_T_SIZES + CHUNK_ALIGN_MASK);
  size_t offset = align_offset(chunk2mem(rawsp));
  char* asp = rawsp + offset;
  char* csp = (asp < (old_top + MIN_CHUNK_SIZE))? old_top : asp;
  mchunkptr sp = (mchunkptr)csp;
  msegmentptr ss = (msegmentptr)(chunk2mem(sp));
  mchunkptr tnext = chunk_plus_offset(sp, ssize);
  mchunkptr p = tnext;
  int nfences = 0;

  /* reset top to new space */
  init_top(m, (mchunkptr)tbase, tsize - TOP_FOOT_SIZE);

  /* Set up segment record */
  assert(is_aligned(ss));
  set_size_and_pinuse_of_inuse_chunk(m, sp, ssize);
  *ss = m->seg; /* Push current record */
  m->seg.base = tbase;
  m->seg.size = tsize;
  m->seg.sflags = mmapped;
  m->seg.next = ss;

  /* Insert trailing fenceposts */
  for (;;) {
    mchunkptr nextp = chunk_plus_offset(p, SIZE_T_SIZE);
    p->head = FENCEPOST_HEAD;
    ++nfences;
    if ((char*)(&(nextp->head)) < old_end)
      p = nextp;
    else
      break;
  }
  assert(nfences >= 2);

  /* Insert the rest of old top into a bin as an ordinary free chunk */
  if (csp != old_top) {
    mchunkptr q = (mchunkptr)old_top;
    size_t psize = csp - old_top;
    mchunkptr tn = chunk_plus_offset(q, psize);
    set_free_with_pinuse(q, psize, tn);
    insert_chunk(m, q, psize);
  }

  check_top_chunk(m, m->top);
}

/* -------------------------- System allocation -------------------------- */

/* Get memory from system using MORECORE or MMAP */
static void* sys_alloc(mstate m, size_t nb) {
  char* tbase = CMFAIL;
  size_t tsize = 0;
  flag_t mmap_flag = 0;

  init_mparams();

  /* Directly map large chunks */
  if (use_mmap(m) && nb >= mparams.mmap_threshold) {
    void* mem = mmap_alloc(m, nb);
    if (mem != 0)
      return mem;
  }

  /*
    Try getting memory in any of three ways (in most-preferred to
    least-preferred order):
    1. A call to MORECORE that can normally contiguously extend memory.
       (disabled if not MORECORE_CONTIGUOUS or not HAVE_MORECORE or
       or main space is mmapped or a previous contiguous call failed)
    2. A call to MMAP new space (disabled if not HAVE_MMAP).
       Note that under the default settings, if MORECORE is unable to
       fulfill a request, and HAVE_MMAP is true, then mmap is
       used as a noncontiguous system allocator. This is a useful backup
       strategy for systems with holes in address spaces -- in this case
       sbrk cannot contiguously expand the heap, but mmap may be able to
       find space.
    3. A call to MORECORE that cannot usually contiguously extend memory.
       (disabled if not HAVE_MORECORE)
  */

  if (MORECORE_CONTIGUOUS && !use_noncontiguous(m)) {
    char* br = CMFAIL;
    msegmentptr ss = (m->top == 0)? 0 : segment_holding(m, (char*)m->top);
    size_t asize = 0;
    ACQUIRE_MORECORE_LOCK();

    if (ss == 0) {  /* First time through or recovery */
      char* base = (char*)CALL_MORECORE(0);
      if (base != CMFAIL) {
        asize = granularity_align(nb + TOP_FOOT_SIZE + MALLOC_ALIGNMENT + SIZE_T_ONE);
        /* Adjust to end on a page boundary */
        if (!is_page_aligned(base))
          asize += (page_align((size_t)base) - (size_t)base);
        /* Can't call MORECORE if size is negative when treated as signed */
        if (asize < HALF_MAX_SIZE_T &&
            (br = (char*)(CALL_MORECORE(asize))) == base) {
          tbase = base;
          tsize = asize;
        }
      }
    }
    else {
      /* Subtract out existing available top space from MORECORE request. */
      asize = granularity_align(nb - m->topsize + TOP_FOOT_SIZE + MALLOC_ALIGNMENT + SIZE_T_ONE);
      /* Use mem here only if it did continuously extend old space */
      if (asize < HALF_MAX_SIZE_T &&
          (br = (char*)(CALL_MORECORE(asize))) == ss->base+ss->size) {
        tbase = br;
        tsize = asize;
      }
    }

    if (tbase == CMFAIL) {    /* Cope with partial failure */
      if (br != CMFAIL) {    /* Try to use/extend the space we did get */
        if (asize < HALF_MAX_SIZE_T &&
            asize < nb + TOP_FOOT_SIZE + SIZE_T_ONE) {
          size_t esize = granularity_align(nb + TOP_FOOT_SIZE + MALLOC_ALIGNMENT + SIZE_T_ONE - asize);
          if (esize < HALF_MAX_SIZE_T) {
            char* end = (char*)CALL_MORECORE(esize);
            if (end != CMFAIL)
              asize += esize;
            else {            /* Can't use; try to release */
              end = (char*)CALL_MORECORE(-asize);
              br = CMFAIL;
            }
          }
        }
      }
      if (br != CMFAIL) {    /* Use the space we did get */
        tbase = br;
        tsize = asize;
      }
      else
        disable_contiguous(m); /* Don't try contiguous path in the future */
    }

    RELEASE_MORECORE_LOCK();
  }

  if (HAVE_MMAP && tbase == CMFAIL) {  /* Try MMAP */
    size_t req = nb + TOP_FOOT_SIZE + MALLOC_ALIGNMENT + SIZE_T_ONE;
    size_t rsize = granularity_align(req);
    if (rsize > nb) { /* Fail if wraps around zero */
      char* mp = (char*)(CALL_MMAP(rsize));
      if (mp != CMFAIL) {
        tbase = mp;
        tsize = rsize;
        mmap_flag = IS_MMAPPED_BIT;
      }
    }
  }

  if (HAVE_MORECORE && tbase == CMFAIL) { /* Try noncontiguous MORECORE */
    size_t asize = granularity_align(nb + TOP_FOOT_SIZE + MALLOC_ALIGNMENT + SIZE_T_ONE);
    if (asize < HALF_MAX_SIZE_T) {
      char* br = CMFAIL;
      char* end = CMFAIL;
      ACQUIRE_MORECORE_LOCK();
      br = (char*)(CALL_MORECORE(asize));
      end = (char*)(CALL_MORECORE(0));
      RELEASE_MORECORE_LOCK();
      if (br != CMFAIL && end != CMFAIL && br < end) {
        size_t ssize = end - br;
        if (ssize > nb + TOP_FOOT_SIZE) {
          tbase = br;
          tsize = ssize;
        }
      }
    }
  }

  if (tbase != CMFAIL) {

    if ((m->footprint += tsize) > m->max_footprint)
      m->max_footprint = m->footprint;

    if (!is_initialized(m)) { /* first-time initialization */
      m->seg.base = m->least_addr = tbase;
      m->seg.size = tsize;
      m->seg.sflags = mmap_flag;
      m->magic = mparams.magic;
      init_bins(m);
      if (is_global(m)) 
        init_top(m, (mchunkptr)tbase, tsize - TOP_FOOT_SIZE);
      else {
        /* Offset top by embedded malloc_state */
        mchunkptr mn = next_chunk(mem2chunk(m));
        init_top(m, mn, (size_t)((tbase + tsize) - (char*)mn) -TOP_FOOT_SIZE);
      }
    }

    else {
      /* Try to merge with an existing segment */
      msegmentptr sp = &m->seg;
      while (sp != 0 && tbase != sp->base + sp->size)
        sp = sp->next;
      if (sp != 0 &&
          !is_extern_segment(sp) &&
          (sp->sflags & IS_MMAPPED_BIT) == mmap_flag &&
          segment_holds(sp, m->top)) { /* append */
        sp->size += tsize;
        init_top(m, m->top, m->topsize + tsize);
      }
      else {
        if (tbase < m->least_addr)
          m->least_addr = tbase;
        sp = &m->seg;
        while (sp != 0 && sp->base != tbase + tsize)
          sp = sp->next;
        if (sp != 0 &&
            !is_extern_segment(sp) &&
            (sp->sflags & IS_MMAPPED_BIT) == mmap_flag) {
          char* oldbase = sp->base;
          sp->base = tbase;
          sp->size += tsize;
          return prepend_alloc(m, tbase, oldbase, nb);
        }
        else
          add_segment(m, tbase, tsize, mmap_flag);
      }
    }

    if (nb < m->topsize) { /* Allocate from new or extended top space */
      size_t rsize = m->topsize -= nb;
      mchunkptr p = m->top;
      mchunkptr r = m->top = chunk_plus_offset(p, nb);
      r->head = rsize | PINUSE_BIT;
      set_size_and_pinuse_of_inuse_chunk(m, p, nb);
      check_top_chunk(m, m->top);
      check_malloced_chunk(m, chunk2mem(p), nb);
      return chunk2mem(p);
    }
  }

  MALLOC_FAILURE_ACTION;
  return 0;
}

/* -----------------------  system deallocation -------------------------- */

/* Unmap and unlink any mmapped segments that don't contain used chunks */
static size_t release_unused_segments(mstate m) {
  size_t released = 0;
  msegmentptr pred = &m->seg;
  msegmentptr sp = pred->next;
  while (sp != 0) {
    char* base = sp->base;
    size_t size = sp->size;
    msegmentptr next = sp->next;
    if (is_mmapped_segment(sp) && !is_extern_segment(sp)) {
      mchunkptr p = align_as_chunk(base);
      size_t psize = chunksize(p);
      /* Can unmap if first chunk holds entire segment and not pinned */
      if (!cinuse(p) && (char*)p + psize >= base + size - TOP_FOOT_SIZE) {
        tchunkptr tp = (tchunkptr)p;
        assert(segment_holds(sp, (char*)sp));
        if (p == m->dv) {
          m->dv = 0;
          m->dvsize = 0;
        }
        else {
          unlink_large_chunk(m, tp);
        }
        if (CALL_MUNMAP(base, size) == 0) {
          released += size;
          m->footprint -= size;
          /* unlink obsoleted record */
          sp = pred;
          sp->next = next;
        }
        else { /* back out if cannot unmap */
          insert_large_chunk(m, tp, psize);
        }
      }
    }
    pred = sp;
    sp = next;
  }
  return released;
}

static int sys_trim(mstate m, size_t pad) {
  size_t released = 0;
  if (pad < MAX_REQUEST && is_initialized(m)) {
    pad += TOP_FOOT_SIZE; /* ensure enough room for segment overhead */

    if (m->topsize > pad) {
      /* Shrink top space in granularity-size units, keeping at least one */
      size_t unit = mparams.granularity;
      size_t extra = ((m->topsize - pad + (unit - SIZE_T_ONE)) / unit -
                      SIZE_T_ONE) * unit;
      msegmentptr sp = segment_holding(m, (char*)m->top);

      if (!is_extern_segment(sp)) {
        if (is_mmapped_segment(sp)) {
          if (HAVE_MMAP &&
              sp->size >= extra &&
              !has_segment_link(m, sp)) { /* can't shrink if pinned */
            size_t newsize = sp->size - extra;
            /* Prefer mremap, fall back to munmap */
            if ((CALL_MREMAP(sp->base, sp->size, newsize, 0) != MFAIL) ||
                (CALL_MUNMAP(sp->base + newsize, extra) == 0)) {
              released = extra;
            }
          }
        }
        else if (HAVE_MORECORE) {
          if (extra >= HALF_MAX_SIZE_T) /* Avoid wrapping negative */
            extra = (HALF_MAX_SIZE_T) + SIZE_T_ONE - unit;
          ACQUIRE_MORECORE_LOCK();
          {
            /* Make sure end of memory is where we last set it. */
            char* old_br = (char*)(CALL_MORECORE(0));
            if (old_br == sp->base + sp->size) {
              char* rel_br = (char*)(CALL_MORECORE(-extra));
              char* new_br = (char*)(CALL_MORECORE(0));
              if (rel_br != CMFAIL && new_br < old_br)
                released = old_br - new_br;
            }
          }
          RELEASE_MORECORE_LOCK();
        }
      }

      if (released != 0) {
        sp->size -= released;
        m->footprint -= released;
        init_top(m, m->top, m->topsize - released);
        check_top_chunk(m, m->top);
      }
    }

    /* Unmap any unused mmapped segments */
    if (HAVE_MMAP) 
      released += release_unused_segments(m);

    /* On failure, disable autotrim to avoid repeated failed future calls */
    if (released == 0)
      m->trim_check = MAX_SIZE_T;
  }

  return (released != 0)? 1 : 0;
}

/* ---------------------------- malloc support --------------------------- */

/* allocate a large request from the best fitting chunk in a treebin */
static void* tmalloc_large(mstate m, size_t nb) {
  tchunkptr v = 0;
  size_t rsize = -nb; /* Unsigned negation */
  tchunkptr t;
  bindex_t idx;
  compute_tree_index(nb, idx);

  if ((t = *treebin_at(m, idx)) != 0) {
    /* Traverse tree for this bin looking for node with size == nb */
    size_t sizebits = nb << leftshift_for_tree_index(idx);
    tchunkptr rst = 0;  /* The deepest untaken right subtree */
    for (;;) {
      tchunkptr rt;
      size_t trem = chunksize(t) - nb;
      if (trem < rsize) {
        v = t;
        if ((rsize = trem) == 0)
          break;
      }
      rt = t->child[1];
      t = t->child[(sizebits >> (SIZE_T_BITSIZE-SIZE_T_ONE)) & 1];
      if (rt != 0 && rt != t)
        rst = rt;
      if (t == 0) {
        t = rst; /* set t to least subtree holding sizes > nb */
        break;
      }
      sizebits <<= 1;
    }
  }

  if (t == 0 && v == 0) { /* set t to root of next non-empty treebin */
    binmap_t leftbits = left_bits(idx2bit(idx)) & m->treemap;
    if (leftbits != 0) {
      bindex_t i;
      binmap_t leastbit = least_bit(leftbits);
      compute_bit2idx(leastbit, i);
      t = *treebin_at(m, i);
    }
  }

  while (t != 0) { /* find smallest of tree or subtree */
    size_t trem = chunksize(t) - nb;
    if (trem < rsize) {
      rsize = trem;
      v = t;
    }
    t = leftmost_child(t);
  }

  /*  If dv is a better fit, return 0 so malloc will use it */
  if (v != 0 && rsize < (size_t)(m->dvsize - nb)) {
    if (RTCHECK(ok_address(m, v))) { /* split */
      mchunkptr r = chunk_plus_offset(v, nb);
      assert(chunksize(v) == rsize + nb);
      if (RTCHECK(ok_next(v, r))) {
        unlink_large_chunk(m, v);
        if (rsize < MIN_CHUNK_SIZE)
          set_inuse_and_pinuse(m, v, (rsize + nb));
        else {
          set_size_and_pinuse_of_inuse_chunk(m, v, nb);
          set_size_and_pinuse_of_free_chunk(r, rsize);
          insert_chunk(m, r, rsize);
        }
        return chunk2mem(v);
      }
    }
    CORRUPTION_ERROR_ACTION(m);
  }
  return 0;
}

/* allocate a small request from the best fitting chunk in a treebin */
static void* tmalloc_small(mstate m, size_t nb) {
  tchunkptr t, v;
  size_t rsize;
  bindex_t i;
  binmap_t leastbit = least_bit(m->treemap);
  compute_bit2idx(leastbit, i);

  v = t = *treebin_at(m, i);
  rsize = chunksize(t) - nb;

  while ((t = leftmost_child(t)) != 0) {
    size_t trem = chunksize(t) - nb;
    if (trem < rsize) {
      rsize = trem;
      v = t;
    }
  }

  if (RTCHECK(ok_address(m, v))) {
    mchunkptr r = chunk_plus_offset(v, nb);
    assert(chunksize(v) == rsize + nb);
    if (RTCHECK(ok_next(v, r))) {
      unlink_large_chunk(m, v);
      if (rsize < MIN_CHUNK_SIZE)
        set_inuse_and_pinuse(m, v, (rsize + nb));
      else {
        set_size_and_pinuse_of_inuse_chunk(m, v, nb);
        set_size_and_pinuse_of_free_chunk(r, rsize);
        replace_dv(m, r, rsize);
      }
      return chunk2mem(v);
    }
  }

  CORRUPTION_ERROR_ACTION(m);
  return 0;
}

/* --------------------------- realloc support --------------------------- */

static void* internal_realloc(mstate m, void* oldmem, size_t bytes) {
  if (bytes >= MAX_REQUEST) {
    MALLOC_FAILURE_ACTION;
    return 0;
  }
  if (!PREACTION(m)) {
    mchunkptr oldp = mem2chunk(oldmem);
    size_t oldsize = chunksize(oldp);
    mchunkptr next = chunk_plus_offset(oldp, oldsize);
    mchunkptr newp = 0;
    void* extra = 0;

    /* Try to either shrink or extend into top. Else malloc-copy-free */

    if (RTCHECK(ok_address(m, oldp) && ok_cinuse(oldp) &&
                ok_next(oldp, next) && ok_pinuse(next))) {
      size_t nb = request2size(bytes);
      if (is_mmapped(oldp))
        newp = mmap_resize(m, oldp, nb);
      else if (oldsize >= nb) { /* already big enough */
        size_t rsize = oldsize - nb;
        newp = oldp;
        if (rsize >= MIN_CHUNK_SIZE) {
          mchunkptr remainder = chunk_plus_offset(newp, nb);
          set_inuse(m, newp, nb);
          set_inuse(m, remainder, rsize);
          extra = chunk2mem(remainder);
        }
      }
      else if (next == m->top && oldsize + m->topsize > nb) {
        /* Expand into top */
        size_t newsize = oldsize + m->topsize;
        size_t newtopsize = newsize - nb;
        mchunkptr newtop = chunk_plus_offset(oldp, nb);
        set_inuse(m, oldp, nb);
        newtop->head = newtopsize |PINUSE_BIT;
        m->top = newtop;
        m->topsize = newtopsize;
        newp = oldp;
      }
    }
    else {
      USAGE_ERROR_ACTION(m, oldmem);
      POSTACTION(m);
      return 0;
    }

    POSTACTION(m);

    if (newp != 0) {
      if (extra != 0) {
        internal_free(m, extra);
      }
      check_inuse_chunk(m, newp);
      return chunk2mem(newp);
    }
    else {
      void* newmem = internal_malloc(m, bytes);
      if (newmem != 0) {
        size_t oc = oldsize - overhead_for(oldp);
        memcpy(newmem, oldmem, (oc < bytes)? oc : bytes);
        internal_free(m, oldmem);
      }
      return newmem;
    }
  }
  return 0;
}

/* --------------------------- memalign support -------------------------- */

static void* internal_memalign(mstate m, size_t alignment, size_t bytes) {
  if (alignment <= MALLOC_ALIGNMENT)    /* Can just use malloc */
    return internal_malloc(m, bytes);
  if (alignment <  MIN_CHUNK_SIZE) /* must be at least a minimum chunk size */
    alignment = MIN_CHUNK_SIZE;
  if ((alignment & (alignment-SIZE_T_ONE)) != 0) {/* Ensure a power of 2 */
    size_t a = MALLOC_ALIGNMENT << 1;
    while (a < alignment) a <<= 1;
    alignment = a;
  }
  
  if (bytes >= MAX_REQUEST - alignment) {
    if (m != 0)  { /* Test isn't needed but avoids compiler warning */
      MALLOC_FAILURE_ACTION;
    }
  }
  else {
    size_t nb = request2size(bytes);
    size_t req = nb + alignment + MIN_CHUNK_SIZE - CHUNK_OVERHEAD;
    char* mem = (char*)internal_malloc(m, req);
    if (mem != 0) {
      void* leader = 0;
      void* trailer = 0;
      mchunkptr p = mem2chunk(mem);

      if (PREACTION(m)) return 0;
      if ((((size_t)(mem)) % alignment) != 0) { /* misaligned */
        /*
          Find an aligned spot inside chunk.  Since we need to give
          back leading space in a chunk of at least MIN_CHUNK_SIZE, if
          the first calculation places us at a spot with less than
          MIN_CHUNK_SIZE leader, we can move to the next aligned spot.
          We've allocated enough total room so that this is always
          possible.
        */
        char* br = (char*)mem2chunk((size_t)(((size_t)(mem +
                                                       alignment -
                                                       SIZE_T_ONE)) &
                                             -alignment));
        char* pos = ((size_t)(br - (char*)(p)) >= MIN_CHUNK_SIZE)?
          br : br+alignment;
        mchunkptr newp = (mchunkptr)pos;
        size_t leadsize = pos - (char*)(p);
        size_t newsize = chunksize(p) - leadsize;

        if (is_mmapped(p)) { /* For mmapped chunks, just adjust offset */
          newp->prev_foot = p->prev_foot + leadsize;
          newp->head = (newsize|CINUSE_BIT);
        }
        else { /* Otherwise, give back leader, use the rest */
          set_inuse(m, newp, newsize);
          set_inuse(m, p, leadsize);
          leader = chunk2mem(p);
        }
        p = newp;
      }

      /* Give back spare room at the end */
      if (!is_mmapped(p)) {
        size_t size = chunksize(p);
        if (size > nb + MIN_CHUNK_SIZE) {
          size_t remainder_size = size - nb;
          mchunkptr remainder = chunk_plus_offset(p, nb);
          set_inuse(m, p, nb);
          set_inuse(m, remainder, remainder_size);
          trailer = chunk2mem(remainder);
        }
      }

      assert (chunksize(p) >= nb);
      assert((((size_t)(chunk2mem(p))) % alignment) == 0);
      check_inuse_chunk(m, p);
      POSTACTION(m);
      if (leader != 0) {
        internal_free(m, leader);
      }
      if (trailer != 0) {
        internal_free(m, trailer);
      }
      return chunk2mem(p);
    }
  }
  return 0;
}

/* ------------------------ comalloc/coalloc support --------------------- */

static void** ialloc(mstate m,
                     size_t n_elements,
                     size_t* sizes,
                     int opts,
                     void* chunks[]) {
  /*
    This provides common support for independent_X routines, handling
    all of the combinations that can result.

    The opts arg has:
    bit 0 set if all elements are same size (using sizes[0])
    bit 1 set if elements should be zeroed
  */

  size_t    element_size;   /* chunksize of each element, if all same */
  size_t    contents_size;  /* total size of elements */
  size_t    array_size;     /* request size of pointer array */
  void*     mem;            /* malloced aggregate space */
  mchunkptr p;              /* corresponding chunk */
  size_t    remainder_size; /* remaining bytes while splitting */
  void**    marray;         /* either "chunks" or malloced ptr array */
  mchunkptr array_chunk;    /* chunk for malloced ptr array */
  flag_t    was_enabled;    /* to disable mmap */
  size_t    size;
  size_t    i;

  /* compute array length, if needed */
  if (chunks != 0) {
    if (n_elements == 0)
      return chunks; /* nothing to do */
    marray = chunks;
    array_size = 0;
  }
  else {
    /* if empty req, must still return chunk representing empty array */
    if (n_elements == 0)
      return (void**)internal_malloc(m, 0);
    marray = 0;
    array_size = request2size(n_elements * (sizeof(void*)));
  }

  /* compute total element size */
  if (opts & 0x1) { /* all-same-size */
    element_size = request2size(*sizes);
    contents_size = n_elements * element_size;
  }
  else { /* add up all the sizes */
    element_size = 0;
    contents_size = 0;
    for (i = 0; i != n_elements; ++i)
      contents_size += request2size(sizes[i]);
  }

  size = contents_size + array_size;

  /*
     Allocate the aggregate chunk.  First disable direct-mmapping so
     malloc won't use it, since we would not be able to later
     free/realloc space internal to a segregated mmap region.
  */
  was_enabled = use_mmap(m);
  disable_mmap(m);
  mem = internal_malloc(m, size - CHUNK_OVERHEAD);
  if (was_enabled)
    enable_mmap(m);
  if (mem == 0)
    return 0;

  if (PREACTION(m)) return 0;
  p = mem2chunk(mem);
  remainder_size = chunksize(p);

  assert(!is_mmapped(p));

  if (opts & 0x2) {       /* optionally clear the elements */
    memset((size_t*)mem, 0, remainder_size - SIZE_T_SIZE - array_size);
  }

  /* If not provided, allocate the pointer array as final part of chunk */
  if (marray == 0) {
    size_t  array_chunk_size;
    array_chunk = chunk_plus_offset(p, contents_size);
    array_chunk_size = remainder_size - contents_size;
    marray = (void**) (chunk2mem(array_chunk));
    set_size_and_pinuse_of_inuse_chunk(m, array_chunk, array_chunk_size);
    remainder_size = contents_size;
  }

  /* split out elements */
  for (i = 0; ; ++i) {
    marray[i] = chunk2mem(p);
    if (i != n_elements-1) {
      if (element_size != 0)
        size = element_size;
      else
        size = request2size(sizes[i]);
      remainder_size -= size;
      set_size_and_pinuse_of_inuse_chunk(m, p, size);
      p = chunk_plus_offset(p, size);
    }
    else { /* the final element absorbs any overallocation slop */
      set_size_and_pinuse_of_inuse_chunk(m, p, remainder_size);
      break;
    }
  }

#if DEBUG
  if (marray != chunks) {
    /* final element must have exactly exhausted chunk */
    if (element_size != 0) {
      assert(remainder_size == element_size);
    }
    else {
      assert(remainder_size == request2size(sizes[i]));
    }
    check_inuse_chunk(m, mem2chunk(marray));
  }
  for (i = 0; i != n_elements; ++i)
    check_inuse_chunk(m, mem2chunk(marray[i]));

#endif /* DEBUG */

  POSTACTION(m);
  return marray;
}


/* -------------------------- public routines ---------------------------- */

#if !ONLY_MSPACES

void* dlmalloc(size_t bytes) {
  /*
     Basic algorithm:
     If a small request (< 256 bytes minus per-chunk overhead):
       1. If one exists, use a remainderless chunk in associated smallbin.
          (Remainderless means that there are too few excess bytes to
          represent as a chunk.)
       2. If it is big enough, use the dv chunk, which is normally the
          chunk adjacent to the one used for the most recent small request.
       3. If one exists, split the smallest available chunk in a bin,
          saving remainder in dv.
       4. If it is big enough, use the top chunk.
       5. If available, get memory from system and use it
     Otherwise, for a large request:
       1. Find the smallest available binned chunk that fits, and use it
          if it is better fitting than dv chunk, splitting if necessary.
       2. If better fitting than any binned chunk, use the dv chunk.
       3. If it is big enough, use the top chunk.
       4. If request size >= mmap threshold, try to directly mmap this chunk.
       5. If available, get memory from system and use it

     The ugly goto's here ensure that postaction occurs along all paths.
  */

  if (!PREACTION(gm)) {
    void* mem;
    size_t nb;
    if (bytes <= MAX_SMALL_REQUEST) {
      bindex_t idx;
      binmap_t smallbits;
      nb = (bytes < MIN_REQUEST)? MIN_CHUNK_SIZE : pad_request(bytes);
      idx = small_index(nb);
      smallbits = gm->smallmap >> idx;

      if ((smallbits & 0x3U) != 0) { /* Remainderless fit to a smallbin. */
        mchunkptr b, p;
        idx += ~smallbits & 1;       /* Uses next bin if idx empty */
        b = smallbin_at(gm, idx);
        p = b->fd;
        assert(chunksize(p) == small_index2size(idx));
        unlink_first_small_chunk(gm, b, p, idx);
        set_inuse_and_pinuse(gm, p, small_index2size(idx));
        mem = chunk2mem(p);
        check_malloced_chunk(gm, mem, nb);
        goto postaction;
      }

      else if (nb > gm->dvsize) {
        if (smallbits != 0) { /* Use chunk in next nonempty smallbin */
          mchunkptr b, p, r;
          size_t rsize;
          bindex_t i;
          binmap_t leftbits = (smallbits << idx) & left_bits(idx2bit(idx));
          binmap_t leastbit = least_bit(leftbits);
          compute_bit2idx(leastbit, i);
          b = smallbin_at(gm, i);
          p = b->fd;
          assert(chunksize(p) == small_index2size(i));
          unlink_first_small_chunk(gm, b, p, i);
          rsize = small_index2size(i) - nb;
          /* Fit here cannot be remainderless if 4byte sizes */
          if (SIZE_T_SIZE != 4 && rsize < MIN_CHUNK_SIZE)
            set_inuse_and_pinuse(gm, p, small_index2size(i));
          else {
            set_size_and_pinuse_of_inuse_chunk(gm, p, nb);
            r = chunk_plus_offset(p, nb);
            set_size_and_pinuse_of_free_chunk(r, rsize);
            replace_dv(gm, r, rsize);
          }
          mem = chunk2mem(p);
          check_malloced_chunk(gm, mem, nb);
          goto postaction;
        }

        else if (gm->treemap != 0 && (mem = tmalloc_small(gm, nb)) != 0) {
          check_malloced_chunk(gm, mem, nb);
          goto postaction;
        }
      }
    }
    else if (bytes >= MAX_REQUEST)
      nb = MAX_SIZE_T; /* Too big to allocate. Force failure (in sys alloc) */
    else {
      nb = pad_request(bytes);
      if (gm->treemap != 0 && (mem = tmalloc_large(gm, nb)) != 0) {
        check_malloced_chunk(gm, mem, nb);
        goto postaction;
      }
    }

    if (nb <= gm->dvsize) {
      size_t rsize = gm->dvsize - nb;
      mchunkptr p = gm->dv;
      if (rsize >= MIN_CHUNK_SIZE) { /* split dv */
        mchunkptr r = gm->dv = chunk_plus_offset(p, nb);
        gm->dvsize = rsize;
        set_size_and_pinuse_of_free_chunk(r, rsize);
        set_size_and_pinuse_of_inuse_chunk(gm, p, nb);
      }
      else { /* exhaust dv */
        size_t dvs = gm->dvsize;
        gm->dvsize = 0;
        gm->dv = 0;
        set_inuse_and_pinuse(gm, p, dvs);
      }
      mem = chunk2mem(p);
      check_malloced_chunk(gm, mem, nb);
      goto postaction;
    }

    else if (nb < gm->topsize) { /* Split top */
      size_t rsize = gm->topsize -= nb;
      mchunkptr p = gm->top;
      mchunkptr r = gm->top = chunk_plus_offset(p, nb);
      r->head = rsize | PINUSE_BIT;
      set_size_and_pinuse_of_inuse_chunk(gm, p, nb);
      mem = chunk2mem(p);
      check_top_chunk(gm, gm->top);
      check_malloced_chunk(gm, mem, nb);
      goto postaction;
    }

    mem = sys_alloc(gm, nb);

  postaction:
    POSTACTION(gm);
    return mem;
  }

  return 0;
}

void dlfree(void* mem) {
  /*
     Consolidate freed chunks with preceeding or succeeding bordering
     free chunks, if they exist, and then place in a bin.  Intermixed
     with special cases for top, dv, mmapped chunks, and usage errors.
  */

  if (mem != 0) {
    mchunkptr p  = mem2chunk(mem);
#if FOOTERS
    mstate fm = get_mstate_for(p);
    if (!ok_magic(fm)) {
      USAGE_ERROR_ACTION(fm, p);
      return;
    }
#else /* FOOTERS */
#define fm gm
#endif /* FOOTERS */
    if (!PREACTION(fm)) {
      check_inuse_chunk(fm, p);
      if (RTCHECK(ok_address(fm, p) && ok_cinuse(p))) {
        size_t psize = chunksize(p);
        mchunkptr next = chunk_plus_offset(p, psize);
        if (!pinuse(p)) {
          size_t prevsize = p->prev_foot;
          if ((prevsize & IS_MMAPPED_BIT) != 0) {
            prevsize &= ~IS_MMAPPED_BIT;
            psize += prevsize + MMAP_FOOT_PAD;
            if (CALL_MUNMAP((char*)p - prevsize, psize) == 0)
              fm->footprint -= psize;
            goto postaction;
          }
          else {
            mchunkptr prev = chunk_minus_offset(p, prevsize);
            psize += prevsize;
            p = prev;
            if (RTCHECK(ok_address(fm, prev))) { /* consolidate backward */
              if (p != fm->dv) {
                unlink_chunk(fm, p, prevsize);
              }
              else if ((next->head & INUSE_BITS) == INUSE_BITS) {
                fm->dvsize = psize;
                set_free_with_pinuse(p, psize, next);
                goto postaction;
              }
            }
            else
              goto erroraction;
          }
        }

        if (RTCHECK(ok_next(p, next) && ok_pinuse(next))) {
          if (!cinuse(next)) {  /* consolidate forward */
            if (next == fm->top) {
              size_t tsize = fm->topsize += psize;
              fm->top = p;
              p->head = tsize | PINUSE_BIT;
              if (p == fm->dv) {
                fm->dv = 0;
                fm->dvsize = 0;
              }
              if (should_trim(fm, tsize))
                sys_trim(fm, 0);
              goto postaction;
            }
            else if (next == fm->dv) {
              size_t dsize = fm->dvsize += psize;
              fm->dv = p;
              set_size_and_pinuse_of_free_chunk(p, dsize);
              goto postaction;
            }
            else {
              size_t nsize = chunksize(next);
              psize += nsize;
              unlink_chunk(fm, next, nsize);
              set_size_and_pinuse_of_free_chunk(p, psize);
              if (p == fm->dv) {
                fm->dvsize = psize;
                goto postaction;
              }
            }
          }
          else
            set_free_with_pinuse(p, psize, next);
          insert_chunk(fm, p, psize);
          check_free_chunk(fm, p);
          goto postaction;
        }
      }
    erroraction:
      USAGE_ERROR_ACTION(fm, p);
    postaction:
      POSTACTION(fm);
    }
  }
#if !FOOTERS
#undef fm
#endif /* FOOTERS */
}

void* dlcalloc(size_t n_elements, size_t elem_size) {
  void* mem;
  size_t req = 0;
  if (n_elements != 0) {
    req = n_elements * elem_size;
    if (((n_elements | elem_size) & ~(size_t)0xffff) &&
        (req / n_elements != elem_size))
      req = MAX_SIZE_T; /* force downstream failure on overflow */
  }
  mem = dlmalloc(req);
  if (mem != 0 && calloc_must_clear(mem2chunk(mem)))
    memset(mem, 0, req);
  return mem;
}

void* dlrealloc(void* oldmem, size_t bytes) {
  if (oldmem == 0)
    return dlmalloc(bytes);
#ifdef REALLOC_ZERO_BYTES_FREES
  if (bytes == 0) {
    dlfree(oldmem);
    return 0;
  }
#endif /* REALLOC_ZERO_BYTES_FREES */
  else {
#if ! FOOTERS
    mstate m = gm;
#else /* FOOTERS */
    mstate m = get_mstate_for(mem2chunk(oldmem));
    if (!ok_magic(m)) {
      USAGE_ERROR_ACTION(m, oldmem);
      return 0;
    }
#endif /* FOOTERS */
    return internal_realloc(m, oldmem, bytes);
  }
}

void* dlmemalign(size_t alignment, size_t bytes) {
  return internal_memalign(gm, alignment, bytes);
}

void** dlindependent_calloc(size_t n_elements, size_t elem_size,
                                 void* chunks[]) {
  size_t sz = elem_size; /* serves as 1-element array */
  return ialloc(gm, n_elements, &sz, 3, chunks);
}

void** dlindependent_comalloc(size_t n_elements, size_t sizes[],
                                   void* chunks[]) {
  return ialloc(gm, n_elements, sizes, 0, chunks);
}

void* dlvalloc(size_t bytes) {
  size_t pagesz;
  init_mparams();
  pagesz = mparams.page_size;
  return dlmemalign(pagesz, bytes);
}

void* dlpvalloc(size_t bytes) {
  size_t pagesz;
  init_mparams();
  pagesz = mparams.page_size;
  return dlmemalign(pagesz, (bytes + pagesz - SIZE_T_ONE) & ~(pagesz - SIZE_T_ONE));
}

int dlmalloc_trim(size_t pad) {
  int result = 0;
  if (!PREACTION(gm)) {
    result = sys_trim(gm, pad);
    POSTACTION(gm);
  }
  return result;
}

size_t dlmalloc_footprint(void) {
  return gm->footprint;
}

size_t dlmalloc_max_footprint(void) {
  return gm->max_footprint;
}

#if !NO_MALLINFO
struct mallinfo dlmallinfo(void) {
  return internal_mallinfo(gm);
}
#endif /* NO_MALLINFO */

void dlmalloc_stats() {
  internal_malloc_stats(gm);
}

size_t dlmalloc_usable_size(void* mem) {
  if (mem != 0) {
    mchunkptr p = mem2chunk(mem);
    if (cinuse(p))
      return chunksize(p) - overhead_for(p);
  }
  return 0;
}

int dlmallopt(int param_number, int value) {
  return change_mparam(param_number, value);
}

#endif /* !ONLY_MSPACES */

/* ----------------------------- user mspaces ---------------------------- */

#if MSPACES

static mstate init_user_mstate(char* tbase, size_t tsize) {
  size_t msize = pad_request(sizeof(struct malloc_state));
  mchunkptr mn;
  mchunkptr msp = align_as_chunk(tbase);
  mstate m = (mstate)(chunk2mem(msp));
  memset(m, 0, msize);
  INITIAL_LOCK(&m->mutex);
  msp->head = (msize|PINUSE_BIT|CINUSE_BIT);
  m->seg.base = m->least_addr = tbase;
  m->seg.size = m->footprint = m->max_footprint = tsize;
  m->magic = mparams.magic;
  m->mflags = mparams.default_mflags;
  disable_contiguous(m);
  init_bins(m);
  mn = next_chunk(mem2chunk(m));
  init_top(m, mn, (size_t)((tbase + tsize) - (char*)mn) - TOP_FOOT_SIZE);
  check_top_chunk(m, m->top);
  return m;
}

mspace create_mspace(size_t capacity, int locked) {
  mstate m = 0;
  size_t msize = pad_request(sizeof(struct malloc_state));
  init_mparams(); /* Ensure pagesize etc initialized */

  if (capacity < (size_t) -(msize + TOP_FOOT_SIZE + mparams.page_size)) {
    size_t rs = ((capacity == 0)? mparams.granularity :
                 (capacity + TOP_FOOT_SIZE + msize));
    size_t tsize = granularity_align(rs);
    char* tbase = (char*)(CALL_MMAP(tsize));
    if (tbase != CMFAIL) {
      m = init_user_mstate(tbase, tsize);
      m->seg.sflags = IS_MMAPPED_BIT;
      set_lock(m, locked);
    }
  }
  return (mspace)m;
}

mspace create_mspace_with_base(void* base, size_t capacity, int locked) {
  mstate m = 0;
  size_t msize = pad_request(sizeof(struct malloc_state));
  init_mparams(); /* Ensure pagesize etc initialized */

  if (capacity > msize + TOP_FOOT_SIZE &&
      capacity < (size_t) -(msize + TOP_FOOT_SIZE + mparams.page_size)) {
    m = init_user_mstate((char*)base, capacity);
    m->seg.sflags = EXTERN_BIT;
    set_lock(m, locked);
  }
  return (mspace)m;
}

size_t destroy_mspace(mspace msp) {
  size_t freed = 0;
  mstate ms = (mstate)msp;
  if (ok_magic(ms)) {
    msegmentptr sp = &ms->seg;
    while (sp != 0) {
      char* base = sp->base;
      size_t size = sp->size;
      flag_t flag = sp->sflags;
      sp = sp->next;
      if ((flag & IS_MMAPPED_BIT) && !(flag & EXTERN_BIT) &&
          CALL_MUNMAP(base, size) == 0)
        freed += size;
    }
  }
  else {
    USAGE_ERROR_ACTION(ms,ms);
  }
  return freed;
}



void* mspace_malloc(mspace msp, size_t bytes) {
  mstate ms = (mstate)msp;
  if (!ok_magic(ms)) {
    USAGE_ERROR_ACTION(ms,ms);
    return 0;
  }
  if (!PREACTION(ms)) {
    void* mem;
    size_t nb;
    if (bytes <= MAX_SMALL_REQUEST) {
      bindex_t idx;
      binmap_t smallbits;
      nb = (bytes < MIN_REQUEST)? MIN_CHUNK_SIZE : pad_request(bytes);
      idx = small_index(nb);
      smallbits = ms->smallmap >> idx;

      if ((smallbits & 0x3U) != 0) { /* Remainderless fit to a smallbin. */
        mchunkptr b, p;
        idx += ~smallbits & 1;       /* Uses next bin if idx empty */
        b = smallbin_at(ms, idx);
        p = b->fd;
        assert(chunksize(p) == small_index2size(idx));
        unlink_first_small_chunk(ms, b, p, idx);
        set_inuse_and_pinuse(ms, p, small_index2size(idx));
        mem = chunk2mem(p);
        check_malloced_chunk(ms, mem, nb);
        goto postaction;
      }

      else if (nb > ms->dvsize) {
        if (smallbits != 0) { /* Use chunk in next nonempty smallbin */
          mchunkptr b, p, r;
          size_t rsize;
          bindex_t i;
          binmap_t leftbits = (smallbits << idx) & left_bits(idx2bit(idx));
          binmap_t leastbit = least_bit(leftbits);
          compute_bit2idx(leastbit, i);
          b = smallbin_at(ms, i);
          p = b->fd;
          assert(chunksize(p) == small_index2size(i));
          unlink_first_small_chunk(ms, b, p, i);
          rsize = small_index2size(i) - nb;
          /* Fit here cannot be remainderless if 4byte sizes */
          if (SIZE_T_SIZE != 4 && rsize < MIN_CHUNK_SIZE)
            set_inuse_and_pinuse(ms, p, small_index2size(i));
          else {
            set_size_and_pinuse_of_inuse_chunk(ms, p, nb);
            r = chunk_plus_offset(p, nb);
            set_size_and_pinuse_of_free_chunk(r, rsize);
            replace_dv(ms, r, rsize);
          }
          mem = chunk2mem(p);
          check_malloced_chunk(ms, mem, nb);
          goto postaction;
        }

        else if (ms->treemap != 0 && (mem = tmalloc_small(ms, nb)) != 0) {
          check_malloced_chunk(ms, mem, nb);
          goto postaction;
        }
      }
    }
    else if (bytes >= MAX_REQUEST)
      nb = MAX_SIZE_T; /* Too big to allocate. Force failure (in sys alloc) */
    else {
      nb = pad_request(bytes);
      if (ms->treemap != 0 && (mem = tmalloc_large(ms, nb)) != 0) {
        check_malloced_chunk(ms, mem, nb);
        goto postaction;
      }
    }

    if (nb <= ms->dvsize) {
      size_t rsize = ms->dvsize - nb;
      mchunkptr p = ms->dv;
      if (rsize >= MIN_CHUNK_SIZE) { /* split dv */
        mchunkptr r = ms->dv = chunk_plus_offset(p, nb);
        ms->dvsize = rsize;
        set_size_and_pinuse_of_free_chunk(r, rsize);
        set_size_and_pinuse_of_inuse_chunk(ms, p, nb);
      }
      else { /* exhaust dv */
        size_t dvs = ms->dvsize;
        ms->dvsize = 0;
        ms->dv = 0;
        set_inuse_and_pinuse(ms, p, dvs);
      }
      mem = chunk2mem(p);
      check_malloced_chunk(ms, mem, nb);
      goto postaction;
    }

    else if (nb < ms->topsize) { /* Split top */
      size_t rsize = ms->topsize -= nb;
      mchunkptr p = ms->top;
      mchunkptr r = ms->top = chunk_plus_offset(p, nb);
      r->head = rsize | PINUSE_BIT;
      set_size_and_pinuse_of_inuse_chunk(ms, p, nb);
      mem = chunk2mem(p);
      check_top_chunk(ms, ms->top);
      check_malloced_chunk(ms, mem, nb);
      goto postaction;
    }

    mem = sys_alloc(ms, nb);

  postaction:
    POSTACTION(ms);
    return mem;
  }

  return 0;
}

void mspace_free(mspace msp, void* mem) {
  if (mem != 0) {
    mchunkptr p  = mem2chunk(mem);
#if FOOTERS
    mstate fm = get_mstate_for(p);
#else /* FOOTERS */
    mstate fm = (mstate)msp;
#endif /* FOOTERS */
    if (!ok_magic(fm)) {
      USAGE_ERROR_ACTION(fm, p);
      return;
    }
    if (!PREACTION(fm)) {
      check_inuse_chunk(fm, p);
      if (RTCHECK(ok_address(fm, p) && ok_cinuse(p))) {
        size_t psize = chunksize(p);
        mchunkptr next = chunk_plus_offset(p, psize);
        if (!pinuse(p)) {
          size_t prevsize = p->prev_foot;
          if ((prevsize & IS_MMAPPED_BIT) != 0) {
            prevsize &= ~IS_MMAPPED_BIT;
            psize += prevsize + MMAP_FOOT_PAD;
            if (CALL_MUNMAP((char*)p - prevsize, psize) == 0)
              fm->footprint -= psize;
            goto postaction;
          }
          else {
            mchunkptr prev = chunk_minus_offset(p, prevsize);
            psize += prevsize;
            p = prev;
            if (RTCHECK(ok_address(fm, prev))) { /* consolidate backward */
              if (p != fm->dv) {
                unlink_chunk(fm, p, prevsize);
              }
              else if ((next->head & INUSE_BITS) == INUSE_BITS) {
                fm->dvsize = psize;
                set_free_with_pinuse(p, psize, next);
                goto postaction;
              }
            }
            else
              goto erroraction;
          }
        }

        if (RTCHECK(ok_next(p, next) && ok_pinuse(next))) {
          if (!cinuse(next)) {  /* consolidate forward */
            if (next == fm->top) {
              size_t tsize = fm->topsize += psize;
              fm->top = p;
              p->head = tsize | PINUSE_BIT;
              if (p == fm->dv) {
                fm->dv = 0;
                fm->dvsize = 0;
              }
              if (should_trim(fm, tsize))
                sys_trim(fm, 0);
              goto postaction;
            }
            else if (next == fm->dv) {
              size_t dsize = fm->dvsize += psize;
              fm->dv = p;
              set_size_and_pinuse_of_free_chunk(p, dsize);
              goto postaction;
            }
            else {
              size_t nsize = chunksize(next);
              psize += nsize;
              unlink_chunk(fm, next, nsize);
              set_size_and_pinuse_of_free_chunk(p, psize);
              if (p == fm->dv) {
                fm->dvsize = psize;
                goto postaction;
              }
            }
          }
          else
            set_free_with_pinuse(p, psize, next);
          insert_chunk(fm, p, psize);
          check_free_chunk(fm, p);
          goto postaction;
        }
      }
    erroraction:
      USAGE_ERROR_ACTION(fm, p);
    postaction:
      POSTACTION(fm);
    }
  }
}

void* mspace_calloc(mspace msp, size_t n_elements, size_t elem_size) {
  void* mem;
  size_t req = 0;
  mstate ms = (mstate)msp;
  if (!ok_magic(ms)) {
    USAGE_ERROR_ACTION(ms,ms);
    return 0;
  }
  if (n_elements != 0) {
    req = n_elements * elem_size;
    if (((n_elements | elem_size) & ~(size_t)0xffff) &&
        (req / n_elements != elem_size))
      req = MAX_SIZE_T; /* force downstream failure on overflow */
  }
  mem = internal_malloc(ms, req);
  if (mem != 0 && calloc_must_clear(mem2chunk(mem)))
    memset(mem, 0, req);
  return mem;
}

void* mspace_realloc(mspace msp, void* oldmem, size_t bytes) {
  if (oldmem == 0)
    return mspace_malloc(msp, bytes);
#ifdef REALLOC_ZERO_BYTES_FREES
  if (bytes == 0) {
    mspace_free(msp, oldmem);
    return 0;
  }
#endif /* REALLOC_ZERO_BYTES_FREES */
  else {
#if FOOTERS
    mchunkptr p  = mem2chunk(oldmem);
    mstate ms = get_mstate_for(p);
#else /* FOOTERS */
    mstate ms = (mstate)msp;
#endif /* FOOTERS */
    if (!ok_magic(ms)) {
      USAGE_ERROR_ACTION(ms,ms);
      return 0;
    }
    return internal_realloc(ms, oldmem, bytes);
  }
}

void* mspace_memalign(mspace msp, size_t alignment, size_t bytes) {
  mstate ms = (mstate)msp;
  if (!ok_magic(ms)) {
    USAGE_ERROR_ACTION(ms,ms);
    return 0;
  }
  return internal_memalign(ms, alignment, bytes);
}

void** mspace_independent_calloc(mspace msp, size_t n_elements,
                                 size_t elem_size, void* chunks[]) {
  size_t sz = elem_size; /* serves as 1-element array */
  mstate ms = (mstate)msp;
  if (!ok_magic(ms)) {
    USAGE_ERROR_ACTION(ms,ms);
    return 0;
  }
  return ialloc(ms, n_elements, &sz, 3, chunks);
}

void** mspace_independent_comalloc(mspace msp, size_t n_elements,
                                   size_t sizes[], void* chunks[]) {
  mstate ms = (mstate)msp;
  if (!ok_magic(ms)) {
    USAGE_ERROR_ACTION(ms,ms);
    return 0;
  }
  return ialloc(ms, n_elements, sizes, 0, chunks);
}

int mspace_trim(mspace msp, size_t pad) {
  int result = 0;
  mstate ms = (mstate)msp;
  if (ok_magic(ms)) {
    if (!PREACTION(ms)) {
      result = sys_trim(ms, pad);
      POSTACTION(ms);
    }
  }
  else {
    USAGE_ERROR_ACTION(ms,ms);
  }
  return result;
}

void mspace_malloc_stats(mspace msp) {
  mstate ms = (mstate)msp;
  if (ok_magic(ms)) {
    internal_malloc_stats(ms);
  }
  else {
    USAGE_ERROR_ACTION(ms,ms);
  }
}

size_t mspace_footprint(mspace msp) {
  size_t result;
  mstate ms = (mstate)msp;
  if (ok_magic(ms)) {
    result = ms->footprint;
  }
  USAGE_ERROR_ACTION(ms,ms);
  return result;
}


size_t mspace_max_footprint(mspace msp) {
  size_t result;
  mstate ms = (mstate)msp;
  if (ok_magic(ms)) {
    result = ms->max_footprint;
  }
  USAGE_ERROR_ACTION(ms,ms);
  return result;
}


#if !NO_MALLINFO
struct mallinfo mspace_mallinfo(mspace msp) {
  mstate ms = (mstate)msp;
  if (!ok_magic(ms)) {
    USAGE_ERROR_ACTION(ms,ms);
  }
  return internal_mallinfo(ms);
}
#endif /* NO_MALLINFO */

int mspace_mallopt(int param_number, int value) {
  return change_mparam(param_number, value);
}

#endif /* MSPACES */

/* -------------------- Alternative MORECORE functions ------------------- */




#endif /* !HAVE_MALLOC */
