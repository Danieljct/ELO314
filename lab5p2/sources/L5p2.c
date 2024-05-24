/***************************************************************************//**
* \file     L5p2_solution.c
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

/******************************************************************************
**      MODULE PREPROCESSOR CONSTANTS
******************************************************************************/

/******************************************************************************
**      MODULE MACROS
******************************************************************************/
#define RMS_THRESHOLD       (0.0)

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
} dataPairLR;

/******************************************************************************
**      MODULE VARIABLE DEFINITIONS
******************************************************************************/

/*---------------------------------------------------------------------------*/
/* VARIABLES AUXILIARES PARA BUFFERS DE TRANSFERENCIA DMA */
/*---------------------------------------------------------------------------*/
/*
 * Punteros a buffers ping y pong (definidos en 'L138_LCDK_aic3106_init.c')
 */
extern int16_t *pingIN, *pingOUT, *pongIN, *pongOUT;

/*
 * Flag de indicación que un nuevo frame está listo para procesarse
 */
volatile int processingBufferIsFull = 0;

/*
 * Puntero a buffer con pares de entradas Izquierdo/Derecho desde DMA
 */
int16_t *pInputPairBuffer;

/*
 * Puntero a buffer con pares de salidas Izquierdo/Derecho para DMA
 */
int16_t *pOutputPairBuffer;

/*
 * Contador de frames procesados
 */
int framesCounter = 0;

/*---------------------------------------------------------------------------*/
/* DEFINICIÓN DE VARIABLES GLOBALES PARA LPC */
/*---------------------------------------------------------------------------*/

/*
 * Buffer de entrada izquierda
 */
float floatInputBufferL[FRAME_SIZE];

/*
 * Buffer de entrada derecha
 */
float floatInputBufferR[FRAME_SIZE];

/*
 * Buffer intermedio para enventanar señal del frame entrada
 */
float inputWindowed[FRAME_SIZE];

/*
 * Valor RMS del frame de entrada
 */
float inputFrameRms;

/*
 * Valor RMS del frame sintetizado con arFilter
 */
float synthFrameRms;

/*
 * Buffer de salida sintetizada
 */
float synthFrame[FRAME_SIZE];

/*
 * Arreglo con valores de autocorrelación
 */
float correlationsArray[LPC_ORDER + 1];

/*
 * Parámetros del lpc identificados con levinson()
 */
float lpcParameters[LPC_ORDER+1];

/*
 * Buffer para ruido generado dinámicamente
 */
float prandNoise[FRAME_SIZE];

/*
 * Índice buffer de ruido blanco para llenarlo
 */
unsigned int idxPrandNoise;

/*
 * Factor de corrección de amplitud de señal sintetizada
 */
float synthAmpFactor;

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
    /* Inicialización de algoritmo de Levinso-Durbin */
    levinson_init(LPC_ORDER);
    /*----------------------------------------------------------------------*/
    /* Background: procesamiento tras levantar bandera */
    while(1)
    {
        /* Retención hasta que 'processingBufferIsFull' es alto */
        while (!processingBufferIsFull);

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
    framesCounter+=1;
    /*-----------------------------------------------------------------------*/
    /* Llenado del vector complejo de entrada */
    for (idxBuffer = 0; idxBuffer < (FRAME_SIZE) ; idxBuffer++)
    {
        floatInputBufferL[idxBuffer] =
                AIC_2_FLT( ((dataPairLR *)pInputPairBuffer)[idxBuffer].leftChannel );

        floatInputBufferR[idxBuffer] =
                AIC_2_FLT( ((dataPairLR *)pInputPairBuffer)[idxBuffer].rightChannel );
    }

    /************************************************************************/
    // PROCESAMIENTO PARA IMPLEMENTAR COMPRESION DE VOZ (en la transmisión)
    /*----------------------------------------------------------------------*/
    /*  Obtener valor RMS (potencia de la señal) */
    
    /*----------------------------------------------------------------------*/
    /*  Enventanamiento */

    /*----------------------------------------------------------------------*/
    /*  Autocorrelación */

    /*----------------------------------------------------------------------*/
    /*  Obtención de coeficientes de la predictor */
    //levinson();

    /*----------------------------------------------------------------------*/
    /*  Detección de voz o silencio/siseo */
    
    
    /************************************************************************/
    // PROCESAMIENTO PARA IMPLEMENTAR SÍNTESIS DE VOZ (en la recepción)
    /*----------------------------------------------------------------------*/
    /* arFilter() con señales de estímulo depediente de voz o silencios */

    /*----------------------------------------------------------------------*/
    /*  Calculo del factor de corrección de valor RMS */
    // synthAmpFactor = ?
    
    /************************************************************************/
    // Escritura en el buffer de salida para el DMA
    for (idxBuffer = 0; idxBuffer < (FRAME_SIZE) ; idxBuffer++)
    {
        /* Canal izquierdo */                                       // PUEDE USAR synthAmpFactor AQUÍ
        ( ((dataPairLR *)pOutputPairBuffer)[idxBuffer].leftChannel ) = FLT_2_AIC(floatInputBufferL[idxBuffer]);

        /* Canal derecho igual a izquierdo */
        ( ((dataPairLR *)pOutputPairBuffer)[idxBuffer].rightChannel ) = FLT_2_AIC(floatInputBufferL[idxBuffer]);
    }
    /*----------------------------------------------------------------------*/
    /* Se baja flag de procesamiento*/
    processingBufferIsFull = 0;
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
    processingBufferIsFull = 1;

    return;
}

/******************************************************************************
**      END OF SOURCE FILE
******************************************************************************/

