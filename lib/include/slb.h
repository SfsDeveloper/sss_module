/*******************************************************************************
 Copyright (C) 2024 Some Fellow System, Inc. All rights reserved.
 ===============================================================================
 SLB
 -------------------------------------------------------------------------------
 Public header for SLB
 *******************************************************************************/
#ifndef SLB_H
#define SLB_H

#include "slb_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***** MP type *****/
typedef enum
{
	MP_NONE,			/* Not use MP	*/
	MP_OMP				/* OpenMP		*/
} SLB_MP_TYPE;

/***** Memory management callback *****/
typedef void* (*SLB_ALLOC)(void* param, slb_uint_t size);
typedef void (*SLB_FREE)(void* param, void* mem, slb_uint_t size, slb_bool_t cleared);

/***** Random function callback *****/
typedef slb_bool_t (*SLB_RAND_CALLBACK)(void* param, slb_uint_t len, void* rnd_buff);

/***** Return value (common) *****

  FE DCBA98 76543210
 +--+------+--------+
 |SV|MODULE| CODE   |
 +--+------+--------+
	SV     - 00(Success)
	         01(Coding Error)
	         10(Error)
	         11(Fatal Error)
	MODULE - Module code (=0:common)
	CODE   - Return code by module
*/

typedef slb_uint_t	SLB_RC;		/* High word is always zero */

#define R_SUCCESS			0x0000U		/* Success									*/
#define R_TERMINATE			0x0001U		/* Higher data end detection				*/
#define R_COMPLETE			0x0002U		/* Coding complete							*/

#define R_RAND_FAIL			0x8001U		/* Random number generation not possible	*/

#define R_LOW_MEMORY		0xc001U		/* Insufficient memory						*/
#define R_NOT_CONFIG		0xc002U		/* Not config								*/
#define R_INVALID_HANDLE	0xc003U		/* Invalid handle							*/
#define R_INVALID_PARAM		0xc004U		/* Invalid parameter						*/

/* Return value check macro */
#define SLB_R_SUCCEEDED(r)		( ((r) & 0xc000U) == 0U )
#define SLB_R_FAILED(r)			( ((r) & 0xc000U) != 0U )
#define SLB_R_NOT_FATAL(r)		( ((r) & 0xc000U) != 0xc000U )

/***** Generic macro *****/

/* Unused parameter declaration */
#define SLB_UNREFERENCED(p)	((void)(p))

/* Get the offset value to the specified member in the structure */
#define SLB_MEMBER_OFFSET(str, member) \
			((slb_uintptr_t)( &(((str)*)(0))->(member) ) )

/***** Memory read/write in Big-Endian area (not affected by processing system endian) *****/

/* Write a 16-bit value */
#define SLB_WRITE16(pd, val) \
	((slb_uint8_t*)(pd))[0] = (slb_uint8_t)((slb_uint16_t)(val) >> SLB_SHIFT_OF_1B); \
	((slb_uint8_t*)(pd))[1] = (slb_uint8_t)((slb_uint16_t)(val) & SLB_MASK_OF_UINT8)

/* Write a 32-bit value */
#define SLB_WRITE32(pd, val) \
	((slb_uint8_t*)(pd))[0] = (slb_uint8_t)((slb_uint32_t)(val) >> SLB_SHIFT_OF_3B); \
	((slb_uint8_t*)(pd))[1] = (slb_uint8_t)((slb_uint32_t)(val) >> SLB_SHIFT_OF_2B); \
	((slb_uint8_t*)(pd))[2] = (slb_uint8_t)((slb_uint32_t)(val) >> SLB_SHIFT_OF_1B); \
	((slb_uint8_t*)(pd))[3] = (slb_uint8_t)((slb_uint32_t)(val) & SLB_MASK_OF_UINT8)

/* Read a 16-bit value */
#define SLB_READ16(ps, p_val) \
	*(p_val) =  (slb_uint16_t)( (slb_uint16_t)( ((slb_uint8_t*)(ps))[0] ) << SLB_SHIFT_OF_1B ); \
	*(p_val) |= (slb_uint16_t)( ((slb_uint8_t*)(ps))[1] )

/* Read a 32-bit value */
#define SLB_READ32(ps, p_val) \
	*(p_val) =  (slb_uint32_t)( (slb_uint32_t)( ((slb_uint8_t*)(ps))[0] ) << SLB_SHIFT_OF_3B ); \
	*(p_val) |= (slb_uint32_t)( (slb_uint32_t)( ((slb_uint8_t*)(ps))[1] ) << SLB_SHIFT_OF_2B ); \
	*(p_val) |= (slb_uint32_t)( (slb_uint32_t)( ((slb_uint8_t*)(ps))[2] ) << SLB_SHIFT_OF_1B ); \
	*(p_val) |= (slb_uint32_t)( ((slb_uint8_t*)(ps))[3] )

/***** Function *****/
void slb_config(
	SLB_ALLOC	alloc_func,
	SLB_FREE	free_func
);

slb_bool_t slb_is_config(void);

void* slb_alloc(
	void*		param,
	slb_uint_t	size
);

void* slb_alloc_aligned(
	void*		param,
	slb_uint_t	size,
	slb_uint_t	alignment
);

void slb_free(
	void*		param,
	void*		mem,
	slb_bool_t	clear
);

slb_int_t slb_get_nmb_of_cores(void);

#ifdef __cplusplus
}
#endif

#endif	/* ifndef SLB_H */
