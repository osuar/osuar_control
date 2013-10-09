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

#ifdef APOLLO
#include <osuar_config_apollo.h>
#endif

#ifdef HERMES
#include <osuar_config_hermes.h>
#endif

#ifdef AEOLUS
#include <osuar_config_aeolus.h>
#endif

#ifdef ZEPHYRUS
#include <osuar_config_zephyrus.h>
#endif

#endif // CONFIG_H

