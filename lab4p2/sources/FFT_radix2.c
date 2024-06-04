// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
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
 * Definición del vector de factores de Tweddle
 */
Complex Wn[FFT_NPOINTS];

/******************************************************************************
**      PRIVATE FUNCTION DECLARATIONS (PROTOTYPES)
******************************************************************************/

/******************************************************************************
**      FUNCTION DEFINITIONS
******************************************************************************/

/***************************************************************************//**
*   \brief  Función que calcula la FFT del vector complejo de entrada
*           'inputSignal' y que escribe el vector complejo de frecuencias
*           en 'freqOutputVector'
*
*   \param  fftSize             : largo del vector a calcular.
*   \param  *inputSignal        : puntero a vector complejo de señal
*   \param  *freqOutputVector   : puntero a vector complejo de frecuencias
*
*   \return Void.
*******************************************************************************/
extern void fftRadix2(unsigned int fftSize, Complex *inputSignal, Complex *freqOutputVector){
    if(fftSize == 2){
        freqOutputVector[0].real = inputSignal[0].real + inputSignal[1].real;
        freqOutputVector[0].img = inputSignal[0].img + inputSignal[1].img;
        freqOutputVector[1].real = inputSignal[0].real - inputSignal[1].real;
        freqOutputVector[1].img = inputSignal[0].img - inputSignal[1].img;

    }
    else{
        Complex W[fftSize/2];
        int n;
        for(n = 0; n < fftSize/2; n++){
            W[n].real = cos(-2*PI*n/((float)fftSize));
            W[n].img  = sin(-2*PI*n/((float)fftSize));
        }

        fftRadix2_(fftSize/2, inputSignal, freqOutputVector);

        for(n = 0; n < fftSize/2; n++){
            Complex multi = multiplicar(freqOutputVector[n+fftSize/2],W[n]);
            freqOutputVector[n + fftSize/2].real = freqOutputVector[n].real - multi.real;
            freqOutputVector[n + fftSize/2].img = freqOutputVector[n].img - multi.img;
            freqOutputVector[n].real = freqOutputVector[n].real + multi.real;
            freqOutputVector[n].img = freqOutputVector[n].img + multi.img;

        }
    }
}












extern void fftRadix2_(unsigned int fftSize, Complex *inputSignal, Complex *freqOutputVector){
    if(fftSize == 2){
        freqOutputVector[0].real = inputSignal[0].real + inputSignal[1].real;
        freqOutputVector[0].img = inputSignal[0].img + inputSignal[1].img;
        freqOutputVector[1].real = inputSignal[0].real - inputSignal[1].real;
        freqOutputVector[1].img = inputSignal[0].img - inputSignal[1].img;

    }
    else{
    Complex x_par[fftSize/2], x_impar[fftSize/2], W[fftSize/2];
    int n;
    for(n = 0; n < fftSize/2; n++){
        x_par[n] = inputSignal[2*n];
        x_impar[n] = inputSignal[2*n + 1];
        W[n].real = cos(-2*PI*n/((float)fftSize)); W[n].img = sin(-2*PI*n/((float)fftSize));
    }

    Complex X_par[fftSize/2], X_impar[fftSize/2];

    fftRadix2_(fftSize/2, x_par, X_par);
    fftRadix2_(fftSize/2, x_impar, X_impar);
    for(n = 0; n < fftSize/2; n++){
        Complex multi = multiplicar(X_impar[n],W[n]);
        freqOutputVector[n].real = X_par[n].real + multi.real;
        freqOutputVector[n].img = X_par[n].img + multi.img;
        freqOutputVector[n + fftSize/2].real = X_par[n].real - multi.real;
        freqOutputVector[n + fftSize/2].img = X_par[n].img - multi.img;
    }}
}

/***************************************************************************//**
*   \brief  Función que inicializa los valores de los factores de Tweddle
*           segùn el tamaño de la FFT a realizar
*
*   \param  size    : tamaño de la tabla (debe ser una potencia de 2).
*   \param  *bitReverseTable : puntero a vector que será la tabla
*
*   \return Void.
*******************************************************************************/
extern void initBitReversalTable(unsigned int size, unsigned int *bitReverseTable){
    unsigned int idx;
    unsigned int p = 1;
    /* Llenado inicial de tabla con índices correlativos */
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
*   \brief  Función que calcula la magnitud de un vector complejo de frecuencias
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
*   \brief  Función que inicializa los valores de los factores de Tweddle
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


Complex multiplicar(Complex f1, Complex f2){
    Complex resultado;
    resultado.real = f1.real*f2.real-f1.img*f2.img;
    resultado.img = f1.real*f2.img+f1.img*f2.real;
    return resultado;
}
