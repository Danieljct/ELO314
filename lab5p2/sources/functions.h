#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif
/*************************************************************************************************/
void windowFrame(float *output, float *input, float *window);

void xcorr(float* autocorrOut, float* input, unsigned int length, unsigned int frameSize);

void arFilter(float *output, float *denominatorCoeff, unsigned int p, float *input, unsigned int frameSize);

void excitation_generatePulses(float *pulsesOutput, unsigned int pulsesPeriodInSamples, unsigned int frameSize);

/*************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif
