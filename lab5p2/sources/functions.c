/***************************************************************************//**
* \file     Funciones que deben impementar los alumnos
*
* \brief    Función 'window' para aplicar ventana hamming a frame
*           Función 'xcorr' para hacer las autocorrelaciones del frame
*           Función 'filter' para implementar el filtro autoregresivo (solo polos)
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

/******************************************************************************
**      MODULE MACROS
******************************************************************************/

/******************************************************************************
**      MODULE DATATYPES
******************************************************************************/

/******************************************************************************
**      MODULE VARIABLE DEFINITIONS
******************************************************************************/
//float filterOutExtention[LPC_ORDER];

/******************************************************************************
**      PRIVATE FUNCTION DECLARATIONS (PROTOTYPES)
******************************************************************************/


/******************************************************************************
**      FUNCTION DEFINITIONS
******************************************************************************/

/***************************************************************************//**
*   \brief Pondera cada elemento del frame por los coeficientes
*           correspondientes de la ventana Hamming.
*
*   \param  input : puntero a arreglo de entrada para frame
*           output : puntero a arreglo de salida para frame
*           window: puntero a arreglo con ventana
*
*   \return Void.
*******************************************************************************/
void windowFrame(float *output, float *input, float *window){
    // IMPLEMENTAR VENTANA USANDO hammWin[] de const_arrays.h
}

/***************************************************************************//**
*   \brief  Función para computar el arreglo de autocorrelaciones del frame
*
*   \param  rmsOut     : cómputo de el valor RMS del frame de entrada
*           autocorrOut: vector de autocorrelaciones para 'lags' diferentes
*                        resultando [r(0) r(1) .. r(pp1-1)]
*           input       : arreglo del frame de entrada
*           pp1         : largo del arreglo de autocorrelaciones
*           frameSize   : largo del arreglo de datos de 'input'
*
*   \return Void.
*******************************************************************************/
void xcorr(float* rmsOut, float* autocorrOut, float* input, unsigned int pp1, unsigned int frameSize){
    // IMPLEMENTAR AUTOCORRELACIÓN
}

/***************************************************************************//**
*   \brief  Función paraimplementar filtro autoregresivo (solo polos) para la
*           síntesis de voz.
*
*   \param  rmsOut     : cómputo del valor RMS del frame de salida del filtro
*           output      : arreglo de salida filtrada
*           denominatorCoeff   : arreglo de coeficientes de denominador
*           p           : orden del filtro
*           input       : arreglo de entrada a filtar
*           frameSize   : largo del arreglo de datos de 'input'
*
*   \return Void.
*******************************************************************************/
void arFilter(float *rmsOut, float *output, float *denominatorCoeff, unsigned int p, float *input, unsigned int frameSize){
    // IMPLEMENTAR FILTRADO
}

/******************************************************************************
**      END OF SOURCE FILE
******************************************************************************/
