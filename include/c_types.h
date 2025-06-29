#ifndef H_CTYPES
#define H_CTYPES

/*=======================================================================*/

typedef unsigned char      t_bool;
typedef unsigned char      t_exec_code;
typedef char               t_char;

typedef unsigned char      t_uint8;
typedef signed char        t_sint8;
typedef char               t_int8;

typedef unsigned short     t_uint16;
typedef signed short       t_sint16;
typedef short              t_int16;

typedef unsigned int       t_uint32;
typedef signed int         t_sint32;
typedef int                t_int32;

typedef unsigned long long t_uint64;
typedef signed long long   t_sint64;
typedef long long          t_int64;

typedef float              t_float32;

typedef double             t_float64;

#if defined(__x86_64__) || defined(__WIN64) 
typedef long double        t_float128;
#endif

typedef void*              t_any;

typedef void               t_none;

/*=======================================================================*/

#define CTYPE_INT8_MAX   (t_int8)127
#define CTYPE_INT8_MIN   (t_int8)-128
#define CTYPE_UINT8_MAX  (t_uint8)255
#define CTYPE_UINT8_MIN  (t_uint8)0
#define CTYPE_SINT8_MAX  (t_sint8)-255
#define CTYPE_SINT8_MIN  (t_sint8)0

#define CTYPE_INT16_MAX  (t_int16)32767
#define CTYPE_INT16_MIN  (t_int16)-32768
#define CTYPE_UINT16_MAX (t_uint16)65535
#define CTYPE_UINT16_MIN (t_uint16)0
#define CTYPE_SINT16_MAX (t_sint16)-65534
#define CTYPE_SINT16_MIN (t_sint16)0

#define CTYPE_INT32_MAX  (t_int32)2147483647
#define CTYPE_INT32_MIN  (t_int32)-2147483648
#define CTYPE_UINT32_MAX (t_uint32)4294967295
#define CTYPE_UINT32_MIN (t_uint32)0
#define CTYPE_SINT32_MAX (t_sint32)-4294967295
#define CTYPE_SINT32_MIN (t_sint32)0

#define CTYPE_INT64_MAX  (t_int64)9223372036854775807
#define CTYPE_INT64_MIN  (t_int64)-9223372036854775807
#define CTYPE_UINT64_MAX (t_uint64)18446744073709551615
#define CTYPE_UINT64_MIN (t_uint64)0
#define CTYPE_SINT64_MAX (t_sint64)-18446744073709551615
#define CTYPE_SINT64_MIN (t_sint64)0

/*=======================================================================*/

#define true             (t_bool)1
#define false            (t_bool)0

/*=======================================================================*/

#endif