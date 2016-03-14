/* pinmux_k64.c - pin out mapping for the Freescale FRDM-K64F board */

/*
 * Copyright (c) 2016, Wind River Systems, Inc.
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

#include <errno.h>

#include <stdbool.h>
#include <nanokernel.h>
#include <device.h>
#include <init.h>
#include <sys_io.h>

#include <pinmux.h>
#include <i2c.h>
#include <gpio.h>
#include <pwm.h>

#include <gpio/gpio_k64.h>
#include <pinmux/pinmux.h>
#include <pinmux/pinmux_k64.h>

/* port pin number conversion from pin ID */
#define PIN_FROM_ID(pin_id)	(pin_id % K64_PINMUX_NUM_PINS)

#ifdef CONFIG_GPIO_K64_A
static inline int config_port_a(mem_addr_t *addr)
{
	*addr = CONFIG_PORT_K64_A_BASE_ADDR;
	return 0;
}
#else
#define config_port_a(addr) -EACCES
#endif

#ifdef CONFIG_GPIO_K64_B
static inline int config_port_b(mem_addr_t *addr)
{
	*addr = CONFIG_PORT_K64_B_BASE_ADDR;
	return 0;
}
#else
#define config_port_b(addr) -EACCES
#endif

#ifdef CONFIG_GPIO_K64_C
static inline int config_port_c(mem_addr_t *addr)
{
	*addr = CONFIG_PORT_K64_C_BASE_ADDR;
	return 0;
}
#else
#define config_port_c(addr) -EACCES
#endif

#ifdef CONFIG_GPIO_K64_D
static inline int config_port_d(mem_addr_t *addr)
{
	*addr = CONFIG_PORT_K64_D_BASE_ADDR;
	return 0;
}
#else
#define config_port_d(addr) -EACCES
#endif

#ifdef CONFIG_GPIO_K64_E
static inline int config_port_e(mem_addr_t *addr)
{
	*addr = CONFIG_PORT_K64_E_BASE_ADDR;
	return 0;
}
#else
#define config_port_e(addr) -EACCES
#endif

static int _fsl_k64_get_port_addr(uint8_t pin_id, mem_addr_t *port_addr_ptr)
{

	/* determine the port base address associated with the pin identifier */

	if (pin_id < K64_PIN_PTB0) {		   /* Port A pin */

		return config_port_a(port_addr_ptr);

	} else if (pin_id < K64_PIN_PTC0) {	/* Port B pin */

		return config_port_b(port_addr_ptr);

	} else if (pin_id < K64_PIN_PTD0) {	/* Port C pin */

		return config_port_c(port_addr_ptr);

	} else if (pin_id < K64_PIN_PTE0) {	/* Port D pin */

		return config_port_d(port_addr_ptr);

	} else {				/* Port E pin */

		return config_port_e(port_addr_ptr);

	}

}

static int _fsl_k64_set_pin(struct device *dev, uint32_t pin_id,
			    uint32_t func)
{
	mem_addr_t port_base_addr;
	uint8_t port_pin;
	uint32_t status;

	if (pin_id >= CONFIG_PINMUX_NUM_PINS) {
		return -ENOTSUP;
	}

	/* determine the pin's port register base address */
	status = _fsl_k64_get_port_addr(pin_id, &port_base_addr);

	if (status != 0) {
		return status;
	}

	/* extract the pin number within its port */
	port_pin = PIN_FROM_ID(pin_id);

	/* set pin function and control settings */
	sys_write32(func, port_base_addr + K64_PINMUX_CTRL_OFFSET(port_pin));

	return 0;
}

static int _fsl_k64_get_pin(struct device *dev, uint32_t pin_id,
			    uint32_t *func)
{
	mem_addr_t port_base_addr;
	uint8_t port_pin;
	uint32_t status;

	if (pin_id >= CONFIG_PINMUX_NUM_PINS) {
		return -ENOTSUP;
	}

	/* determine the pin's port register base address */
	status = _fsl_k64_get_port_addr(pin_id, &port_base_addr);
	if (status != 0) {
		return status;
	}

	/* extract the pin number within its port */
	port_pin = PIN_FROM_ID(pin_id);

	/* get pin function and control settings */
	*func = sys_read32(port_base_addr + K64_PINMUX_CTRL_OFFSET(port_pin));

	return 0;
}

static int fsl_k64_dev_set(struct device *dev, uint32_t pin,
			   uint32_t func)
{
	return _fsl_k64_set_pin(dev, pin, func);
}

static int fsl_k64_dev_get(struct device *dev, uint32_t pin, uint32_t *func)
{
	return _fsl_k64_get_pin(dev, pin, func);
}

static struct pinmux_driver_api api_funcs = {
	.set = fsl_k64_dev_set,
	.get = fsl_k64_dev_get
};

int pinmux_fsl_k64_initialize(struct device *port)
{
	port->driver_api = &api_funcs;

	return 0;
}

struct pinmux_config fsl_k64_pmux = {
	.base_address = 0x00000000,
};

/* must be initialized after GPIO */
DEVICE_INIT(pmux, PINMUX_NAME, &pinmux_fsl_k64_initialize,
	    NULL, &fsl_k64_pmux,
	    SECONDARY, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);
