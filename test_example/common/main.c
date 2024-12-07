/*******************************************************************************
 test example for libslbsss
 -------------------------------------------------------------------------------
 "-m" option: Start in measurement mode
 *******************************************************************************/
#include <stdlib.h>

#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#if defined(_MSC_VER)
#	include <crtdbg.h>
#	include <windows.h>
#	define TID_T		DWORD
#	pragma warning( disable : 4127 )		/* Suppress the warning that conditional expression is constant. */
#	pragma warning( disable : 6386 6385 )	/* Suppress the false detection of buffer overrun by code analyzer. */
#elif defined(__GNUC__)
#	include <unistd.h>
#	include <sys/syscall.h>
#	define TID_T		pid_t
#	define GetCurrentThreadId()		syscall(SYS_gettid)
#	define InterlockedIncrement(p)	__atomic_add_fetch(p, 1, __ATOMIC_RELAXED)
#	define InterlockedDecrement(p)	__atomic_sub_fetch(p, 1, __ATOMIC_RELAXED)
#endif

#include "main.h"
#include "cpuinfo.h"

#define ASSERT(f)	if (!(f)) {printf("\n!!! Assertion Failed !!!  %s (%d)\n", __FILE__, __LINE__); \
								exit(0);} else ((void)0)

slb_bool_t	g_measure;	/* =SLB_TRUE: measurement mode */

int		g_mem_callback_param;	/* Parameter for alloc and  free callback function (verify only address value) */
int		g_rand_callback_param;	/* Parameter for get random number callback function (verify only address value) */
int		g_user_callback_param;	/* Parameter for contribution callback function (verify only address value) */

TID_T	g_MyThreadId;			/* My thread ID */

#define	CODING_MAX_CHUNK	1024	/* Maximum number of data processed in one encoding/decoding */

#define F_SSE2		0x1U	/* SIMD flag: SSE2 */
#define F_AVX2		0x2U	/* SIMD flag: AVX2 */
#define F_AVX512	0x4U	/* SIMD flag: AVX-512 */
#define F_ALL		0x7U	/* SIMD flag: all */

#define MAX_PLAYERS			100			/* Maximum players */
#define MAX_PLAIN_BYTES		0x300000	/* Maximum plain bytes */
slb_uint8_t	g_stream_mem[MAX_PLAYERS][ ( sizeof(slb_uint16_t) * MAX_PLAIN_BYTES ) + MAX_PLAIN_BYTES ];
										/* Allocation memory for each stream */

/* Main function */
int main(int argc, char* argv[])
{
	slb_int_t plainsize, n, k_min, k_max;

	/* Get number of CPU logical cores */
	slb_int_t cores = slb_get_nmb_of_cores();

	/* Get SIMD availability */
	slb_bool_t bSSE2 = IsSimdAvailableSSE2();
	slb_bool_t bAVX2 = IsSimdAvailableAVX2();
	slb_bool_t bAVX512 = IsSimdAvailableAVX512();

	/* Display library version and max players */
	const char* lib_version = libslbsss_get_version();

	/* Get measurement mode */
	g_measure = ( (1 < argc) && ( strcmp(argv[1], "-m") == 0 ) );

	/* Start message */
	printf("\n############################ start\n");
	printf("Library version = %s\n", lib_version);

	/* SLB configuration */
	slb_config(alloc_callback, free_callback);

	/* Set MP type */
	slb_sss_set_mp(MP_OMP, 0);

	/* Initialize the common resource for decryption */
	slb_sss_init_decode_res();

	/* Display this computer spec */
	printf("Number of logical cores = %d\n", cores);
	printf("Implemented SIMD =");
	print_simd_names(F_ALL);
	printf("\n");

	/* Get my thread ID */
	g_MyThreadId = GetCurrentThreadId();

	/* Initialize random numbers */
	srand(1);

	/* Normal mode */
	if (g_measure == SLB_FALSE) {
		plainsize = 0x10000;
		n = 100;
		k_min = 10;
		k_max = 13;
	}

	/* Measurement mode
		Please change each value depending on the content you want to measure and this PC spec. */
	else {
		plainsize = MAX_PLAIN_BYTES;
		n = 100;
		k_min = 100;
		k_max = 100;
	}

	printf("Plaintext bytes = %d, n = %d, k_min = %d, k_max = %d\n",
				plainsize, n, k_min, k_max);

	/* Example subroutine */
	sss_example_sub( plainsize, n, k_min, k_max, SLB_FALSE, MP_NONE, 0 );
	if (g_measure == SLB_FALSE) {
		sss_example_sub( plainsize, n, k_min, k_max, SLB_TRUE, MP_NONE, 0 );
	}

	if (bSSE2) {
		sss_example_sub( plainsize, n, k_min, k_max, SLB_FALSE, MP_NONE, F_SSE2 );
		if (g_measure == SLB_FALSE) {
			sss_example_sub( plainsize, n, k_min, k_max, SLB_TRUE, MP_NONE, F_SSE2 );
		}
	}

	if (bAVX2) {
		sss_example_sub( plainsize, n, k_min, k_max, SLB_FALSE, MP_NONE, F_AVX2 );
		if (g_measure == SLB_FALSE) {
			sss_example_sub( plainsize, n, k_min, k_max, SLB_TRUE, MP_NONE, F_AVX2 );
		}
	}

	if (bAVX512) {
		sss_example_sub( plainsize, n, k_min, k_max, SLB_FALSE, MP_NONE, F_AVX512 );
		if (g_measure == SLB_FALSE) {
			sss_example_sub( plainsize, n, k_min, k_max, SLB_TRUE, MP_NONE, F_AVX512 );
		}
	}

	if (1 < cores) {
		sss_example_sub( plainsize, n, k_min, k_max, SLB_FALSE, MP_OMP, 0 );
		if (g_measure == SLB_FALSE) {
			sss_example_sub( plainsize, n, k_min, k_max, SLB_TRUE, MP_OMP, 0 );
		}
	}

	if ( (1 < cores) && (bSSE2 || bAVX2 || bAVX512) ) {
		sss_example_sub( plainsize, n, k_min, k_max, SLB_FALSE, MP_OMP, F_ALL );
		if (g_measure == SLB_FALSE) {
			sss_example_sub( plainsize, n, k_min, k_max, SLB_TRUE, MP_OMP, F_ALL );
		}
	}

	/* End message */
	printf("############################ end\n");

#if defined(_MSC_VER)
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}

/* Example subroutine */
void sss_example_sub(
	slb_int_t			plainsize,		/* plain size */
	slb_int_t			n,				/* number of shares */
	slb_int_t			k_min,			/* minimum threshold */
	slb_int_t			k_max,			/* maximum threshold */
	slb_bool_t			random_x,		/* =SLB_TRUE: random x coordinates */
	SLB_MP_TYPE			mp_type,		/* MP type */
	slb_uint_t			simd_flags		/* SIMD flags to use */
)
{
	H_SLB_SSS hEncode, hDecode;
	SLB_RC rc;
	slb_int_t i;
	slb_int_t cur_k;

	/* Decide whether SIMD can be used */
	slb_bool_t bSSE2   = (simd_flags & F_SSE2) && IsSimdAvailableSSE2();
	slb_bool_t bAVX2   = (simd_flags & F_AVX2) && IsSimdAvailableAVX2();
	slb_bool_t bAVX512 = (simd_flags & F_AVX512) && IsSimdAvailableAVX512();

	/* Allocate resources */
	slb_uint16_t*const plain = slb_alloc( &g_mem_callback_param, sizeof(slb_uint16_t) * plainsize );
	STREAM_BUFF*const stream = slb_alloc( &g_mem_callback_param, sizeof(STREAM_BUFF) * n );

	for (i = 0; i < n; ++i) {
		stream[i].data = g_stream_mem[i];
	}

	/* Set SIMD usage */
	slb_sss_set_simd(bSSE2, bAVX2, bAVX512);

	/* Set MP type */
	slb_sss_set_mp(mp_type, 0);

	/* Start message */
	printf("\n===== mp_type=%d simd_flags=%u [", mp_type, simd_flags);
	print_simd_names(simd_flags);
	printf(" ] random_x=%d\n", random_x);

	/* Open as encode */
	{
		SLB_SSS_HANDLE_STATE hstate;
		SLB_SSS_ENCODE_OPEN_PARAM encode_param;

		/* Edit the open parameter */
		encode_param.k_max = (slb_uint_t)k_max;
		encode_param.n_max = (slb_uint_t)n;
		encode_param.mem_param = &g_mem_callback_param;
		encode_param.rand_func = rand_callback;
		encode_param.rand_param = &g_rand_callback_param;

		/* Open as encoding */
		hEncode = slb_sss_open_as_encode(&encode_param, &rc);
		hstate = slb_sss_get_handle_state(hEncode);
		printf_detail("slb_sss_open_as_encode: rc=0x%04x hstate=%d\n", rc, hstate);
		ASSERT( SLB_R_SUCCEEDED(rc) );
		ASSERT( hstate == HS_SSS_ENCODE );

		/* Check the inner parameter */
		memset(&encode_param, 0, sizeof(encode_param));
		ASSERT( slb_sss_get_encode_param(hEncode, &encode_param) == R_SUCCESS );
		ASSERT( encode_param.k_max == (slb_uint_t)k_max );
		ASSERT( encode_param.n_max == (slb_uint_t)n );
		ASSERT( encode_param.mem_param == &g_mem_callback_param );
		ASSERT( encode_param.rand_func == rand_callback );
		ASSERT( encode_param.rand_param == &g_rand_callback_param );

		/* Check the information */
		check_max(hEncode, k_max, n);

		/* Set callback function */
		rc = slb_sss_set_callback(hEncode, user_callback, &g_user_callback_param);
		printf_detail("slb_sss_set_callback: rc=0x%04x\n", rc);
		ASSERT( SLB_R_SUCCEEDED(rc) );
	}

	/* Open as decode */
	{
		SLB_SSS_HANDLE_STATE hstate;
		SLB_SSS_DECODE_OPEN_PARAM decode_param;
		slb_uint_t max_players_dec = slb_sss_get_max_players_decode();

		/* Edit the open parameter */
		decode_param.k_max = (slb_uint_t)n;
		decode_param.mem_param = &g_mem_callback_param;

		/* Open as decoding */
		hDecode = slb_sss_open_as_decode(&decode_param, &rc);
		hstate = slb_sss_get_handle_state(hDecode);
		printf_detail("slb_sss_open_as_decode: rc=0x%04x hstate=%d\n", rc, hstate);
		ASSERT( SLB_R_SUCCEEDED(rc) );
		ASSERT( hstate == HS_SSS_DECODE );

		/* Check the inner parameter */
		memset(&decode_param, 0, sizeof(decode_param));
		ASSERT( slb_sss_get_decode_param(hDecode, &decode_param) == R_SUCCESS);
		ASSERT( decode_param.k_max == (slb_uint_t)n );
		ASSERT( decode_param.mem_param == &g_mem_callback_param );

		/* Check the information */
		check_max(hDecode, n, (slb_int_t)max_players_dec);

		/* Set callback function */
		rc = slb_sss_set_callback(hDecode, user_callback, &g_user_callback_param);
		printf_detail("slb_sss_set_callback: rc=0x%04x\n", rc);
		ASSERT( SLB_R_SUCCEEDED(rc) );
	}

	/* Store random values in plain data */
	rc = slb_sss_rand(hEncode, plainsize, plain);
	printf_detail("slb_sss_rand: rc=0x%04x\n", rc);
	ASSERT( SLB_R_SUCCEEDED(rc) );

	/* k loop */
	for (cur_k = k_min; cur_k <= k_max; ++cur_k) {

		/* Encode */
		printf_detail("--- encode: k=%d\n", cur_k);
		sss_example_encode( hEncode, plainsize, n, cur_k, random_x, plain, stream );

		/* Decode */
		printf_detail("--- decode: k=%d\n", cur_k);
		sss_example_decode( hDecode, plainsize, n, cur_k, plain, stream, SLB_TRUE );

		if (g_measure) {
			continue;
		}

		/* Decode success (= cur_k + 1) */
		if (cur_k < n) {
			printf_detail("--- decode: k=%d\n", cur_k + 1);
			sss_example_decode( hDecode, plainsize, n, cur_k + 1, plain, stream, SLB_TRUE );
		}

		/* Decode fail (= cur_k - 1) */
		if ((slb_int_t)SLB_SSS_MIN_PLAYERS < cur_k) {
			printf_detail("--- decode: k=%d\n", cur_k - 1);
			sss_example_decode( hDecode, plainsize, n, cur_k - 1, plain, stream, SLB_FALSE );
		}
	}

	/* Close SSS control handles */
	slb_sss_close(hEncode);
	slb_sss_close(hDecode);

	/* Free resources */
	slb_free( &g_mem_callback_param, stream, SLB_TRUE );
	slb_free( &g_mem_callback_param, plain, SLB_TRUE );
}

/* Encode */
void sss_example_encode(
	H_SLB_SSS			hEncode,		/* control handle to encode */
	slb_int_t			plainsize,		/* plain size */
	slb_int_t			n,				/* number of shares */
	slb_int_t			cur_k,			/* current threshold */
	slb_bool_t			random_x,		/* =SLB_TRUE: random x coordinates */
	const slb_uint16_t*	plain,			/* plain data */
	STREAM_BUFF*		stream			/* stream */
)
{
	SLB_RC rc;
	SLB_SSS_HANDLE_STATE hstate;
	slb_uint16_t*const x = slb_alloc( &g_mem_callback_param, sizeof(slb_uint16_t) * n );
	slb_int_t i;
	slb_int_t offset;
	slb_int_t best_nmb;
	time_t tm_start;
	int elapsed;

	/* Allocate resources */
	slb_uint32_t**const share = slb_alloc( &g_mem_callback_param, sizeof(slb_uint32_t**) * n );
	for (i = 0; i < n; ++i) {
		share[i] = slb_alloc( &g_mem_callback_param, sizeof(slb_uint32_t) * CODING_MAX_CHUNK );
	}

	/* Uses random x coordinates */
	if (random_x) {
		sss_get_random_x(hEncode, n, x);
	}

	/* Start encoding */
	rc = slb_sss_start_encode(hEncode, cur_k, n, random_x, x);
	hstate = slb_sss_get_handle_state(hEncode);
	printf_detail("slb_sss_start_encode: rc=0x%04x hstate=%d\n", rc, hstate);
	ASSERT( SLB_R_SUCCEEDED(rc) );
	ASSERT( hstate == HS_SSS_ENCODE_STARTED );

	/* Check the information */
	check_info(hEncode, cur_k, n, n - 1, x[n-1]);

	/* Copy x to stream-x */
	for (i = 0; i < n; ++i) {
		stream[i].x = x[i];
	}

	/* Get the best number of data */
	best_nmb = slb_sss_get_bestnmb(hEncode, CODING_MAX_CHUNK);
	printf_detail("slb_sss_get_bestnmb: best_nmb=%d\n", best_nmb);

	/* Start statistics update */
	rc = slb_sss_start_statistics(hEncode, CODING_MAX_CHUNK);
	ASSERT( SLB_R_SUCCEEDED(rc) );

	/* Start measurement */
	tm_start = time(SLB_NULL);

	/* Encode loop */
	offset = 0;

	/* Reset position in stream */
	sss_reset_stream_pos(n, stream);

	/* Process in optimal number units */
	while ( (offset < plainsize) && SLB_R_SUCCEEDED(rc) ) {

		/* Determine number of data */
		slb_int_t nmb = best_nmb;
		if ( (plainsize - offset) < nmb ) {
			nmb = plainsize - offset;
		}

		/* Encode */
		rc = slb_sss_encode(hEncode, nmb, &plain[offset], share);

		/* Reflect share contents to stream */
		sss_share_to_stream(n, nmb, share, stream);

		/* Prepare for next loop */
		offset += nmb;
	}

	ASSERT( SLB_R_SUCCEEDED(rc) );

	/* Stop statistics update */
	rc = slb_sss_stop_statistics(hEncode);
	ASSERT( SLB_R_SUCCEEDED(rc) );

	/* End measurement */
	elapsed = (int)( time(SLB_NULL) - tm_start );
	printf_detail("slb_sss_encode: rc=0x%04x\n", rc);
	if (g_measure) {
		printf("Encode %3d sec,  ", elapsed);
	}
	ASSERT( SLB_R_SUCCEEDED(rc) );

	/* Display statistics */
	if (g_measure == SLB_FALSE) {
		disp_stat(hEncode);
	}

	/* Free resources */
	for (i = 0; i < n; ++i) {
		slb_free( &g_mem_callback_param, share[i], SLB_TRUE );
	}

	slb_free( &g_mem_callback_param, x, SLB_TRUE );
	slb_free( &g_mem_callback_param, share, SLB_TRUE );
}

/* Decode */
void sss_example_decode(
	H_SLB_SSS			hDecode,		/* control handle to decode */
	slb_int_t			plainsize,		/* plain size */
	slb_int_t			n,				/* number of shares */
	slb_int_t			cur_k,			/* current threshold */
	const slb_uint16_t*	plain,			/* plain data */
	STREAM_BUFF*		stream,			/* stream */
	slb_bool_t			plain_match		/* =SLB_TRUE: expects plain data match */
)
{
	SLB_RC rc;
	SLB_SSS_HANDLE_STATE hstate;
	slb_uint16_t*const x = slb_alloc( &g_mem_callback_param, sizeof(slb_uint16_t) * n );
	slb_int_t i;
	slb_int_t offset;
	slb_int_t best_nmb;
	time_t tm_start;
	int elapsed;

	/* Allocate resources */
	slb_uint16_t*const plain2 = slb_alloc( &g_mem_callback_param, sizeof(slb_uint16_t) * plainsize );
	slb_uint32_t**const share = slb_alloc( &g_mem_callback_param, sizeof(slb_uint32_t**) * n );

	for (i = 0; i < n; ++i) {
		share[i] = slb_alloc( &g_mem_callback_param, sizeof(slb_uint32_t) * CODING_MAX_CHUNK );
	}

	memset(plain2, 0, plainsize * sizeof(slb_uint16_t));

	/* Copy stream-x to x */
	for (i = 0; i < n; ++i) {
		x[i] = stream[i].x;
	}

	/* Start decoding */
	rc = slb_sss_start_decode(hDecode, cur_k, x);
	hstate = slb_sss_get_handle_state(hDecode);
	printf_detail("slb_sss_start_decode: rc=0x%04x hstate=%d\n", rc, hstate);
	ASSERT( SLB_R_SUCCEEDED(rc) );
	ASSERT( hstate == HS_SSS_DECODE_STARTED );

	/* Check the information */
	check_info(hDecode, cur_k, 0, cur_k - 1, x[cur_k-1]);

	/* Get the best number of data */
	best_nmb = slb_sss_get_bestnmb(hDecode, CODING_MAX_CHUNK);
	printf_detail("slb_sss_get_bestnmb: best_nmb=%d\n", best_nmb);

	/* Start statistics update */
	rc = slb_sss_start_statistics(hDecode, CODING_MAX_CHUNK);
	ASSERT( SLB_R_SUCCEEDED(rc) );

	/* Start measurement */
	tm_start = time(SLB_NULL);

	/* Decode loop */
	offset = 0;

	/* Reset position in stream */
	sss_reset_stream_pos(n, stream);

	/* Process in optimal number units */
	while ( (offset < plainsize) && SLB_R_SUCCEEDED(rc) ) {

		/* Determine number of data */
		slb_int_t nmb = best_nmb;
		if ( (plainsize - offset) < nmb ) {
			nmb = plainsize - offset;
		}

		/* Reflect stream contents to share */
		sss_stream_to_share(n, nmb, stream, share);

		/* Decode */
		rc = slb_sss_decode(hDecode, nmb, (const slb_uint32_t **)share, &plain2[offset]);

		/* Prepare for next loop */
		offset += nmb;
	}

	ASSERT( SLB_R_SUCCEEDED(rc) );

	/* Stop statistics update */
	rc = slb_sss_stop_statistics(hDecode);
	ASSERT( SLB_R_SUCCEEDED(rc) );

	/* End measurement */
	elapsed = (int)( time(SLB_NULL) - tm_start );
	printf_detail("slb_sss_decode: rc=0x%04x\n", rc);
	if (g_measure) {
		printf("Decode %3d sec\n", elapsed);
	}
	ASSERT( SLB_R_SUCCEEDED(rc) );

	/* Compare plain data */
	if (plain_match) {
		if ( memcmp(plain, plain2, sizeof(slb_uint16_t) * plainsize) == 0 ) {
			printf_detail("plain data matched.\n");
		}
		else {
			printf("!!! plain data unmatched !!!\n");
			ASSERT(SLB_FALSE);
		}
	}
	else {
		if ( memcmp(plain, plain2, sizeof(slb_uint16_t) * plainsize) == 0 ) {
			printf_detail("!!! plain data matched !!!\n");
			ASSERT(SLB_FALSE);
		}
		else {
			printf("plain data unmatched.\n");
		}
	}

	/* Display statistics */
	if (g_measure == SLB_FALSE) {
		disp_stat(hDecode);
	}

	/* Free resources */
	for (i = 0; i < n; ++i) {
		slb_free( &g_mem_callback_param, share[i], SLB_TRUE );
	}

	slb_free( &g_mem_callback_param, x, SLB_TRUE );
	slb_free( &g_mem_callback_param, share, SLB_TRUE );
	slb_free( &g_mem_callback_param, plain2, SLB_TRUE );
}

/* Resets stream position */
void sss_reset_stream_pos(
	slb_int_t			n,			/* number of shares */
	STREAM_BUFF*const	stream		/* stream buffer */
)
{
	slb_int_t i;
	for (i = 0; i < n; ++i) {
		stream[i].pos = 0;
	}
}

/* Reflects share contents to stream
	The encoded data has meaning only from the LSB to 17 bits.
	And the 17th bit is needed only if the lower 16 bit value is zero.
	Here writes byte values if the lower 16 bit value is zero for simplicity.
	Since the probability that the lower 16-bit value will be zero is low,
	this method is practical when encoding plaintext of indeterministic size.
	However, when encoding plaintext with a fixed size,
	a structure in which the corresponding 17th bit is placed at a fixed position can be considered.
*/
void sss_share_to_stream(
	slb_int_t			n,			/* number of shares */
	slb_int_t			nmb,		/* number of use */
	slb_uint32_t**const	share,		/* share buffer */
	STREAM_BUFF*const	stream		/* stream buffer */
)
{
	slb_int_t i, j;

	/* Reflect share contents to stream */
	for (i = 0; i < n; ++i) {

		/* Get write position of stream */
		slb_uint_t pos = stream[i].pos;

		for (j = 0; j < nmb; ++j) {

			/* Get the low value of share and write it to stream */
			slb_uint16_t low = (slb_uint16_t)share[i][j];
			SLB_WRITE16( &stream[i].data[pos], low );
			pos += sizeof(slb_uint16_t);

			/* Value unknown at that low width */
			if (low == 0) {

				/* Get high value and write to stream */
				slb_uint8_t high = (slb_uint8_t)( share[i][j] >> SLB_SHIFT_OF_2B );
				stream[i].data[pos] = high;
				++pos;
			}
		}

		/* Update position of stream */
		stream[i].pos = pos;
	}
}

/* Reflects stream contents to share */
void sss_stream_to_share(
	slb_int_t			n,			/* number of shares */
	slb_int_t			nmb,		/* number of use */
	STREAM_BUFF*const	stream,		/* stream buffer */
	slb_uint32_t**const	share		/* share buffer */
)
{
	slb_int_t i, j;

	/* Reflect stream contents to share */
	for (i = 0; i < n; ++i) {

		/* Get the reading position of the stream */
		slb_uint_t pos = stream[i].pos;

		for (j = 0; j < nmb; ++j) {

			/* Get the low value of stream */
			slb_uint16_t low;
			SLB_READ16( &stream[i].data[pos], &low );

			/* Write low value to share */
			share[i][j] = (slb_uint32_t)low;
			pos += sizeof(slb_uint16_t);

			/* Value unknown in low width */
			if (low == 0) {

				/* Reflect high value to share */
				slb_uint32_t high = (slb_uint32_t)stream[i].data[pos] << SLB_SHIFT_OF_2B;
				share[i][j] |= high;
				++pos;
			}
		}

		/* Update position of stream */
		stream[i].pos = pos;
	}
}

/* Gets random x coordinates */
void sss_get_random_x(H_SLB_SSS hEncode, slb_int_t n, slb_uint16_t x[])
{
	const slb_int_t max_players = (slb_int_t)slb_sss_get_max_players_decode();
	const slb_uint_t x_src_size = sizeof(slb_uint16_t) * (slb_uint_t)max_players;
	slb_uint16_t* x_src = (slb_uint16_t*)slb_alloc( &g_mem_callback_param, x_src_size );
	const slb_uint_t rand_val_size = sizeof(slb_uint_t) * (slb_uint_t)n;
	slb_uint_t* rand_val = (slb_uint_t*)slb_alloc( &g_mem_callback_param, rand_val_size );
	slb_int_t i, j;

	for (i = 0; i < max_players; ++i) {
		x_src[i] = (slb_uint16_t)(i + 1);
	}

	ASSERT( slb_sss_rand(hEncode, rand_val_size, rand_val) == R_SUCCESS );

	for (i = 0; i < n; ++i) {
		j = (slb_int_t)( rand_val[i] % (slb_uint_t)(max_players - i) );
		j += i;
		x[i] = x_src[j];
		x_src[j] =  x_src[i];
	}

	slb_free( &g_mem_callback_param, x_src, SLB_FALSE );
	slb_free( &g_mem_callback_param, rand_val, SLB_FALSE );
}

/* Checks information */
void check_max(H_SLB_SSS handle, slb_int_t k_max, slb_int_t n_max)
{
	slb_uint_t info_val;

	info_val = slb_sss_get_info_k_max(handle);
	if ( info_val != (slb_uint_t)k_max ) {
		printf("!!! unexpected k_max\n");
		ASSERT(SLB_FALSE);
	}

	info_val = slb_sss_get_info_n_max(handle);
	if ( info_val != (slb_uint_t)n_max ) {
		printf("!!! unexpected n_max\n");
		ASSERT(SLB_FALSE);
	}
}

void check_info(H_SLB_SSS handle, slb_int_t k, slb_int_t n, slb_int_t index, slb_uint16_t xval)
{
	slb_uint_t info_val;
	slb_uint16_t info_x;

	info_val = slb_sss_get_info_k(handle);
	if ( info_val != (slb_uint_t)k ) {
		printf("!!! unexpected k\n");
		ASSERT(SLB_FALSE);
	}

	info_val = slb_sss_get_info_n(handle);
	if ( info_val != (slb_uint_t)n ) {
		printf("!!! unexpected n\n");
		ASSERT(SLB_FALSE);
	}

	info_x = slb_sss_get_info_x( handle, index );
	if ( info_x != xval ) {
		printf("!!! unexpected x\n");
		ASSERT(SLB_FALSE);
	}
}

/* Print details */
void printf_detail(const char* form, ...)
{
	if (g_measure == SLB_FALSE) {
		va_list vl;
		va_start( vl, form );
		vprintf( form, vl );
		va_end(vl);
	}
}

/* Edit SIMD names */
void print_simd_names(slb_uint_t simd_flags)
{
	slb_bool_t bSSE2   = (simd_flags & F_SSE2) && IsSimdAvailableSSE2();
	slb_bool_t bAVX2   = (simd_flags & F_AVX2) && IsSimdAvailableAVX2();
	slb_bool_t bAVX512 = (simd_flags & F_AVX512) && IsSimdAvailableAVX512();

	if (bSSE2) {
		printf(" SSE2");
	}
	if (bAVX2) {
		printf(" AVX2");
	}
	if (bAVX512) {
		printf(" AVX-512");
	}
}

/* Displays  statistics */
void disp_stat(H_SLB_SSS handle)
{
	SLB_SSS_STATISTICS statistics;
	SLB_RC rc = slb_sss_get_statistics(handle, &statistics, sizeof(statistics));
	ASSERT( SLB_R_SUCCEEDED(rc) );

	printf("slb_sss_get_statistics:\n");
	printf(	"maxnmb[%d] optimalnmb[%d] bestnmb[%d]\n"
			"called_cnt[%u] best_called_cnt[%u] max_nmb_called[%d]\n"
			"coding_cnt[%u] best_coding_cnt[%u] optimal_cnt[%u] best_optimal_cnt[%u]\n"
			"sse2_cnt[%u] avx2_cnt[%u] avx512_cnt[%u]\n"
			"parallel_cnt[%u] max_cores[%u] max_paran[%u]\n",

			statistics.maxnmb,
			statistics.optimalnmb,
			statistics.bestnmb,

			statistics.called_cnt,
			statistics.best_called_cnt,
			statistics.max_nmb_called,

			statistics.coding_cnt,
			statistics.best_coding_cnt,
			statistics.optimal_cnt,
			statistics.best_optimal_cnt,

			statistics.sse2_cnt,
			statistics.avx2_cnt,
			statistics.avx512_cnt,

			statistics.parallel_cnt,
			statistics.max_cores,
			statistics.max_paran
	);
}

/* Allocates and frees memory callback function
	Although the malloc/free functions are used to focus on libslbsss usage,
	the memory handled by these functions might be swapped and written out to storage.
	When creating practical applications, consider memory manipulation functions that are not swapped.
	SSharing published by our company uses non-swap memory using VirtualLock.

	libslbsss might temporarily store plaintext in the memory obtained by the alloc callback function,
	but the contents are securely erased before calling the free callback function, so there is no need to implement that process.
*/

void* alloc_callback(void* param, slb_uint_t size)
{
	ASSERT( GetCurrentThreadId() == g_MyThreadId );		/* Check thread ID */
	ASSERT( param == &g_mem_callback_param );			/* Check parameter */
	return malloc(size);
}

void free_callback(void* param, void* p, slb_uint_t size, slb_bool_t cleared)
{
	ASSERT( GetCurrentThreadId() == g_MyThreadId );		/* Check thread ID */
	ASSERT( param == &g_mem_callback_param );			/* Check parameter */

	/* Checks memory is cleared */
	if (cleared) {
		ASSERT( is_mem_cleared(p, size) );
	}

	free(p);
}

/* Random function to be called back
	Although the rand function is used to focus on how to use libslbsss,
	when creating practical applications, please use a high-quality random number generation function.
	SSharing published by our company uses BCryptGenRandom.
*/
slb_bool_t rand_callback(void* param, slb_uint_t len, void* rnd_buff)
{
	static long s_entry;
	slb_uint_t i;
	const slb_uint_t len16 = len / sizeof(slb_uint16_t);
	slb_uint16_t* buff16 = (slb_uint16_t*)rnd_buff;
	slb_uint8_t* buff8 = (slb_uint8_t*)rnd_buff;

	long entry = InterlockedIncrement(&s_entry);
	ASSERT( entry == 1 );						/* Check no concurrent calls */
	ASSERT( param == &g_rand_callback_param );	/* Check parameter */

	for (i = 0; i < len16; ++i) {
		buff16[i] = (slb_uint16_t)rand();
	}

	i *= sizeof(slb_uint16_t);
	if (i < len) {
		buff8[i] = (slb_uint8_t)rand();
	}

	InterlockedDecrement(&s_entry);

	return SLB_TRUE;
}

/* My function to be called back */
slb_bool_t user_callback(void* param)
{
	ASSERT( GetCurrentThreadId() == g_MyThreadId );		/* Check thread ID */
	ASSERT( param == &g_user_callback_param );			/* Check parameter */
	return SLB_FALSE;
}

/* Checks that memory is cleared */
slb_bool_t is_mem_cleared(const void* p, slb_uint_t size)
{
	slb_bool_t cleared = SLB_TRUE;
	slb_uint_t i;
	const slb_uint_t dwcount = size >> 2;
	const slb_uint_t fraction = size - (dwcount << 2);
	const slb_uint32_t* pdwd = (const slb_uint32_t*)p;
	const slb_uint8_t* pbd;

	for (i = 0U; i < dwcount; ++i) {
		if (*pdwd != 0U) {
			cleared = SLB_FALSE;
			break;
		}
		++pdwd;
	}

	if (cleared) {
		pbd = (const slb_uint8_t*)pdwd;

		for (i = 0U; i < fraction; ++i) {
			if (*pbd != 0U) {
				cleared = SLB_FALSE;
				break;
			}
			++pbd;
		}
	}

	return cleared;
}
