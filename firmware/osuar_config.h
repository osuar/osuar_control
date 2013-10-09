/******************************************************************************
 * TODO: Add a better description.
 *
 * Some notes on prefixes: I'll be trying to organize these definitions with
 * prefixes. So far:
 *    I_       - Index
 *****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

/* Loop periods */
#define CONTROL_DT 0.001   /* Note that MS2ST won't work here. See its implementation to find out why. */

#if (BOARD == APOLLO)
#include <osuar_config_apollo.h>
#endif

#if (BOARD == HERMES)
#include <osuar_config_hermes.h>
#endif

#if (BOARD == AEOLUS)
#include <osuar_config_aeolus.h>
#endif

#if (BOARD == ZEPHYRUS)
#include <osuar_config_zephyrus.h>
#endif

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

