#ifndef DLU_CODEC_CONFIG_H
#define DLU_CODEC_CONFIG_H

#include "L138_LCDK_aic3106_init.h"

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************/
/* Parémetros de configuración del CODEC */

/*
 * Frecuencia de muestreo del CODEC:
 * FS_9600_HZ
 * FS_8000_HZ
 * FS_11025_HZ
 * FS_12000_HZ
 * FS_16000_HZ
 * FS_19200_HZ
 * FS_22050_HZ
 * FS_24000_HZ
 * FS_32000_HZ
 * FS_44100_HZ
 * FS_48000_HZ
 */
#define CODEC_FS            FS_16000_HZ

/*
 * Selección de entrada de audio para el CODEC:
 * LCDK_LINE_INPUT  : Entrada estereo
 * LCDK_MIC_INPUT   : Entrada estereo con polarización de Microfonos
 */
#define CODEC_INPUT_CFG     LCDK_MIC_INPUT

/*
 * Ganancia de entrada (PGA) al CODEC:
 * ADC_GAIN_0DB
 * ADC_GAIN_3DB
 * ADC_GAIN_6DB
 * ADC_GAIN_9DB
 * ADC_GAIN_12DB
 * ADC_GAIN_15DB
 * ADC_GAIN_18DB
 * ADC_GAIN_21DB
 * ADC_GAIN_24DB
 * ADC_GAIN_27DB
 * ADC_GAIN_30DB
 * ADC_GAIN_33DB
 * ADC_GAIN_36DB
 * ADC_GAIN_39DB
 * ADC_GAIN_42DB
 * ADC_GAIN_45DB
 * ADC_GAIN_48DB
 * ADC_GAIN_51DB
 */
#define CODEC_ADC_GAIN      ADC_GAIN_18DB

/*
 * Atenuación de salida del CODEC:
 * DAC_ATTEN_0DB
 * DAC_ATTEN_3DB
 * DAC_ATTEN_6DB
 * DAC_ATTEN_9DB
 * DAC_ATTEN_12DB
 * DAC_ATTEN_15DB
 * DAC_ATTEN_18DB
 * DAC_ATTEN_21DB
 * DAC_ATTEN_24DB
 */
#define CODEC_DAC_ATTEN     DAC_ATTEN_0DB

/*---------------------------------------------------------------------------*/
/* Define perido de muestreo de audio TS en segundos según CODEC_FS */
#if     CODEC_FS == FS_8000_HZ
#define TS              1.25e-4
#elif   CODEC_FS == FS_9600_HZ
#define TS              1.04166667e-4
#elif   CODEC_FS == FS_11025_HZ
#define TS              9.07029478e-5
#elif   CODEC_FS == FS_12000_HZ
#define TS              8.33333333e-5
#elif   CODEC_FS == FS_16000_HZ
#define TS              6.25e-5
#elif   CODEC_FS == FS_19200_HZ
#define TS              5.20833333e-5
#elif   CODEC_FS == FS_22050_HZ
#define TS              4.53514739e-5
#elif   CODEC_FS == FS_24000_HZ
#define TS              4.16666667e-5
#elif   CODEC_FS == FS_32000_HZ
#define TS              3.125e-5
#elif   CODEC_FS == FS_44100_HZ
#define TS              2.26757370e-5
#elif   CODEC_FS == FS_48000_HZ
#define TS              2.08333333e-5
#endif
/*****************************************************************************/
#ifdef __cplusplus
}
#endif
#endif
