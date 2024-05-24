/******************************************************************************
* \file     dsp_lab_utils.h  Version 1.2
*
* \brief    Este archivo contiene funciones de prop�sito general para
*           simplificar tareas para varias experiencias.
*
* \authors  Gonzalo Carrasco
******************************************************************************/
#ifndef DSP_LAB_UTILS_H
#define DSP_LAB_UTILS_H
/*****************************************************************************/

/******************************************************************************
**      INCLUDES
******************************************************************************/

/******************************************************************************
**      PREPROCESSOR CONSTANTS
******************************************************************************/

/******************************************************************************
**      CONFIGURATION CONSTANTS
******************************************************************************/

/*---------------------------------------------------------------------------*/
/* GPIOS DE LEDS EN LCDK */
/*---------------------------------------------------------------------------*/
/* Par�metros 'value' para funciones
 * DLU_writeLedD4(),
 * DLU_writeLedD5(),
 * DLU_writeLedD6() y
 * DLU_writeLedD7()
 */
#define LED_ON                                  (1)
#define LED_OFF                                 (0)

/* Par�metros 'led' para funci�n
 * DLU_toggleLedD
 */
#define LED_D4                                  (4)
#define LED_D5                                  (5)
#define LED_D6                                  (6)
#define LED_D7                                  (7)

/******************************************************************************
**      MACRO DEFINITIONS
******************************************************************************/

/*---------------------------------------------------------------------------*/
/* CODEC */
/*---------------------------------------------------------------------------*/
/*
 * Ancho de palabra fijo del CODEC para la librer�a L138_LCDK_aic3106
 */
#define CODEC_WORD_LENGTH   (16)

/* Macro Typecasting CODEC a flotante */
#define AIC_2_FLT(x)    ( (float)(x)  * ( 1.0 / ( (1 << (CODEC_WORD_LENGTH-1)) - 1.0) ) );
#define FLT_2_AIC(x)    ( (int16_t)(x * ( (1 << (CODEC_WORD_LENGTH-1)) - 1) ) );

/******************************************************************************
**      PUBLIC DATATYPES
******************************************************************************/
/*
 * Definci�n del tipo para funciones de PushBottons. Debe ser alg�n tipo
 * entero sin signo: uint8_t, uint16_t, uint32_t, uint64_t
 */
typedef uint8_t     PB_INT_TYPE;

/******************************************************************************
**      PUBLIC (GLOBAL) VARIABLES DECLARATIONS
******************************************************************************/

/*---------------------------------------------------------------------------*/
/* BUFFER PARA GRAFICAR */
/*---------------------------------------------------------------------------*/

/* Buffer para gr�ficar 1 */
extern GraphTypeBuff1_t gGraphBuff1[DLU_GRAPH_BUFF_SIZE];

/* Buffer para gr�ficar 2 */
extern GraphTypeBuff2_t gGraphBuff2[DLU_GRAPH_BUFF_SIZE];


/*---------------------------------------------------------------------------*/
/* TIC-TOC */
/*---------------------------------------------------------------------------*/
/*
 * Variable para medici�n de tiempos de ejecuci�n.
 */
extern unsigned long long DLU_timeTicToc;

/******************************************************************************
**      PUBLIC FUNCTION PROTOTYPES
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************/

/******************************************************************************
*   \brief  Declaraci�n de puntero a tabla de vectores de interrupci�n
*           definido en vectors.asm
******************************************************************************/
extern void vectors(void);

/******************************************************************************
*   \brief  Abstracci�n de la escritura al codec y casteo a entero de ambos
*           canales con datos en tipo flotantes.
*
*   \param  float_out_l : salida izquierda normalizada [-1..1]
*   \param  float_out_r : salida derecha normalizada [-1..1]
*
*   \return Void.
******************************************************************************/
extern int DLU_writeCodecOutputs(float float_out_l, float float_out_r);

/******************************************************************************
*   \brief  Abstracci�n de la lectura de ambos canales del codec y
*           casteo a flotante.
*
*   \param  *float_in_l : puntero a destino de entrada izquierda normalizada
*   \param  *float_in_r : puntero a destino de entrada derecha normalizada
*
*   \return Void.
******************************************************************************/
extern void DLU_readCodecInputs(float *float_in_l, float *float_in_r);

/******************************************************************************
*   \brief  Establece estado del LED D4
*
*   \param value    : LED_OFF - Apagado
*                   : LED_ON - Encendido
*
*   \return Void.
******************************************************************************/
extern void DLU_writeLedD4(PB_INT_TYPE value);

/******************************************************************************
*   \brief  Establece estado del LED D5
*
*   \param value    : LED_OFF - Apagado
*                   : LED_ON - Encendido
*
*   \return Void.
******************************************************************************/
extern void DLU_writeLedD5(PB_INT_TYPE value);

/******************************************************************************
*   \brief  Establece estado del LED D6
*
*   \param value    : LED_OFF - Apagado
*                   : LED_ON - Encendido
*
*   \return Void.
******************************************************************************/
extern void DLU_writeLedD6(PB_INT_TYPE value);

/******************************************************************************
*   \brief  Establece estado del LED D7
*
*   \param value    : LED_OFF - Apagado
*                   : LED_ON - Encendido
*
*   \return Void.
******************************************************************************/
extern void DLU_writeLedD7(PB_INT_TYPE value);

/******************************************************************************
*   \brief  Cambia el estado actual del LED indicado a su complemento
*
*   \param led      : LED_D4 - Cambia led D4
*                   : LED_D5 - Cambia led D5
*                   : LED_D6 - Cambia led D6
*                   : LED_D7 - Cambia led D7
*
*   \return Void.
******************************************************************************/
extern void DLU_toggleLedD(uint32_t led);

/******************************************************************************
*   \brief  Habilita el uso de los leds, D4, D5, D6 y D7.
*
*           Configura El pinmux y la direcci�n de los GPIOs correspondientes
*           a los LEDs D4, D5, D6 y D7 como salidas.
*
*   \param Void.
*
*   \return Void.
******************************************************************************/
extern void DLU_initLeds(void);

/******************************************************************************
*   \brief  Habilita el uso de los pulsadores User1 y User2.
*
*           Configura El pinmux y la direcci�n de los GPIOs correspondientes
*           a los pulsadores S2 (USER 1) y S3 (USER 2) como entradas.
*
*   \param Void.
*
*   \return Void.
******************************************************************************/
extern void DLU_initPushButtons(void);

/******************************************************************************
*   \brief  Lee el estado del push button User1
*
*   \param Void.
*
*   \return Es 0 si el push button no est� presionado.
*           Es 1 si el push button est� presionado.
******************************************************************************/
extern PB_INT_TYPE DLU_readPushButton1(void);

/******************************************************************************
*   \brief  Lee el estado del push button User2
*
*   \param Void.
*
*   \return Es 0 si el push button no est� presionado.
*           Es 1 si el push button est� presionado.
******************************************************************************/
extern PB_INT_TYPE DLU_readPushButton2(void);

/******************************************************************************
*   \brief  Lee el estado binario que cambia (toggle) cada vez que se presiona
*           el push button User1.
*
*   \param Void.
*
*   \return Puede ser 0 o 1, pues cambia y se retiene despu�s de que se
*           presiona el push button User1.
******************************************************************************/
extern PB_INT_TYPE DLU_readToggleStatePB1(void);

/******************************************************************************
*   \brief  Lee el estado binario que cambia (toggle) cada vez que se presiona
*           el push button User2.
*
*   \param Void.
*
*   \return Puede ser 0 o 1, pues cambia y se retiene despu�s de que se
*           presiona el push button User2.
******************************************************************************/
extern PB_INT_TYPE DLU_readToggleStatePB2(void);

/******************************************************************************
*   \brief  Lee el estado binario que cambia (toggle) cada vez que se presiona
*           simult�neamente los push button User1 y User2
*
*   \param Void.
*
*   \return Puede ser 0 o 1, pues cambia y se retiene despu�s de que se
*           presiona simult�neamente los push button User1 y User2
******************************************************************************/
extern PB_INT_TYPE DLU_readToggleStatePB12(void);

/******************************************************************************
*   \brief  Lee el valor del contador (Trimmer) ajustable por los pulsadores.
*           Es una variable adecuada para hacer ajustes de par�metros en
*           tiempo de ejecuci�n (tiempo real).
*           User2: incrementa el valor
*           User1: decrementa el valor
*           Cualquiera que se mantenga presionado por m�s del perido
*           'delayPeriod_ms', causar� un incremento o decremento autom�tico
*           r�pido. Al auto incremento o auto decremento ser� m�s r�pido
*           para valores de 'autoincrementPeriod_ms' mas bajos, y vice versa.
*
*   \param Void.
*
*   \return Puede ser 0 o 1, pues cambia y se retiene despu�s de que se
*           presiona simult�neamente los push button User1 y User2
******************************************************************************/
extern int32_t DLU_readTrimmerCounter(void);

/******************************************************************************
*   \brief  Configura los l�mites del contador Trimmer
*
*   \param minimumCount: valor m�nimo que tomar� el Trimmer
*   \param maximumCount: valor m�ximo que tomar� el Trimmer
*
*   \return 0 si se configur� correctamente los l�mites
*           1 si no se configur� los l�mites debido al error
*               minimumCount > maximumCount
******************************************************************************/
extern uint32_t DLU_configTrimmerCounter(uint32_t minimumCount, uint32_t maximumCount);

/******************************************************************************
*   \brief  Configura los tiempos de auto incremento
*
*   \param delayPeriod_ms: tiempo de espera en ms antes de autoincrementar
*                           (m�nimo: 200)
*   \param autoincrementPeriod_ms: periodo en ms que toma incrementar o
*           decrementar en 1 el Trimmer (m�nimo: 1)
*
*   \return 0
******************************************************************************/
extern uint32_t DLU_configTrimmerAutoIncrement(int32_t delayPeriod_ms, int32_t autoincrementPeriod_ms );

/******************************************************************************
*   \brief  Habilita el uso de funciones DLU_tic y DLU_toc.
*
*           Funci�n que inicializa el Time Stamp Counter de la CPU
*
*   \param Void.
*
*   \return Void.
******************************************************************************/
extern void DLU_initTicToc(void);

/******************************************************************************
*   \brief Funci�n que registra time-stamp inicial
*
*   \param *tsc_samp :   es el puntero a un entero sin signo de 64
*                       bits donde de registrar� el tiempo inicial (con TIC) y
*                       tiempo final (con TOC).
*
*   \return Void.
******************************************************************************/
extern void DLU_tic(void);

/******************************************************************************
*   \brief  Funci�n que captura el time-stamp actual y calcula la diferencia
*           con el valor inicial registrado con TIC, dejando la diferencia en
*           cuentas entre TIC y TOC en la variable apuntada por el par�metro de
*           esta funci�n.
*
*   \param *tsc_samp : es el puntero a un entero sin signo de 64
*                       bits donde se registrar� la diferencia en cuentas entre
*                       TIC y TOC. Las cuentas ocurren a la frecuencia del CPU.
*
*   \return Void.
******************************************************************************/
extern void DLU_toc(void);

/******************************************************************************
*   \brief  Funci�n que llena gGraphBuff1 con valores de variable entregada
*           luego de levantar la se�al gatillo 'gSingleGraphBuff1'
*
*   \param variable : Valor a ingresar al buffer de gr�fico 1
*
*   \return Void.
******************************************************************************/
extern void DLU_appendGraphBuff1(GraphTypeBuff1_t variable);

/******************************************************************************
*   \brief  Funci�n que llena gGraphBuff2 con valores de variable entregada
*           luego de levantar la se�al gatillo 'gSingleGraphBuff2'
*
*   \param variable : Valor a ingresar al buffer de gr�fico 2
*
*   \return Void.
******************************************************************************/
extern void DLU_appendGraphBuff2(GraphTypeBuff2_t variable);

/******************************************************************************
*   \brief  Funci�n que inicializa gGraphBuff1 y gGraphBuff2 en cero.
*
*   \return Void.
******************************************************************************/
extern void DLU_clearAllGraphBuffers(void);

/******************************************************************************
*   \brief  Funci�n que levanta gatillos de todos los gr�ficos con subir el
*           gatillo general 'gSingleSyncGraph'
*
*   \return Void.
******************************************************************************/
extern void DLU_enableSynchronicSingleCaptureOnAllGraphBuff(void);

/******************************************************************************
*   \brief  Funci�n que inicia la captura simple de datos en el 'GraphBuff1'.
*           Cada vez que se llame la funci�n 'DLU_appendGraphBuff1(data)' el dato
*           ser� puesto en el 'GraphBuff1' hasta completar 'DLU_GRAPH_BUFF_SIZE'
*           llamados y escritura de datos.
*
*   \return Void.
******************************************************************************/
extern void DLU_enableSingleCaptureOnGraphBuff1(void);

/******************************************************************************
*   \brief  Funci�n que inicia la captura simple de datos en el 'GraphBuff2'.
*           Cada vez que se llame la funci�n 'DLU_appendGraphBuff2(data)' el dato
*           ser� puesto en el 'GraphBuff2' hasta completar 'DLU_GRAPH_BUFF_SIZE'
*           llamados y escritura de datos.
*
*   \return Void.
******************************************************************************/
extern void DLU_enableSingleCaptureOnGraphBuff2(void);

/******************************************************************************
*   \brief  Funci�n que usa prand() de la librer�a L138_LCDK_aic3106_int"
*           para generar una se�al pseudo aleatoria con rango int16_t.
*
*   \return Void.
******************************************************************************/
int16_t DLU_rand(void);

/******************************************************************************
*   \brief  Funci�n que usa prand() de la librer�a L138_LCDK_aic3106_int"
*           para generar una se�al pseudo aleatoria con resoluci�n de 16 bit
*           pero de tipo float.
*
*   \return Void.
******************************************************************************/
float DLU_fRand(void);



/*****************************************************************************/
#ifdef __cplusplus
}
#endif
#endif
/******************************************************************************
**      END OF HEADER FILE
******************************************************************************/
