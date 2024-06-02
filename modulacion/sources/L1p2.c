// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/******************************************************************************
* \file     Lab1p2_solucion.c
*
* \brief    Experiencia 1 de laboratorio DSP ELO314
*
* \authors  Gonzalo Carrasco
******************************************************************************/

/******************************************************************************
**      HEADER FILES
******************************************************************************/
#include "dlu_global_defs.h"
#include "L138_LCDK_aic3106_init.h"
#include "dsp_lab_utils.h"
#include <math.h>
#include "dlu_codec_config.h"
#include "oscilloscope_meas.h"

/******************************************************************************
**      MODULE PREPROCESSOR CONSTANTS
******************************************************************************/

#define PI 3.1415926535897932384626433

/* Para los ejercicios iniciales son en punto fijo, 1 al 11 de la Guía Lab1P2 2022
 * NO definir 'FLOAT_OPERATIONS'
 *
 * Para los puntos 12 y 13 de la Guía Lab1P2 2022
 * Definir 'FLOAT_OPERATIONS'
 */
//#define FLOAT_OPERATIONS

typedef struct bqStatus_t {
    float bqA1;
    float bqA2;
    float bqB0;
    float bqB1;
    float bqB2;
    float bqInput[3];
    float bqOutput[3];
} bqStatus_t;

typedef enum FILTERTYPE{
    NOTCH,
    PASS
}FILTERTYPE;

int BW1 = 10;
int FPB = 110;

float filterBiquad(bqStatus_t *filterNState, float filterInput);
bqStatus_t CrearFiltro(FILTERTYPE type, float BW, float fc);
void passUpdate(bqStatus_t* Filtro, float fc, float BW);

float salidafiltroL = 0;
float salidafiltroR = 0;
bqStatus_t FiltroL;
bqStatus_t FiltroR;
bqStatus_t FiltroNR;
bqStatus_t FiltroNL;


int32_t gIdxOscMeas = 0;
float sonidoizq;
float sonidoder;
/*---------------------------------------------------------------------------*/
/* ENTRADAS Y SALIDAS DEL AIC CODEC */
/*---------------------------------------------------------------------------*/
/*
 * Tipo de dato para el CODEC (Union)
 */
AIC31_data_type codec_data;

/*
 * Varibles de entrada y salida en formato entero
 */
short gIntCodecInputR, gIntCodecInputL;
short gIntCodecOutputR, gIntCodecOutputL;

/*
 * Varibles de entrada y salida en formato flotante
 */
float gFloatCodecInputR,gFloatCodecInputL;
float gFloatCodecOutputR,gFloatCodecOutputL;

/*
 * Variables de estado de salida saturada
 */
int gOutSaturationStat = 0;

int16_t         gCosSignal;
int16_t         gAmpCosine = 1024;
int16_t         gAmplitudeModulationL = 0;
int16_t         gAmplitudeModulationR = 0;

float           gThetaCosineSignal = 0.0;
float           gFreqCosineMin = 40.0;

uint8_t sonido = 0;

/* Máscada de bits */
int16_t         gMaskLsbs = 0xF800;
int16_t         gMaskedLsbOutput = 0;


interrupt void interrupt4(void) // interrupt service routine
{

        codec_data.uint = input_sample();
        gIntCodecInputL = codec_data.channel[LEFT];
        gIntCodecInputR = codec_data.channel[RIGHT];


        /*-------------------------------------------------------------------*/
        // USO DE ESTADO DE PULSADOR USER1 y USER2 PARA ACTIVAR LEDs
        /*-------------------------------------------------------------------*/
        // Obtención de valores de estados para cambiar estado de leds

        /* Actualizando índice de lectura para medición con osciloscopio */
        if (sonido){
            gIdxOscMeas++;


            if (gIdxOscMeas >= OSCILLOSCOPE_SIGSIZE){
                gIdxOscMeas = 0;
                sonido = 0;
            }
            sonidoizq  = izq[gIdxOscMeas]*32768;
            sonidoder  = der[gIdxOscMeas]*32768;
        }

        /*-------------------------------------------------------------------*/
        /* GENERACIÓN DE REFERENCIA SINUSOIDAL */
        /*-------------------------------------------------------------------*/
        // Ángulo se incrementa linealmente segun frecuencia angular
        // dada por un valor base más el valor del contador de ajuste
        gThetaCosineSignal  = gThetaCosineSignal + 2.0 * M_PI* ( gFreqCosineMin) * TS;
        // Theta entre [0, 2*pi]
        if ( gThetaCosineSignal > 2.0 * M_PI )
        {
            gThetaCosineSignal = gThetaCosineSignal - 2.0*M_PI;
        }
        // Calculo de y cos(Theta) 
        gCosSignal = (int16_t)( 2 * gAmpCosine * cosf(gThetaCosineSignal) );


        /*-------------------------------------------------------------------*/
        /* Enmascaramiento de bits menos significativos */
        /*-------------------------------------------------------------------*/
        gMaskedLsbOutput = gIntCodecInputL & gMaskLsbs;

        /*-------------------------------------------------------------------*/
        /* MODULACIÓN DE AMPLITUD usando representación en punto fijo */
        /*-------------------------------------------------------------------*/
        // cos() * Signal
        gAmplitudeModulationL = (int16_t)( ( (int32_t)gCosSignal * (int32_t)gIntCodecInputL)  >> 15 );
        gAmplitudeModulationR = (int16_t)( ( (int32_t)gCosSignal * (int32_t)gIntCodecInputR)  >> 15 );
        //  0.5*( 1 + cos()) * Signal

        DLU_tic();
        if (DLU_readToggleStatePB1() || DLU_readToggleStatePB2()){
            sonido = 1;
            gIdxOscMeas = 0;
        }

        salidafiltroL = filterBiquad(&FiltroL, gAmplitudeModulationL);
        salidafiltroR = filterBiquad(&FiltroR, gAmplitudeModulationR);

        salidafiltroL = filterBiquad(&FiltroNL, salidafiltroL);
        salidafiltroR = filterBiquad(&FiltroNR, salidafiltroR);
        DLU_toc(); // Se debe observar la variable 'DLU_timeTicToc'


        // OPERACIÓN EN PUNTO FIJO
        // Selección de señales de salida

        if (sonido){
            gIntCodecOutputL = sonidoizq;
            gIntCodecOutputR = sonidoder;
        }
        else{
            gIntCodecOutputL = salidafiltroL;
            gIntCodecOutputR = salidafiltroR;
        }

        // Escritura en buffer de salida hacia el codec
        codec_data.channel[LEFT] = gIntCodecOutputL;
        codec_data.channel[RIGHT] = gIntCodecOutputR;
        output_sample(codec_data.uint);

//#############################################################################
    return;
}

/*****************************************************************************/
void main()
{
    FiltroR = CrearFiltro(PASS, BW1, FPB);
    FiltroL = CrearFiltro(PASS, BW1, FPB);
    FiltroNL = CrearFiltro(NOTCH, 10, gFreqCosineMin);
    FiltroNR = CrearFiltro(NOTCH, 10, gFreqCosineMin);
    /* Inicialización de Pulsadores User 1 y User 2 */
    DLU_initPushButtons();
    /* Inicializa función de medición de tiempos de ejecución */
    DLU_initTicToc();
    /* Inicializacion BSL y AIC31 Codec */
    L138_initialise_intr(CODEC_FS, CODEC_ADC_GAIN, CODEC_DAC_ATTEN, CODEC_INPUT_CFG);
    /* Inicialización de LEDs */
    DLU_initLeds();
    DLU_configTrimmerCounter(20, 300);
    /* Configuración de Trimmer ajustado con Pushbuttons */


   /* Loop infinito a espera de interrupción del Codec */
    while(1);
}


/******************************************************************************
**      END OF SOURCE FILE
******************************************************************************/
float filterBiquad(bqStatus_t *filterNState, float filterInput){
    // COMPLETAR
    filterNState->bqInput[2] = filterNState->bqInput[1];
    filterNState->bqInput[1] = filterNState->bqInput[0];
    filterNState->bqInput[0] = filterInput;

    filterNState->bqOutput[2] = filterNState->bqOutput[1];
    filterNState->bqOutput[1] = filterNState->bqOutput[0];

    filterNState->bqOutput[0] =
                   filterNState->bqB0 * filterNState->bqInput[0] +
                   filterNState->bqB1 * filterNState->bqInput[1] +
                   filterNState->bqB2 * filterNState->bqInput[2] -
                   filterNState->bqA1 * filterNState->bqOutput[1] -
                   filterNState->bqA2 * filterNState->bqOutput[2];



    /* Se retorna la salida */
    return filterNState->bqOutput[0];
}

bqStatus_t CrearFiltro(FILTERTYPE type, float BW, float fc){
    bqStatus_t Filtro;
    float th = 2*PI*fc/CODEC_FS;
    BW = 2*PI*BW/CODEC_FS;
    float d =  (1 - sin(BW))/cos(BW);

    if(type == NOTCH){
        Filtro.bqB0 = 1.0/2 * (1 + d);
        Filtro.bqB1 = -(1 + d) * cos(th);
        Filtro.bqB2 = (1 + d)/2;
        Filtro.bqA1 = -(1 + d) * cos(th);
        Filtro.bqA2 = d;
        Filtro.bqInput[0] = 0;
        Filtro.bqOutput[0] = 0;
        Filtro.bqInput[1] = 0;
        Filtro.bqOutput[1] = 0;
        Filtro.bqInput[2] = 0;
        Filtro.bqOutput[2] = 0;
        return Filtro;
    }

    Filtro.bqB0 = 1 + d;
    Filtro.bqB1 = 0;
    Filtro.bqB2 = -(1 + d);
    Filtro.bqA1 = (-1 - d) * cos(th);
    Filtro.bqA2 = d;
    Filtro.bqInput[0] = 0;
    Filtro.bqOutput[0] = 0;
    Filtro.bqInput[1] = 0;
    Filtro.bqOutput[1] = 0;
    Filtro.bqInput[2] = 0;
    Filtro.bqOutput[2] = 0;
    return Filtro;


}

void passUpdate(bqStatus_t* Filtro, float fc, float BW){
        float th = 2*PI*fc/CODEC_FS;
        BW = 2*PI*BW/CODEC_FS;
        float d =  (1 - sin(BW))/cos(BW);

        Filtro->bqB0 = 1 + d;
        Filtro->bqB2 = -(1 + d);
        Filtro->bqA1 = (-1 - d) * cos(th);
        Filtro->bqA2 = d;

}

