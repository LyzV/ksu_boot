#ifndef _VTYPES_H_
#define _VTYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>//size_t

typedef void * ptr_t;
#ifdef nullptr_v
#undef nullptr_v
#endif
#ifdef __cplusplus
#	define nullptr_v    0
#else
#   define nullptr_v    ((ptr_t)0);
#endif

typedef ptr_t handle_t;
#ifdef invalidhandle_v
#undef invalidhandle_v
#endif
#ifdef __cplusplus
#	define invalidhandle_v    (handle_t)0
#else
#   define invalidhandle_v    ((handle_t)0);
#endif

#endif//_VTYPES_H_
 
