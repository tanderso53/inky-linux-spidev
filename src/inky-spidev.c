#include <inky-spidev.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

static gpiod_line *get_line_struct(inky_spidev_intf *intf_ptr,
				   inky_pin gpin);

/*
**********************************************************************
******************* USER API IMPLEMENTATION **************************
**********************************************************************
*/

inky_error_state inky_spidev_gpio_initialize(void *intf_ptr)
{
	return INKY_OK;
}

inky_error_state inky_spidev_gpio_setup_pin(inky_pin gpin,
					    inky_gpio_direction gdir,
					    inky_pin_state gstate,
					    inky_gpio_pull_up_down gcfg,
					    void *intf_ptr)
{
	int rst;
	struct gpio_line *this_line;
	struct gpiod_line_request_config cfg;
	int pinstate;
	inky_spidev_intf *iptr = (inky_spidev_intf*) intf_ptr;

	/* zero out line request struct */
	cfg.consumer = INKY_SPIDEV_CONSUMER;
	cfg.request_type = 0;
	cfg.flags = 0; /* TODO: GPIOD_LINE_REQUEST_FLAG_ACTIVE_LOW? */

	/* Select appropriate pin config
	 *
	 * TODO: Create helper function for this?
	 */
	switch (gpin) {
	INKY_PIN_RESET:
		this_line = iptr->gpio_reset;
		break;
	INKY_PIN_BUSY:
		this_line = iptr->gpio_busy;
		break;
	INKY_PIN_CD:
		this_line = iptr->gpio_cd;
		break;
	default:
		return INKY_E_NOT_CONFIGURED;
		break;
	}

	/* Select direction */
	switch (gdir) {
	INKY_DIR_IN:
		cfg->request_type = GPIOD_LINE_REQUEST_DIRECTION_INPUT;
		break;
	INKY_DIR_OUT:
		cfg->request_type = GPIOD_LINE_REQUEST_DIRECTION_OUTPUT;
		break;
	}

	/* Select any extra required flags */
	switch (gcfg) {
	INKY_PINCFG_OFF:
		cfg->flags = cfg->flags | GPIOD_LINE_REQUEST_FLAG_BIAS_DISABLE;
		break;
	INKY_PINCFG_PULLUP:
		cfg->flags = cfg->flags | GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP;
		break;
	INKY_PINCFG_PULLDOWN:
		cfg->flags = cfg->flags | GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_DOWN;
		break;
	}

	/* Set initial pin value */
	if (gstate == INKY_PINSTATE_HIGH) {
		pinstate = 1;
	} else {
		pinstate = 0;
	}

	/* Send request for the line, failing if less than 0 returned */
	rst = gpiod_line_request(this_line, &cfg, pinstate);

	if (rst < 0) {
		return INKY_E_NOT_CONFIGURED;
	}

	return INKY_OK;
}

inky_error_state inky_spidev_gpio_output_state(inky_pin gpin,
					       inky_pin_state gstate,
					       void *intf_ptr)
{
	int rst;
	struct gpio_line *this_line;
	int pinstate;
	inky_spidev_intf *iptr = (inky_spidev_intf*) intf_ptr;
	
	this_line = get_line_struct(iptr, gpin);

	if (gstate == INKY_PINSTATE_HIGH) {
		pinstate = 1;
	} else {
		pinstate = 0;
	}

	rst = gpiod_line_set_value(this_line, pinstate);

	if (rst < 0) {
		return INKY_E_FAILURE;
	}

	return INKY_OK;
}

inky_error_state inky_spidev_gpio_input_state(inky_pin gpin,
					      inky_pin_state* out,
					      void *intf_ptr)
{
	int rst;
	struct gpio_line *this_line;
	inky_spidev_intf *iptr = (inky_spidev_intf*) intf_ptr;

	if (!out) {
		return INKY_E_NULL_PTR;
	}

	this_line = get_line_struct(iptr, gpin);

	rst = gpio_line_get_value(this_line);

	if (rst < 0) {
		return INKY_E_FAILURE;
	}

	/* These might flip if active low is flagged in */
	if (rst == 1) {
		*out = INKY_PINSTATE_HIGH;
	} else {
		*out = INKY_PINSTATE_LOW;
	}

	return INKY_OK;
}

inky_error_state inky_spidev_gpio_poll_pin(inky_pin gpin,
					   uint16_t timeout,
					   void *intf_ptr)
{
	int rst = 0;
	inky_pin_state pinstate;
	struct gpio_line *this_line;
	inky_spidev_intf *iptr = (inky_spidev_intf*) intf_ptr;

	rst = iptr->dev->gpio_input_cb(gpin, &pinstate, intf_ptr);

	if (rst < 0) {
		return INKY_E_FAILURE;
	}

	/* Return early if pin is not set */
	if (pinstate == INKY_PINSTATE_HIGH) {
		return INKY_OK;
	}

	this_line = get_line_struct(iptr, gpin);

	if (!this_line) {
		return INKY_E_NULL_PTR;
	}

	rst = gpiod_line_request_rising_edge_events(this_line,
						    "INKY_BUSY_WAIT");

	if (rst < 0) {
		return INKY_E_NOT_AVAILABLE;
	}

	/* Wait for line to go high */
	rst = gpiod_line_event_wait(this_line, timeout);

	if (rst < 0) {
		return INKY_E_FAILURE;
	}

	if (rst == 0) {
		return INKY_E_TIMEOUT;
	}

	return INKY_OK;
}

inky_error_state inky_spidev_spi_setup(void *intf_ptr)
{
	int err;
	uint8_t mode = SPI_MODE_0;
	uint8_t bits = INKY_SPI_BITS_DEFAULT;
	uint8_t speed = INKY_SPI_SPEED_HZ_MAX;
	inky_spidev_intf *iptr = (inky_spidev_intf*) intf_ptr;

	iptr->fd = open(iptr->special, O_RDWR);

	if (iptr->fd < 0) {
		return errno == EACCES ? INKY_E_BAD_PERMISSIONS :
			INKY_E_COMM_FAILURE;
	}

	err = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (err == -1) {
		return INKY_E_COMM_FAILURE;
	}

	err = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (err == -1) {
		return INKY_E_COMM_FAILURE;
	}

	err = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (err == -1) {
		return INKY_E_COMM_FAILURE;
	}
}

inky_error_state inky_spidev_delay(uint32_t delay_us, void *intf_ptr)
{
	int rst;

	rst = usleep(delay_us);

	if (rst < 0) {
		return INKY_E_FAILURE;
	}

	return INKY_OK;
}

inky_error_state inky_spidev_spi_write(const uint8_t* buf, uint32_t len,
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

inky_error_state inky_spidev_spi_write16(const uint16_t* buf, uint32_t len)
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

int8_t inky_spidev_init(inky_spidev_intf *intf_ptr, const char* spidev,
			const char* gpiochip, unsigned int reset_offset,
			unsigned int busy_offset, unsigned int dc_offset)
{
	intf_ptr->gpio_chip = gpiod_chip_open_lookup(gpiochip);
	intf_ptr->gpio_reset = gpiod_chip_get_line(intf_ptr->gpio_chip,
						   reset_offset);
	intf_ptr->gpio_busy = gpiod_chip_get_line(intf_ptr->gpio_chip,
						   busy_offset);
	intf_ptr->gpio_dc = gpiod_chip_get_line(intf_ptr->gpio_chip,
						   dc_offset);

	if (!intf_ptr->gpio_chip || !intf_ptr->gpio_reset
	    || !intf_ptr->gpio_busy || !intf_ptr->gpio_dc) {
		return -1;
	}

	return 0;
}

int8_t inky_spidev_deinit(inky_spidev_intf *intf_ptr)
{
	close(intf_ptr->fd);
	gpiod_chip_close(intf_ptr->gpio_chip);

	return 0;
}
/*
**********************************************************************
********************** INTERNAL API FUNCTIONS ************************
**********************************************************************
*/

static gpiod_line *get_line_struct(inky_spidev_intf *intf_ptr,
				   inky_pin gpin)
{
	gpiod_line *this_line;

	/* Select appropriate pin config */
	switch (gpin) {
	INKY_PIN_RESET:
		this_line = iptr->gpio_reset;
		break;
	INKY_PIN_BUSY:
		this_line = iptr->gpio_busy;
		break;
	INKY_PIN_CD:
		this_line = iptr->gpio_cd;
		break;
	default:
		this_line = NULL;
		break;
	}

	return this_line;
}
