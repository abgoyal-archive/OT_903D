
#ifndef COMMON_H
#define COMMON_H

#include "os.h"

#ifdef __linux__
#include <endian.h>
#include <byteswap.h>
#endif /* __linux__ */

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
#include <sys/types.h>
#include <sys/endian.h>
#define __BYTE_ORDER	_BYTE_ORDER
#define	__LITTLE_ENDIAN	_LITTLE_ENDIAN
#define	__BIG_ENDIAN	_BIG_ENDIAN
#define bswap_16 bswap16
#define bswap_32 bswap32
#define bswap_64 bswap64
#endif /* defined(__FreeBSD__) || defined(__NetBSD__) ||
	* defined(__DragonFly__) */

#ifdef CONFIG_TI_COMPILER
#define __BIG_ENDIAN 4321
#define __LITTLE_ENDIAN 1234
#ifdef __big_endian__
#define __BYTE_ORDER __BIG_ENDIAN
#else
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif
#endif /* CONFIG_TI_COMPILER */

#ifdef CONFIG_NATIVE_WINDOWS
#include <winsock.h>

typedef int socklen_t;

#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0 /* not supported */
#endif

#endif /* CONFIG_NATIVE_WINDOWS */

#if defined(__CYGWIN__) || defined(CONFIG_NATIVE_WINDOWS)

#ifdef _MSC_VER
#define inline __inline
#endif /* _MSC_VER */

static inline unsigned short wpa_swap_16(unsigned short v)
{
	return ((v & 0xff) << 8) | (v >> 8);
}

static inline unsigned int wpa_swap_32(unsigned int v)
{
	return ((v & 0xff) << 24) | ((v & 0xff00) << 8) |
		((v & 0xff0000) >> 8) | (v >> 24);
}

#define le_to_host16(n) (n)
#define host_to_le16(n) (n)
#define be_to_host16(n) wpa_swap_16(n)
#define host_to_be16(n) wpa_swap_16(n)
#define le_to_host32(n) (n)
#define be_to_host32(n) wpa_swap_32(n)
#define host_to_be32(n) wpa_swap_32(n)

#else /* __CYGWIN__ */

#ifndef __BYTE_ORDER
#ifndef __LITTLE_ENDIAN
#ifndef __BIG_ENDIAN
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#if defined(sparc)
#define __BYTE_ORDER __BIG_ENDIAN
#endif
#endif /* __BIG_ENDIAN */
#endif /* __LITTLE_ENDIAN */
#endif /* __BYTE_ORDER */

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define le_to_host16(n) (n)
#define host_to_le16(n) (n)
#define be_to_host16(n) bswap_16(n)
#define host_to_be16(n) bswap_16(n)
#define le_to_host32(n) (n)
#define be_to_host32(n) bswap_32(n)
#define host_to_be32(n) bswap_32(n)
#define le_to_host64(n) (n)
#define host_to_le64(n) (n)
#define be_to_host64(n) bswap_64(n)
#define host_to_be64(n) bswap_64(n)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define le_to_host16(n) bswap_16(n)
#define host_to_le16(n) bswap_16(n)
#define be_to_host16(n) (n)
#define host_to_be16(n) (n)
#define le_to_host32(n) bswap_32(n)
#define be_to_host32(n) (n)
#define host_to_be32(n) (n)
#define le_to_host64(n) bswap_64(n)
#define host_to_le64(n) bswap_64(n)
#define be_to_host64(n) (n)
#define host_to_be64(n) (n)
#ifndef WORDS_BIGENDIAN
#define WORDS_BIGENDIAN
#endif
#else
#error Could not determine CPU byte order
#endif

#endif /* __CYGWIN__ */

/* Macros for handling unaligned 16-bit variables */
#define WPA_GET_BE16(a) ((u16) (((a)[0] << 8) | (a)[1]))
#define WPA_PUT_BE16(a, val)			\
	do {					\
		(a)[0] = ((u16) (val)) >> 8;	\
		(a)[1] = ((u16) (val)) & 0xff;	\
	} while (0)

#define WPA_GET_LE16(a) ((u16) (((a)[1] << 8) | (a)[0]))
#define WPA_PUT_LE16(a, val)			\
	do {					\
		(a)[1] = ((u16) (val)) >> 8;	\
		(a)[0] = ((u16) (val)) & 0xff;	\
	} while (0)

#define WPA_GET_BE24(a) ((((u32) (a)[0]) << 16) | (((u32) (a)[1]) << 8) | \
			 ((u32) (a)[2]))
#define WPA_PUT_BE24(a, val)				\
	do {						\
		(a)[0] = (u8) (((u32) (val)) >> 16);	\
		(a)[1] = (u8) (((u32) (val)) >> 8);	\
		(a)[2] = (u8) (((u32) (val)) & 0xff);	\
	} while (0)

#define WPA_GET_BE32(a) ((((u32) (a)[0]) << 24) | (((u32) (a)[1]) << 16) | \
			 (((u32) (a)[2]) << 8) | ((u32) (a)[3]))
#define WPA_PUT_BE32(a, val)				\
	do {						\
		(a)[0] = (u8) (((u32) (val)) >> 24);	\
		(a)[1] = (u8) (((u32) (val)) >> 16);	\
		(a)[2] = (u8) (((u32) (val)) >> 8);	\
		(a)[3] = (u8) (((u32) (val)) & 0xff);	\
	} while (0)

#define WPA_PUT_BE64(a, val)				\
	do {						\
		(a)[0] = (u8) (((u64) (val)) >> 56);	\
		(a)[1] = (u8) (((u64) (val)) >> 48);	\
		(a)[2] = (u8) (((u64) (val)) >> 40);	\
		(a)[3] = (u8) (((u64) (val)) >> 32);	\
		(a)[4] = (u8) (((u64) (val)) >> 24);	\
		(a)[5] = (u8) (((u64) (val)) >> 16);	\
		(a)[6] = (u8) (((u64) (val)) >> 8);	\
		(a)[7] = (u8) (((u64) (val)) & 0xff);	\
	} while (0)


#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

#ifdef _MSC_VER
typedef UINT64 u64;
typedef UINT32 u32;
typedef UINT16 u16;
typedef UINT8 u8;
typedef INT64 s64;
typedef INT32 s32;
typedef INT16 s16;
typedef INT8 s8;
#define WPA_TYPES_DEFINED
#endif /* _MSC_VER */

#ifdef __vxworks
typedef unsigned long long u64;
typedef UINT32 u32;
typedef UINT16 u16;
typedef UINT8 u8;
typedef long long s64;
typedef INT32 s32;
typedef INT16 s16;
typedef INT8 s8;
#define WPA_TYPES_DEFINED
#endif /* __vxworks */

#ifdef CONFIG_TI_COMPILER
#ifdef _LLONG_AVAILABLE
typedef unsigned long long u64;
#else
typedef unsigned long u64;
#endif
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
#define WPA_TYPES_DEFINED
#endif /* CONFIG_TI_COMPILER */

#ifndef WPA_TYPES_DEFINED
#ifdef CONFIG_USE_INTTYPES_H
#include <inttypes.h>
#else
#include <stdint.h>
#endif
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;
#define WPA_TYPES_DEFINED
#endif /* !WPA_TYPES_DEFINED */

#define hostapd_get_rand os_get_random
int hwaddr_aton(const char *txt, u8 *addr);
int hexstr2bin(const char *hex, u8 *buf, size_t len);
void inc_byte_array(u8 *counter, size_t len);
void wpa_get_ntp_timestamp(u8 *buf);


#ifdef __GNUC__
#define PRINTF_FORMAT(a,b) __attribute__ ((format (printf, (a), (b))))
#define STRUCT_PACKED __attribute__ ((packed))
#else
#define PRINTF_FORMAT(a,b)
#define STRUCT_PACKED
#endif



enum { MSG_MSGDUMP, MSG_DEBUG, MSG_INFO, MSG_WARNING, MSG_ERROR };

#ifdef ANDROID

#define wpa_debug_print_timestamp() do {} while (0)
#define wpa_hexdump(...)            do {} while (0)
#define wpa_hexdump_key(...)        do {} while (0)
#define wpa_hexdump_ascii(...)      do {} while (0)
#define wpa_hexdump_ascii_key(...)  do {} while (0)
#define wpa_debug_open_file(...)    do {} while (0)
#define wpa_debug_close_file()      do {} while (0)

void android_printf(int level, char *format, ...);

#define wpa_printf(level, ...) \
        do {                                            \
            if ((level) >= MSG_INFO) {                  \
                android_printf((level), __VA_ARGS__);   \
            }                                           \
        } while (0)

#else /* ANDROID */

#ifdef CONFIG_NO_STDOUT_DEBUG

#define wpa_debug_print_timestamp() do { } while (0)
#define wpa_printf(args...) do { } while (0)
#define wpa_hexdump(l,t,b,le) do { } while (0)
#define wpa_hexdump_key(l,t,b,le) do { } while (0)
#define wpa_hexdump_ascii(l,t,b,le) do { } while (0)
#define wpa_hexdump_ascii_key(l,t,b,le) do { } while (0)
#define wpa_debug_open_file(p) do { } while (0)
#define wpa_debug_close_file() do { } while (0)

#else /* CONFIG_NO_STDOUT_DEBUG */

int wpa_debug_open_file(const char *path);
void wpa_debug_close_file(void);

void wpa_debug_print_timestamp(void);

void wpa_printf(int level, char *fmt, ...)
PRINTF_FORMAT(2, 3);

void wpa_hexdump(int level, const char *title, const u8 *buf, size_t len);

void wpa_hexdump_key(int level, const char *title, const u8 *buf, size_t len);

void wpa_hexdump_ascii(int level, const char *title, const u8 *buf,
		       size_t len);

void wpa_hexdump_ascii_key(int level, const char *title, const u8 *buf,
			   size_t len);

#endif /* CONFIG_NO_STDOUT_DEBUG */

#endif /* ANDROID */

#ifdef CONFIG_NO_WPA_MSG
#define wpa_msg(args...) do { } while (0)
#define wpa_msg_register_cb(f) do { } while (0)
#else /* CONFIG_NO_WPA_MSG */
void wpa_msg(void *ctx, int level, char *fmt, ...) PRINTF_FORMAT(3, 4);

typedef void (*wpa_msg_cb_func)(void *ctx, int level, const char *txt,
				size_t len);

void wpa_msg_register_cb(wpa_msg_cb_func func);
#endif /* CONFIG_NO_WPA_MSG */


int wpa_snprintf_hex(char *buf, size_t buf_size, const u8 *data, size_t len);
int wpa_snprintf_hex_uppercase(char *buf, size_t buf_size, const u8 *data,
			       size_t len);


#ifdef EAPOL_TEST
#define WPA_ASSERT(a)						       \
	do {							       \
		if (!(a)) {					       \
			printf("WPA_ASSERT FAILED '" #a "' "	       \
			       "%s %s:%d\n",			       \
			       __FUNCTION__, __FILE__, __LINE__);      \
			exit(1);				       \
		}						       \
	} while (0)
#else
#define WPA_ASSERT(a) do { } while (0)
#endif


#ifdef _MSC_VER
#undef vsnprintf
#define vsnprintf _vsnprintf
#undef close
#define close closesocket
#endif /* _MSC_VER */


#ifdef CONFIG_ANSI_C_EXTRA

#if !defined(_MSC_VER) || _MSC_VER < 1400
int snprintf(char *str, size_t size, const char *format, ...);

/* vsnprintf - only used for wpa_msg() in wpa_supplicant.c */
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
#endif /* !defined(_MSC_VER) || _MSC_VER < 1400 */

/* getopt - only used in main.c */
int getopt(int argc, char *const argv[], const char *optstring);
extern char *optarg;
extern int optind;

#ifndef CONFIG_NO_SOCKLEN_T_TYPEDEF
#ifndef __socklen_t_defined
typedef int socklen_t;
#endif
#endif

/* inline - define as __inline or just define it to be empty, if needed */
#ifdef CONFIG_NO_INLINE
#define inline
#else
#define inline __inline
#endif

#ifndef __func__
#define __func__ "__func__ not defined"
#endif

#ifndef bswap_16
#define bswap_16(a) ((((u16) (a) << 8) & 0xff00) | (((u16) (a) >> 8) & 0xff))
#endif

#ifndef bswap_32
#define bswap_32(a) ((((u32) (a) << 24) & 0xff000000) | \
		     (((u32) (a) << 8) & 0xff0000) | \
     		     (((u32) (a) >> 8) & 0xff00) | \
     		     (((u32) (a) >> 24) & 0xff))
#endif

#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0
#endif

#ifdef _WIN32_WCE
void perror(const char *s);
#endif /* _WIN32_WCE */

#endif /* CONFIG_ANSI_C_EXTRA */

#define wpa_zalloc(s) os_zalloc((s))

#ifdef CONFIG_NATIVE_WINDOWS
void wpa_unicode2ascii_inplace(TCHAR *str);
TCHAR * wpa_strdup_tchar(const char *str);
#else /* CONFIG_NATIVE_WINDOWS */
#define wpa_unicode2ascii_inplace(s) do { } while (0)
#define wpa_strdup_tchar(s) strdup((s))
#endif /* CONFIG_NATIVE_WINDOWS */

const char * wpa_ssid_txt(u8 *ssid, size_t ssid_len);

#ifndef ANDROID
typedef u32 __be32;
typedef u64 __be64;
#endif /* Dm: */

#endif /* COMMON_H */
