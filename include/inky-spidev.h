#ifndef INKY_SPIDEV_H
#define INKY_SPIDEV_H

#include "inky.h"

#include <gpiod.h>

#include <stdint.h>

/**
 * @defgroup inkyspidevapi Inky Linux Userspace API
 * @{
 */

#define INKY_SPIDEV_CONSUMER "inky-spidev"
#define INKY_SPIDEV_SPEED 800000
#define INKY_SPIDEV_SPECIAL_LEN 64

/** @brief interface object for inky-spidev driver
 *
 * This must be filled in and passed to init prior to use of the
 * interface.
 */
typedef struct {
	char special[INKY_SPIDEV_SPECIAL_LEN];
	int fd;
	inky_config dev;
	struct gpiod_chip *gpio_chip;
	struct gpiod_line *gpio_reset;
	struct gpiod_line *gpio_busy;
	struct gpiod_line *gpio_dc;
	inky_color_config color_cfg;
} inky_spidev_intf;

/** @defgroup inkyspidevgpiocb GPIO function user callbacks
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
inky_error_state inky_spidev_gpio_poll_pin(inky_pin, uint64_t,
					   void *intf_ptr);

/**
 * @}
 */

/** @defgroup inkyspidevspicb SPI function callbacks
 * @{
 */

inky_error_state inky_spidev_spi_setup(void *intf_ptr);

/* Typical kernel callbacks */
inky_error_state inky_spidev_delay(uint32_t delay_us, void *intf_ptr);

/** @brief User callback to write byte to SPI
 *  @param buf ptr to buffer to write
 *  @param len length of buffer to write
 */
inky_error_state inky_spidev_spi_write(const uint8_t* buf, uint32_t len,
				       void *intf_ptr);

/** @brief User callback to write 16bit word to SPI
 *  @param buf Ptr to buffer to write
 *  @param len Length of buffer to write
 */
inky_error_state inky_spidev_spi_write_16(uint16_t* buf, uint32_t len,
					  void *intf_ptr);

/**
 * @}
 */

/**
 * @defgroup inkyspidevinit Userspace initialization functions
 * @{
 */

/** @brief Initialize Inky with spidev userspace library
 *  @param intf_ptr Interface driver device pointer
 *  @param spidev Path to spi special file, ex: /dev/spidev1.1
 *  @param gpiochip Device path or description of gpio chip device
 *  @param reset_offset GPIO line offset for reset pin
 *  @param busy_offset GPIO line offset for busy pin
 *  @param dc_offset GPIO line offset for dc_offset
 */
int8_t inky_spidev_init(inky_spidev_intf *intf_ptr, const char* spidev,
			const char* gpiochip, unsigned int reset_offset,
			unsigned int busy_offset, unsigned int dc_offset);

/** @brief Deinitialize and return resources to GPIO and SPI devices
 *  @param intf_ptr Device interface pointer
 */
int8_t inky_spidev_deinit(inky_spidev_intf *intf_ptr);

/**
 * @}
 */

/**
 * @}
 */

#endif /* #ifndef INKY_SPIDEV_H */
