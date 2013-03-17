#ifndef OSUAR_ADC_H
#define OSUAR_ADC_H

#include <ch.h>
#include <hal.h>

/* Total number of channels to be sampled by a single ADC operation.*/
#define ADC_NUM_CHANNELS   6

/* Depth of the conversion buffer, channels are sampled four times each.*/
#define ADC_GRP1_BUF_DEPTH      4

void setup_adc(void);
void update_adc(void);
extern void adccb(ADCDriver *adcp, adcsample_t *buffer, size_t n);

/* Average values from ADC samples. */
extern adcsample_t avg_ch[ADC_NUM_CHANNELS];

/*
 * ADC samples buffer.
 */
//adcsample_t samples[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];

/*
 * ADC conversion group.
 * Mode:        Linear buffer, 4 samples of 2 channels, SW triggered.
 * Channels:    IN11   (48 cycles sample time)
 *              Sensor (192 cycles sample time)
 */
static const ADCConversionGroup adcgrpcfg = {
  FALSE,   // Linear buffer (TRUE for circular)
  ADC_NUM_CHANNELS,   // Number of analog channels belonging to this group
  adccb,   // Callback (NULL if none)
  NULL,   // Error callback (NULL if none)
  /* HW dependent part.*/
  0,   // CR1 initialization data
  ADC_CR2_SWSTART,   // CR2 initialization data
  ADC_SMPR1_SMP_AN11(ADC_SAMPLE_56),   // SMPR1 initialization data
  0,    // SMPR2 initialization data
  ADC_SQR1_NUM_CH(ADC_NUM_CHANNELS),   // SQR1 initialization data
  0,   // SQR2 initialization data
  ADC_SQR3_SQ6_N(ADC_CHANNEL_IN13) | ADC_SQR3_SQ5_N(ADC_CHANNEL_IN12) |
  ADC_SQR3_SQ4_N(ADC_CHANNEL_IN11) | ADC_SQR3_SQ3_N(ADC_CHANNEL_IN10) |
  ADC_SQR3_SQ2_N(ADC_CHANNEL_IN5)  | ADC_SQR3_SQ1_N(ADC_CHANNEL_IN4)   // SQR3 initialization data
};

#endif // OSUAR_ADC_H

