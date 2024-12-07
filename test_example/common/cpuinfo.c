/*******************************************************************************
 CPU information
 *******************************************************************************/
#include "cpuinfo.h"

#if defined(_MSC_VER)
#	include <windows.h>
#	include <intrin.h>
#	define	GET_CPUID(inf, id)			__cpuid(inf, id)
#	define	GET_CPUIDEX(inf, id, sid)	__cpuidex(inf, id, sid)
#elif defined(__GNUC__)
#	include <unistd.h>
#	include <cpuid.h>
#	define	GET_CPUID(inf, id)			__cpuid(id, inf[0], inf[1], inf[2], inf[3])
#	define	GET_CPUIDEX(inf, id, sid)	__cpuid_count(id, sid, inf[0], inf[1], inf[2], inf[3])
#endif

/* Returns if SSE2 is available */
slb_bool_t IsSimdAvailableSSE2(void)
{
	static slb_bool_t s_bAvailable = -1;
	if (s_bAvailable < 0) {
		int eax_max;
		int info[4];
		GET_CPUID(info, 0);
		eax_max = info[0];
		if (1 <= eax_max) {
			GET_CPUID(info, 1);
			s_bAvailable = (slb_bool_t)( (info[3] & (1 << 26)) != 0 );
		}
		else {
			s_bAvailable = SLB_FALSE;
		}
	}
	return s_bAvailable;
}

/* Returns if AVX2 is available */
slb_bool_t IsSimdAvailableAVX2(void)
{
	static slb_bool_t s_bAvailable = -1;
	if (s_bAvailable < 0) {
		int eax_max;
		int info[4];
		GET_CPUID(info, 0);
		eax_max = info[0];
		if (7 <= eax_max) {
			GET_CPUIDEX(info, 7, 0);
			s_bAvailable = (slb_bool_t)( (info[1] & (1 << 5)) != 0 );
		}
		else {
			s_bAvailable = SLB_FALSE;
		}
	}
	return s_bAvailable;
}

/* Returns if AVX-512 is available */
slb_bool_t IsSimdAvailableAVX512(void)
{
	static slb_bool_t s_bAvailable = -1;
	if (s_bAvailable < 0) {
		int eax_max;
		int info[4];
		GET_CPUID(info, 0);
		eax_max = info[0];
		if (7 <= eax_max) {
			GET_CPUIDEX(info, 7, 0);
			s_bAvailable = (slb_bool_t)( (info[1] & (1 << 16)) != 0 );
		}
		else {
			s_bAvailable = SLB_FALSE;
		}
	}
	return s_bAvailable;
}
