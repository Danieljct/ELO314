/******************************************************************************
* \file     Lab1p2_solucion.c
*
* \brief    Experiencia 2 de laboratorio DSP ELO314
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
#include "phaser_func.h"

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

/*---------------------------------------------------------------------------*/
/* ENTRADAS Y SALIDAS DEL AIC CODEC */
/*---------------------------------------------------------------------------*/

/*
 * Varibles de entrada y salida en formato flotante
 */
float gFloatCodecInputR, gFloatCodecInputL;
float gFloatCodecOutputR, gFloatCodecOutputL;

extern float phaser_audio_l_out;
extern float phaser_audio_r_out;

float gSignal1 = 0.0;
float gSignal2 = 0.0;
float gSignal3 = 0.0;
float gSignal4 = 0.0;
int  gNumCounts = 0;
float gNoiseGain = 0.65;

/*
 * Variables de estado de salida saturada
 */
int gOutSaturationStat = 0;

/*---------------------------------------------------------------------------*/
/* EFECTO DE AUDIO ELEFIDO: PHASER */
/*---------------------------------------------------------------------------*/
float gPhaserLfoSpeed = 0.8;
int32_t gPhaserFbEn = 0;

/******************************************************************************
**      PRIVATE FUNCTION DECLARATIONS (PROTOTYPES)
******************************************************************************/

/******************************************************************************
**      FUNCTION DEFINITIONS
******************************************************************************/

interrupt void interrupt4(void) // interrupt service routine
{
//#############################################################################
        /*-------------------------------------------------------------------*/
        /* LECTURA DE ENTRADAS DEL CODEC */
        /*-------------------------------------------------------------------*/
        DLU_readCodecInputs(&gFloatCodecInputL, &gFloatCodecInputR);

        /*-------------------------------------------------------------------*/
        /* Inicia medición de tiempo de ejecución */
        DLU_tic();

        gSignal1 = gNoiseGain*DLU_fRand();

        /*-------------------------------------------------------------------*/
        /* EFECTO DE AUDIO PHASER */
        /*-------------------------------------------------------------------*/
        gPhaserLfoSpeed = ((float)DLU_readTrimmerCounter()) / (float)PB_TRIM_COUNTER_MAX;
        gPhaserFbEn = DLU_readToggleStatePB12();

        phaser_fx_stereo(gFloatCodecInputL, gFloatCodecInputL, gPhaserLfoSpeed, gPhaserFbEn, TS);

        /*-------------------------------------------------------------------*/
        /* PARA VISUALIZAR EN GRÁFICO */
        /*-------------------------------------------------------------------*/
        if ( DLU_readPushButton1() == 1 )
        {
            DLU_enableSingleCaptureOnGraphBuff1();
            DLU_enableSingleCaptureOnGraphBuff2();

        }

        DLU_appendGraphBuff1(gSignal1);
        DLU_appendGraphBuff2(phaser_audio_r_out);

        /*-------------------------------------------------------------------*/
        /* ESCRITURA EN SALIDA DEL CODEC */
        /*-------------------------------------------------------------------*/
        gFloatCodecOutputL = phaser_audio_l_out;
        gFloatCodecOutputR = phaser_audio_r_out;

        //gFloatCodecOutputL = gSignal1;
        //gFloatCodecOutputR = gSignal1;

        //gFloatCodecOutputL = gFloatCodecInputL;
        //gFloatCodecOutputR = gFloatCodecInputR;

        gOutSaturationStat = DLU_writeCodecOutputs(gFloatCodecOutputL,gFloatCodecOutputR);
        DLU_writeLedD4((PB_INT_TYPE)gOutSaturationStat);

//#############################################################################
    return;
}

void main()

{
    /* Inicialización de Pulsadores User 1 y User 2 */
    DLU_initPushButtons();
    /* Inicializa función de medición de tiempos de ejecución */
    DLU_initTicToc();
    /* Inicializacion BSL y AIC31 Codec */
    L138_initialise_intr(CODEC_FS, CODEC_ADC_GAIN, CODEC_DAC_ATTEN, CODEC_INPUT_CFG);
    /* Inicialización de LEDs */
    DLU_initLeds();

   /* Loop infinito a espera de interrupción del Codec */
    while(1);
}

/******************************************************************************
**      END OF SOURCE FILE
******************************************************************************/
