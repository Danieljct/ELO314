
float phaser_audio_out = 0.0;
extern float phaser_audio_l_out = 0.0;
extern float phaser_audio_r_out = 0.0;
float phaser_lfo_stat = PHASER_MIN_TAU;
unsigned int phaser_lfo_slope = 1;
unsigned int phaser_bypass = 0;

apf_stat apf_stages_stat[APF_STAGES];

apf_stat apf_stages_l_stat[APF_STAGES];
apf_stat apf_stages_r_stat[APF_STAGES];

float phaser_fb_gain = PHASER_FB_CONST;

float phaser_lfo_frec_stat = PHASER_MAX_FREC;

float stat_pos = 0.0;
float delta_nl_coeff = 0.75;
float stat_pos_offset = 0.2;
