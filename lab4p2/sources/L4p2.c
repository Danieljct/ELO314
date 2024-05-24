/***************************************************************************//**
* \file     L4p2_solution.c
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

/******************************************************************************
**      MODULE PREPROCESSOR CONSTANTS
******************************************************************************/

/******************************************************************************
**      MODULE MACROS
******************************************************************************/
#define FREQ_ALARM  (500.0)
#define AMP_ALARM   (2000.0)    // De magnitud máxima 16383. Enteros de 16 bits


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
/* DEFINICIÓN DE VARIABLES GLOBALES PARA FFT */
/*---------------------------------------------------------------------------*/

/*
 * Arreglo con muestras de señal de alarma audible
 */
float    alarm[FFT_NPOINTS]; // alarm signal  buffer

/*
 * Vector de entrada a FFT
 */
//#pragma DATA_SECTION(input, ".EXT_RAM")
//#pragma DATA_SECTION(input, ".SHRAM")
Complex  inputFFT[FFT_NPOINTS];

/*
 * Vector de salida de FFT
 */
//#pragma DATA_SECTION(FFT, ".EXT_RAM")
//#pragma DATA_SECTION(FFT, ".SHRAM")
Complex    FFT[FFT_NPOINTS]; // FFT result    buffer

/*
 * Vector de valor absoluto de FFT
 */
float   absFFT[FFT_NPOINTS]; // FFT magnitude buffer

/*
 * Tabla para reordenar muestras
 */
unsigned int bitReverseTable[FFT_NPOINTS];

/*----------------------------------------------------------------------*/
// Señales de prueba con 8 muestras
Complex fft8TestConstant[8] =
{{1.0, 0.0},
 {1.0, 0.0},
 {1.0, 0.0},
 {1.0, 0.0},
 {1.0, 0.0},
 {1.0, 0.0},
 {1.0, 0.0},
 {1.0, 0.0}};

Complex fft8TestDeltaKronecker[8] =
{{1.0, 0.0},
 {0.0, 0.0},
 {0.0, 0.0},
 {0.0, 0.0},
 {0.0, 0.0},
 {0.0, 0.0},
 {0.0, 0.0},
 {0.0, 0.0}};

Complex fft8TestCosine[8] =
{{1.0, 0.0},
 {0.707106781186548, 0.0},
 {0.0, 0.0},
 {-0.707106781186547, 0.0},
 {-1.0, 0.0},
 {-0.707106781186547, 0.0},
 {0.0, 0.0},
 {0.707106781186547, 0.0}};

Complex fftTestImExponential[8] =
{{1.0, 0.0},
 {0.707106781186548, -0.707106781186548},
 {0.0, -1.0},
 {-0.707106781186548, -0.707106781186548},
 {-1.0, 0.0},
 {-0.707106781186548, 0.707106781186548},
 {0.0, 1.0},
 {0.707106781186548, 0.707106781186548}};

Complex fft8TestInput[8];
Complex fft8TestOutput[8];
float fft8TestMagnitud[8];

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
    /* Inicializació de LEDs */
    DLU_initLeds();
    DLU_writeLedD4(LED_OFF);
    DLU_writeLedD5(LED_OFF);
    DLU_writeLedD6(LED_OFF);
    DLU_writeLedD7(LED_OFF);
    DLU_initTicToc();
    /*----------------------------------------------------------------------*/
    int n;
    /* Inicialización de factores arreglos y parámetros */
    initTweddleFactors();

    for (n=0 ; n< FFT_NPOINTS ; n++)
    {
        /* Vector de alarma */
        alarm[n]    = AMP_ALARM*cos(2*M_PI*n*FREQ_ALARM/CODEC_FS);
        /* Pate imaginaria de entrada a FFT es siempre cero */
        inputFFT[n].img = 0.0;
    }

    /*----------------------------------------------------------------------*/
    /* Prueba de algoritmo FFT usando 8 puntos */
    testingFft8();

    /*----------------------------------------------------------------------*/
    // Inicialización del Codec de audio
    L138_initialise_edma(CODEC_FS, CODEC_ADC_GAIN, DAC_ATTEN_0DB, CODEC_INPUT_CFG);
    /*----------------------------------------------------------------------*/
    /* Background: procesamiento FFT tras levantar bandera */
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
    for (idxBuffer = 0; idxBuffer < (FFT_NPOINTS) ; idxBuffer++)
    {
        inputFFT[idxBuffer].real =
                (float)( ((dataPairLR *)pInputPairBuffer)[idxBuffer].leftChannel );

        /* Bypass directo del canal izquierdo de entrada al canal derecho desde
         * salida: 'InputPairBuffer' a 'OutputPairBuffer' */
        ( ((dataPairLR *)pOutputPairBuffer)[idxBuffer].rightChannel ) =
                (float)( ((dataPairLR *)pInputPairBuffer)[idxBuffer].leftChannel );
    }
    /*----------------------------------------------------------------------*/
    /* Cómputo de FFT radix-2 */
    fftRadix2(FFT_NPOINTS, inputFFT, FFT);

    /* Cómputo de magnitudes en frecuencia */
    fftMag(FFT_NPOINTS, FFT, absFFT);

    /*----------------------------------------------------------------------*/
    // Escritura en el buffer de salida para el DMA
    for (idxBuffer = 0; idxBuffer < (FFT_NPOINTS) ; idxBuffer++)
    {
        /* Canal izquierdo */
        ( ((dataPairLR *)pOutputPairBuffer)[idxBuffer].leftChannel ) = (int16_t)(alarm[idxBuffer]);

        /* El canal derecho ya fue copiado */
    }
    /*----------------------------------------------------------------------*/
    /* Se baja flag */
    processingBufferIsFull = 0;
    return;
}

/***************************************************************************//**
*   \brief  Función pra implementar pruebas de una FFT de 8 puntos usando
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
    fftRadix2(8, fft8TestConstant, fft8TestOutput);
    fftMag(8, fft8TestOutput, fft8TestMagnitud);

    /*----------------------------------------------------------------------*/
    /* Para muestras de delta kronecker */
    fftRadix2(8, fft8TestDeltaKronecker, fft8TestOutput);
    fftMag(8, fft8TestOutput, fft8TestMagnitud);

    /*----------------------------------------------------------------------*/
    /* Para muestras de un coseno */
    fftRadix2(8, fft8TestCosine, fft8TestOutput);
    fftMag(8, fft8TestOutput, fft8TestMagnitud);

    /*----------------------------------------------------------------------*/
    /* Para muestras de una exponencial de rotación negativa */
    fftRadix2(8, fftTestImExponential, fft8TestOutput);
    fftMag(8, fft8TestOutput, fft8TestMagnitud);

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

