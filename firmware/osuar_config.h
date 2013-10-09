/******************************************************************************
 * TODO: Add a better description.
 *
 * Some notes on prefixes: I'll be trying to organize these definitions with
 * prefixes. So far:
 *    I_       - Index
 *****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#define NUM_ROTORS 4
#define ESC_COMM PWM   // Set to either PWM or SPI.

/* Loop periods */
#define CONTROL_DT 0.001   /* Note that MS2ST won't work here. See its implementation to find out why. */

/******************************************************************************
 * 2-rotor system.
 *
 * Currently, this applies only to the robat.
 *****************************************************************************/
#if (NUM_ROTORS == 2)
#include <osuar_config_2_rotor.h>
#endif // NUM_ROTORS == 2

/******************************************************************************
 * 3-rotor system.
 *
 * Currently, this applies only to Soo-Hyun's tricopter.
 *****************************************************************************/
#if (NUM_ROTORS == 3)
#include <osuar_config_3_rotor.h>
#endif // NUM_ROTORS == 3

/******************************************************************************
 * 4-rotor system.
 *
 * Currently, we have two quadrotors. We may or may not decide to keep separate
 * config files for the two robots.
 *****************************************************************************/
#if (NUM_ROTORS == 4)
#include <osuar_config_4_rotor.h>
#endif // NUM_ROTORS == 4

#endif // CONFIG_H

