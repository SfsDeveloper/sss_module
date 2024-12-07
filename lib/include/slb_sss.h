/*******************************************************************************
 Copyright (C) 2024 Some Fellow System, Inc. All rights reserved.
 ===============================================================================
 SLB
 -------------------------------------------------------------------------------
 SSS (Secret Sharing Scheme by Adi Shamir)
 -------------------------------------------------------------------------------
 Public header for SLB-SSS

 The byte order (endian) of SLB-SSS depends on the implementation.
 To ensure the compatibility of encoded data between different processing systems,
 it is necessary for the user to perform appropriate conversion processing.
 *******************************************************************************/
#ifndef SLB_SSS_H
#define SLB_SSS_H

#include "slb.h"

#ifdef __cplusplus
extern "C" {
#endif

/***** Constants *****/
#define SLB_SSS_MIN_PLAYERS		2U		/* Minimum players */

/***** SSS configuration table *****/
typedef struct slb_sss_config_st
{
	slb_int_t		enc_paran_ratio_to_cores;
	slb_int_t		enc_cores_ratio_to_k;

	slb_int_t		dec_paran_ratio_to_cores;
	slb_int_t		dec_cores_ratio_to_k;
	slb_uint_t		dec_paran_expand_limit_k;

	slb_uint_t		k_max_x_diff;
	slb_uint_t		callback_steps;

	slb_uint_t		dec_addsteps_k_max_x_diff;
} SLB_SSS_CONFIG;

/***** Handle structure (For type safety, not true constructs) *****/
typedef struct slb_sss_handle_st
{
	slb_uint32_t	for_typesafe;
} *H_SLB_SSS;

/***** Processing result added to the common *****/
#define R_SSS_STOP				0x0101U		/* User ordered stop			*/
#define R_SSS_INVALID_X			0x4101U		/* Incorrect x coordinate		*/
#define R_SSS_IDENTIC_X			0x4102U		/* Have the same x-coordinate	*/
#define R_SSS_NOT_STARTED		0x4103U		/* Not started					*/
#define R_SSS_NOT_INIT			0xc101U		/* Not initialized				*/

/***** Handle state *****/
typedef enum
{
	HS_SSS_INVALID,			/* Invalid handle				*/
	HS_SSS_ENCODE,			/* Encoding handle				*/
	HS_SSS_ENCODE_STARTED,	/* Encoding handle (started)	*/
	HS_SSS_DECODE,			/* Decoding handle				*/
	HS_SSS_DECODE_STARTED	/* Encoding handle (started)	*/
} SLB_SSS_HANDLE_STATE;

/***** Control contribution callback function *****/
typedef slb_bool_t (*SLB_SSS_CALLBACK)(void* events_param);

/***** Encoding open parameter *****/
typedef struct slb_sss_encode_open_param_st
{
	slb_uint_t			k_max;
	slb_uint_t			n_max;
	void*				mem_param;
	SLB_RAND_CALLBACK	rand_func;
	void*				rand_param;
} SLB_SSS_ENCODE_OPEN_PARAM;

/***** Decoding open parameter *****/
typedef struct slb_sss_decode_open_param_st
{
	slb_uint_t			k_max;
	void*				mem_param;
} SLB_SSS_DECODE_OPEN_PARAM;

/***** Statistics *****/
typedef struct slb_sss_statistics_st
{
	slb_int_t	maxnmb;
	slb_int_t	optimalnmb;
	slb_int_t	bestnmb;

	slb_uint_t	called_cnt;
	slb_uint_t	best_called_cnt;
	slb_int_t	max_nmb_called;

	slb_uint_t	coding_cnt;
	slb_uint_t	best_coding_cnt;
	slb_uint_t	optimal_cnt;
	slb_uint_t	best_optimal_cnt;

	slb_uint_t	sse2_cnt;
	slb_uint_t	avx2_cnt;
	slb_uint_t	avx512_cnt;

	slb_uint_t	parallel_cnt;
	slb_int_t	max_cores;
	slb_int_t	max_paran;
} SLB_SSS_STATISTICS;

/***** Function *****/
void slb_sss_get_config(
	SLB_SSS_CONFIG*	p_config
);

SLB_RC slb_sss_change_config(
	const SLB_SSS_CONFIG*	p_config
);

void slb_sss_set_simd(
	slb_bool_t	sse2,
	slb_bool_t	avx2,
	slb_bool_t	avx512
);

void slb_sss_set_mp(
	SLB_MP_TYPE		mp_type,
	slb_int_t		cores
);

void slb_sss_init_decode_res(void);

H_SLB_SSS slb_sss_open_as_encode(
	const SLB_SSS_ENCODE_OPEN_PARAM*	open_param,
	SLB_RC*								rc
);

SLB_RC slb_sss_start_encode(
	H_SLB_SSS		handle,
	slb_uint_t		k,
	slb_uint_t		n,
	slb_bool_t		xassign,
	slb_uint16_t	x[]
);

SLB_RC slb_sss_encode(
	H_SLB_SSS				handle,
	slb_int_t				nmb,
	const slb_uint16_t*		plain,
	slb_uint32_t**			share
);

H_SLB_SSS slb_sss_open_as_decode(
	const SLB_SSS_DECODE_OPEN_PARAM*	open_param,
	SLB_RC*								rc
);

SLB_RC slb_sss_start_decode(
	H_SLB_SSS			handle,
	slb_uint_t			k,
	const slb_uint16_t	x[]
);

SLB_RC slb_sss_decode(
	H_SLB_SSS				handle,
	slb_int_t				nmb,
	const slb_uint32_t**	share,
	slb_uint16_t*			plain
);

SLB_RC slb_sss_set_callback(
	H_SLB_SSS			handle,
	SLB_SSS_CALLBACK	events_func,
	void*				events_param
);

void slb_sss_close(
	H_SLB_SSS	handle
);

SLB_SSS_HANDLE_STATE slb_sss_get_handle_state(
	H_SLB_SSS	handle
);

SLB_RC slb_sss_rand(
	H_SLB_SSS	handle,
	slb_uint_t	len,
	void*		rnd_buff
);

slb_int_t slb_sss_get_bestnmb(
	H_SLB_SSS		handle,
	slb_int_t		maxnmb
);

SLB_RC slb_sss_start_statistics(
	H_SLB_SSS		handle,
	slb_int_t		maxnmb
);

SLB_RC slb_sss_stop_statistics(
	H_SLB_SSS		handle
);

SLB_RC slb_sss_get_statistics(
	H_SLB_SSS			handle,
	SLB_SSS_STATISTICS*	p_stat,
	slb_uint_t			bytes
);

slb_uint_t slb_sss_get_max_players_encode(void);
slb_uint_t slb_sss_get_max_players_decode(void);

SLB_RC slb_sss_get_encode_param(
	H_SLB_SSS					handle,
	SLB_SSS_ENCODE_OPEN_PARAM*	p_param
);

SLB_RC slb_sss_get_decode_param(
	H_SLB_SSS					handle,
	SLB_SSS_DECODE_OPEN_PARAM*	p_param
);

slb_uint_t slb_sss_get_info_k_max(H_SLB_SSS handle);
slb_uint_t slb_sss_get_info_n_max(H_SLB_SSS handle);
slb_uint_t slb_sss_get_info_k(H_SLB_SSS handle);
slb_uint_t slb_sss_get_info_n(H_SLB_SSS handle);
slb_uint16_t slb_sss_get_info_x(H_SLB_SSS handle, slb_uint_t index);

#ifdef __cplusplus
}
#endif

#endif	/* ifndef SLB_SSS_H */
