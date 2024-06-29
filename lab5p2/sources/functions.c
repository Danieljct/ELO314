/***************************************************************************//**
* \file     Funciones que deben impementar los alumnos
*
* \brief    Función 'window' para aplicar ventana hamming a frame
*           Función 'xcorr' para hacer las autocorrelaciones del frame
*           Función 'arFilter' para implementar el filtro autoregresivo (solo polos)
*           Función 'excitation_generatePulses' para generar señal de impulsos.
*
* \authors  Gonzalo Carrasco
*******************************************************************************/

/******************************************************************************
**      HEADER FILES
******************************************************************************/
#include <math.h>
#include "dlu_global_defs.h"
#include "functions.h"

/******************************************************************************
**      MODULE PREPROCESSOR CONSTANTS
******************************************************************************/
#define     EXCITATION_PULSE_MIN    0.0
#define     EXCITATION_PULSE_MAX    0.98

/******************************************************************************
**      MODULE MACROS
******************************************************************************/

/******************************************************************************
**      MODULE DATATYPES
******************************************************************************/

/******************************************************************************
**      MODULE VARIABLE DEFINITIONS
******************************************************************************/

/******************************************************************************
**      PRIVATE FUNCTION DECLARATIONS (PROTOTYPES)
******************************************************************************/


/******************************************************************************
**      FUNCTION DEFINITIONS
******************************************************************************/

/***************************************************************************//**
*   \brief Pondera cada elemento del frame por los coeficientes
*           correspondientes de la ventana dada.
*
*   \param  *input  : arreglo de entrada para frame
*           *output : arreglo de salida para frame
*           *window : arreglo de coeficientes de una ventana
*
*   \return Void.
*******************************************************************************/
void windowFrame(float *output, float *input, float *window)
{
    // IMPLEMENTAR VENTANA USANDO const_arrays.h

}

/***************************************************************************//**
*   \brief  Función para computar el arreglo de autocorrelaciones del frame
*
*   \param  *rmsOut     : cómputo de el valor RMS del frame recibido como entrada
*           *autocorrOut: vector de autocorrelaciones para 'lags' diferentes
*                        resultando [r(0) r(1) .. r(length-1)]
*           *input      : arreglo del frame de entrada
*           length      : largo del arreglo de autocorrelaciones
*           frameSize   : largo del arreglo de datos de 'input'
*
*   \return Void.
*******************************************************************************/
void xcorr(float* autocorrOut, float* input, unsigned int length, unsigned int frameSize)
{
    memset(autocorrOut, 0,length*sizeof(float));
    int j, n;
    for(j = 0; j < length; j++){
        for (n = 0; n < frameSize - j; n++){
            autocorrOut[j] += (input[n])*(input[n+j]);
        }
    }
}

/***************************************************************************//**
*   \brief  Función paraimplementar filtro autoregresivo (solo polos) para la
*           síntesis de voz.
*           Filtro AR que considera estados del frame anterior
*
*   \param  *rmsOut     : cómputo del valor RMS del frame de salida del filtro
*           *output      : arreglo de salida filtrada
*           denominatorCoeff   : arreglo de coeficientes de denominador
*           p           : orden del filtro
*           *input       : arreglo de entrada a filtar
*           frameSize   : largo del arreglo de datos de 'input' y 'output'
*   \return Void.
*******************************************************************************/
void arFilter(float *output, float *denominatorCoeff, unsigned int p, float *input, unsigned int frameSize)
{
    int k,n;
    for (n = 0; n < frameSize; n++){
        output[n] = input[n];
        for(k = 1; k < p; k++){
            output[n] -= denominatorCoeff[k]*output[(n+frameSize-k)%frameSize];
        }
    }

}

/***************************************************************************//**
*   \brief  Función para generar pulsos de excitación a filtro, con frecuencia
*           variable. Evita contar con transiciones espurias de frecuencia
*           de operación, ajustando los cambios de periodo de forma suave.
*
*   \param  *pulsesOutput     : puntero a vector donde se sintetiza tren de pulsos
*           pulsesPeriodInSamples   : parámetro de entrada que indica el perido en
*                             muestras
*           frameSize         : Tamaño del arreglo de salida que será generado
*
*
*   \return Void.
*******************************************************************************/
void excitation_generatePulses(float *pulsesOutput, unsigned int pulsesPeriodInSamples, unsigned int frameSize)
{
    int remainingSamplesForPeriod;
    int i;
    static int lastRemainingSamples = 0;
    /*----------------------------------------------------------------------*/
    if (lastRemainingSamples >= pulsesPeriodInSamples)
        remainingSamplesForPeriod = 0;
    else
        remainingSamplesForPeriod = pulsesPeriodInSamples - lastRemainingSamples;
    /*----------------------------------------------------------------------*/
    // Llenado de buffer con pulsos sintetizados
    for(i = 0; i < frameSize; i++){
        if (remainingSamplesForPeriod == 0)
        {
            pulsesOutput[i] = EXCITATION_PULSE_MAX;
            remainingSamplesForPeriod = pulsesPeriodInSamples - 1;
        }
        else
        {
            pulsesOutput[i] = EXCITATION_PULSE_MIN;
            remainingSamplesForPeriod--;
        }
    }
    /*----------------------------------------------------------------------*/
    lastRemainingSamples = pulsesPeriodInSamples - remainingSamplesForPeriod;

}

/******************************************************************************
**      END OF SOURCE FILE
******************************************************************************/
