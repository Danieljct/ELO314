// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/***************************************************************************//**
* \file     L5p2.c
*
* \brief    Base para el laboratorio L4p2
*
* \authors  Gonzalo Carrasco
*******************************************************************************/

/******************************************************************************
**      HEADER FILES
******************************************************************************/
#include "dlu_global_defs.h"
#include "L138_LCDK_aic3106_init.h"
#include "dsp_lab_utils.h"
#include <math.h>
#include "dlu_codec_config.h"
#include "levinson.h"
#include "functions.h"
#include "const_arrays.h"
#include "vowel_a.h"
#include "stdio.h"
#include "string.h"


/******************************************************************************
**      MODULE PREPROCESSOR CONSTANTS
******************************************************************************/

/******************************************************************************
**      MODULE MACROS
******************************************************************************/

/* Macro para no sobrecargar con tanto indexado de punteros y estructuras */
#define codecInputArrayAsInt16L(idx)    ( ((dataPairLR_t *)pInputPairBuffer)[idx].leftChannel)
#define codecInputArrayAsInt16R(idx)    ( ((dataPairLR_t *)pInputPairBuffer)[idx].rightChannel)
#define codecOutputArrayAsInt16L(idx)   ( ((dataPairLR_t *)pOutputPairBuffer)[idx].leftChannel)
#define codecOutputArrayAsInt16R(idx)   ( ((dataPairLR_t *)pOutputPairBuffer)[idx].rightChannel)

/******************************************************************************
**      MODULE DATATYPES
******************************************************************************/
/*
 * Estructura para descomponer en canales la trama del codec en DMA
 */
typedef struct
{
    int16_t rightChannel;
    int16_t leftChannel;
} dataPairLR_t;

typedef enum
{
    VU_UNVOICED = 0,
    VU_VOICED
}vuSound_e;

typedef enum
{
    NOISE_STATIC = 0,
    NOISE_RAND
}noiseType_e;

typedef enum
{
    SYNTHMODE_ALWAYS_VOICED = 0,
    SYNTHMODE_ALWAYS_UNVOICED,
    SYNTHMODE_AUTOMATIC,
    SYNTHMODE_EXTERNALSOURCE
}synthMode_e;

/******************************************************************************
**      MODULE VARIABLE DEFINITIONS
******************************************************************************/
float amp_factor = 10;
float factor_ruido = 10;
/*---------------------------------------------------------------------------*/
/* VARIABLES AUXILIARES PARA BUFFERS DE TRANSFERENCIA DMA */
/*---------------------------------------------------------------------------*/
/*
 * Punteros a buffers ping y pong (definidos en 'L138_LCDK_aic3106_init.c')
 */
extern int16_t *pingIN, *pingOUT, *pongIN, *pongOUT;

/*
 * Puntero a buffer con pares de entradas Izquierdo/Derecho desde DMA
 */
int16_t *pInputPairBuffer;

/*
 * Puntero a buffer con pares de salidas Izquierdo/Derecho para DMA
 */
int16_t *pOutputPairBuffer;

/*
 * Flag de indicación que un nuevo frame está listo para procesarse
 */
volatile int gProcessingBufferIsFull = 0;

/*
 * Contador de frames procesados
 */
int gFramesCounter = 0;

/*---------------------------------------------------------------------------*/
/* DEFINICIÓN DE VARIABLES GLOBALES PARA LPC */
/*---------------------------------------------------------------------------*/

/*------------------- ARREGLOS SEÑALES DE AUDIO --------------------*/
/*
 * Buffer de entrada izquierda
 */
float gFloatInputBufferL[FRAME_SIZE];

/*
 * Buffer de entrada derecha
 */
float gFloatInputBufferR[FRAME_SIZE];

/*
 * Buffer intermedio para enventanar señal del frame entrada
 */
float gInputWindowed[FRAME_SIZE];

/*
 * Buffer de salida sintetizada
 */
float gSynthVoiceFrame[FRAME_SIZE]={0};

/*
 * Arreglo para crear señal de pulsos
 */
float gVariableFreqPulsesData[FRAME_SIZE];

/*
 * Periodo en muestras de la señal de pulsos generada
 */
unsigned int gVariableFreqPulsesPeriod = 80;

/*
 * Buffer para ruido generado en tiempo real
 */
float gRandNoise[FRAME_SIZE];

/*
 * Índice buffer de ruido blanco para llenarlo
 */
unsigned int gIdxPrandNoise;

/*
 * Fuente de esíimulo para filtro AR
 */
float *gpArInputBuffer = NULL;

/*------------------------ ARREGLOS PARA LPC -----------------------*/
/*
 * Arreglo con valores de autocorrelación
 */
float gCorrelationsArray[LPC_ORDER + 1];

/*
 * Parámetros del lpc identificados con levinson()
 */
float gLpcARcoeffs[LPC_ORDER+1] = {0};

/*--------------- VARIABLES PARA CORREGIR AMPLITUD  -----------------*/
/*
 * Valor RMS del frame de entrada
 */
float gInputFrameRms;

/*
 * Valor RMS del frame sintetizado con arFilter
 */
float gSynthVoiceFrameRms;

/*
 * Factor de corrección de amplitud de señal sintetizada
 */
float gSynthAmpFactor = 1;

/*
 * Variable auxiliar para calcular RMS del canal de entada derecho
 */
float gAuxInputVoiceRms = 0.0;

/*
 * Valor RMS del canal de entada derecho
 */
float gInputVoiceRms = 0.0;

/*--------------- VARIABLES PARA CLASIFICACIÓN V/U  -----------------*/
float rx_total[FRAME_SIZE] = {0};
float rx_total2[FRAME_SIZE]={  487.3316,
                               266.0273,
                               -39.9709,
                              -242.5073,
                              -309.7243,
                              -167.2438,
                                68.6085,
                               129.5407,
                                17.7249,
                                 1.3680,
                                -0.7304,
                                51.8641,
                               163.8870,
                               127.9988,
                               -63.4785
                              -194.3970};
/*
 * Clasificador VU
 */
vuSound_e gVUanalysis = VU_VOICED;

/*
 * Selector de tipo de ruido
 */
noiseType_e gNoiseTypeSelector = NOISE_RAND;

/*
 * Modo de síntesis
 */
synthMode_e gSynthMode = SYNTHMODE_EXTERNALSOURCE;

/*
 * Umbral RMS
 */
float gVUrmsThreshold = 0.004;


/*--------------- VARIABLES PARA MEJORAS DE CALIDAD -----------------*/

float gMicNoiseFloorRms = 0.033;

/*---------------------------------------------------------------------------*/
/* DEFINICIÓN DE VARIABLES GLOBALES PARA PRUEBAS Y DEBUGGING */
/*---------------------------------------------------------------------------*/

/*
 * LPC parámetros de prueba 'a'
 */
float gLpcA[LPC_ORDER+1] = {
   1.000000000000000,
  -0.582606819084131,
   0.192116093188965,
   0.288607088091191,
   0.493535676031534,
   0.009828286172478,
  -0.190959469422861,
  -0.015447520819975,
   0.682408683890808,
  -0.383201985116514,
  -0.058132732768475,
  -0.015920507797531,
  -0.014047716756587,
  -0.054775473919274,
  -0.027721114114396,
  -0.029782070482511
};

/******************************************************************************
**      PRIVATE FUNCTION DECLARATIONS (PROTOTYPES)
******************************************************************************/
interrupt void interrupt4(void);
void processFrame(void);

/******************************************************************************
**      FUNCTION DEFINITIONS
******************************************************************************/

/***************************************************************************//**
*   \brief  MAIN
*
*   \param  Void.
*
*   \return Void.
*******************************************************************************/
int main(void)
 {
    /*----------------------------------------------------------------------*/
    // Inicialización del Codec de audio
    L138_initialise_edma(CODEC_FS, CODEC_ADC_GAIN, DAC_ATTEN_0DB, CODEC_INPUT_CFG);
    /*----------------------------------------------------------------------*/
    /* Inicialización de Pulsadores User 1 y User 2 */
    DLU_initPushButtons();
    /* Inicializació de LEDs */
    DLU_initLeds();
    DLU_writeLedD4(LED_OFF);
    DLU_writeLedD5(LED_OFF);
    DLU_writeLedD6(LED_OFF);
    DLU_writeLedD7(LED_OFF);
    DLU_initTicToc();



    /*----------------------------------------------------------------------*/
    /* Inicialización a cero del vector que usa el filtro AR */


    /*----------------------------------------------------------------------*/
    /* Inicialización de algoritmo de Levinso-Durbin */
    levinson_init(LPC_ORDER);

     //TESTS

    float arreglosalida[20];
    float entrada[] = {0,1, 2, 3, 4, 5 ,6 ,7 ,8, 9, 10,11,12,13,14,15,16,17,18,19};
    xcorr(arreglosalida, entrada, 20,20);
    float acoeff[LPC_ORDER+1];
    levinson_computeCoeffs(acoeff, arreglosalida,LPC_ORDER);
    float output_filter[20] = {0};
    arFilter(output_filter, acoeff, LPC_ORDER, entrada, 20);

    memset(gSynthVoiceFrame, 0, sizeof(gSynthVoiceFrame));
    /*----------------------------------------------------------------------*/
    /* Background: procesamiento tras levantar bandera */
    while(1)
    {
        /* Retención hasta que 'processingBufferIsFull' es alto */
        while (!gProcessingBufferIsFull);

        /* Una vez llenado el buffer, se procesa en 'processFrame()' */
        DLU_writeLedD4(LED_ON); // Ciclo de trabajo del LED4 indica uso approx de CPU
        DLU_tic();
        processFrame();
        DLU_toc();  // Tiempo de cómputo total del frame
        DLU_writeLedD4(LED_OFF);
    }
}

/***************************************************************************//**
*   \brief  Función de procesamiento de un frame (ventana de captura).
*           Debe ser llamada desde el background en el loop infinito del main.
*
*   \param  Void.
*
*   \return Void.
*******************************************************************************/
void processFrame(void)
{
    int idxBuffer;
    gFramesCounter+=1;
    gAuxInputVoiceRms = 0.0;
    /*-------------------------------------------------------------------*/
    /* PARA VISUALIZAR EN GRÁFICO: presionando el PB2 actualizará los GraphBuff */
    if ( DLU_readPushButton2 () ){
        DLU_enableSynchronicSingleCaptureOnAllGraphBuff();
    }

    /*-----------------------------------------------------------------------*/
    /* Copia a arreglo de entrada al proceso de voz */
    for (idxBuffer = 0; idxBuffer < (FRAME_SIZE) ; idxBuffer++)
    {
        gFloatInputBufferL[idxBuffer] = AIC_2_FLT( codecInputArrayAsInt16L(idxBuffer) );
        gFloatInputBufferR[idxBuffer] = AIC_2_FLT( codecInputArrayAsInt16R(idxBuffer) );
        DLU_appendGraphBuff1(gFloatInputBufferL[idxBuffer]);

    }

    /************************************************************************/
    // PROCESAMIENTO PARA IMPLEMENTAR ANÁLISIS DE VOZ
    /*----------------------------------------------------------------------*/
    /* 1) Valor RMS de frame de voz */
    int ir;
    for(ir = 0; ir < FRAME_SIZE; ir++){
        gInputVoiceRms  += gFloatInputBufferL[ir]* gFloatInputBufferL[ir];
    }
    gInputVoiceRms = sqrt(gInputVoiceRms/FRAME_SIZE);

    /*----------------------------------------------------------------------*/
    /* 2) Enventanamiento */
    for(ir = 0; ir < FRAME_SIZE; ir++){
        gFloatInputBufferL[ir] = gFloatInputBufferL[ir]*hammWin160[ir];
    }
    /*----------------------------------------------------------------------*/
    /* 3) Autocorrelación */

    xcorr(rx_total, gFloatInputBufferL, FRAME_SIZE,FRAME_SIZE);
    int i = 0;
    for(i =0; i < LPC_ORDER+1; i++){
        gCorrelationsArray[i] = rx_total[i];
       // gCorrelationsArray[i] = rx_total2[i];
    }


    /*----------------------------------------------------------------------*/
    /* 4) Determinación V/U:
     * Debe actualizar 'gVUanalysis' con  'VU_VOICED' o 'VU_UNVOICED'
     * Para visualización también:
     * VOICED enciende el led 7 y
     * UNVOICED apaga el led 7 */

    //  Cuando es Voiced:
    gInputVoiceRms -= gMicNoiseFloorRms;

    if (gInputVoiceRms > gVUrmsThreshold){
      gVUanalysis = VU_VOICED;
      DLU_writeLedD7(LED_ON);
    }
    else{
    //  Cuando es Unvoiced:
      gVUanalysis = VU_UNVOICED;
      DLU_writeLedD7(LED_OFF);
    }
    /************************************************************************/
    // PROCESAMIENTO PARA IMPLEMENTAR SÍNTESIS DE VOZ
    /*----------------------------------------------------------------------*/
    /* 5) Levinson */
    if ( DLU_readToggleStatePB1() )
    {
        // No actualiza los parámetros lpc. Se retienen últimos y se fija RMS
        gInputFrameRms = gInputVoiceRms;
        // Se sobrescribe forzando VOICED
        gVUanalysis = VU_VOICED;
    }
    else
    {
        // Se calculan los coeficientes del filtro IIR.
        // Llame 'levinson_computeCoeffs()'

        levinson_computeCoeffs(gLpcARcoeffs, gCorrelationsArray,LPC_ORDER);

    }

    /*----------------------------------------------------------------------*/
    /* 6) Generar señales de excitación del filtro */

    /* Señal de pulsos ajustables en frecuencia */
    excitation_generatePulses(gVariableFreqPulsesData, gVariableFreqPulsesPeriod, FRAME_SIZE);

    /* Ruido blanco calculado en tiempo real en arreglo 'gRandNoise' */
    int irand;
    for(irand = 0; irand < FRAME_SIZE; irand++){
        gRandNoise[irand] = DLU_fRand()/factor_ruido;
    }

    /*----------------------------------------------------------------------*/
    /* 7) Selección de señal de excitación */
    switch(gSynthMode)
    {
    case SYNTHMODE_EXTERNALSOURCE:
        gpArInputBuffer = gFloatInputBufferR;               // AR <-- gFloatInputBufferR
        DLU_writeLedD5(LED_OFF);
        break;
        //------------------------------------------------
    case SYNTHMODE_AUTOMATIC: // Selección según criterio
        if ( gVUanalysis == VU_VOICED){
            gpArInputBuffer = gVariableFreqPulsesData;      // AR <-- gVariableFreqPulsesData
            DLU_writeLedD5(LED_ON);
            break;
        } // No hay brake si es UNVOICED y sí debe pasar
          // al siguiente caso UNVOICED
        //------------------------------------------------
    case SYNTHMODE_ALWAYS_UNVOICED:
        if ( gNoiseTypeSelector == NOISE_STATIC){
            gpArInputBuffer = excitationNoise160;           // AR <-- excitationNoise160
            DLU_writeLedD6(LED_OFF);
        }
        if ( gNoiseTypeSelector == NOISE_RAND){
            gpArInputBuffer = gRandNoise;                  // AR <-- gRandNoise
            DLU_writeLedD6(LED_ON);
        }
        break;
        //------------------------------------------------
    case SYNTHMODE_ALWAYS_VOICED: // Siempre U
        // Si brake por que es también la opción default
    default:
        gpArInputBuffer = gVariableFreqPulsesData;          // AR <-- gVariableFreqPulsesData
        DLU_writeLedD5(LED_ON);
        //------------------------------------------------
    }

    /*----------------------------------------------------------------------*/
    /* 8) Filtrado */
      arFilter(gSynthVoiceFrame, gLpcARcoeffs, LPC_ORDER, gpArInputBuffer, FRAME_SIZE); // excitation_pulses_100 gVariableFreqPulsesData
   //  arFilter(gSynthVoiceFrame, gLpcA, LPC_ORDER, gpArInputBuffer, FRAME_SIZE); // excitation_pulses_100 gVariableFreqPulsesData

    /*----------------------------------------------------------------------*/
    /* 9) Corrección de amplitud a señal de entrada */
    gSynthAmpFactor = gInputVoiceRms*amp_factor;

    /************************************************************************/
    /*----------------------------------------------------------------------*/
    // Escritura en el buffer de salida para el DMA
    for (idxBuffer = 0; idxBuffer < (FRAME_SIZE) ; idxBuffer++)
    {
        float auxSample = gSynthAmpFactor*gSynthVoiceFrame[idxBuffer];
        /* Canal izquierdo */
        codecOutputArrayAsInt16L(idxBuffer) = FLT_2_AIC(auxSample);
        /* Canal derecho igual a izquierdo */
        codecOutputArrayAsInt16R(idxBuffer) = FLT_2_AIC(auxSample);
        /* En caso de actvar la captura en buffer de gráfico */
        DLU_appendGraphBuff2(auxSample);
    }
    /*----------------------------------------------------------------------*/
    /* Se baja flag de procesamiento*/
    gProcessingBufferIsFull = 0;
    return;
}

/***************************************************************************//**
*   \brief  Llenado de buffers con DMA
*
*   \param  Void.
*
*   \return Void.
*******************************************************************************/
interrupt void interrupt4(void) // interrupt service routine
{
    switch(EDMA_3CC_IPR)
    {
        case 1:                     // TCC = 0
            /* currentProcessingBuffer = PING */
            pInputPairBuffer = pingIN;
            pOutputPairBuffer = pingOUT;
            EDMA_3CC_ICR = 0x0001;    // clear EDMA3 IPR bit TCC
            break;
        case 2:                     // TCC = 1
            /* currentProcessingBuffer = PONG */
            pInputPairBuffer = pongIN;
            pOutputPairBuffer = pongOUT;
            EDMA_3CC_ICR = 0x0002;    // clear EDMA3 IPR bit TCC
            break;
        default:                    // may have missed an interrupt
            EDMA_3CC_ICR = 0x0003;    // clear EDMA3 IPR bits 0 and 1
            break;
    }

    /* Se baja flag de interrupción */
    EVTCLR0 = 0x00000100;

    /* Se levanta flag de buffer lleno */
    gProcessingBufferIsFull = 1;

    return;
}

/******************************************************************************
**      END OF SOURCE FILE
******************************************************************************/

