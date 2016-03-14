/* pinmux.c - pin out mapping for the Freescale FRDM K64F SoC */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <board.h>
#include <device.h>
#include <gpio.h>
#include <init.h>
#include <pinmux.h>
#include <pinmux/pinmux.h>
#include <pinmux/pinmux_k64.h>
#include <pwm.h>

#include "pinmux/pinmux.h"

/*
 * Number of default pin settings, used for Arduino Rev 3 pinout.
 *
 * NOTE: The FRDM-K64F board routes the PTA0/1/2 pins for JTAG/SWD signals that
 * are used for the OpenSDAv2 debug interface.  These pins are also routed to
 * the Arduino header pins as D8, D3 and D5, respectively.
 * Since the K64 MCU configures these pins for JTAG/SWD signaling at reset,
 * they should only be re-configured if the debug interface is not used.
 */

#ifndef CONFIG_PRESERVE_JTAG_IO_PINS
#define NUM_DFLT_PINS_SET   22
#else
#define NUM_DFLT_PINS_SET   (22 - 3)
#endif

/*
 * Alter this table to change the default Arduino pin settings on the Freescale
 * FRDM-K64F boards.  Specifically, change the PINMUX_* values to represent
 * the functionality desired.
 */
struct pin_config mux_config[NUM_DFLT_PINS_SET] = {
	/* pin,		selected mode */
	{ K64_PIN_PTC16, K64_PINMUX_FUNC_GPIO },
	{ K64_PIN_PTC17, K64_PINMUX_FUNC_GPIO },
	{ K64_PIN_PTB9,  K64_PINMUX_FUNC_GPIO },
#ifndef CONFIG_PRESERVE_JTAG_IO_PINS
	{ K64_PIN_PTA1,  K64_PINMUX_FUNC_GPIO },
#endif
	{ K64_PIN_PTB23, K64_PINMUX_FUNC_GPIO },
#ifndef CONFIG_PRESERVE_JTAG_IO_PINS
	{ K64_PIN_PTA2,  K64_PINMUX_FUNC_GPIO },
#endif
	{ K64_PIN_PTC2,  K64_PINMUX_FUNC_GPIO },
	{ K64_PIN_PTC3,  K64_PINMUX_FUNC_GPIO },
#ifndef CONFIG_PRESERVE_JTAG_IO_PINS
	{ K64_PIN_PTA0,  K64_PINMUX_FUNC_GPIO },
#endif
	{ K64_PIN_PTC4,  K64_PINMUX_FUNC_GPIO },
	{ K64_PIN_PTD0,  K64_PINMUX_FUNC_GPIO },
	{ K64_PIN_PTD2,  K64_PINMUX_FUNC_GPIO },
	{ K64_PIN_PTD3,  K64_PINMUX_FUNC_GPIO },
	{ K64_PIN_PTD1,  K64_PINMUX_FUNC_GPIO },
	/* I2C0_SDA */
	{ K64_PIN_PTE25, (K64_PINMUX_ALT_5 | K64_PINMUX_OPEN_DRN_ENABLE) },
	/* I2C0_SCL */
	{ K64_PIN_PTE24, (K64_PINMUX_ALT_5 | K64_PINMUX_OPEN_DRN_ENABLE) },
	{ K64_PIN_PTB2,  K64_PINMUX_FUNC_ANALOG },  /* ADC0_SE12/Analog In 0 */
	{ K64_PIN_PTB3,  K64_PINMUX_FUNC_ANALOG },  /* ADC0_SE13/Analog In 1 */
	{ K64_PIN_PTB10, K64_PINMUX_FUNC_ANALOG },  /* ADC1_SE14/Analog In 2 */
	{ K64_PIN_PTB11, K64_PINMUX_FUNC_ANALOG },  /* ADC1_SE15/Analog In 3 */
	{ K64_PIN_PTC11, K64_PINMUX_FUNC_ANALOG },  /* ADC1_SE7b/Analog In 4 */
	{ K64_PIN_PTC10, K64_PINMUX_FUNC_ANALOG },  /* ADC1_SE6b/Analog In 5 */
};


int fsl_frdm_k64f_pin_init(struct device *arg)
{
	ARG_UNUSED(arg);
	struct device *pmux;
	int i;

	pmux = device_get_binding(PINMUX_NAME);

	if (!pmux) {
		return -EPERM;
	}

	/* configure the pins from the default mapping above */

	for (i = 0; i < NUM_DFLT_PINS_SET; i++) {
		pinmux_pin_set(pmux, mux_config[i].pin_num, mux_config[i].mode);
	}

	return 0;
}

DEVICE_INIT(frdm_k64f_pmux, "", fsl_frdm_k64f_pin_init, NULL, NULL,
			SECONDARY, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);
