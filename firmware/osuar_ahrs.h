#ifndef AHRS_H
#define AHRS_H

#include <ch.h>
#include <hal.h>
#include <osuar_i2c.h>
#include <osuar_config.h>

void setup_ahrs(void);
void update_ahrs(void);

#endif // AHRS_H

