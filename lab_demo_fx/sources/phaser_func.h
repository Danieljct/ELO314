#ifndef PHASER_FUNC_H
#define PHASER_FUNC_H

#ifdef __cplusplus
extern "C" {
#endif
/*************************************************************************************************/
/* Macro definitions for phaser fx function*/
#define PHASER_FB_CONST         0.4         // Proportion of output feed-backed to input
#define PHASER_MIN_TAU          0.000053   // Central frequency until 2kHz -> Tau = 1/(2*Pi*3000)
#define PHASER_MAX_TAU          0.000796   // Central frequency from 200Hz -> Tau = 1/(2*Pi*200)

#define PHASER_MIN_FREC         (1.0 / PHASER_MAX_TAU)
#define PHASER_MAX_FREC         (1.0 / PHASER_MIN_TAU)

#define PHASER_MIN_SWEEP_PERIOD 0.05
#define PHASER_MAX_SWEEP_PERIOD 20
#define PHASER_MIN_SWEEP_FREC   0.05
#define PHASER_MAX_SWEEP_FREC   12.0

#define APF_STAGES              4

#define PHASER_LFO_DOWN         0
#define PHASER_LFO_UP           1

/* Typedefs */
/* Variable definitions */
/* Internal states for every APF stage */
typedef struct {
    float in_buff;
    float out_buff;
} apf_stat;


/* Declaration of variables to upper layer that calls phaser functions */
extern float phaser_audio_out;
extern float phaser_lfo_stat;
extern unsigned int phaser_lfo_slope;
extern unsigned int phaser_bypass;

extern apf_stat apf_1_stat;
extern apf_stat apf_2_stat;
extern apf_stat apf_3_stat;
extern apf_stat apf_4_stat;

extern float phaser_fb_gain;

extern float phaser_lfo_frec_stat;

extern float stat_pos;
extern float delta_nl_coeff;
extern float stat_pos_offset;

extern float phaser_audio_l_out;
extern float phaser_audio_r_out;

/* Function prototypes */
float phaser_fx(float audio_in, float speed, unsigned int feedback_en, float sampling_time);

void phaser_fx_stereo(float audio_in_l, float audio_in_r, float speed, unsigned int feedback_en, float sampling_time);

float allpass_filter(float audio_in, float alpha, apf_stat *stat);

float lfo_tau(float speed, float sampling_time);

/*************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif
