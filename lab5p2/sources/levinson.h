#ifndef LEVINSON_H
#define LEVINSON_H

#ifdef __cplusplus
extern "C" {
#endif
/*************************************************************************************************/
//#include "global_defs.h"

void levinson_init(short order);

void levinson_computeCoeffs(float* arCoeffs, float* rx, short order);

/*************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif
