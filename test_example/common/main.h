/*******************************************************************************
 test example for libslbsss
 *******************************************************************************/
#ifndef MAIN_H
#define MAIN_H

#include "libslbsss.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Stream buffer
	This is a structure that virtually represents the secret data to be given to the distribution destination.
	In a real application it could also be a file.
*/
typedef struct {
	slb_uint16_t	x;		/* x-coordinate */
	slb_uint_t		pos;	/* write/read position */
	slb_uint8_t*	data;	/* data */
} STREAM_BUFF;

int main(int argc, char* argv[]);

void sss_example_sub(
	slb_int_t			plainsize,		/* plain size */
	slb_int_t			n,				/* number of shares */
	slb_int_t			k_min,			/* minimum threshold */
	slb_int_t			k_max,			/* maximum threshold */
	slb_bool_t			random_x,		/* =SLB_TRUE: random x coordinates */
	SLB_MP_TYPE			mp_type,		/* MP type */
	slb_uint_t			simd_flags		/* SIMD flags to use */
);

void sss_example_encode(
	H_SLB_SSS			hEncode,		/* control handle to encode */
	slb_int_t			plainsize,		/* plain size */
	slb_int_t			n,				/* number of shares */
	slb_int_t			cur_k,			/* current threshold */
	slb_bool_t			random_x,		/* =SLB_TRUE: random x coordinates */
	const slb_uint16_t*	plain,			/* plain data */
	STREAM_BUFF*		stream			/* stream */
);

void sss_example_decode(
	H_SLB_SSS			hDecode,		/* control handle to decode */
	slb_int_t			plainsize,		/* plain size */
	slb_int_t			n,				/* number of shares */
	slb_int_t			cur_k,			/* current threshold */
	const slb_uint16_t*	plain,			/* plain data */
	STREAM_BUFF*		stream,			/* stream */
	slb_bool_t			plain_match		/* =SLB_TRUE: expects plain data match */
);

void sss_reset_stream_pos(
	slb_int_t			n,			/* number of shares */
	STREAM_BUFF*const	stream		/* stream buffer */
);

void sss_share_to_stream(
	slb_int_t			n,			/* number of shares */
	slb_int_t			nmb,		/* number of use */
	slb_uint32_t**const	share,		/* share buffer */
	STREAM_BUFF*const	stream		/* stream buffer */
);

void sss_stream_to_share(
	slb_int_t			n,			/* number of shares */
	slb_int_t			nmb,		/* number of use */
	STREAM_BUFF*const	stream,		/* stream buffer */
	slb_uint32_t**const	share		/* share buffer */
);

void sss_get_random_x(H_SLB_SSS hEncode, slb_int_t n, slb_uint16_t x[]);

void check_max(H_SLB_SSS handle, slb_int_t k_max, slb_int_t n_max);
void check_info(H_SLB_SSS handle, slb_int_t k, slb_int_t n, slb_int_t index, slb_uint16_t xval);

void printf_detail(const char* form, ...);
void print_simd_names(slb_uint_t simd_flags);
void disp_stat(H_SLB_SSS handle);

void* alloc_callback(void* param, slb_uint_t size);
void free_callback(void* param, void* p, slb_uint_t size, slb_bool_t cleared);
slb_bool_t rand_callback(void* param, slb_uint_t len, void* rnd_buff);
slb_bool_t user_callback(void* param);

slb_bool_t is_mem_cleared(const void* p, slb_uint_t size);

#ifdef __cplusplus
}
#endif

#endif	/* ifndef MAIN_H */
