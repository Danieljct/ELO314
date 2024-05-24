#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif
/*************************************************************************************************/
void windowFrame(float *output, float *input, float *window);

void xcorr(float* rmsOut, float* autocorrOut, float* input, unsigned int pp1, unsigned int frameSize);

void arFilter(float *rmsOut, float *output, float *denominatorCoeff, unsigned int p, float *input, unsigned int frameSize);

void arFilter2(float *rmsOut, float *output, float *denominatorCoeff, unsigned int p, float *input, unsigned int frameSize);

void arFilter3(float *rmsOut, float *output, float *denominatorCoeff, unsigned int p, float *input, unsigned int frameSize);
/*************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif
