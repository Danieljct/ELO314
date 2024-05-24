/******************************************************************************
* \file     dsp_lab_utils.c  Version 1.2
*
* \brief    Este archivo contiene funciones de propósito general para simplificar
*           tareas para varias experiencias.
*
* \authors  Gonzalo Carrasco
******************************************************************************/

/******************************************************************************
**      HEADER FILES
******************************************************************************/
#include <dlu_global_defs.h>
#include <L138_LCDK_aic3106_init.h>  // Es mandatorio contar con las definiciones del la librería L138_LCDK_aic3106_init
#include "dsp_lab_utils.h"      // Encabezado de lab_dsp_utils.c
#include <c6x.h>
#include <string.h>


/******************************************************************************
**      MODULE PREPROCESSOR CONSTANTS
******************************************************************************/

/******************************************************************************
**      MODULE MACROS
******************************************************************************/

/*---------------------------------------------------------------------------*/
/* TIC-TOC */
/*---------------------------------------------------------------------------*/
#define TICTOC_TSC_NIT     (TSCL = 0;)
#define TIC_TSC(cu,cl) cl = TSCL; cu = TSCH;
#define TOC_TSC(cu,cl) cl = TSCL; cu = TSCH;

/*---------------------------------------------------------------------------*/
/* GPIOS DE LEDS EN LCDK */
/*---------------------------------------------------------------------------*/
// Configuración del Pinmux para usar DSP_BSL en la LCDK
// LED D4 -> GPIO6[13]
// LED D5 -> GPIO6[12]
#define PINMUX_LCDK_LEDS_D4_D5_GPIO_OE_REG      (13)
#define PINMUX_LCDK_LEDS_D4_D5_GPIO_OE_MASK     (0x0000FF00)
#define PINMUX_LCDK_LEDS_D4_D5_GPIO_OE_VAL      (0x00008800)

// LED D6 -> GPIO2[12]
#define PINMUX_LCDK_LEDS_D6_GPIO_OE_REG         (5)
#define PINMUX_LCDK_LEDS_D6_GPIO_OE_MASK        (0x0000F000)
#define PINMUX_LCDK_LEDS_D6_GPIO_OE_VAL         (0x00008000)

// LED D7 -> GPIO0[9]
#define PINMUX_LCDK_LEDS_D7_GPIO_OE_REG         (0)
#define PINMUX_LCDK_LEDS_D7_GPIO_OE_MASK        (0x0F000000)
#define PINMUX_LCDK_LEDS_D7_GPIO_OE_VAL         (0x08000000)

/*---------------------------------------------------------------------------*/
/* GPIOS DE PULSADORES USER_1 Y USER_2 EN LCDK */
/*---------------------------------------------------------------------------*/
// Configuración del Pinmux para usar DSP_BSL en la LCDK
// USER 1, S2 -> GPIO2[4]
// USER 2, S3 -> GPIO2[5]
#define PINMUX_LCDK_USER_1_USER_2_GPIO_IE_REG   (6)
#define PINMUX_LCDK_USER_1_USER_2_GPIO_IE_MASK  (0x0000FF00)
#define PINMUX_LCDK_USER_1_USER_2_GPIO_IE_VAL   (0x00008800)

#define PB_HW_PRESSED                   (0u)    // Por diseño de la LCDK
#define PB_HW_RELEASED                  (1u)    // Por diseño de la LCDK

#define PB_SW_PRESSED                   (1u)    // Por convención.
#define PB_SW_RELEASED                  (0u)    // Por convención.

#define PB_DEBOUNCE_MAXCOUNT            (10)

#define PB_STATES_STAT_P1BUFF_BIT       (0u)
#define PB_STATES_STAT_P1_PEDGE_BIT     (1u)
#define PB_STATES_STAT_P2BUFF_BIT       (2u)
#define PB_STATES_STAT_P2_PEDGE_BIT     (3u)
#define PB_STATES_STAT_P12_PEDGE_BIT    (4u)
#define PB_STATES_STAT_AUTO_INCDEC_BIT  (14u)
#define PB_STATES_STAT_WAIT_DELAY_BIT   (15u)

/* Configuración Timer 2 para 'debouncing' de pushbottons
 * Notar que la librería 'L138_LCDK_aic3106_init' arranca el nucleo
 * a 300MHz. Los Timers reciven 150MHz.
 */

/*
 * Dirección base de registros del Timer2 (Timer0 y 1, definidos en
 * 'evmomapl138.h')
 */
#define TIMER2_REG_BASE                 (0x01F0C000)

/*
 * Macro para usar 'TMR2' como puntero a Estructura de registros de
 * configuración
 */
#define TMR2                            ((timer_regs_t *)TIMER2_REG_BASE)

/*
 * Timer2 prescaler (solo válido entre [1..32] )
 */
#define TMR2_DIVISION                   (10)

/*
 * Timer2, periodo de cuenta máxima (cercano a 995Hz de sampling)
 */
#define TMR2_PRD34                      (15075)

/*
 * Puntero a multiplexon de interrupciones
 */
#define INTC_INTMUX2                    *( unsigned int* )( 0x01800108 )

/*---------------------------------------------------------------------------*/
/* GPIOS DE PULSADORES USER_1 Y USER_2 EN LCDK */
/*---------------------------------------------------------------------------*/
/*
 * Asignación de bits de estado PbToggle
 */
#define PB_TOGGLES_T1_BIT                       (0u)
#define PB_TOGGLES_T2_BIT                       (1u)
#define PB_TOGGLES_T12_BIT                      (2u)

/* Macro para obtener valor de un bit en un registro */
#define DLU_GET_REG_BIT(reg,b_position)     ( ( reg & ( 1 << b_position ) ) >> b_position )

/*---------------------------------------------------------------------------*/
/* VALORES INICIALES PARA FUNCIONES DE LOS PULSADORES USER1 Y USER2 DE LA LDCK */
/*---------------------------------------------------------------------------*/
#define PB_TRIM_COUNTER_DELAY_PERIOD    (1000)  // (1000) para 1s
#define PB_TRIM_COUNTER_AUTOINC_PERIOD  (5)     // (5)    para 5ms (el mínimo es 1)
#define PB_TRIM_COUNTER_MIN             (1)   // Mínimo valor del contador de Trimmer
#define PB_TRIM_COUNTER_MAX             (1000)   // Máximo valor del contador de Trimmer


/******************************************************************************
**      MODULE DATATYPES
******************************************************************************/
/*
 * Internal states for every Input to debounce using dlu_debouncer
 */
typedef struct {
    int integral_selector;
    PB_INT_TYPE debounced;
} deb_stat;

/******************************************************************************
**      MODULE VARIABLE DEFINITIONS
******************************************************************************/

/*---------------------------------------------------------------------------*/
/* TIC-TOC */
/*---------------------------------------------------------------------------*/
extern unsigned long long DLU_timeTicToc = 0;
unsigned long long DLU_timeStampAux = 0;


/*---------------------------------------------------------------------------*/
/* PULSADORES */
/*---------------------------------------------------------------------------*/

/*
 * Estado actual del pulsador User1 (rebote filtrado)
 */
PB_INT_TYPE DLU_gPbUser1 = 0;

/*
 * Estado actual del pulsador User2 (rebote filtrado)
 */
PB_INT_TYPE DLU_gPbUser2 = 0;

/*
 * Variable de estados que cambian con los pulsadores User1 y User2
 * DLU_gPbToggle[PB_TOGGLES_T1_BIT] : es un bit que cambia con User1
 * DLU_gPbToggle[PB_TOGGLES_T2_BIT] : es un bit que cambia con User2
 * DLU_gPbToggle[PB_TOGGLES_T12_BIT] : es un bit que cambia al presionar
 *   simultáneamente User1 y User2.
 */
PB_INT_TYPE DLU_gPbToggle = 0;

/*
 * Estructura de configuraciones del contador de trimmer
 */
struct TrimmerConfig {
    int32_t  trimmerCounter;
    uint32_t minCount;
    uint32_t maxCount;
    int32_t  delayPeriod;
    int32_t  autoIncPeriod;
};

/* Valores iniciales del configuración del trimmer */
struct TrimmerConfig gTrimmer = {
        PB_TRIM_COUNTER_MIN,
        PB_TRIM_COUNTER_MIN,
        PB_TRIM_COUNTER_MAX,
        PB_TRIM_COUNTER_DELAY_PERIOD,
        PB_TRIM_COUNTER_AUTOINC_PERIOD
        };

deb_stat pb_user1_deb_stat = {0,0};
deb_stat pb_user2_deb_stat = {0,0};

uint32_t pb_states_stat = 0;
uint32_t pb_trim_counter_delay_count = 0;
uint32_t pb_trim_counter_autoincdec_count = 0;


/*---------------------------------------------------------------------------*/
/* BUFFER PARA GRAFICAR */
/*---------------------------------------------------------------------------*/

/* Buffer1, índice y variable gatillo */
#pragma DATA_SECTION(gGraphBuff1,".EXT_RAM")
extern GraphTypeBuff1_t gGraphBuff1[DLU_GRAPH_BUFF_SIZE] = {0};
static uint32_t gIdxGraphBuff1 = 0;
static uint32_t gSingleGraphBuff1 = 0;

/* Buffer2, índice y variable gatillo */
#pragma DATA_SECTION(gGraphBuff2,".EXT_RAM")
extern GraphTypeBuff2_t gGraphBuff2[DLU_GRAPH_BUFF_SIZE] = {0};
static uint32_t gIdxGraphBuff2 = 0;
static uint32_t gSingleGraphBuff2 = 0;

/******************************************************************************
**      PRIVATE FUNCTION DECLARATIONS (PROTOTYPES)
******************************************************************************/

extern uint32_t rand31_next(void);

/******************************************************************************
*   \brief  This function debounces the input signal. This function must
*           be called at a regular rate so as to create a sampled version
*           of the input and filtered here. The debounced output only changes
*           when 'integral_select' has accumulated more than DEBOUNCE_MAXCOUNT
*           number of readings of the same state.
*
*           Every signal to be debounce using this function, must have its
*           own structure of inner states with 'deb_sta' structure.
*
*   \param  input : integer representing the binary input to be debounced.
*   \param  *ds : pointer to the structure that stores the states for the
*                 processed signal.
*   \return debounced : binary output already debounced.
******************************************************************************/
static PB_INT_TYPE DLU_debouncer(deb_stat *ds, PB_INT_TYPE input);

/******************************************************************************
*   \brief  Lee el estado del pulsador USER 1
*
*   \param Void.
*
*   \return PB_INT_TYPE     1 : pulsador presionado.
*                       0 : pulsador liberado
******************************************************************************/
static PB_INT_TYPE DLU_getPbUser1NotDebounced(void);

/******************************************************************************
*   \brief  Lee el estado del pulsador USER 2
*
*   \param Void.
*
*   \return PB_INT_TYPE     1 : pulsador presionado.
*                       0 : pulsador liberado
******************************************************************************/
static PB_INT_TYPE DLU_getPbUser2NotDebounced(void);

/******************************************************************************
*   \brief  Función que inicializa el Timer2 del DSP para sus uso en leer
*           pulsadores User1 y User2. Utiliza interrupciones que también
*           son configuradas aquí.
*
*   \param Void.
*
*   \return Void.
******************************************************************************/
static void DLU_initPbTimer2(void);

/******************************************************************************
*   \brief  Retorna el estado del pulsador USER 1, con filtro antirebote.
*
*   \param Void.
*
*   \return PB_INT_TYPE     1 : pulsador presionado.
*                       0 : pulsador liberado
******************************************************************************/
static PB_INT_TYPE DLU_getPbUser1Debounced(void);

/******************************************************************************
*   \brief  Retorna el estado del pulsador USER 2, con filtro antirebote.
*
*   \param Void.
*
*   \return PB_INT_TYPE     1 : pulsador presionado.
*                       0 : pulsador liberado
******************************************************************************/
static PB_INT_TYPE DLU_getPbUser2Debounced(void);

/******************************************************************************
*   \brief  Función que interpreta la señal de dos pulsadores para generar
*           y modificar la varable de estado "pb_toggles".
*           "pb_toggles" cuenta con tres bits:
*           PB_TOGGLES_T1_BIT cambia entre 0 y 1 con presionar pb1
*           PB_TOGGLES_T2_BIT cambia entre 0 y 1 con presionar pb2
*           PB_TOGGLES_T12_BIT  cambia entre 0 y 1 con presionar pb1 y  pb2
*           a la vez.
*
*   \param  pb_toggles :    puntero a variable entera sin signo para registrar
*                           bits de estado.
*           pb1 :   señal del pulsador 1 (debe estár filtrada contra rebotes)
*           pb2 :   señal del pulsador 2 (debe estár filtrada contra rebotes)
*
*   \return Void
******************************************************************************/
static void DLU_updatePbToggle(PB_INT_TYPE *pb_toggles, PB_INT_TYPE pb1, PB_INT_TYPE pb2);

/******************************************************************************
*   \brief  Función que interpreta los estados "pb_states_stat" internos de la
*           función DLU_pb_toggle_update() para modificar el valor de cuenta en
*           "pb_trim_counter". Este valor entero se puede incrementar o
*           decrementar entre un rango máximo y mínimo usando dos pulsadores.
*
*           SE DEBE INVOCAR SIEMPRE DESPUÉS DE INVOCAR LA FUNCIÓN
*           DLU_pb_toggle_update().
*
*           Al presionar pb1, la cuenta en "pb_trim_counter" se decrementa en 1
*           Al presionar pb2, la cuenta en "pb_trim_counter" se inrementa en 1
*
*           AL presionar uno de los pulsador por más de
*           PB_TRIM_COUNTER_DELAY_PERIOD
*           llamados de función, se ingresa al modo autoincdec, que permite
*           incrementar o decrementar "pb_trim_counter" cada
*           PB_TRIM_COUNTER_AUTOINC_PERIOD llamados de función.
*
*   \param  pb_trim_counter :       puntero a variable entera con signo para
*                                   registrar la cuenta de ajuste.
*                                   de estado.
*           pb_trim_counter_min :   valor de saturación mínima de la cuenta
*           pb_trim_counter_max :   valor de saturación máxima de la cuenta
*
*           "pb_trim_counter_max" debe ser siempre mayor que
*           "pb_trim_counter_min", de caso contrario, "pb_trim_counter" tomará
*           siempre el valor "pb_trim_counter_min".
*
*   \return Void
******************************************************************************/
static void DLU_updatePbTrimCounter(int32_t *pb_trim_counter,  int32_t pb_trim_counter_delay_period, int32_t pb_trim_counter_autoinc_period, int32_t pb_trim_counter_min, int32_t pb_trim_counter_max);


/******************************************************************************
**      FUNCTION DEFINITIONS
******************************************************************************/

extern int DLU_writeCodecOutputs(float float_out_l, float float_out_r)
{
    int RetVal = 0;             // Sin saturación por defecto
    AIC31_data_type codec_data;
    /*-----------------------------------------------------------------------*/
    /* Detección de saturación en canal izquierdo */
    if (float_out_l > 1.0)
    {
        float_out_l = 1.0;
        RetVal = 1;
    }
    else if (float_out_l < -1.0)
    {
        float_out_l = -1.0;
        RetVal = 1;
    }
    /* Detección de saturación en canal derecho */
    if (float_out_r > 1.0)
    {
        float_out_r = 1.0;
        RetVal = 1;
    }
    else if (float_out_r < -1.0)
    {
        float_out_r = -1.0;
        RetVal = 1;
    }
    /*-----------------------------------------------------------------------*/
    //typecasting - full range 16-bit signed integer
    codec_data.channel[LEFT]  = FLT_2_AIC(float_out_l);
    codec_data.channel[RIGHT] = FLT_2_AIC(float_out_r);
    output_sample(codec_data.uint);
    /*-----------------------------------------------------------------------*/
    return RetVal;
}

/*****************************************************************************/
extern void DLU_readCodecInputs(float *float_in_l, float *float_in_r)
{
    AIC31_data_type codec_data;
    /*-----------------------------------------------------------------------*/
    codec_data.uint = input_sample();
    *float_in_l = AIC_2_FLT( codec_data.channel[LEFT]  );
    *float_in_r = AIC_2_FLT( codec_data.channel[RIGHT] );
}

/*****************************************************************************/
extern void DLU_writeLedD4(PB_INT_TYPE value){
    // LED D4 -> GPIO6[13]
    GPIO_setOutput(GPIO_BANK6, GPIO_PIN13, value);
}

/*****************************************************************************/
extern void DLU_writeLedD5(PB_INT_TYPE value){
    // LED D5 -> GPIO6[12]
    GPIO_setOutput(GPIO_BANK6, GPIO_PIN12, value);
}

/*****************************************************************************/
extern void DLU_writeLedD6(PB_INT_TYPE value){
    // LED D6 -> GPIO2[12]
    GPIO_setOutput(GPIO_BANK2, GPIO_PIN12, value);
}

/*****************************************************************************/
extern void DLU_writeLedD7(PB_INT_TYPE value){
    // LED D7 -> GPIO0[9]
    GPIO_setOutput(GPIO_BANK0, GPIO_PIN9, value);
}

/*****************************************************************************/
extern void DLU_toggleLedD(uint32_t led){
    uint8_t old_state;
    switch(led)
    {
    case LED_D4:
        GPIO_getInput(GPIO_BANK6, GPIO_PIN13, &old_state);
        GPIO_setOutput(GPIO_BANK6, GPIO_PIN13, (uint8_t)(0x1 & (~old_state) ));
        break;
    case LED_D5:
        GPIO_getInput(GPIO_BANK6, GPIO_PIN12, &old_state);
        GPIO_setOutput(GPIO_BANK6, GPIO_PIN12, (uint8_t)(0x1 & (~old_state) ));
        break;
    case LED_D6:
        GPIO_getInput(GPIO_BANK2, GPIO_PIN12, &old_state);
        GPIO_setOutput(GPIO_BANK2, GPIO_PIN12, (uint8_t)(0x1 & (~old_state) ));
        break;
    case LED_D7:
        GPIO_getInput(GPIO_BANK0, GPIO_PIN9, &old_state);
        GPIO_setOutput(GPIO_BANK0, GPIO_PIN9, (uint8_t)(0x1 & (~old_state) ));
        break;
    default:
        break;
    }
}

/*****************************************************************************/
extern void DLU_initLeds(void){
    /*-----------------------------------------------------------------------*/
    // LED D4 -> GPIO6[13]
    // LED D5 -> GPIO6[12]
    EVMOMAPL138_pinmuxConfig(PINMUX_LCDK_LEDS_D4_D5_GPIO_OE_REG, PINMUX_LCDK_LEDS_D4_D5_GPIO_OE_MASK, PINMUX_LCDK_LEDS_D4_D5_GPIO_OE_VAL);
    GPIO_setDir(GPIO_BANK6, GPIO_PIN13, GPIO_OUTPUT);
    GPIO_setDir(GPIO_BANK6, GPIO_PIN12, GPIO_OUTPUT);
    /*-----------------------------------------------------------------------*/
    // LED D6 -> GPIO2[12]
    EVMOMAPL138_pinmuxConfig(PINMUX_LCDK_LEDS_D6_GPIO_OE_REG, PINMUX_LCDK_LEDS_D6_GPIO_OE_MASK, PINMUX_LCDK_LEDS_D6_GPIO_OE_VAL);
    GPIO_setDir(GPIO_BANK2, GPIO_PIN12, GPIO_OUTPUT);
    /*-----------------------------------------------------------------------*/
    // LED D7 -> GPIO0[9] Cuidar que la inicialización dec codec configura este
    // pin con otro fin. Llamar DLU_initLeds() después del
    // L138_initialise_intr()
    EVMOMAPL138_pinmuxConfig(PINMUX_LCDK_LEDS_D7_GPIO_OE_REG, PINMUX_LCDK_LEDS_D7_GPIO_OE_MASK, PINMUX_LCDK_LEDS_D7_GPIO_OE_VAL);
    GPIO_setDir(GPIO_BANK0, GPIO_PIN9, GPIO_OUTPUT);
}

/*############################################################################*/
/* API de pushbuttons */
extern PB_INT_TYPE DLU_readPushButton1(void)
{
    return DLU_gPbUser1;
}

/*****************************************************************************/
extern PB_INT_TYPE DLU_readPushButton2(void)
{
    return DLU_gPbUser2;
}

/*****************************************************************************/
extern PB_INT_TYPE DLU_readToggleStatePB1(void)
{
    return DLU_GET_REG_BIT(DLU_gPbToggle, PB_TOGGLES_T1_BIT);
}

/*****************************************************************************/
extern PB_INT_TYPE DLU_readToggleStatePB2(void)
{
    return DLU_GET_REG_BIT(DLU_gPbToggle, PB_TOGGLES_T2_BIT);
}

/*****************************************************************************/
extern PB_INT_TYPE DLU_readToggleStatePB12(void)
{
    return DLU_GET_REG_BIT(DLU_gPbToggle, PB_TOGGLES_T12_BIT);
}

/*****************************************************************************/
extern int32_t DLU_readTrimmerCounter(void)
{
    return gTrimmer.trimmerCounter;

}

/*****************************************************************************/
extern uint32_t DLU_configTrimmerCounter(uint32_t minimumCount, uint32_t maximumCount)
{
    // Sanity check
    if ( minimumCount >= maximumCount)
        return 1; // Error

    gTrimmer.minCount = minimumCount;
    gTrimmer.maxCount = maximumCount;

    // Corregir si ha quedado el contador fuera del nuevo rango
    if ( gTrimmer.trimmerCounter < minimumCount )
        gTrimmer.trimmerCounter = minimumCount;
    if ( gTrimmer.trimmerCounter > maximumCount )
        gTrimmer.trimmerCounter = maximumCount;

    return 0;
}

/*****************************************************************************/
extern uint32_t DLU_configTrimmerAutoIncrement(int32_t delayPeriod_ms, int32_t autoincrementPeriod_ms )
{

    // Minimo de 200 ms de delay
    if ( delayPeriod_ms < 200 )
        delayPeriod_ms = 200;

    // Minimo de 1 ms de periodo de autoincremento
    if ( autoincrementPeriod_ms < 1 )
        autoincrementPeriod_ms = 1;

    gTrimmer.autoIncPeriod = autoincrementPeriod_ms;
    gTrimmer.delayPeriod = delayPeriod_ms;

    // Corregir si ha quedado el contador de retardo fuera del nuevo rango
    if ( pb_trim_counter_delay_count > delayPeriod_ms )
        pb_trim_counter_delay_count = delayPeriod_ms;

    // Corregir si ha quedado el contador de autoincremento fuera del nuevo rango
    if ( pb_trim_counter_autoincdec_count > autoincrementPeriod_ms )
        pb_trim_counter_autoincdec_count = autoincrementPeriod_ms;

    return 0;
}

/*****************************************************************************/
static PB_INT_TYPE DLU_getPbUser1NotDebounced(void){
    PB_INT_TYPE value;
    // USER 1, S2 -> GPIO2[4]
    GPIO_getInput(GPIO_BANK2, GPIO_PIN4, &value);
    if (value == PB_HW_PRESSED)
        return(PB_SW_PRESSED);
    else
        return(PB_SW_RELEASED);
}

/*****************************************************************************/
static PB_INT_TYPE DLU_getPbUser2NotDebounced(void){
    PB_INT_TYPE value;
    // USER 2, S3 -> GPIO2[5]
    GPIO_getInput(GPIO_BANK2, GPIO_PIN5, &value);
    if (value == PB_HW_PRESSED)
        return(PB_SW_PRESSED);
    else
        return(PB_SW_RELEASED);
}

/*****************************************************************************/
extern void DLU_initPushButtons(void){
    /*-----------------------------------------------------------------------*/
    // USER 1, S2 -> GPIO2[4]
    // USER 2, S3 -> GPIO2[5]
    EVMOMAPL138_pinmuxConfig(PINMUX_LCDK_USER_1_USER_2_GPIO_IE_REG, PINMUX_LCDK_USER_1_USER_2_GPIO_IE_MASK, PINMUX_LCDK_USER_1_USER_2_GPIO_IE_VAL);
    GPIO_setDir(GPIO_BANK2, GPIO_PIN4, GPIO_INPUT);
    GPIO_setDir(GPIO_BANK2, GPIO_PIN5, GPIO_INPUT);
    /* Habilita timer2 que por interrupciones lee los pulsadores */
    DLU_initPbTimer2();
}

/*****************************************************************************/
static PB_INT_TYPE DLU_debouncer(deb_stat *ds, PB_INT_TYPE input){
    /* Integration with saturation */
    if (input)
        if (++ds->integral_selector > PB_DEBOUNCE_MAXCOUNT){
            ds->integral_selector = PB_DEBOUNCE_MAXCOUNT;
            ds->debounced = PB_SW_PRESSED;
            return ds->debounced;
        }
        else
            return ds->debounced;
    else
        if (--ds->integral_selector < (-PB_DEBOUNCE_MAXCOUNT)){
            ds->integral_selector = -PB_DEBOUNCE_MAXCOUNT;
            ds->debounced = PB_SW_RELEASED;
            return ds->debounced;
        }
        else
            return ds->debounced;
}

/*****************************************************************************/
static PB_INT_TYPE DLU_getPbUser1Debounced(void){
    PB_INT_TYPE ret;
    ret = DLU_debouncer(&pb_user1_deb_stat, DLU_getPbUser1NotDebounced() );
    return(ret);
}

/*****************************************************************************/
static PB_INT_TYPE DLU_getPbUser2Debounced(void){
    PB_INT_TYPE ret;
    ret = DLU_debouncer(&pb_user2_deb_stat, DLU_getPbUser2NotDebounced() );
    return(ret);
}

/*****************************************************************************/
static void DLU_updatePbToggle(PB_INT_TYPE *pb_toggles, PB_INT_TYPE pb1, PB_INT_TYPE pb2){
    /*-----------------------------------------------------------------------*/
    // Detectando canto de subida en pb1
    if ( ( pb1 == PB_SW_PRESSED ) && ( DLU_GET_REG_BIT(pb_states_stat, PB_STATES_STAT_P1BUFF_BIT) == PB_SW_RELEASED ) )
        pb_states_stat |= (1 << PB_STATES_STAT_P1_PEDGE_BIT );
    else
        pb_states_stat &= ~(1 << PB_STATES_STAT_P1_PEDGE_BIT );
    /*-----------------------------------------------------------------------*/
    // Actualización del estado Toggle 1
    if ( pb_states_stat & ( 1 << PB_STATES_STAT_P1_PEDGE_BIT ) )
        *pb_toggles ^= ( 1 << PB_TOGGLES_T1_BIT );
    /*-----------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------*/
    // Detectando canto de subida en pb2
    if ( ( pb2 == PB_SW_PRESSED ) && ( ( DLU_GET_REG_BIT(pb_states_stat, PB_STATES_STAT_P2BUFF_BIT) == PB_SW_RELEASED ) ) )
        pb_states_stat |= (1 << PB_STATES_STAT_P2_PEDGE_BIT );
    else
        pb_states_stat &= ~(1 << PB_STATES_STAT_P2_PEDGE_BIT );
    /*-----------------------------------------------------------------------*/
    // Actualización del estado Toggle 1
    if ( pb_states_stat & ( 1 << PB_STATES_STAT_P2_PEDGE_BIT ) )
        *pb_toggles ^= ( 1 << PB_TOGGLES_T2_BIT );
    /*-----------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------*/
    // Detectando canto de presionar pb1 y pb2
    if (       ( pb1 == PB_SW_PRESSED )
            && ( pb2 == PB_SW_PRESSED )
            && (    ( DLU_GET_REG_BIT(pb_states_stat, PB_STATES_STAT_P1BUFF_BIT) == PB_SW_RELEASED ) ||
                    ( DLU_GET_REG_BIT(pb_states_stat, PB_STATES_STAT_P2BUFF_BIT) == PB_SW_RELEASED )    )  )
        pb_states_stat |= (1 << PB_STATES_STAT_P12_PEDGE_BIT );
    else
        pb_states_stat &= ~(1 << PB_STATES_STAT_P12_PEDGE_BIT );
    /*-----------------------------------------------------------------------*/
    // Actualización del estado Toggle 12
    if ( pb_states_stat & ( 1 << PB_STATES_STAT_P12_PEDGE_BIT ) )
        *pb_toggles ^= ( 1 << PB_TOGGLES_T12_BIT );
    /*-----------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------*/
    // Limpiando buffers
    pb_states_stat &= ~(1 << PB_STATES_STAT_P1BUFF_BIT );
    pb_states_stat &= ~(1 << PB_STATES_STAT_P2BUFF_BIT );
    // Actualización de buffers
    pb_states_stat |= ( (uint32_t)pb1 << PB_STATES_STAT_P1BUFF_BIT );
    pb_states_stat |= ( (uint32_t)pb2 << PB_STATES_STAT_P2BUFF_BIT );
}

/*****************************************************************************/
static void DLU_updatePbTrimCounter(int32_t *pb_trim_counter, int32_t pb_trim_counter_delay_period, int32_t pb_trim_counter_autoinc_period, int32_t pb_trim_counter_min, int32_t pb_trim_counter_max){
    // Solo un pulsador presonado a la vez iniciará la cuenta del retardo para incrementar
    if ( DLU_GET_REG_BIT(pb_states_stat, PB_STATES_STAT_P1BUFF_BIT) ^ DLU_GET_REG_BIT(pb_states_stat, PB_STATES_STAT_P2BUFF_BIT) )
    {
        if ( !DLU_GET_REG_BIT(pb_states_stat, PB_STATES_STAT_AUTO_INCDEC_BIT) )
            pb_trim_counter_delay_count++;

        if (pb_trim_counter_delay_count > pb_trim_counter_delay_period)
            pb_states_stat |= ( 1 << PB_STATES_STAT_AUTO_INCDEC_BIT );
    }
    else
    {
        pb_trim_counter_delay_count = 0;
        pb_states_stat &= ~( 1 << PB_STATES_STAT_AUTO_INCDEC_BIT );
    }
    /*-----------------------------------------------------------------------*/
    // Autoincremento o por pulsos manuales
    if ( DLU_GET_REG_BIT(pb_states_stat, PB_STATES_STAT_AUTO_INCDEC_BIT) )
    {
        // Modo autoincremento
        pb_trim_counter_autoincdec_count++;
        if ( pb_trim_counter_autoincdec_count > pb_trim_counter_autoinc_period )
        {
            pb_trim_counter_autoincdec_count = 0;
            // Autoincrementa solo cuando ha transcurrido el periodo de autoincremento
            if ( DLU_GET_REG_BIT(pb_states_stat, PB_STATES_STAT_P2BUFF_BIT) )
                (*pb_trim_counter)++;
            if ( DLU_GET_REG_BIT(pb_states_stat, PB_STATES_STAT_P1BUFF_BIT) )
                (*pb_trim_counter)--;
        }
    }
    else
    {
        // Modo pulsos manuales
        if ( DLU_GET_REG_BIT(pb_states_stat, PB_STATES_STAT_P2_PEDGE_BIT) )
            (*pb_trim_counter)++;
        if ( DLU_GET_REG_BIT(pb_states_stat, PB_STATES_STAT_P1_PEDGE_BIT) )
            (*pb_trim_counter)--;
    }
    /*-----------------------------------------------------------------------*/
    // Saturación
    if ( *pb_trim_counter > pb_trim_counter_max )
        *pb_trim_counter = pb_trim_counter_max;
    if ( *pb_trim_counter < pb_trim_counter_min )
        *pb_trim_counter = pb_trim_counter_min;
}

/*############################################################################*/
extern void DLU_initTicToc(void){
    TSCL = 0;
}

/*****************************************************************************/
extern void DLU_tic(void){
    unsigned int aux;
    aux = TSCL;
    DLU_timeStampAux = ((unsigned long long)TSCH << 32);
    DLU_timeStampAux += (unsigned long long)aux;
}

/*****************************************************************************/
extern void DLU_toc(void){
    unsigned int aux;
    unsigned long long aux_long;

    aux = TSCL;
    aux_long = ((unsigned long long)TSCH << 32);
    aux_long += (unsigned long long)aux;

    DLU_timeTicToc = aux_long - DLU_timeStampAux;
}

/*############################################################################*/
static void DLU_initPbTimer2(void)
{
    /* Configura Timer2 cuenta libre. */
    /* Timer2 doble no encadenado, se usa la parte Timer2 34 */

    /* Posibles pines de entrada y salida del timer se dejan como libres para
     * GPIO */
   TMR2->GPINT_GPEN = GPENO12 | GPENI12;
   TMR2->GPDATA_GPDIR = GPDIRO12 | GPDIRI12;

   // stop and reset timer.
   TMR2->TGCR = 0x00000000;
   TMR2->TCR = 0x00000000;

   /* Habilitar interrupción por periodo en Timer2 12 */
   TMR2->INTCTLSTAT = PRDINTEN34;
   /* Que en modo emulación siga contando el timer */
   SETBIT(TMR2->EMUMGT, SOFT | FREE);

   /* Timer2 como contador doble de 32 bits, no encadenados */
   /* Se libera el Timer2 34 de reset */
   SETBIT(TMR2->TGCR, PRESCALER(TMR2_DIVISION - 1) | TIMMODE_32BIT_UNCHAINED | TIM34RS );

   /* Inicialización en cero */
   TMR2->TIM34 = 0x00000000;
   /* Carga del valor de pediodo de la cuenta */
   TMR2->PRD34 = TMR2_PRD34;

   /* Habilitación de cuenta continua */
   SETBIT(TMR2->TCR, ENAMODE34_CONT);

   /*------------------------------------------------------------------------*/
   /* Limpia bit de interrupción */
   CLRBIT(TMR2->INTCTLSTAT, PRDINTSTAT34);

   /* Se asocia evento de interrupción por T64P_34n(evento 25: T64P2_TINTALL)
    * con la interrupción 8
    */
   INTC_INTMUX2 = 0x00000019;

   /*------------------------------------------------------------------------*/
   /* Configuración global de interrupciones*/
   ISTP = (unsigned int)vectors;

   // clear all pending interrupt flags
   // interrupt clear register ICR is used to clear bits in interrupt flag register IFR
   ICR = 0xFFF0;  // ICR bits 3, 2, 1, and 0 are reserved, read as 0, write has no effect
   IER |= 0x102;  //enable NMI (bit 1) and INT4 (bit 8)

   CSR |= 0x01;  // enable interrupts globally

}

/*############################################################################*/
/* ISR para lectura de pushbuttons */
interrupt void int8_pb(void)
{
    // Obtención de nivel de pulsadores (con antirebote)
    DLU_gPbUser1 = DLU_getPbUser1Debounced();
    DLU_gPbUser2 = DLU_getPbUser2Debounced();
    // Generación de estados "pb_toggles" y cuenta de ajuste "pb_trim_counter" con los pulsadores
    DLU_updatePbToggle(&DLU_gPbToggle, DLU_gPbUser1, DLU_gPbUser2);
    DLU_updatePbTrimCounter(&(gTrimmer.trimmerCounter),  gTrimmer.delayPeriod, gTrimmer.autoIncPeriod, gTrimmer.minCount, gTrimmer.maxCount);
}

/*############################################################################*/
/* API de gráficos */
extern void DLU_appendGraphBuff1(GraphTypeBuff1_t variable){
    /* Esritura en buffer de gráfico */
    if (gSingleGraphBuff1)
    {
        gGraphBuff1[gIdxGraphBuff1] = variable;
        gIdxGraphBuff1++;
        if (gIdxGraphBuff1 >= DLU_GRAPH_BUFF_SIZE)
        {
            gSingleGraphBuff1 = 0;
            gIdxGraphBuff1 = 0;
        }
    }
}

/*****************************************************************************/
extern void DLU_appendGraphBuff2(GraphTypeBuff2_t variable){
    /* Esritura en buffer de gráfico */
    if (gSingleGraphBuff2)
    {
        gGraphBuff2[gIdxGraphBuff2] = variable;
        gIdxGraphBuff2++;
        if (gIdxGraphBuff2 >= DLU_GRAPH_BUFF_SIZE)
        {
            gSingleGraphBuff2 = 0;
            gIdxGraphBuff2 = 0;
        }
    }
}

/*****************************************************************************/
extern void DLU_enableSynchronicSingleCaptureOnAllGraphBuff(void)

{
    /* Sube todos los gatillos de buffers */
    gSingleGraphBuff1 = 1;
    gSingleGraphBuff2 = 1;

    return;
}

/*****************************************************************************/
extern void DLU_clearAllGraphBuffers(void)
{

    size_t bufferSize = sizeof(gGraphBuff1);

    memset(gGraphBuff1, 0, bufferSize);
    memset(gGraphBuff2, 0, bufferSize);

    return;
}

/*****************************************************************************/
extern void DLU_enableSingleCaptureOnGraphBuff1(void)
{
    gSingleGraphBuff1 = 1;

    return;
}

/*****************************************************************************/
extern void DLU_enableSingleCaptureOnGraphBuff2(void)
{
    gSingleGraphBuff2 = 1;

    return;
}



/*############################################################################*/
/* API de generación de señal aleatoria */
int16_t DLU_rand(void)
{
return ((int16_t)(rand31_next()>>15)-32768);
}

/*****************************************************************************/
float DLU_fRand(void)
{
    int16_t intRand = (int16_t)(rand31_next()>>15)-32768;

    return (((float)intRand)*(1/40000.0));
}


/******************************************************************************
**      END OF SOURCE FILE
******************************************************************************/
