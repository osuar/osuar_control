#ifndef AHRS_H
#define AHRS_H

#include <ch.h>
#include <hal.h>
#include <osuar_i2c.h>
#include <osuar_config.h>
#include <osuar_mag_lsm303.h>   // TODO: This is the magnetometer on the STM32F3 board, for testing purposes, as we do not yet have our flight control board fully assembled.

void setup_ahrs(void);
void update_ahrs(void);

#endif // AHRS_H

