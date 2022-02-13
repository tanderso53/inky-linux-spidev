#ifndef INKY_SPIDEV_H
#define INKY_SPIDEV_H

#include "inky.h"

#include <gpiod.h>

#include <stdint.h>

#define INKY_SPIDEV_CONSUMER "inky-spidev"

/** @brief interface object for inky-spidev driver
 *
 * This must be filled in and passed to init prior to use of the
 * interface.
 */
typedef struct {
	char special[64];
	int fd;
	inky_config dev;
	struct gpiod_chip *gpio_chip;
	struct gpiod_line *gpio_reset;
	struct gpiod_line *gpio_busy;
	struct gpiod_line *gpio_dc;
} inky_spidev_intf;

/** @defgroup GPIO function user callbacks
 * @{
 */

inky_error_state inky_spidev_gpio_initialize(void *intf_ptr);
inky_error_state inky_spidev_gpio_setup_pin(inky_pin,
					  inky_gpio_direction,
					  inky_pin_state,
					  inky_gpio_pull_up_down,
					  void *intf_ptr);
inky_error_state inky_spidev_gpio_output_state(inky_pin, inky_pin_state,
					     void *intf_ptr);
inky_error_state inky_spidev_gpio_input_state(inky_pin, inky_pin_state*,
					      void *intf_ptr);
inky_error_state inky_spidev_gpio_poll_pin(inky_pin, uint16_t,
					   void *intf_ptr);

/**
 * @}
 */

/** @defgroup SPI function callbacks
 * @{
 */

inky_error_state inky_spidev_spi_setup(void *intf_ptr);

/* Typical kernel callbacks */
inky_error_state inky_spidev_delay(uint32_t delay_us, void *intf_ptr);

/** @brief inky_spidev_spi_write
 *  @param buf ptr to buffer to write
 *  @param len length of buffer to write
 */
inky_error_state inky_spidev_spi_write(const uint8_t* buf, uint32_t len,
				       void *intf_ptr);

/** @brief inky_spidev_spi_write_16
 *  @param UINT16_t buf: ptr to buffer to write
 *  @param UINT32_t len: length of buffer to write
 */
inky_error_state inky_spidev_spi_write_16(uint16_t* buf, uint32_t len,
					  void *intf_ptr);

/**
 * @}
 */

/** @brief Initialize Inky with spidev userspace library
 *  @param special path to spi special file (ex: /dev/spidev1.1)
 */

int8_t inky_spidev_init(inky_spidev_intf *intf_ptr);

int8_t inky_spidev_deinit(inky_spidev_intf *intf_ptr);

#endif /* #ifndef INKY_SPIDEV_H */
