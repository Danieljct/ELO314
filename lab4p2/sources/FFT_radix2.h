#ifndef FFT_RADIX2_H
#define FFT_RADIX2_H

#ifdef __cplusplus
extern "C" {
#endif
/*************************************************************************************************/
#include "dlu_global_defs.h"
#include "complex.h"

extern void fftRadix2(unsigned int size, Complex *time_input, Complex *freq_output);
extern void fftMag(unsigned int size, Complex *fft_freq_vec, float *fft_mag_vec);
extern void fftRadix2BR(unsigned int size, Complex *time_input, Complex *freq_output);
extern void initBitReversalTable(unsigned int size, unsigned int *br_table);
extern void initTweddleFactors(void);

/*************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif
