// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/******************************************************************************
* \file     Lab3p2.c
*
* \brief    Experiencia 3 de laboratorio DSP ELO314
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

/******************************************************************************
**      MODULE PREPROCESSOR CONSTANTS
******************************************************************************/
#define PI 3.1415926535897932384626433
/******************************************************************************
**      MODULE MACROS
******************************************************************************/
#define DTMF_ENV_FRAME_SIZE     (32)  // 1ms
#define DTMF_DET_LEVEL          (0.002)
#define DTMF_CH_SNR_RATE        (2.5)

/******************************************************************************
**      MODULE DATATYPES
******************************************************************************/
/*
 * Estructura de estado de filtros biquad
 */
typedef struct bqStatus_t {
    float bqA1;
    float bqA2;
    float bqB0;
    float bqB1;
    float bqB2;
    float bqInput[3];
    float bqOutput[3];
} bqStatus_t;

/******************************************************************************
**      MODULE VARIABLE DEFINITIONS
******************************************************************************/
typedef enum FILTERTYPE{
    NOTCH,
    PASS
}FILTERTYPE;
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
 * Variables de estado de salida saturada
 */
int outSaturationStat = 0;

//#pragma DATA_SECTION(audioBufferR,".EXT_RAM")
//#pragma DATA_SECTION(audioBufferL,".EXT_RAM")

/*---------------------------------------------------------------------------*/
/* VARABLES DETECTOR DTMF */
/*---------------------------------------------------------------------------*/
/* Señales de salida para cada filtro */
float dtmfTones[7];

int32_t framePos = 0;
float tonesAmplitud[7]={0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float tonesAmpAux[7]={0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
int32_t dtmfSymbol = 0;

/*---------------------------------------------------------------------------*/
/* VARABLES FILTRO NOTCH */
/*---------------------------------------------------------------------------*/
bqStatus_t FiltroParte2;

int BW1 = 20;
float tuneBsfOutput = 0.0;
float notchFreq     = 20.0;
//#define IMPULSO
#ifdef IMPULSO
int entrada = 0;
int contador = 1600;
#endif
float salidafiltro = 0;


uint16_t tones[7] = {697, 770, 852, 941, 1209, 1336, 1477};
uint16_t bwtones[7] = {15, 15, 15, 15, 30, 30, 40};
bqStatus_t filtros[7];


/******************************************************************************
**      PRIVATE FUNCTION DECLARATIONS (PROTOTYPES)
******************************************************************************/
float filterBiquad(bqStatus_t *filterNState, float filterInput);
void notchUpdate(bqStatus_t* Filtro, float fc, float BW);
void envelopeDetector(float *tonesInputs);
void dtmfDetection(float *tonesInputs);

bqStatus_t CrearFiltro(FILTERTYPE type, float BW, float fc);

/******************************************************************************
**      FUNCTION DEFINITIONS
******************************************************************************/
int count = 0;
interrupt void interrupt4(void) // interrupt service routine
{
//#############################################################################
        /*-------------------------------------------------------------------*/
        /* LECTURA DE ENTRADAS DEL CODEC */
        /*-------------------------------------------------------------------*/
        DLU_readCodecInputs(&floatCodecInputL, &floatCodecInputR);

        /*-------------------------------------------------------------------*/
        /* Inicia medición de tiempo de ejecución */
        DLU_tic();
#ifdef IMPULSO
        entrada++;
        if (!(entrada%contador)){
            floatCodecInputL = 1;
            entrada = 0;
        }
        else {
            floatCodecInputL = 0;
        }

#endif

        salidafiltro = filterBiquad(&FiltroParte2, floatCodecInputL);
        //floatCodecInputR = filterBiquad(&FiltroParte2, floatCodecInputR);
        /*-------------------------------------------------------------------*/
        /* FILTRO NOTCH SINTONIZABLE */
        /*-------------------------------------------------------------------*/
        tuneBsfOutput = 0.0;

        if ( DLU_readToggleStatePB1() || DLU_readToggleStatePB2()){
            notchUpdate(&FiltroParte2, DLU_readTrimmerCounter(), 100);
        }

        /*-------------------------------------------------------------------*/
        /* FILTROS PARA DTMF */
        /*-------------------------------------------------------------------*/
        int i;
        for(i = 0; i < 7; i++){
            dtmfTones[i] = filterBiquad(&filtros[i], floatCodecInputL);
        }

        dtmfDetection(dtmfTones);

        /*-------------------------------------------------------------------*/
        /* PARA VISUALIZAR EN GRÁFICO */
        /*-------------------------------------------------------------------*/
        DLU_enableSynchronicSingleCaptureOnAllGraphBuff();
        DLU_appendGraphBuff1(floatCodecInputL);
        DLU_appendGraphBuff2(salidafiltro);

        /*-------------------------------------------------------------------*/
        /* ESCRITURA EN SALIDA DEL CODEC */
        /*-------------------------------------------------------------------*/
        floatCodecOutputL = dtmfTones[0];
        floatCodecOutputR = floatCodecInputL;

        /* Medición de tiempo de ejecución */
        DLU_toc();

        outSaturationStat = DLU_writeCodecOutputs(floatCodecOutputL,floatCodecOutputR);

//#############################################################################
    return;
}

void main()
{
    /* Inicialización de Pulsadores User 1 y User 2 */
    DLU_initPushButtons();
    /* Inicializa función de medición de tiempos de ejecución */
    DLU_initTicToc();

    FiltroParte2 = CrearFiltro(NOTCH, 100, 4000);
    int i;
    for(i = 0; i < 7; i++){
        filtros[i] = CrearFiltro(PASS, bwtones[i], tones[i]);
    }

    /* Inicializacion BSL y AIC31 Codec */
    L138_initialise_intr(CODEC_FS, CODEC_ADC_GAIN, CODEC_DAC_ATTEN, CODEC_INPUT_CFG);
    /* Inicialización de LEDs */
    DLU_initLeds();
    /* Configuración de Trimmer ajustado con Pushbuttons */
    DLU_configTrimmerCounter(100, 4000);
    //DLU_configTrimmerAutoIncrement(1000, 5);


   /* Loop infinito a espera de interrupción del Codec */
    while(1);
}

/******************************************************************************
*   \brief  Esta función implementa una etapa de filtro biquad
*
*   \param filterNState     : puntero a la estructura del biquad a ejecutar
*   \param filterInput      : señal de entrada al filtro biquad a ejecutar
*
*   \return filterOutput    : señal de salida del filtro biquad ejecutado
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

/******************************************************************************
*   \brief Esta función modifica los parámetros del filtro notch para ajustar
*           su frecuencia de sintonía. El ancho de banda en cambio se mantiene
*           constante y dependiente de la constante TUNE_BSF_D.
*
*   \param tuneFreq :  es la frecuencia de sintonía deseada en Hz.
*
*   \return Void.
******************************************************************************/
void notchUpdate(bqStatus_t* Filtro, float fc, float BW){
        float th = 2*PI*fc/CODEC_FS;
        BW = 2*PI*BW/CODEC_FS;
        float d =  (1 - sin(BW))/cos(BW);
        Filtro->bqB1 = -(1 + d) * cos(th);
        Filtro->bqA1 = -(1 + d) * cos(th);

}

/******************************************************************************
*   \brief Esta función permite detectar de forma sencilla la envolvente
*           de los tonos filtrados.
*           Una vez se retorna de la función quedan actualizados los valores
*           de la variable 'tonesAmplitud'.

*   \param *tonesInputs : puntero a arreglo de canales filtrados
*
*   \return Void
******************************************************************************/
void envelopeDetector(float *tonesInputs) {
    int32_t idxTones;

    if( fabs( tonesInputs[0] ) > tonesAmpAux[0])
        tonesAmpAux[0] = fabs( tonesInputs[0] );
    if( fabs( tonesInputs[1] ) > tonesAmpAux[1])
        tonesAmpAux[1] = fabs( tonesInputs[1] );
    if( fabs( tonesInputs[2] ) > tonesAmpAux[2])
        tonesAmpAux[2] = fabs( tonesInputs[2] );
    if( fabs( tonesInputs[3] ) > tonesAmpAux[3])
        tonesAmpAux[3] = fabs( tonesInputs[3] );
    if( fabs( tonesInputs[4] ) > tonesAmpAux[4])
        tonesAmpAux[4] = fabs( tonesInputs[4] );
    if( fabs( tonesInputs[5] ) > tonesAmpAux[5])
        tonesAmpAux[5] = fabs( tonesInputs[5] );
    if( fabs( tonesInputs[6] ) > tonesAmpAux[6])
        tonesAmpAux[6] = fabs( tonesInputs[6] );

    framePos++;
    if ( framePos > DTMF_ENV_FRAME_SIZE )
    {
        framePos = 0;

        for (idxTones = 0; idxTones < 7; idxTones++)
        {
            tonesAmplitud[idxTones] = 0.5 * (tonesAmpAux[idxTones] + tonesAmplitud[idxTones]);
            tonesAmpAux[idxTones] = 0.0;
        }

    }

}

/******************************************************************************
*   \brief Función que actualiza el estado de los leds para indicar símbolo
*           detectado en base a reconocer DTFM.
*           Al retornar de esta función, los led se actualizan con el último
*           símbolo detectado.

*   \param *tonesInputs : puntero a arreglo de canales filtrados
*
*   \return Void
******************************************************************************/
void dtmfDetection(float *tonesInputs) {
    float levelAux;
    int32_t dtmf_row = 0;
    int32_t dtmf_col = 0;
    /*-----------------------------------------------------------------------*/
    /* Actualización de amplitudes */
    envelopeDetector(tonesInputs);

    /* Promedio de canales */
    levelAux = 0.143 * (tonesAmplitud[0] +
            tonesAmplitud[1] +
            tonesAmplitud[2] +
            tonesAmplitud[3] +
            tonesAmplitud[4] +
            tonesAmplitud[5] +
            tonesAmplitud[6] );
    /*-----------------------------------------------------------------------*/
    /* Detección de canal bajo */
    do
    {
        /* ¿Será fila 1? */
        levelAux = tonesAmplitud[0] / (tonesAmplitud[1] + tonesAmplitud[2] +tonesAmplitud[3]);
        if (levelAux > DTMF_CH_SNR_RATE)
        {
            dtmf_row = 1;
            break;
        }
        /* ¿Será fila 2? */
        levelAux = tonesAmplitud[1] / (tonesAmplitud[0] + tonesAmplitud[2] +tonesAmplitud[3]);
        if (levelAux > DTMF_CH_SNR_RATE)
        {
            dtmf_row = 2;
            break;
        }
        /* ¿Será fila 3? */
        levelAux = tonesAmplitud[2] / (tonesAmplitud[1] + tonesAmplitud[0] +tonesAmplitud[3]);
        if (levelAux > DTMF_CH_SNR_RATE)
        {
            dtmf_row = 3;
            break;
        }
        /* ¿Será fila 4? */
        levelAux = tonesAmplitud[3] / (tonesAmplitud[1] + tonesAmplitud[2] +tonesAmplitud[0]);
        if (levelAux > DTMF_CH_SNR_RATE)
        {
            dtmf_row = 4;
            break;
        }

    } while(0);

    /*-----------------------------------------------------------------------*/
    /* Detección de canal alto */
    do
    {
        /* ¿Será columna 1? */
        levelAux = tonesAmplitud[4] / (tonesAmplitud[5] + tonesAmplitud[6]);
        if (levelAux > DTMF_CH_SNR_RATE)
        {
            dtmf_col = 1;
            break;
        }
        /* ¿Será columna 2? */
        levelAux = tonesAmplitud[5] / (tonesAmplitud[4] + tonesAmplitud[6]);
        if (levelAux > DTMF_CH_SNR_RATE)
        {
            dtmf_col = 2;
            break;
        }
        /* ¿Será columna 3? */
        levelAux = tonesAmplitud[6] / (tonesAmplitud[4] + tonesAmplitud[5]);
        if (levelAux > DTMF_CH_SNR_RATE)
        {
            dtmf_col = 3;
            break;
        }

    } while(0);

    /*-----------------------------------------------------------------------*/
    /* Decodificación de número de símbolo */
    if ( ( dtmf_row >= 1 ) && (dtmf_col >= 1) )
        dtmfSymbol = dtmf_col + 3*(dtmf_row - 1);
    else
        dtmfSymbol = 0;

    /*-----------------------------------------------------------------------*/
    /* Actualización de LEDs */
    if ( dtmfSymbol == 1) // Símbolo: 1
    {
                    DLU_writeLedD4(LED_OFF);
                    DLU_writeLedD5(LED_OFF);
                    DLU_writeLedD6(LED_OFF);
                    DLU_writeLedD7(LED_ON);
    }
    else if ( dtmfSymbol == 2) // Símbolo: 2
    {
                    DLU_writeLedD4(LED_OFF);
                    DLU_writeLedD5(LED_OFF);
                    DLU_writeLedD6(LED_ON);
                    DLU_writeLedD7(LED_OFF);
    }
    else if ( dtmfSymbol == 3) // Símbolo: 3
    {
                    DLU_writeLedD4(LED_OFF);
                    DLU_writeLedD5(LED_OFF);
                    DLU_writeLedD6(LED_ON);
                    DLU_writeLedD7(LED_ON);
    }
    else if ( dtmfSymbol == 4) // Símbolo: 4
    {
                    DLU_writeLedD4(LED_OFF);
                    DLU_writeLedD5(LED_ON);
                    DLU_writeLedD6(LED_OFF);
                    DLU_writeLedD7(LED_OFF);
    }
    else if ( dtmfSymbol == 5) // Símbolo: 5
    {
                    DLU_writeLedD4(LED_OFF);
                    DLU_writeLedD5(LED_ON);
                    DLU_writeLedD6(LED_OFF);
                    DLU_writeLedD7(LED_ON);
    }
    else if ( dtmfSymbol == 6) // Símbolo: 6
    {
                    DLU_writeLedD4(LED_OFF);
                    DLU_writeLedD5(LED_ON);
                    DLU_writeLedD6(LED_ON);
                    DLU_writeLedD7(LED_OFF);
    }
    else if ( dtmfSymbol == 7) // Símbolo: 7
    {
                    DLU_writeLedD4(LED_OFF);
                    DLU_writeLedD5(LED_ON);
                    DLU_writeLedD6(LED_ON);
                    DLU_writeLedD7(LED_ON);
    }
    else if ( dtmfSymbol == 8) // Símbolo: 8
    {
                    DLU_writeLedD4(LED_ON);
                    DLU_writeLedD5(LED_OFF);
                    DLU_writeLedD6(LED_OFF);
                    DLU_writeLedD7(LED_OFF);
    }
    else if ( dtmfSymbol == 9) // Símbolo: 9
    {
                    DLU_writeLedD4(LED_ON);
                    DLU_writeLedD5(LED_OFF);
                    DLU_writeLedD6(LED_OFF);
                    DLU_writeLedD7(LED_ON);
    }
    else if ( dtmfSymbol == 10) // Símbolo: *
    {
                    DLU_writeLedD4(LED_ON);
                    DLU_writeLedD5(LED_OFF);
                    DLU_writeLedD6(LED_ON);
                    DLU_writeLedD7(LED_OFF);
    }
    else if ( dtmfSymbol == 11) // Símbolo: 0
    {
                    DLU_writeLedD4(LED_ON);
                    DLU_writeLedD5(LED_OFF);
                    DLU_writeLedD6(LED_ON);
                    DLU_writeLedD7(LED_ON);
    }
    else if ( dtmfSymbol == 12) // Símbolo: #
    {
                    DLU_writeLedD4(LED_ON);
                    DLU_writeLedD5(LED_ON);
                    DLU_writeLedD6(LED_OFF);
                    DLU_writeLedD7(LED_OFF);
    }
    else
    {
                    DLU_writeLedD4(LED_ON);
                    DLU_writeLedD5(LED_ON);
                    DLU_writeLedD6(LED_ON);
                    DLU_writeLedD7(LED_ON);
    }
}

/******************************************************************************
**      END OF SOURCE FILE
******************************************************************************/
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
