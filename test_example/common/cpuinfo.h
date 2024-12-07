/*******************************************************************************
 CPU information
 *******************************************************************************/
#ifndef CPUINFO_H
#define CPUINFO_H

#include "slb_types.h"

#ifdef __cplusplus
extern "C" {
#endif

slb_bool_t IsSimdAvailableSSE2(void);
slb_bool_t IsSimdAvailableAVX2(void);
slb_bool_t IsSimdAvailableAVX512(void);

#ifdef __cplusplus
}
#endif

#endif	/* ifndef CPUINFO_H */
