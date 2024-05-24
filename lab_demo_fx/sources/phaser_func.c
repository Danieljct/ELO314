/***************************************************************************//**
* \file     pahser_func.c
*
* \brief    This file defines all functions necessary to implement a Phaser
*           audio effect.
*
* \authors  Gonzalo Carrasco
*******************************************************************************/

/***************************************************************************//**
**      HEADER FILES
*******************************************************************************/
#include "phaser_func.h" // There should be declarations, constants definitions, etc.
#include "phaser_func_vars.h"


/******************************************************************************
**      MACRO DEFINITIONS
*******************************************************************************/
//#define CONSTANT  1 // If necessary, because they should be in code_template.h


/***************************************************************************//**
**      GLOBAL VARIABLES
*******************************************************************************/
//#include "global_vars.h" // If there are too many variables, they will be in a .h
//unsigned int x;


/***************************************************************************//**
**      INTERNAL FUNCTION DEFINITIONS
*******************************************************************************/

/***************************************************************************//**
*   \brief  Function that creates the pahser effect
*
*   \param  audio_in    audio input as float
*
*   \param speed
*
*   \return float.
*******************************************************************************/
float phaser_fx(float audio_in, float speed, unsigned int feedback_en, float sampling_time)
{
    float apf_audio_stage[APF_STAGES];
    float audio_in_fb = 0.0;
    float tau;
    float apf_alpha;
    unsigned int stage;

    if (phaser_bypass == 0)
    {
        /* Upadating tau */
        tau = lfo_tau(speed, sampling_time);

        /* Updating Alpha factor for discrete APF */
        apf_alpha =  ( 2.0 * tau - sampling_time ) / ( 2.0 * tau + sampling_time );

        /* Feedback or not*/
        if (feedback_en != 0 )
            audio_in_fb = audio_in + phaser_fb_gain * phaser_audio_out;
        else
            audio_in_fb = audio_in;

        /* Computing AFPs outputs */
        apf_audio_stage[0] = allpass_filter(audio_in_fb, apf_alpha, &apf_stages_stat[0]);
        for (stage = 1; stage < APF_STAGES; stage++ )
            apf_audio_stage[stage] = allpass_filter(apf_audio_stage[stage-1], apf_alpha, &apf_stages_stat[stage]);

        /* Mixing phased signal and input */
        phaser_audio_out = audio_in + apf_audio_stage[APF_STAGES-1];
    }
    else
        phaser_audio_out = audio_in;

    /* End of algorithm */
    return phaser_audio_out;
}

/***************************************************************************//**
*   \brief  Function that creates the pahser effect
*
*   \param  audio_in    audio input as float
*
*   \param speed
*
*   \return float.
*******************************************************************************/
void phaser_fx_stereo(float audio_in_l, float audio_in_r, float speed, unsigned int feedback_en, float sampling_time)
{
    float apf_audio_l_stage[APF_STAGES];
    float apf_audio_r_stage[APF_STAGES];
    float audio_in_l_fb = 0.0;
    float audio_in_r_fb = 0.0;
    float tau;
    float apf_alpha;
    unsigned int stage;

    if (phaser_bypass == 0)
    {
        /* Upadating tau from LFO */
        tau = lfo_tau(speed, sampling_time);
        /*----------------------------------------------------------------------*/
        /* Updating Alpha factor for discrete APF */
        apf_alpha =  ( 2.0 * tau - sampling_time ) / ( 2.0 * tau + sampling_time );
        /*----------------------------------------------------------------------*/
        /* Feedback or not*/
        if (feedback_en != 0 )
        {
            audio_in_l_fb = audio_in_l + phaser_fb_gain * phaser_audio_l_out;
            audio_in_r_fb = audio_in_r + phaser_fb_gain * phaser_audio_r_out;
        }
        else
        {
            audio_in_l_fb = audio_in_l;
            audio_in_r_fb = audio_in_r;
        }
        /*----------------------------------------------------------------------*/
        /* Computing AFPs stages outputs */
        apf_audio_l_stage[0] = allpass_filter(audio_in_l_fb, apf_alpha, &apf_stages_l_stat[0]);
        apf_audio_r_stage[0] = allpass_filter(audio_in_r_fb, apf_alpha, &apf_stages_r_stat[0]);

        for (stage = 1; stage < APF_STAGES; stage++ )
        {
            apf_audio_l_stage[stage] = allpass_filter(apf_audio_l_stage[stage-1], apf_alpha, &apf_stages_l_stat[stage]);
            apf_audio_r_stage[stage] = allpass_filter(apf_audio_r_stage[stage-1], apf_alpha, &apf_stages_r_stat[stage]);
        }
        /*----------------------------------------------------------------------*/
        /* Mixing phased signal and input */
        phaser_audio_l_out = audio_in_l + apf_audio_l_stage[APF_STAGES-1];
        phaser_audio_r_out = audio_in_r + apf_audio_r_stage[APF_STAGES-1];
    }
    else
    {
        /*----------------------------------------------------------------------*/
        /* Bypassing the input to the output */
        phaser_audio_l_out = audio_in_l;
        phaser_audio_r_out = audio_in_r;
    }

    /* End of algorithm */
}

/***************************************************************************//**
*   \brief  This function implement a single stage of a All-Pass Filter (APF)
*
*   \param  .
*
*   \return float.
*******************************************************************************/
float allpass_filter(float audio_in, float alpha, apf_stat *stat)
{
    /* The old output values is used along with the old input and present input
     * to obtain the new output. The output is stored for the next iteration.  */
    /*----------------------------------------------------------------------*/
    stat->out_buff = ( alpha * ( stat->out_buff + audio_in ) ) - stat->in_buff;
    stat->in_buff = audio_in;
    /*----------------------------------------------------------------------*/
    return stat->out_buff;
}


/***************************************************************************//**
*   \brief  This function creates the sweep of Tau to tune the APFs.
*
*   \param  .
*
*   \return float.
*******************************************************************************/
float lfo_tau(float speed, float sampling_time)
{
    float delta_tau;
    float lfo_sweep_counts;

    /* Number of increments for tau in a period */
    lfo_sweep_counts = ( ( 1 - speed) * ( PHASER_MAX_SWEEP_PERIOD - PHASER_MIN_SWEEP_PERIOD ) + PHASER_MIN_SWEEP_PERIOD ) / sampling_time;
    //lfo_sweep_counts = 1.0 / ( ( speed * ( PHASER_MAX_SWEEP_FREC - PHASER_MIN_SWEEP_FREC ) + PHASER_MIN_SWEEP_FREC ) * sampling_time );

    /* Delta tau */
    delta_tau = (2.0 * ( PHASER_MAX_TAU - PHASER_MIN_TAU ) ) / lfo_sweep_counts;

    /* Non-linear correction of delta_tau */
    stat_pos = ( (  2.0 * phaser_lfo_stat - (PHASER_MAX_TAU + PHASER_MIN_TAU) ) / (PHASER_MAX_TAU - PHASER_MIN_TAU) ) - stat_pos_offset;
    delta_tau = delta_tau * ( 1 + delta_nl_coeff * stat_pos) ;

    /* Tau computation */
    if (phaser_lfo_slope != PHASER_LFO_DOWN)
        phaser_lfo_stat = phaser_lfo_stat + delta_tau;
    else
        phaser_lfo_stat = phaser_lfo_stat - delta_tau;

    /* Tau slope updating */
    if (phaser_lfo_stat > PHASER_MAX_TAU)
    {
        phaser_lfo_slope = PHASER_LFO_DOWN;
        phaser_lfo_stat  = 2.0 * PHASER_MAX_TAU - phaser_lfo_stat;
    }
    if (phaser_lfo_stat < PHASER_MIN_TAU)
    {
        phaser_lfo_slope = PHASER_LFO_UP;
        phaser_lfo_stat  = 2.0 * PHASER_MIN_TAU - phaser_lfo_stat;
    }

    /* End of tau algorithm */
    return phaser_lfo_stat;
}
