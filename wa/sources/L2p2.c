// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/******************************************************************************
* \file     Lab1p2.c
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
int8_t sign(float x);
/******************************************************************************
**      MODULE PREPROCESSOR CONSTANTS
******************************************************************************/
// Selección de buffer lineal. No definir este flag implica modo buffer circular
//#define LINEAR_BUFFER_MODE

/******************************************************************************
**      MODULE MACROS
******************************************************************************/
// MultiTap
#define DELAY_MT_M              (.125*CODEC_FS)               // ¿para lograr x ms ?
#define DELAY_MT_N              (4)                 // Etapas
#define DELAY_MT_ATTEN_FACTOR   (0.35)                 // k

// Buffer
#define AUDIOBUFFERSIZE          (4000)              // ¿para lograr x ms ?

// RMS
#define RMS_WINDOW_SAMPLES       (256)              // ¿para lograr x ms de ventana?
#define NOISE_FLOOR_RMS          (0.02)              // Valor RMS sin hablar a micrófono

// Biquads
#define BQ2VARIANTS              (8)                // Variantes en oscilador
#define BQ2TIMERMAXCOUNT         (10)               // ¿Cuentas para 0.5s?
#define BQAMPLITUDE              (0.8)              // Amplitud de impulso



/******************************************************************************
**      MODULE DATATYPES
******************************************************************************/


/******************************************************************************
**      MODULE VARIABLE DEFINITIONS
******************************************************************************/
uint16_t indice = 0;

/*---------------------------------------------------------------------------*/
/* ENTRADAS Y SALIDAS DEL AIC CODEC */
/*---------------------------------------------------------------------------*/
/*
 * Tipo de dato para el CODEC (Union)
 */
AIC31_data_type codec_data;

/*
 * Varibles de entrada y salida en formato flotante
 */
float floatCodecInputR,floatCodecInputL;
float floatCodecOutputR,floatCodecOutputL;

/*
 * Variables de estado que indica salida saturada
 */
int outSaturationStat = 0;

/*---------------------------------------------------------------------------*/
/* BUFFER DE AUDIO E ÍNDICES */
/*---------------------------------------------------------------------------*/
//#pragma DATA_SECTION(gAudioBufferL,".EXT_RAM")
//#pragma DATA_SECTION(gAudioBufferR,".EXT_RAM")
float           gAudioBufferL[AUDIOBUFFERSIZE];
float           gAudioBufferR[AUDIOBUFFERSIZE];

float gAudioBufferOutputL = 0.0;
float gAudioBufferOutputR = 0.0;


/*---------------------------------------------------------------------------*/
/* CÓMPUTO RMS Y BUFFERS AUXILIARES */
/*---------------------------------------------------------------------------*/
uint32_t        gSampleCounterRmsWindow = 0;
float           gAudioRmsPartial = 0.0;
float           gAudioRms = 0.0;
float           gModRms = 0.0;

/*---------------------------------------------------------------------------*/
/* GENERACIÓN DE SEÑALES */
/*---------------------------------------------------------------------------*/
/* Coeficientes para biquiad con polinomio de denominador mónico en z^-1 */
float gBq1B0 = 0.051342456225139;
float gBq1A1 = -1.997362212708321;
float gBq1A2 = 1.0;
#define PI 3.14159265358979323846

float frecuencias[8] = {261.6,293.7,329.6,349.2,392.0,440,493.9,523.3};
float gBq2B0_b[8];
float gBq2A1_b[8];

float gBq2B0 = 0.064671642929027;
float gBq2A1 = -1.995813196269491;
float gBq2A2 = 1.0;

/* Para generar un impulso de estímulo a biquads
 */
float gImpulseGen1 = BQAMPLITUDE;
float gImpulseGen2 = BQAMPLITUDE;

// Estados de Biquad 1
float gBq1Input = 0.0;
float gBq1Output[3] = {0.0,0.0,0.0};
// Estados de Biquad 2
float gBq2Input = 0.0;
float gBq2Output[3] = {0.0,0.0,0.0};

/* Variables de cuenta de tiempo para cambio en biquad 2 */
uint32_t gBq2TimeCounter = 0;
/* Conteo de notas */
uint32_t gBq2Note = 0;

/*---------------------------------------------------------------------------*/
/* VARABLES OVERDRIVE */
/*---------------------------------------------------------------------------*/
float gOverdriveInput = 0;
float gOverdriveOutput = 0;

float gOverdriveInput_gain   =  3.0;
float gOverdriveOutput_gain  =  1.0;
float gOverdriveBeta         =  0.05;
float gOverdriveAlpha        =  0.1;

/*---------------------------------------------------------------------------*/
/* DELAY MULTITAP */
/*---------------------------------------------------------------------------*/
float       gDelayMTOutput = 0.0;       // Salida del Delay multitap
uint32_t    gIdxDelayMTStage = 0;       // Etapa
float       delayMTB[DELAY_MT_N];       // Ganancia por etapa: b_k
//TO DO
const int buffsize = DELAY_MT_M*DELAY_MT_N;
int indiceMT = 0;
int biquadcount = 0;
int biquadi = 0;
#pragma DATA_SECTION(buffermultitap,".EXT_RAM")
float buffermultitap[buffsize] = {0};
float multitap(float datoentrada);


/******************************************************************************
**      PRIVATE FUNCTION DECLARATIONS (PROTOTYPES)
******************************************************************************/

/* Notar que se usa el mismo buffer (gAudioBufferL) en memoria tanto para
 * la implementación Lineal como Circular. Por lo tanto se invocará
 * o a la función de gestión lineal o a la función de gestión ciruclar
 * excluyentemente. Para eso es el flag LINEAR_BUFFER_MODE
 */
void initAudioBuffers(void);

#ifdef LINEAR_BUFFER_MODE
//#pragma FUNC_ALWAYS_INLINE(delayThroughLinearBufferLeft)

float delayThroughLinearBufferLeft(float bufferInputData);
float delayThroughLinearBufferRight(float bufferInputData);

#else

float delayThroughCircularBufferLeft(float bufferInputData);
float delayThroughCircularBufferRight(float bufferInputData);
float OVERDRIVE(float OverdriveInput);
#endif


/******************************************************************************
**      FUNCTION DEFINITIONS
******************************************************************************/

interrupt void interrupt4(void) // Interrupt Service Routine
{
//.............................................................................
        /*-------------------------------------------------------------------*/
        /* LECTURA DE ENTRADAS DEL CODEC */
        /*-------------------------------------------------------------------*/
        DLU_readCodecInputs(&floatCodecInputL, &floatCodecInputR);


        /*-------------------------------------------------------------------*/
        /* Inicia medición de tiempo de ejecución */
        DLU_tic();
        /*-------------------------------------------------------------------*/
        /* BUFFERS LINEALES Y CIRCULARES */
        /*-------------------------------------------------------------------*/
#ifdef LINEAR_BUFFER_MODE
        /* Buffer lineal */
        gAudioBufferOutputL = delayThroughLinearBufferLeft(floatCodecInputL);

#else

        /* Buffer circular */
        gAudioBufferOutputL = delayThroughCircularBufferLeft(floatCodecInputL);
#endif
        /* Medición de tiempo de ejecución */
        DLU_toc();


        /*-------------------------------------------------------------------*/
        /* OSCILADORES BIQUAD */
        /*-------------------------------------------------------------------*/
        /* Actualización de biquad 1 */

        gBq1Input = gImpulseGen1;

        gBq1Output[2] = gBq1Output[1];
        gBq1Output[1] = gBq1Output[0];
        gBq1Output[0] = (gBq1B0 * gBq1Input) - (gBq1A1 * gBq1Output[1]) - (gBq1A2 * gBq1Output[2]);

        /* Actualiza generador de impulso para que sea cero de aquí en adelante */
        gImpulseGen1 = 0.0;

        /*-------------------------------------------------------------------*/
        /* Cambio de nota para biquad 2 */


        /* Actualización de biquad 2
        gBq2Input = gImpulseGen2;

        gBq2Output[2] = gBq2Output[1];
        gBq2Output[1] = gBq2Output[0];
        gBq2Output[0] = (gBq2B0 * gBq2Input) - (gBq2A1 * gBq2Output[1]) - (gBq2A2 * gBq2Output[2]);
         gImpulseGen2 = 0.0;
*/


        /* Actualiza generador de impulso para que sea cero de aquí en adelante */


        biquadcount++;
        if(!(biquadcount%CODEC_FS)){
            biquadi=(biquadi+1)%8;
        }
        gBq2Input = gImpulseGen2;


        gBq2Output[2] = gBq2Output[1];
        gBq2Output[1] = gBq2Output[0];
        gBq2Output[0] = (gBq2B0_b[biquadi] * gBq2Input) - (gBq2A1_b[biquadi] * gBq2Output[1]) - (gBq2A2 * gBq2Output[2]);

        gImpulseGen2 = 0.0;




        /*-------------------------------------------------------------------*/
        /* CALCULO VALOR RMS DE VENTANA: gAudioRms */
        /*-------------------------------------------------------------------*/
        gSampleCounterRmsWindow = (gSampleCounterRmsWindow + 1) % RMS_WINDOW_SAMPLES;
        if ( gSampleCounterRmsWindow < RMS_WINDOW_SAMPLES - 1)
        {
            gAudioRmsPartial += floatCodecInputL*floatCodecInputL;
        }
        else
        {
            gAudioRms = sqrt(gAudioRmsPartial/RMS_WINDOW_SAMPLES);
            gAudioRmsPartial = 0;
        }

        /* MODULACIÓN CON RMS */
        gModRms = gBq1Output[0] * (gAudioRms - NOISE_FLOOR_RMS);

        /*-------------------------------------------------------------------*/
        /* DISTORSIÓN OVERDRIVE */
        /*-------------------------------------------------------------------*/
        gOverdriveOutput = OVERDRIVE(floatCodecInputL);

        /*-------------------------------------------------------------------*/
        /* DELAY MULTITAP */
        /*-------------------------------------------------------------------*/


        /*-------------------------------------------------------------------*/
        /* PARA VISUALIZAR EN GRÁFICO */
        /*-------------------------------------------------------------------*/
        DLU_enableSynchronicSingleCaptureOnAllGraphBuff();
        DLU_appendGraphBuff1(floatCodecOutputR);
        DLU_appendGraphBuff2(floatCodecInputL);

        /*-------------------------------------------------------------------*/
        // USO DE ESTADO DE PULSADOR USER1 PARA ACTIVAR LED
        /*-------------------------------------------------------------------*/
        // Obtención de valores de estados para cambiar estado de leds
        // LED 5
        if ( DLU_readToggleStatePB1() )
            DLU_writeLedD5(LED_ON);
        else
            DLU_writeLedD5(LED_OFF);
        // LED 6
        if ( DLU_readToggleStatePB2() )
            DLU_writeLedD6(LED_ON);
        else
            DLU_writeLedD6(LED_OFF);
        // LED 7
        if ( DLU_readToggleStatePB12() )
            DLU_writeLedD7(LED_ON);
        else
            DLU_writeLedD7(LED_OFF);

        /*-------------------------------------------------------------------*/
        /* SELECCIÓN DE ESCRITURA EN SALIDA DEL CODEC */
        /*-------------------------------------------------------------------*/
        // BYPASS
        //floatCodecOutputL = floatCodecInputL;
        //floatCodecOutputR = floatCodecInputL;

        // DELAY
        //floatCodecOutputL = multitap(floatCodecInputL);
        //floatCodecOutputR = gAudioBufferOutputL;

        // BIQUAD
        floatCodecOutputL = gBq2Output[0];
        floatCodecOutputR = gBq2Output[0];

        // Modulación con envolvente RMS
        //floatCodecOutputL = gModRms;
        //floatCodecOutputR = gModRms;

        //floatCodecOutputL = gOverdriveOutput;
        //floatCodecOutputR = gOverdriveOutput;

        //floatCodecOutputL = gDelayMTOutput;
        //floatCodecOutputR = gDelayMTOutput;

        //---------------------------------------------------------------------
        // Escritura en CODEC
        outSaturationStat = DLU_writeCodecOutputs(floatCodecOutputL,floatCodecOutputR);
        // Enciende LED 4 en caso de saturación de alguna de las salidas
        DLU_writeLedD4((PB_INT_TYPE)outSaturationStat);

//.............................................................................
    return; // Fin de interrupción del CODEC
}

//*****************************************************************************
void main()
{
    /* Inicialización de Pulsadores User 1 y User 2 */
    DLU_initPushButtons();
    /* Inicializa función de medición de tiempos de ejecución */
    DLU_initTicToc();

    // TODO
    delayMTB[0] = DELAY_MT_ATTEN_FACTOR;
    int k;
    for(k = 1; k < DELAY_MT_N; k++){
        delayMTB[k] = DELAY_MT_ATTEN_FACTOR * delayMTB[k-1];
    }

    for(k = 0; k < 8; k++){
        gBq2B0_b[k] = sin(2*PI*frecuencias[k]/CODEC_FS);
        gBq2A1_b[k] = -2*cos(2*PI*frecuencias[k]/CODEC_FS);
    }

    /* Inicializacion BSL y AIC31 Codec */
    L138_initialise_intr(CODEC_FS, CODEC_ADC_GAIN, CODEC_DAC_ATTEN, CODEC_INPUT_CFG);
    /* Inicialización de LEDs */
    DLU_initLeds();

    /*-----------------------------------------------------------------------*/
    /* Inicialización de buffers a cero */
    initAudioBuffers();


   /* Loop infinito a espera de interrupción del Codec */
    while(1);
}

/******************************************************************************
*   \brief  Función que inicializa en cero el contenido de los buffers de audio
*
*   \param void
*
*   \return void
******************************************************************************/
void initAudioBuffers(void)
{
    uint32_t idxLinearAudioBuffer = 0;

    for(idxLinearAudioBuffer = 0 ; idxLinearAudioBuffer < AUDIOBUFFERSIZE; idxLinearAudioBuffer++)
    {
        gAudioBufferL[idxLinearAudioBuffer] = 0.0;
        gAudioBufferR[idxLinearAudioBuffer] = 0.0;
    }
    return;
}

/******************************************************************************
*   \brief  Función que recibe una muestra actual de entrada y retorna una
*           muestra AUDIOBUFFERSIZE -esima anterior
*
*   \param bufferInputData : muestra que recibe el buffer como entrada
*
*   \return float muestra (sample) de salida del buffer lineal
******************************************************************************/
float delayThroughLinearBufferLeft(float bufferInputData)
{
    int16_t idxLinearAudioBuffer = AUDIOBUFFERSIZE-2;


    float outputData = gAudioBufferL[AUDIOBUFFERSIZE-1];

    for(idxLinearAudioBuffer = AUDIOBUFFERSIZE-2; idxLinearAudioBuffer >= 0; idxLinearAudioBuffer--)
    {
        gAudioBufferL[idxLinearAudioBuffer+1] = gAudioBufferL[idxLinearAudioBuffer];
    }
    gAudioBufferL[0] = bufferInputData;

    return outputData;
}

/******************************************************************************
*   \brief  Función que recibe una muestra actual de entrada y retorna una
*           muestra AUDIOBUFFERSIZE -esima anterior
*
*   \param bufferInputData : muestra que recibe el buffer como entrada
*
*   \return float muestra (sample) de salida del buffer lineal
******************************************************************************/
float delayThroughCircularBufferLeft(float bufferInputData)
{
    float outputData = gAudioBufferL[indice];
    gAudioBufferL[indice] = bufferInputData;
    indice = (indice + 1) % AUDIOBUFFERSIZE;
    return outputData;
}


/******************************************************************************
**      END OF SOURCE FILE
******************************************************************************/

float OVERDRIVE(float OverdriveInput){

    float OverdriveOutput = OverdriveInput * gOverdriveInput_gain;

    if (fabs(OverdriveOutput) >= gOverdriveAlpha){
        OverdriveOutput = gOverdriveBeta*OverdriveOutput+sign(OverdriveOutput)*(1-gOverdriveBeta)*gOverdriveAlpha;
    }
    return OverdriveOutput * gOverdriveOutput_gain;

}

float multitap(float datoentrada){
    float salida = 0;
    buffermultitap[indiceMT] = datoentrada;

    for(gIdxDelayMTStage = 1; gIdxDelayMTStage <= DELAY_MT_N; gIdxDelayMTStage++){
        int KM = gIdxDelayMTStage*DELAY_MT_M;
        salida += delayMTB[gIdxDelayMTStage - 1] * buffermultitap[(indiceMT + KM + buffsize - 1) % buffsize];
    }
    indiceMT = (indiceMT-1+buffsize) % buffsize;
    return salida;
}

int8_t sign(float x){
    if(x > 0){
        return 1;
    }
    else if(x < 0){
        return -1;
    }
    return 0;
}
