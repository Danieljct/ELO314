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
#define damp 0.05   //ancho del pasabanda
#define fmax 3000
#define fmin 100
#define Fw 1000     //frecuencia del wah
#define periodo (int)((fmax-fmin)/(Fw/2.0)*CODEC_FS)
#define Q1 2*damp
#define PI 3.1415926535897932384626433
/******************************************************************************
**      MODULE MACROS
******************************************************************************/

/******************************************************************************
**      MODULE DATATYPES
******************************************************************************/


/******************************************************************************
**      MODULE VARIABLE DEFINITIONS
******************************************************************************/
int indice = 0;
uint8_t indice2 = 0;
float salidatriangular = 0;
float F1;
float yh[2]={0}, yb[2]={0}, yl[2]={0};

float tri;
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


/******************************************************************************
**      PRIVATE FUNCTION DECLARATIONS (PROTOTYPES)
******************************************************************************/
float triangular(int x);
void parametros(float data);

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

        parametros(floatCodecInputL);
        indice = (indice + 1) % 92800;  //va periodo pero se cae

        /* Medición de tiempo de ejecución */
        DLU_toc();

        /*-------------------------------------------------------------------*/
        /* PARA VISUALIZAR EN GRÁFICO */
        /*-------------------------------------------------------------------*/
        DLU_enableSynchronicSingleCaptureOnAllGraphBuff();
        DLU_appendGraphBuff1(indice);
        DLU_appendGraphBuff2(floatCodecOutputL);

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
        //floatCodecOutputR = floatCodecInputR;

        //TRIANG
        //floatCodecOutputL = salidatriangular;
        //floatCodecOutputR = floatCodecInputL;

        // wah wah
        floatCodecOutputL = yb[indice2];
        floatCodecOutputR = yb[indice2];

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

    //DATOS INICIALES:
    F1 = 2*sin((PI*fmin)/CODEC_FS);

    /* Inicializacion BSL y AIC31 Codec */
    L138_initialise_intr(CODEC_FS, CODEC_ADC_GAIN, CODEC_DAC_ATTEN, CODEC_INPUT_CFG);
    /* Inicialización de LEDs */
    DLU_initLeds();

    /*-----------------------------------------------------------------------*/
    /* Inicialización de buffers a cero */

   /* Loop infinito a espera de interrupción del Codec */
    while(1);
}


/******************************************************************************
**      PRIVATE FUNCTION DEFINITIONS
******************************************************************************/

float triangular(int x){
    if((x) < (periodo/2))
        return Fw * (float)(x)/CODEC_FS + fmin;
    else
        return -Fw*((float)x - periodo/2)/CODEC_FS + fmax;
}

void parametros(float data){
    yh[(indice2+1)%2] = data - yl[indice2] - Q1*yb[indice2];
    yb[(indice2+1)%2] = F1*yh[(indice2+1)%2] + yb[indice2];
    yl[(indice2+1)%2] = F1*yb[(indice2+1)%2] + yl[indice2];

    tri = triangular(indice);
    F1 = 2*sin((PI*tri)/(float)CODEC_FS);
    indice2 = (indice2 + 1) % 2;
}


/******************************************************************************
**      END OF SOURCE FILE
******************************************************************************/


