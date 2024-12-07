/*******************************************************************************
 Copyright (C) 2024 Some Fellow System, Inc. All rights reserved.
 ===============================================================================
 SLB
 -------------------------------------------------------------------------------
 Definition of data types for SLB

	Make SLB-specific type declarations. SLB can only be used with the following
	processing system.

	UCHAR_MAX == 0xff
	USHRT_MAX == 0xffff
	UINT_MAX == 0xffffffff or ULONG_MAX == 0xffffffff
	ULLONG_MAX == 0xffffffffffffffff (only when using 64bit variables)
 *******************************************************************************/
#ifndef SLB_TYPES_H
#define SLB_TYPES_H

#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/***** Definition *****/

/* NULL */
#ifdef __cplusplus
#	define SLB_NULL	0
#else
#	define SLB_NULL	((void*)0)
#endif

/* Boolean */
typedef int	slb_bool_t;
#define SLB_FALSE	0
#define SLB_TRUE	1

/* 8-bit integer */
#if UCHAR_MAX == 0xff
	typedef unsigned char	slb_uint8_t;
	typedef char			slb_int8_t;
#else
#	error slb_types.h: cannot config slb_uint8_t
#endif

/* 16-bit integer */
#if USHRT_MAX == 0xffff
	typedef unsigned short	slb_uint16_t;
	typedef short			slb_int16_t;
#else
#	error slb_types.h: cannot config slb_uint16_t
#endif

/* 32-bit integer */
#if UINT_MAX == 0xffffffff
	typedef unsigned int	slb_uint32_t;
	typedef int				slb_int32_t;
	typedef unsigned int	slb_uint_t;
	typedef int				slb_int_t;
#elif ULONG_MAX == 0xffffffffUL
	typedef unsigned long	slb_uint32_t;
	typedef long			slb_int32_t;
	typedef unsigned long	slb_uint_t;
	typedef long			slb_int_t;
#else
#	error slb_types.h: cannot config slb_uint32_t
#endif

/* 64-bit integer */
#ifndef ULLONG_MAX
#define ULLONG_MAX	_UI64_MAX
#endif
#if ULLONG_MAX == 0xffffffffffffffffU
#	if defined(_MSC_VER)
		typedef unsigned __int64	slb_uint64_t;
		typedef __int64				slb_int64_t;
#	else
		typedef unsigned long long	slb_uint64_t;
		typedef long long			slb_int64_t;
#	endif
#endif

/* PTR integer */
#if defined(_MSC_VER)
#	if defined(_WIN64)
		typedef unsigned __int64		slb_uintptr_t;
#	else
#		if _MSC_VER >= 1300
			typedef _w64 unsigned int	slb_uintptr_t;
#		else
			typedef unsigned int		slb_uintptr_t;
#		endif
#	endif
#elif defined(__GNUC__)
#	if defined(__x86_64__)
		typedef unsigned long long int	slb_uintptr_t;
#	else
		typedef unsigned int			slb_uintptr_t;
#	endif
#else
#	if UINT_MAX == 0xffffffff
		typedef unsigned int			slb_uintptr_t;
#	elif ULONG_MAX == 0xffffffffUL
		typedef unsigned long			slb_uintptr_t;
#	else
#		error slb_types.h: cannot config slb_uintptr_t
#	endif
#endif

/* Maximum value */
#define SLB_UCHAR_MAX	0xffU
#define SLB_USHRT_MAX	0xffffU
#define SLB_INT_MAX		0x7fffffff
#define SLB_UINT_MAX	0xffffffffU
#define SLB_UINT32_MAX	0xffffffffU
#if defined(_MSC_VER)
#	define SLB_ULLONG_MAX	0xffffffffffffffffUI64
#else
#	define SLB_ULLONG_MAX	0xffffffffffffffffU
#endif

/* Generic constants */
#define SLB_BITS_OF_NIBBLE		4			/* number of nibble bits */

#define SLB_BITS_OF_UINT8		8			/* number of bits in uint8 */
#define SLB_BITS_OF_UINT16		16			/* number of bits in uint16 */
#define SLB_BITS_OF_UINT32		32			/* number of bits in uint32 */

#define SLB_BIT_MSB_OF_UINT8	0x80U		/* MSB bit of uint8 */
#define SLB_BIT_LSB_OF_UINT8	0x01U		/* LSB bit of uint8 */
#define SLB_BIT_MSB_OF_UINT16	0x8000U		/* MSB bit of uint16 */
#define SLB_BIT_LSB_OF_UINT16	0x0001U		/* LSB bit of uint16 */
#define SLB_BIT_MSB_OF_UINT32	0x80000000U	/* MSB bit of uint32 */
#define SLB_BIT_LSB_OF_UINT32	0x00000001U	/* LSB bit of uint32 */

#define SLB_SHIFT_OF_1B			8			/* 1 byte shift size */
#define SLB_SHIFT_OF_2B			16			/* 2 byte shift size */
#define SLB_SHIFT_OF_3B			24			/* 3 byte shift size */
#define SLB_SHIFT_OF_4B			32			/* 4 byte shift size */
#define SLB_SHIFT_OF_5B			40			/* 5 byte shift size */
#define SLB_SHIFT_OF_6B			48			/* 6 byte shift size */
#define SLB_SHIFT_OF_7B			56			/* 7 byte shift size */

#define SLB_MASK_OF_UINT8		0xffU		/* uint8 mask */
#define SLB_MASK_OF_UINT16		0xffffU		/* uint16 mask */
#define SLB_MASK_OF_UINT32		0xffffffffU	/* uint32 mask */

#ifdef __cplusplus
}
#endif

#endif	/* ifndef SLB_TYPES_H */
