#ifndef DLU_GLOBAL_DEFS_H
#define DLU_GLOBAL_DEFS_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
/* BUFFER PARA GRAFICAR */
/*---------------------------------------------------------------------------*/
/*
 * Parámetros de buffer de Graficas
 */
#define GRAPH_BUFF_SIZE     (1000)

/*
 * Tipo de dato a graficar en Buffer1
 */
typedef float GraphTypeBuff1_t;

/*
 * Tipo de dato a graficar en Buffer 2
 */
typedef float GraphTypeBuff2_t;

/*---------------------------------------------------------------------------*/
/* CONSTANTES PARA FUNCIONES DE LOS PULSADORES USER1 Y USER2 DE LA LDCK */
/*---------------------------------------------------------------------------*/
#define PB_TRIM_COUNTER_DELAY_PERIOD    (1000)  // (1000) para 1s
#define PB_TRIM_COUNTER_AUTOINC_PERIOD  (5)     // (5)    para 5ms (el mínimo es 1)
#define PB_TRIM_COUNTER_MIN             (440)   // Mínimo valor del contador de Trimmer
#define PB_TRIM_COUNTER_MAX             (880)   // Máximo valor del contador de Trimmer

/*---------------------------------------------------------------------------*/
/* EXPERIENCIA 1 */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* EXPERIENCIA 2 */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* EXPERIENCIA 3 */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* EXPERIENCIA 4 */
/*---------------------------------------------------------------------------*/
#define FFT_NPOINTS 1024

/*---------------------------------------------------------------------------*/
/* EXPERIENCIA 5 */
/*---------------------------------------------------------------------------*/
#define FRAME_SIZE  (80)
#define LPC_ORDER   (15)

/*---------------------------------------------------------------------------*/
/* CONFIGURANDO TAMAÑO PARA BUFFERS PING-PONG AL USAR DMA
 * SE USA EN EXPERIENCIAS 4 Y 5. TÍPICAMENTE SERÁ
 * EXP4: DMA_BUFFSIZE <-- FFT_NPOINTS
 * EXP5: DMA_BUFFSIZE <-- FRAME_SIZE */
/*---------------------------------------------------------------------------*/
#define DMA_BUFFSIZE   FFT_NPOINTS

/*****************************************************************************/
#ifdef __cplusplus
}
#endif
#endif
