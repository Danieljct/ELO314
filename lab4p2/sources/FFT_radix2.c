/***************************************************************************//**
* \file     fft_radix2.c
*
* \brief    Base para el laboratorio L4p2
*
* \authors  Gonzalo Carrasco
*******************************************************************************/

/***************************************************************************//**
**      HEADER FILES
*******************************************************************************/
#include <math.h>
#include <stdlib.h>
#include "complex.h"
#include "FFT_radix2.h"

/******************************************************************************
**      MODULE PREPROCESSOR CONSTANTS
******************************************************************************/

/******************************************************************************
**      MODULE MACROS
******************************************************************************/

/*
 * Constante para precalcular factores de Tweddle
 */
#define PI          3.141592653589793

/******************************************************************************
**      MODULE DATATYPES
******************************************************************************/

/******************************************************************************
**      MODULE VARIABLE DEFINITIONS
******************************************************************************/
/*
 * Definici�n de vector de factores de Tweddle
 */
Complex Wn[FFT_NPOINTS];

/******************************************************************************
**      PRIVATE FUNCTION DECLARATIONS (PROTOTYPES)
******************************************************************************/

/******************************************************************************
**      FUNCTION DEFINITIONS
******************************************************************************/

/***************************************************************************//**
*   \brief  Funci�n que calcula la FFT del vector complejo de entrada
*           'inputSignal' y que escribe el vector complejo de frecuencias
*           en 'freqOutputVector'
*
*   \param  fftSize             : largo del vector a calcular.
*   \param  *inputSignal        : puntero a vector complejo de se�al
*   \param  *freqOutputVector   : puntero a vector complejo de frecuencias
*
*   \return Void.
*******************************************************************************/
extern void fftRadix2(unsigned int fftSize, Complex *inputSignal, Complex *freqOutputVector){
    // TODO
}

/***************************************************************************//**
*   \brief  Funci�n que inicializa los valores de los factores de Tweddle
*           seg�n el tama�o de la FFT a realizar
*
*   \param  size    : tama�o de la tabla (debe ser una potencia de 2).
*   \param  *bitReverseTable : puntero a vector que ser� la tabla
*
*   \return Void.
*******************************************************************************/
extern void initBitReversalTable(unsigned int size, unsigned int *bitReverseTable){
    unsigned int idx;
    unsigned int p = 1;
    /* Llenado inicial de tabla con �ndices correlativos */
    for (idx = 0 ; idx < size ; idx++)
        bitReverseTable[idx] = idx;
    /* Bit reversal */
    while (p < size){
        for (idx = 0 ; idx < p ; idx++){
            bitReverseTable[idx]     = bitReverseTable[idx] << 1; // br_table[idx] * 2
            bitReverseTable[idx + p] = bitReverseTable[idx] + 1;
        }
        p = p << 1;  // p * 2
    }
}

/***************************************************************************//**
*   \brief  Funci�n que calcula la magnitud de un vector complejo de frecuencias
*           'fftFreqVector' entregado por una FFT. Escribe el resultado en
*           'fftMagVector'.
*
*   \param  fftSize    : largo del vector a calcular.
*   \param  *fftMagVector : puntero a vector complejo de frecuencias
*   \param  *fftFreqVector  : puntero a vector de flotantes de magnitud.
*
*   \return Void.
*******************************************************************************/
extern void fftMag(unsigned int fftSize, Complex *fftFreqVector, float *fftMagVector){
    unsigned int idx;

    for (idx = 0; idx < fftSize; idx++)
        fftMagVector[idx] = c_mag(&fftFreqVector[idx]);
}

/***************************************************************************//**
*   \brief  Funci�n que inicializa los valores de los factores de Tweddle
*
*   \return Void.
*******************************************************************************/
extern void initTweddleFactors(void)
{
    int n;
    for (n=0 ; n< FFT_NPOINTS ; n++)
    {
        /* Tweddele factors */
        Wn[n].real = cos(2*PI*n/((float) FFT_NPOINTS));
        Wn[n].img  =-sin(2*PI*n/((float) FFT_NPOINTS));
    }
}

