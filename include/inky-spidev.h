#ifndef INKY_SPIDEV_H
#define INKY_SPIDEV_H

#include "inky.h"
#include <stdint.h>

/** @brief interface object for inky-spidev driver
 */
typedef struct {
	char special[64];
	int fd;
	inky_config dev;
} inky_spidev_intf;

/** @defgroup GPIO function user callbacks
 * @{
 */

inky_error_state inky_user_gpio_initialize();
inky_error_state inky_user_gpio_setup_pin(inky_pin,
					  inky_gpio_direction,
					  inky_pin_state,
					  inky_gpio_pull_up_down);
inky_error_state inky_user_gpio_output_state(inky_pin, inky_pin_state);
inky_error_state inky_user_gpio_input_state(inky_pin, inky_pin_state*);
inky_error_state inky_user_gpio_poll_pin(inky_pin, uint16_t);

/**
 * @}
 */

/** @defgroup SPI function callbacks
 * @{
 */

inky_error_state inky_user_spi_setup();

/* Typical kernel callbacks */
inky_error_state inky_user_delay(uint32_t delay_us);

/** @brief inky_user_spi_write
 *  @param buf ptr to buffer to write
 *  @param len length of buffer to write
 */
inky_error_state inky_user_spi_write(const uint8_t* buf, uint32_t len);

/** @brief inky_user_spi_write_16
 *  @param UINT16_t buf: ptr to buffer to write
 *  @param UINT32_t len: length of buffer to write
 */
inky_error_state inky_user_spi_write_16(uint16_t* buf, uint32_t len);

/**
 * @}
 */

/** @brief Initialize Inky with spidev userspace library
 *  @param special path to spi special file (ex: /dev/spidev1.1)
 */

int8_t inky_spidev_init(inky_spidev_intf *intf_ptr);

int8_t inky_spidev_deinit(inky_spidev_intf *intf_ptr);

#endif /* #ifndef INKY_SPIDEV_H */
