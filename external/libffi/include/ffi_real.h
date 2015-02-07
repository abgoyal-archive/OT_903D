// android -- copy of liffi.h.in with '@' values replaced


#ifndef LIBFFI_H
#define LIBFFI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Specify which architecture libffi is configured for. */
//#ifndef @TARGET@          // android changed
//#define @TARGET@          // android changed
//#endif                    // android changed

/* ---- System configuration information --------------------------------- */

//#include <ffitarget.h>        // android changed -- included earlier

#ifndef LIBFFI_ASM

#include <stddef.h>
#include <limits.h>

#ifdef LONG_LONG_MAX
# define FFI_LONG_LONG_MAX LONG_LONG_MAX
#else
# ifdef LLONG_MAX
#  define FFI_LONG_LONG_MAX LLONG_MAX
# else
#  ifdef __GNUC__
#   define FFI_LONG_LONG_MAX __LONG_LONG_MAX__
#  endif
# endif
#endif

/* The closure code assumes that this works on pointers, i.e. a size_t	*/
/* can hold a pointer.							*/

typedef struct _ffi_type
{
  size_t size;
  unsigned short alignment;
  unsigned short type;
  struct _ffi_type **elements;
} ffi_type;

#ifndef LIBFFI_HIDE_BASIC_TYPES
#if SCHAR_MAX == 127
# define ffi_type_uchar                ffi_type_uint8
# define ffi_type_schar                ffi_type_sint8
#else
 #error "char size not supported"
#endif

#if SHRT_MAX == 32767
# define ffi_type_ushort       ffi_type_uint16
# define ffi_type_sshort       ffi_type_sint16
#elif SHRT_MAX == 2147483647
# define ffi_type_ushort       ffi_type_uint32
# define ffi_type_sshort       ffi_type_sint32
#else
 #error "short size not supported"
#endif

#if INT_MAX == 32767
# define ffi_type_uint         ffi_type_uint16
# define ffi_type_sint         ffi_type_sint16
#elif INT_MAX == 2147483647
# define ffi_type_uint         ffi_type_uint32
# define ffi_type_sint         ffi_type_sint32
#elif INT_MAX == 9223372036854775807
# define ffi_type_uint         ffi_type_uint64
# define ffi_type_sint         ffi_type_sint64
#else
 #error "int size not supported"
#endif

#if LONG_MAX == 2147483647
# if FFI_LONG_LONG_MAX != 9223372036854775807
 #error "no 64-bit data type supported"
# endif
#elif LONG_MAX != 9223372036854775807
 #error "long size not supported"
#endif

#if LONG_MAX == 2147483647
# define ffi_type_ulong        ffi_type_uint32
# define ffi_type_slong        ffi_type_sint32
#elif LONG_MAX == 9223372036854775807
# define ffi_type_ulong        ffi_type_uint64
# define ffi_type_slong        ffi_type_sint64
#else
 #error "long size not supported"
#endif

/* These are defined in types.c */
extern ffi_type ffi_type_void;
extern ffi_type ffi_type_uint8;
extern ffi_type ffi_type_sint8;
extern ffi_type ffi_type_uint16;
extern ffi_type ffi_type_sint16;
extern ffi_type ffi_type_uint32;
extern ffi_type ffi_type_sint32;
extern ffi_type ffi_type_uint64;
extern ffi_type ffi_type_sint64;
extern ffi_type ffi_type_float;
extern ffi_type ffi_type_double;
extern ffi_type ffi_type_pointer;

#if CONF_HAVE_LONG_DOUBLE       // android changed
extern ffi_type ffi_type_longdouble;
#else
#define ffi_type_longdouble ffi_type_double
#endif
#endif /* LIBFFI_HIDE_BASIC_TYPES */

typedef enum {
  FFI_OK = 0,
  FFI_BAD_TYPEDEF,
  FFI_BAD_ABI
} ffi_status;

typedef unsigned FFI_TYPE;

typedef struct {
  ffi_abi abi;
  unsigned nargs;
  ffi_type **arg_types;
  ffi_type *rtype;
  unsigned bytes;
  unsigned flags;
#ifdef FFI_EXTRA_CIF_FIELDS
  FFI_EXTRA_CIF_FIELDS;
#endif
} ffi_cif;

/* ---- Definitions for the raw API -------------------------------------- */

#ifndef FFI_SIZEOF_ARG
# if LONG_MAX == 2147483647
#  define FFI_SIZEOF_ARG        4
# elif LONG_MAX == 9223372036854775807
#  define FFI_SIZEOF_ARG        8
# endif
#endif

#ifndef FFI_SIZEOF_JAVA_RAW
#  define FFI_SIZEOF_JAVA_RAW FFI_SIZEOF_ARG
#endif

typedef union {
  ffi_sarg  sint;
  ffi_arg   uint;
  float	    flt;
  char      data[FFI_SIZEOF_ARG];
  void*     ptr;
} ffi_raw;

#if FFI_SIZEOF_JAVA_RAW == 4 && FFI_SIZEOF_ARG == 8
typedef union {
  signed int	sint;
  unsigned int	uint;
  float		flt;
  char		data[FFI_SIZEOF_JAVA_RAW];
  void*		ptr;
} ffi_java_raw;
#else
typedef ffi_raw ffi_java_raw;
#endif


void ffi_raw_call (ffi_cif *cif,
		   void (*fn)(void),
		   void *rvalue,
		   ffi_raw *avalue);

void ffi_ptrarray_to_raw (ffi_cif *cif, void **args, ffi_raw *raw);
void ffi_raw_to_ptrarray (ffi_cif *cif, ffi_raw *raw, void **args);
size_t ffi_raw_size (ffi_cif *cif);

/* This is analogous to the raw API, except it uses Java parameter	*/
/* packing, even on 64-bit machines.  I.e. on 64-bit machines		*/
/* longs and doubles are followed by an empty 64-bit word.		*/

void ffi_java_raw_call (ffi_cif *cif,
			void (*fn)(void),
			void *rvalue,
			ffi_java_raw *avalue);

void ffi_java_ptrarray_to_raw (ffi_cif *cif, void **args, ffi_java_raw *raw);
void ffi_java_raw_to_ptrarray (ffi_cif *cif, ffi_java_raw *raw, void **args);
size_t ffi_java_raw_size (ffi_cif *cif);

/* ---- Definitions for closures ----------------------------------------- */

#if FFI_CLOSURES

typedef struct {
  char tramp[FFI_TRAMPOLINE_SIZE];
  ffi_cif   *cif;
  void     (*fun)(ffi_cif*,void*,void**,void*);
  void      *user_data;
} ffi_closure __attribute__((aligned (8)));

void *ffi_closure_alloc (size_t size, void **code);
void ffi_closure_free (void *);

ffi_status
ffi_prep_closure (ffi_closure*,
		  ffi_cif *,
		  void (*fun)(ffi_cif*,void*,void**,void*),
		  void *user_data);

ffi_status
ffi_prep_closure_loc (ffi_closure*,
		      ffi_cif *,
		      void (*fun)(ffi_cif*,void*,void**,void*),
		      void *user_data,
		      void*codeloc);

typedef struct {
  char tramp[FFI_TRAMPOLINE_SIZE];

  ffi_cif   *cif;

#if !FFI_NATIVE_RAW_API

  /* if this is enabled, then a raw closure has the same layout 
     as a regular closure.  We use this to install an intermediate 
     handler to do the transaltion, void** -> ffi_raw*. */

  void     (*translate_args)(ffi_cif*,void*,void**,void*);
  void      *this_closure;

#endif

  void     (*fun)(ffi_cif*,void*,ffi_raw*,void*);
  void      *user_data;

} ffi_raw_closure;

typedef struct {
  char tramp[FFI_TRAMPOLINE_SIZE];

  ffi_cif   *cif;

#if !FFI_NATIVE_RAW_API

  /* if this is enabled, then a raw closure has the same layout 
     as a regular closure.  We use this to install an intermediate 
     handler to do the transaltion, void** -> ffi_raw*. */

  void     (*translate_args)(ffi_cif*,void*,void**,void*);
  void      *this_closure;

#endif

  void     (*fun)(ffi_cif*,void*,ffi_java_raw*,void*);
  void      *user_data;

} ffi_java_raw_closure;

ffi_status
ffi_prep_raw_closure (ffi_raw_closure*,
		      ffi_cif *cif,
		      void (*fun)(ffi_cif*,void*,ffi_raw*,void*),
		      void *user_data);

ffi_status
ffi_prep_raw_closure_loc (ffi_raw_closure*,
			  ffi_cif *cif,
			  void (*fun)(ffi_cif*,void*,ffi_raw*,void*),
			  void *user_data,
			  void *codeloc);

ffi_status
ffi_prep_java_raw_closure (ffi_java_raw_closure*,
		           ffi_cif *cif,
		           void (*fun)(ffi_cif*,void*,ffi_java_raw*,void*),
		           void *user_data);

ffi_status
ffi_prep_java_raw_closure_loc (ffi_java_raw_closure*,
			       ffi_cif *cif,
			       void (*fun)(ffi_cif*,void*,ffi_java_raw*,void*),
			       void *user_data,
			       void *codeloc);

#endif /* FFI_CLOSURES */

/* ---- Public interface definition -------------------------------------- */

ffi_status ffi_prep_cif(ffi_cif *cif,
			ffi_abi abi,
			unsigned int nargs,
			ffi_type *rtype,
			ffi_type **atypes);

void ffi_call(ffi_cif *cif,
	      void (*fn)(void),
	      void *rvalue,
	      void **avalue);

/* Useful for eliminating compiler warnings */
#define FFI_FN(f) ((void (*)(void))f)

/* ---- Definitions shared with assembly code ---------------------------- */

#endif

/* If these change, update src/mips/ffitarget.h. */
#define FFI_TYPE_VOID       0    
#define FFI_TYPE_INT        1
#define FFI_TYPE_FLOAT      2    
#define FFI_TYPE_DOUBLE     3
#if CONF_HAVE_LONG_DOUBLE       // android changed
#define FFI_TYPE_LONGDOUBLE 4
#else
#define FFI_TYPE_LONGDOUBLE FFI_TYPE_DOUBLE
#endif
#define FFI_TYPE_UINT8      5   
#define FFI_TYPE_SINT8      6
#define FFI_TYPE_UINT16     7 
#define FFI_TYPE_SINT16     8
#define FFI_TYPE_UINT32     9
#define FFI_TYPE_SINT32     10
#define FFI_TYPE_UINT64     11
#define FFI_TYPE_SINT64     12
#define FFI_TYPE_STRUCT     13
#define FFI_TYPE_POINTER    14

/* This should always refer to the last type code (for sanity checks) */
#define FFI_TYPE_LAST       FFI_TYPE_POINTER

#ifdef __cplusplus
}
#endif

#endif
