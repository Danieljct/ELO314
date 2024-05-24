/***************************************************************************//**
* \file     Funciones que deben impementar los alumnos
*
* \brief    
*
* \authors  Gonzalo Carrasco
*******************************************************************************/

/******************************************************************************
**      HEADER FILES
******************************************************************************/
#include <goertzel.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/******************************************************************************
**      MODULE PREPROCESSOR CONSTANTS
******************************************************************************/


/******************************************************************************
**      MODULE MACROS
******************************************************************************/
#define SAMPLING_FREQ			(16000)

#define GOERTZEL_N				(256)
/*---------------------------------------------------------------------------*/

/*
 * Bins arbitrarios.
 */
 
#define GOERTZEL1_K_BIN	    	(8)			
#define GOERTZEL2_K_BIN	    	(9)
#define GOERTZEL3_K_BIN	    	(10)
#define GOERTZEL4_K_BIN	    	(11)
#define GOERTZEL5_K_BIN	    	(12)
#define GOERTZEL6_K_BIN	    	(13)
#define GOERTZEL7_K_BIN	    	(14)

/*
 * bins para DTMF con N=256 @16 ksps
 */



/*
 * Constante
 */
#ifndef M_PI
#define M_PI					(3.141592653589793)
#endif

/******************************************************************************
**      MODULE DATATYPES
******************************************************************************/

/*
 * Estructura de estado de filtros goertzel
 */
typedef struct goertzelStateTag {
	uint64_t samplesCounter;
    double cosW;
	double sinW;
	double A1;
    double outputs[3];
	double binReal;
	double binImag;
	double binMag;
} goertzelState_t;

/******************************************************************************
**      MODULE VARIABLE DEFINITIONS
******************************************************************************/


/*---------------------------------------------------------------------------*/
/* VARABLES FILTROS GOERTZEL*/
/*---------------------------------------------------------------------------*/
goertzelState_t gGoertzelState1 = {0};


/******************************************************************************
**      PRIVATE FUNCTION DECLARATIONS (PROTOTYPES)
******************************************************************************/

static void initGoertzel(goertzelState_t *state, uint64_t kFrequency);
static void resetGoertzel(goertzelState_t *state);
static double computeGoertzel(goertzelState_t *state, double filterInput);

/******************************************************************************
**      FUNCTION DEFINITIONS
******************************************************************************/


/***************************************************************************//**
*   \brief Función prncipal llamada por la sFunction
*
*   \param  input : una entrada de señal desde simulink
*
*   \param  output: salidas hacia simulink
*
*   \return Void.
*******************************************************************************/
void goertzelFunction(double input1,
				double *output1,
				double *output2,
				double *output3,
				double *output4,
				double *output5,
				double *output6,
				double *output7
				)
{   
    // Bypass
	*output1 = input1;
	*output2 = input1;
	*output3 = input1;
	*output4 = input1;
	*output5 = input1;
	*output6 = input1;
	*output7 = input1;
}

/******************************************************************************
*   \brief  Esta función se ejecuta una vez cuando se corre la simulación
*           de Simulink. Se usa para inicializar variables y estructuras.
*
*   \return  void
******************************************************************************/
extern void initialization(void)
{	
	initGoertzel( &gGoertzelState1 , GOERTZEL1_K_BIN );

	
	return;
}

/******************************************************************************
*   \brief  Esta función inicializa un filtro goertzel
*
*   \param state                : puntero a la estructura del filtro a ejecutar
*   \param kFrequency           : bin de frecuencia a calcular
*
*   \return filterOutput        : señal de salida del filtro goertzel ejecutado
******************************************************************************/
static void initGoertzel(goertzelState_t *state, uint64_t kFrequency)
{
	// TO-DO
	// Inicializar las variables y parámetros de la estructura de estado
	// en función de las constantes M_PI, GOERTZEL_N  y el argumento kFrequency.
	state->samplesCounter 	= 0;
	// state->
    //...
	
	return;
}

/******************************************************************************
*   \brief  Esta función pone en cero los estados de un filtro goertzel
*
*   \param state                : puntero a la estructura del filtro a ejecutar
*
******************************************************************************/
static void resetGoertzel(goertzelState_t *state)
{
	state->samplesCounter 	= 0;
    state->outputs[0] 		= 0.0;
	state->outputs[1] 		= 0.0;
	state->outputs[2] 		= 0.0;
	// Se quere retener el último resultado para el bin sin llevarlos a cero
	
	return;
}

/******************************************************************************
*   \brief  Esta función implementa el filtro goertzel
*
*   \param state               : puntero a la estructura del filtro a ejecutar
*   \param filterInput          : señal de entrada al filtro biquad a ejecutar
*
*   \return filterOutput        : señal de salida del filtro goertzel ejecutado
******************************************************************************/
static double computeGoertzel(goertzelState_t *state, double filterInput)
{
	// TO-DO
	// Completar todo el algoritmo de Goertzel  usando la estructura de estado,
	// filterInput y GOERTZEL_N
	// Existe la función sqrt() provista por el <math.h>
	
	
	
	
	return state->binMag;						
}

/******************************************************************************
**      END OF SOURCE FILE
******************************************************************************/
