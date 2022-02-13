#include <inky-spidev.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

inky_error_state inky_user_gpio_initialize()
{}
inky_error_state inky_user_gpio_setup_pin(inky_pin,
					  inky_gpio_direction,
					  inky_pin_state,
					  inky_gpio_pull_up_down)
{}
inky_error_state inky_user_gpio_output_state(inky_pin, inky_pin_state)
{}
inky_error_state inky_user_gpio_input_state(inky_pin, inky_pin_state*)
{}
inky_error_state inky_user_gpio_poll_pin(inky_pin, uint16_t)
{}

inky_error_state inky_user_spi_setup()
{}

inky_error_state inky_user_delay(uint32_t delay_us)
{}

inky_error_state inky_user_spi_write(const uint8_t* buf, uint32_t len,
				     void *intf_ptr)
{
	int rst;
	inky_spidev_intf *iptr = (inky_spidev_intf*) intf_ptr;

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long) buf,
		.rx_buf = 0,
		.len = len
		.delay_usecs = 0,
		.speed_hz = INKY_SPIDEV_SPEED,
		.bits_per_word = 8
	};

	rst = ioctl(iptr->fd, SPI_IOC_MESSAGE(1), &tr);

	if (rst < 0)
		return INKY_E_FAILURE;

	return INKY_OK;
}

inky_error_state inky_user_spi_write16(const uint16_t* buf, uint32_t len)
{
	int rst;
	inky_spidev_intf *iptr = (inky_spidev_intf*) intf_ptr;

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long) buf,
		.rx_buf = 0,
		.len = len
		.delay_usecs = 0,
		.speed_hz = INKY_SPIDEV_SPEED,
		.bits_per_word = 16
	};

	rst = ioctl(iptr->fd, SPI_IOC_MESSAGE(1), &tr);

	if (rst < 0)
		return INKY_E_FAILURE;

	return INKY_OK;
}

int8_t inky_spidev_init(const char *special)
{}

int8_t inky_spidev_deinit()
{}
