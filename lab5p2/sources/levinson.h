#ifndef LEVINSON_H
#define LEVINSON_H

#ifdef __cplusplus
extern "C" {
#endif
/*************************************************************************************************/
//#include "global_defs.h"

void levinson_init(short order);

void levinson(float* alpha, float* r, short order);

/*************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif
