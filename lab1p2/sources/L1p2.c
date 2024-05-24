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

/* Para los ejercicios iniciales son en punto fijo, 1 al 11 de la Guía Lab1P2 2022
 * NO definir 'FLOAT_OPERATIONS'
 *
 * Para los puntos 12 y 13 de la Guía Lab1P2 2022
 * Definir 'FLOAT_OPERATIONS'
 */
//#define FLOAT_OPERATIONS

/******************************************************************************
**      MODULE MACROS
******************************************************************************/
// Constantes
#define AUDIOBUFFERSIZE          (4000)             // 320 Delay 20ms @ 16ksps
#define RMSBUFFERSIZE            (256)              // 256 para 16ms de ventana
#define AVGBUFFERSIZE            (6)                // Rango pedido
#define OUTPUT_SINGLE_SAMPLE_PULSE_PERIOD   (160)   // Perido en muestras

/******************************************************************************
**      MODULE DATATYPES
******************************************************************************/
/*
 * Definicion de tipo de dato enumerado para elegir muestras para
 * Moving Average (MA)
 */
typedef enum SelectMA_tag
{
    MA_Bypass = 0,
    MA_N2,
    MA_N4,
    MA_N6,
    MA_NumberOfOptions
}SelectMA_e;

/******************************************************************************
**      MODULE VARIABLE DEFINITIONS
******************************************************************************/

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

/*---------------------------------------------------------------------------*/
/* CÓMPUTO MOVING AVERAGE */
/*---------------------------------------------------------------------------*/
SelectMA_e gMASelection = MA_Bypass;

#ifdef  FLOAT_OPERATIONS
float           gMovingAverageOutput = 0;
#else
int16_t         gMovingAverageOutput = 0;
#endif

/*---------------------------------------------------------------------------*/
/* GENERACIÓN DE SEÑALES */
/*---------------------------------------------------------------------------*/
int16_t         gCosSignal;
int16_t         gAmpCosine = 8191;
int16_t         gAmplitudeModulationL = 0;
int16_t         gAmplitudeModulationR = 0;
int16_t         gTremoloL = 0;
int16_t         gTremoloR = 0;

float           gThetaCosineSignal = 0.0;
float           gFreqCosineMin = 100.0;

int16_t         gOutputSingleSamplePulseCounter = 0;
int16_t         gOutputSingleSamplePulse = 0;

/* Máscada de bits */
int16_t         gMaskLsbs = 0xF800;
int16_t         gMaskedLsbOutput = 0;

/*---------------------------------------------------------------------------*/
/* OSCILLOSCOPE MEASUREMENTS */
/*---------------------------------------------------------------------------*/
/* Indice de lectura */
int32_t gIdxOscMeas = 0;
/* Variables de salida */
float gOscilloscopeTestSignalPulses = 0.0;
float gOscilloscopeTestSignalRinging = 0.0;

/******************************************************************************
**      PRIVATE FUNCTION DECLARATIONS (PROTOTYPES)
******************************************************************************/

int16_t computeIntegerMovingAverage(int16_t inputSignal, SelectMA_e selectionMA);
float   computeFloatMovingAverage(float inputSignal, SelectMA_e selectionMA);

/******************************************************************************
**      FUNCTION DEFINITIONS
******************************************************************************/

interrupt void interrupt4(void) // interrupt service routine
{
//#############################################################################
        /*-------------------------------------------------------------------*/
        /* LECTURA DE ENTRADAS DEL CODEC */
        /*-------------------------------------------------------------------*/
#ifdef  FLOAT_OPERATIONS
        // Para operar con variables de tipo flotante
        DLU_readCodecInputs(&gFloatCodecInputL, &gFloatCodecInputR);
#else
        codec_data.uint = input_sample();
        gIntCodecInputL = codec_data.channel[LEFT];
        gIntCodecInputR = codec_data.channel[RIGHT];
#endif

        /*-------------------------------------------------------------------*/
        // USO DE ESTADO DE PULSADOR USER1 y USER2 PARA ACTIVAR LEDs
        /*-------------------------------------------------------------------*/
        // Obtención de valores de estados para cambiar estado de leds
        DLU_writeLedD4( DLU_readPushButton1());

        if ( DLU_readToggleStatePB1() )
            DLU_writeLedD5(LED_ON);
        else
            DLU_writeLedD5(LED_OFF);

        if ( DLU_readToggleStatePB2() )
            DLU_writeLedD6(LED_ON);
        else
            DLU_writeLedD6(LED_OFF);

        if ( DLU_readToggleStatePB12() )
            DLU_writeLedD7(LED_ON);
        else
            DLU_writeLedD7(LED_OFF);

        /*-------------------------------------------------------------------*/
        /* GENERACIÓN DE REFERENCIA SINUSOIDAL */
        /*-------------------------------------------------------------------*/
        // Ángulo se incrementa linealmente segun frecuencia angular
        // dada por un valor base más el valor del contador de ajuste
        gThetaCosineSignal  = gThetaCosineSignal + 2.0 * M_PI* ( (float)DLU_readTrimmerCounter() + gFreqCosineMin) * TS;
        // Theta entre [0, 2*pi]
        if ( gThetaCosineSignal > 2.0 * M_PI )
        {
            gThetaCosineSignal = gThetaCosineSignal - 2.0*M_PI;
        }
        // Calculo de y cos(Theta) 
        gCosSignal = (int16_t)( 2 * gAmpCosine * cosf(gThetaCosineSignal) );

        /*-------------------------------------------------------------------*/
        /* GENERACIÓN DE PULSO DE SALIDA */
        /*-------------------------------------------------------------------*/
        gOutputSingleSamplePulseCounter++;
        if ( gOutputSingleSamplePulseCounter >= OUTPUT_SINGLE_SAMPLE_PULSE_PERIOD )
        {
            // Valor alto sin saturar: 30000/32767 ~ 91.6%
            gOutputSingleSamplePulse = 30000;
            // Reinicia contador de periodo
            gOutputSingleSamplePulseCounter = 0;
        }
        else
        {
            gOutputSingleSamplePulse = 0;
        }

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
        // Para el Tremolo, 'gFreqCosineMin' debe ser 0
        gTremoloL = (int16_t)( ( ( (32768 + (int32_t)gCosSignal)>>1 ) * (int32_t)gIntCodecInputL)  >> 15);
        gTremoloR = (int16_t)( ( ( (32768 - (int32_t)gCosSignal)>>1 ) * (int32_t)gIntCodecInputL)  >> 15);


        /*-------------------------------------------------------------------*/
        /* Inicia medición de tiempo de ejecución */
        DLU_tic();
        /*-------------------------------------------------------------------*/
        /* MOVING AVERAGE  */
        /*-------------------------------------------------------------------*/
        // Cuando use esta sección de código, configure DLU_configTrimmerCounter(0, 3);
        gMASelection = (SelectMA_e) DLU_readTrimmerCounter();
#ifdef  FLOAT_OPERATIONS
        gMovingAverageOutput = computeFloatMovingAverage(gFloatCodecInputL, gMASelection );
#else
        // OPERACIÓN EN PUNTO FIJO
        gMovingAverageOutput = computeIntegerMovingAverage(gIntCodecInputL, gMASelection );
#endif
        /* Medición de tiempo de ejecución */
        DLU_toc(); // Se debe observar la variable 'DLU_timeTicToc'


        /*-------------------------------------------------------------------*/
        /* OSCILLOSCOPE_MEAS  */
        /*-------------------------------------------------------------------*/
#ifdef  FLOAT_OPERATIONS
        /* Actualizando índice de lectura para medición con osciloscopio */
        gIdxOscMeas++;
        if (gIdxOscMeas >= OSCILLOSCOPE_SIGSIZE)
            gIdxOscMeas = 0;
        gOscilloscopeTestSignalPulses  = osc_pulses[gIdxOscMeas];
        gOscilloscopeTestSignalRinging = osc_ringing[gIdxOscMeas];
#endif


        /*-------------------------------------------------------------------*/
        /* PARA VISUALIZAR EN GRÁFICOS */
        /*-------------------------------------------------------------------*/
        if ( DLU_readPushButton1() )
        {
            DLU_enableSynchronicSingleCaptureOnAllGraphBuff();
        }

        DLU_appendGraphBuff1(gIntCodecInputL);   // Se debe graficar la variable 'gGraphBuff1'
        DLU_appendGraphBuff2(gCosSignal);        // Se debe graficar la variable 'gGraphBuff2'
        /*-------------------------------------------------------------------*/
        if ( DLU_readPushButton2() )
        {
            DLU_clearAllGraphBuffers();
        }

        /*-------------------------------------------------------------------*/
        /* ESCRITURA EN SALIDA DEL CODEC */
        /*-------------------------------------------------------------------*/
#ifdef  FLOAT_OPERATIONS
        // Selección de señales de salida
        gFloatCodecOutputL = gAmplitudeModulationL; // gOscilloscopeTestSignalPulses;
        gFloatCodecOutputR = gAmplitudeModulationR; // gOscilloscopeTestSignalRinging;

        // Escritura en buffer de salida hacia el codec
        DLU_writeCodecOutputs(gFloatCodecOutputL, gFloatCodecOutputR);
#else
        // OPERACIÓN EN PUNTO FIJO
        // Selección de señales de salida
        gIntCodecOutputL = gAmplitudeModulationL;
        gIntCodecOutputR = gAmplitudeModulationR;

        // Escritura en buffer de salida hacia el codec
        codec_data.channel[LEFT] = gIntCodecOutputL;
        codec_data.channel[RIGHT] = gIntCodecOutputR;
        output_sample(codec_data.uint);
#endif

//#############################################################################
    return;
}

/*****************************************************************************/
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
    /* Configuración de Trimmer ajustado con Pushbuttons */
    DLU_configTrimmerCounter(0, 1000);
    DLU_configTrimmerAutoIncrement(1000, 5);

   /* Loop infinito a espera de interrupción del Codec */
    while(1);
}

/*****************************************************************************/
int16_t computeIntegerMovingAverage(int16_t inputSignal, SelectMA_e selectionMA)
{
    static int16_t sampleK_0 = 0; // Variables que almacenan hasta 6 muestras recientes
    static int16_t sampleK_1 = 0;
    static int16_t sampleK_2 = 0;
    static int16_t sampleK_3 = 0;
    static int16_t sampleK_4 = 0;
    static int16_t sampleK_5 = 0;
    static int16_t movingAverageN2 = 0;
    static int16_t movingAverageN4 = 0;
    static int16_t movingAverageN6 = 0;
    int16_t movingAverageOutput = 0;

    // Sample shifting
    sampleK_5 = sampleK_4;
    sampleK_4 = sampleK_3;
    sampleK_3 = sampleK_2;
    sampleK_2 = sampleK_1;
    sampleK_1 = sampleK_0;
    sampleK_0 = inputSignal; // Ingreso de la muestra actual

    //Moving average
    movingAverageN2 = ( (int32_t)sampleK_0 + (int32_t)sampleK_1 ) / 2;
    movingAverageN4 = ( (int32_t)sampleK_0 + (int32_t)sampleK_1 + (int32_t)sampleK_2 + (int32_t)sampleK_3 ) / 4;
    movingAverageN6 = ( (int32_t)sampleK_0 + (int32_t)sampleK_1 + (int32_t)sampleK_2 + (int32_t)sampleK_3 + (int32_t)sampleK_4 + (int32_t)sampleK_5 ) / 6;

    switch ( selectionMA )
    {
        case MA_Bypass:
            movingAverageOutput = inputSignal;
            break;
        case MA_N2:
            movingAverageOutput = movingAverageN6;
            break;
        case MA_N4:
            movingAverageOutput = movingAverageN4;
            break;
        case MA_N6:
            movingAverageOutput = movingAverageN6;
            break;
        default:
            movingAverageOutput = inputSignal;
    }

    return movingAverageOutput;
}

/*****************************************************************************/
float computeFloatMovingAverage(float inputSignal, SelectMA_e selectionMA)
{
    static float sampleK_0 = 0; // Variables que almacenan hasta 6 muestras recientes
    static float sampleK_1 = 0;
    static float sampleK_2 = 0;
    static float sampleK_3 = 0;
    static float sampleK_4 = 0;
    static float sampleK_5 = 0;
    static float movingAverageN2 = 0;
    static float movingAverageN4 = 0;
    static float movingAverageN6 = 0;
    float movingAverageOutput = 0;

    // Sample shifting
    sampleK_5 = sampleK_4;
    sampleK_4 = sampleK_3;
    sampleK_3 = sampleK_2;
    sampleK_2 = sampleK_1;
    sampleK_1 = sampleK_0;
    sampleK_0 = inputSignal; // Ingreso de la muestra actual

    //Moving average
    movingAverageN2 = ( sampleK_0 + sampleK_1 ) / 2;
    movingAverageN4 = ( sampleK_0 + sampleK_1 + sampleK_2 + sampleK_3 ) / 4;
    movingAverageN6 = ( sampleK_0 + sampleK_1 + sampleK_2 + sampleK_3 + sampleK_4 + sampleK_5 ) / 6;

    switch ( selectionMA )
    {
        case MA_Bypass:
            movingAverageOutput = inputSignal;
            break;
        case MA_N2:
            movingAverageOutput = movingAverageN6;
            break;
        case MA_N4:
            movingAverageOutput = movingAverageN4;
            break;
        case MA_N6:
            movingAverageOutput = movingAverageN6;
            break;
        default:
            movingAverageOutput = inputSignal;
    }

    return movingAverageOutput;
}


/******************************************************************************
**      END OF SOURCE FILE
******************************************************************************/
