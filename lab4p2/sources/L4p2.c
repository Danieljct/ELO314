// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/***************************************************************************//**
* \file     L4p2.c
*
* \brief    Base para el laboratorio L4p2
*
* \authors  Javier Romero, Gonzalo Carrasco
*******************************************************************************/

/***************************************************************************//**
**      HEADER FILES
*******************************************************************************/
#include "dlu_global_defs.h"
#include "L138_LCDK_aic3106_init.h"
#include "dsp_lab_utils.h"
#include <math.h>
#include "dlu_codec_config.h"
#include "FFT_radix2.h"
#include "complex.h"
#include "stdbool.h"
#include "hamming_windows.h"
#include "cheb_windows.h"

/******************************************************************************
**      MODULE PREPROCESSOR CONSTANTS
******************************************************************************/

/******************************************************************************
**      MODULE MACROS
******************************************************************************/
/* Constantes para ejemplo de generar señal */
#define FREQ_ALARM  (500.0)
#define AMP_ALARM   (2000.0)    // 2000 de 16383 (magnitud máxima), enteros de 16 bits

/* Macro para no sobrecargar con tanto indexado de punteros y estructuras */
#define codecInputArrayAsInt16L(idx)    ( ((dataPairLR_t *)pInputPairBuffer)[idx].leftChannel)
#define codecInputArrayAsInt16R(idx)    ( ((dataPairLR_t *)pInputPairBuffer)[idx].rightChannel)
#define codecOutputArrayAsInt16L(idx)   ( ((dataPairLR_t *)pOutputPairBuffer)[idx].leftChannel)
#define codecOutputArrayAsInt16R(idx)   ( ((dataPairLR_t *)pOutputPairBuffer)[idx].rightChannel)

/******************************************************************************
**      MODULE DATATYPES
******************************************************************************/

/*
 * Estructura auxiliar para descomponer en canales la trama del codec en DMA
 */
typedef struct
{
    int16_t rightChannel;
    int16_t leftChannel;
} dataPairLR_t;

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
volatile bool gProcessingBufferIsFull = false;

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
int gFramesCounter = 0;

/*---------------------------------------------------------------------------*/
/* DEFINICIÓN DE VARIABLES GLOBALES PARA FFT */
/*---------------------------------------------------------------------------*/

/*
 * Arreglo con muestras de señal de alarma audible
 */
float    gAudioSound[FFT_NPOINTS]; // alarm signal  buffer

/*
 * Vector de entrada a FFT
 */
//#pragma DATA_SECTION(gInputFFT, ".EXT_RAM")
//#pragma DATA_SECTION(gInputFFT, ".SHRAM")
Complex  gInputFFT[FFT_NPOINTS];

/*
 * Vector de salida de FFT
 */
//#pragma DATA_SECTION(gFFT, ".EXT_RAM")
//#pragma DATA_SECTION(gFFT, ".SHRAM")
Complex    gFFT[FFT_NPOINTS]; // FFT result    buffer

/*
 * Vector de valor absoluto de FFT
 */
float   gAbsFFT[FFT_NPOINTS]; // FFT magnitude buffer

/*
 * Tabla para reordenar muestras
 */
unsigned int bitReverseTable[FFT_NPOINTS];

/*----------------------------------------------------------------------*/
/* SEÑALES DE PRUEBA CON 8 MUESTRAS  */
/*----------------------------------------------------------------------*/
const Complex gFFT8_TEST_SIGNAL_Constant[8] =
{{1.0, 0.0},
 {1.0, 0.0},
 {1.0, 0.0},
 {1.0, 0.0},
 {1.0, 0.0},
 {1.0, 0.0},
 {1.0, 0.0},
 {1.0, 0.0}};

const Complex gFFT8_TEST_SIGNAL_DeltaKronecker[8] =
{{1.0, 0.0},
 {0.0, 0.0},
 {0.0, 0.0},
 {0.0, 0.0},
 {0.0, 0.0},
 {0.0, 0.0},
 {0.0, 0.0},
 {0.0, 0.0}};

const Complex gFFT8_TEST_SIGNAL_Cosine[8] =
{{1.0, 0.0},
 {0.707106781186548, 0.0},
 {0.0, 0.0},
 {-0.707106781186547, 0.0},
 {-1.0, 0.0},
 {-0.707106781186547, 0.0},
 {0.0, 0.0},
 {0.707106781186547, 0.0}};

const Complex gFFT8_TEST_SIGNAL_ImExponential[8] =
{{1.0, 0.0},
 {0.707106781186548, -0.707106781186548},
 {0.0, -1.0},
 {-0.707106781186548, -0.707106781186548},
 {-1.0, 0.0},
 {-0.707106781186548, 0.707106781186548},
 {0.0, 1.0},
 {0.707106781186548, 0.707106781186548}};

Complex gFft8TestInput[8];
Complex gFft8TestOutput[8];
float gFft8TestMagnitud[8];


/******************************************************************************
**      PRIVATE FUNCTION DECLARATIONS (PROTOTYPES)
******************************************************************************/
void testingFft8(void);
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
    /* Inicialización de LEDs */
    DLU_initLeds();
    DLU_writeLedD4(LED_OFF);
    DLU_writeLedD5(LED_OFF);
    DLU_writeLedD6(LED_OFF);
    DLU_writeLedD7(LED_OFF);
    DLU_initTicToc();
    /* Inicialización de Pulsadores User 1 y User 2 */
    DLU_initPushButtons();
    /*----------------------------------------------------------------------*/
    int n;
    initBitReversalTable(FFT_NPOINTS, bitReverseTable);
    /* Inicialización de vector con factores de twiddle */
    initTweddleFactors();

    /* Inicialización de arreglos y parámetros */
    for (n=0 ; n< FFT_NPOINTS ; n++)
    {
        /* Ejemplo de señal senoidal (guardada en memoria como LUT) para
         * extraer en procesamiento por frames: debe existir el vector con
         * los N datos. Este ejemplo es con LUT para no quitar tiempo al CPU
         * y priorizar la FFT.
         * En este caso su frecuencia es tal que completa periodos exactos
         * en N=FFT_NPOINTS muestras para no causar discontinuidad audible
         * entre un frame y otro. */
        gAudioSound[n]    = AMP_ALARM*cos(2*M_PI*n*FREQ_ALARM/CODEC_FS);

        /* Parte imaginaria de entrada a FFT es siempre cero para nuestro
         * caso */
        gInputFFT[n].img = 0.0;
        /* La parte real se escribirá con los datos, no es necesario
         * iniciarlo en cero.
         */
    }

    /*----------------------------------------------------------------------*/
    /* Prueba de algoritmo FFT usando 8 puntos
     *
     * AQUÍ DEBEN TRABAJAR PARA LOGRAR VALIDAR SU ALGORITMO FFT. Fijar para
     * ello FFT_NPOINTS = 8
     * */
    testingFft8();

    /*----------------------------------------------------------------------*/
    // Inicialización del Codec de audio
    L138_initialise_edma(CODEC_FS, CODEC_ADC_GAIN, DAC_ATTEN_0DB, CODEC_INPUT_CFG);

    /*----------------------------------------------------------------------*/
    /* Background: procesamiento FFT en tiempo real                         */
    /*----------------------------------------------------------------------*/
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

    /*-----------------------------------------------------------------------*/
    /* Uso de vector de entrada de señal Left y/o Right desde el DMA */
    for (idxBuffer = 0; idxBuffer < (FFT_NPOINTS) ; idxBuffer++)
    {
        /* Copia señal en buffer de entrada de audio a vector de entrada FFT
         * Casteo a flotante para procesar así la FFT. */
        gInputFFT[bitReverseTable[idxBuffer]].real = (float) codecInputArrayAsInt16L(idxBuffer);//*cheb256[idxBuffer];
        /*-------------------------------------------------------------------*/
        /* Bypass directo del canal izquierdo al canal izquierdo de salida:
         * 'InputBufferL' => 'OutputBufferL' */
        codecOutputArrayAsInt16L(idxBuffer) = codecInputArrayAsInt16L(idxBuffer);
    }

    /*----------------------------------------------------------------------*/
    /* Cómputo de FFT radix-2 */
    fftRadix2(FFT_NPOINTS, gInputFFT, gFFT);

    /* Cómputo de magnitudes en frecuencia */
    fftMag(FFT_NPOINTS, gFFT, gAbsFFT);

    // Para llenar GrapgBuff 1 con gAbsFFT
    if ( DLU_readPushButton1() ) {
        DLU_enableSynchronicSingleCaptureOnAllGraphBuff();
    }

    /*----------------------------------------------------------------------*/
    /*  Escritura en el buffer de salida para el DMA */
    for (idxBuffer = 0; idxBuffer < (FFT_NPOINTS) ; idxBuffer++)
    {
        /* Canal izquierdo ya fue copiado con Bypass de entrada izquierda */

        /* Canal derecho de salida*/
        codecOutputArrayAsInt16R(idxBuffer) = (int16_t)(gAudioSound[idxBuffer]);

        /* Valor absouto en GraphBuff */
        DLU_appendGraphBuff1( codecInputArrayAsInt16L(idxBuffer) );
        DLU_appendGraphBuff2( gAbsFFT[idxBuffer] );
    }
    /*----------------------------------------------------------------------*/
    /* Se baja flag al terminar de procesar los buffers de datos */
    gProcessingBufferIsFull = false;
    return;
}

/***************************************************************************//**
*   \brief  Función para implementar pruebas de una FFT de 8 puntos usando
*           las cuatro señales: constant, delta Kronecker, Cosine y Exponential
*
*   \param  Void.
*
*   \return Void.
*******************************************************************************/
void testingFft8(void)
{
    /*----------------------------------------------------------------------*/
    /* Para muestras constantes */
    fftRadix2(8, (Complex *)gFFT8_TEST_SIGNAL_Constant, gFft8TestOutput);
    fftMag(8, gFft8TestOutput, gFft8TestMagnitud);
    // Observar resultado en 'gFft8TestMagnitud'

    /*----------------------------------------------------------------------*/
    /* Para muestras de delta kronecker */
    fftRadix2(8, (Complex *)gFFT8_TEST_SIGNAL_DeltaKronecker, gFft8TestOutput);
    fftMag(8, gFft8TestOutput, gFft8TestMagnitud);
    // Observar resultado en 'gFft8TestMagnitud'

    /*----------------------------------------------------------------------*/
    /* Para muestras de un coseno */
    fftRadix2(8, (Complex *)gFFT8_TEST_SIGNAL_Cosine, gFft8TestOutput);
    fftMag(8, gFft8TestOutput, gFft8TestMagnitud);
    // Observar resultado en 'gFft8TestMagnitud'

    /*----------------------------------------------------------------------*/
    /* Para muestras de una exponencial de rotación negativa */
    fftRadix2(8, (Complex *)gFFT8_TEST_SIGNAL_ImExponential, gFft8TestOutput);
    fftMag(8, gFft8TestOutput, gFft8TestMagnitud);
    // Observar resultado en 'gFft8TestMagnitud'

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
    gProcessingBufferIsFull = true;

    return;
}

/******************************************************************************
**      END OF SOURCE FILE
******************************************************************************/

