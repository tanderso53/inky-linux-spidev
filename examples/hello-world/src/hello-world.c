/**
 * @file hello-world.c
 *
 * Simple example program using the userspace driver for Pimoroni
 * Ltd.'s Inky E-ink display
 */

#ifdef INKY_SPIDEV_AS_SUBMODULE
#include "inky-spidev.h"
#else
#include <inkyuserspace/inky-spidev.h>
#endif /* #ifdef INKY_SPIDEV_AS_SUBMODULE */

#include "hello-world.h"

#include <unistd.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define APP_ARG_BUFFER 32

#define ARRAY_LEN(array) sizeof(array)/sizeof(array[0])

/* For getopts */
extern char *optarg;
extern int optind, opterr, optopt;

/* Application definitions */

typedef struct {
	const uint8_t *buf;
	size_t index;
	size_t len;
	bool eof;
} bytestream;

void bytestream_init(bytestream *stream, const uint8_t *buf, size_t len);

size_t bytestream_size(bytestream *stream);

uint8_t bytestream_get_next(bytestream *stream);

char spidev[APP_ARG_BUFFER]; /* Path to SPI special device */
char gpiochip[APP_ARG_BUFFER]; /* Search string for gpio chip */

uint8_t reset_pin; /* Offset for reset gpio line */
uint8_t busy_pin; /* Offset for busy gpio line */
uint8_t dc_pin; /* Offset for DC gpio line */

inky_spidev_intf intf; /* Interface configuration */

bool check_numeric(const char *str);

int parse_options(int argc, char *const argv[]);

void error_handler(int8_t rst);

void print_usage();

/* Application Implementation */

void bytestream_init(bytestream *stream, const uint8_t *buf, size_t len)
{
	stream->buf = buf;
	stream->len = len;
	stream->index = 0;
	stream->eof = false;
}

size_t bytestream_size(bytestream *stream)
{
	return stream->len;
}

uint8_t bytestream_get_next(bytestream *stream)
{
	int ret;

	if (stream->index < stream->len) {
		ret = stream->buf[stream->index];
		++stream->index;
	} else {
		ret = -1u;
		stream->eof = true;
	}

	return ret;
}

uint8_t bytestream_get_next(bytestream *stream);

bool check_numeric(const char *str)
{
	char c;
	unsigned int maxloop  = -1;

	if (str == NULL) {
		return false;
	}

	c = str[0];

	for (unsigned int i = 0; i < maxloop; i++) {
		c = str[i];

		if (c == '\0') {
			break;
		}

		if (!isdigit(c)) {
			return false;
		}
	}

	return true;
}

int parse_options(int argc, char *const argv[])
{
	int opt;

	while ((opt = getopt(argc, argv, "r:b:d:s:g:h")) != -1) {
		switch (opt) {
		case 'r':
			if (!check_numeric(optarg)) {
				print_usage();
				exit(EXIT_FAILURE);
			}

			reset_pin = atoi(optarg);

			break;

		case 'b':
			if (!check_numeric(optarg)) {
				print_usage();
				exit(EXIT_FAILURE);
			}

			busy_pin = atoi(optarg);

			break;

		case 'd':
			if (!check_numeric(optarg)) {
				print_usage();
				exit(EXIT_FAILURE);
			}

			dc_pin = atoi(optarg);

			break;

		case 's':
			strncpy(spidev, optarg, APP_ARG_BUFFER - 1);

			break;

		case 'g':
			strncpy(gpiochip, optarg, APP_ARG_BUFFER -1);

			break;

		case 'h':
			print_usage();
			exit(EXIT_SUCCESS);

		default:
			print_usage();
			exit(EXIT_FAILURE);

			break;
		}
	}

	return 0;
}

void error_handler(int8_t rst) {
	if (rst < 0) {
		fprintf(stderr, "ERROR: Inky Transaction "
			"failed with %d\n", rst);

		exit(EXIT_FAILURE);
	}

	if (rst > 0) {
		fprintf(stderr, "WARNING: Inky transaction returned "
			"%d\n", rst);
	}
}

void print_usage() {
	fprintf(stderr,
		"Usage:\n"
		"inky-hello-world -r <pin> -b <pin> -d <pin> -s <spidev> -g <gpiochip>\n"
		"inky-hello-world -h\n"
		"\n"
		"Options:\n"
		"-r <pin>	GPIO Reset Pin offset\n"
		"-b <pin>	GPIO Busy Pin offset\n"
		"-d <pin>	GPIO DC Pin offset\n"
		"-s <special>	Path to SPI device special file\n"
		"-g <chip>	Path, number, or description of GPIO chip\n"
		"-h		Display this usage message\n");
}

uint8_t write_monochrome_img(inky_config *dev, bytestream *img)
{
	const uint8_t th = 0x80; /* Color Threshold */
	uint8_t rst;

	for (size_t h = 0; h < dev->fb->height; ++h) {
		for (size_t w = 0; w < dev->fb->width; ++w) {
			uint8_t value;
			inky_color c;

			/* Check if bit is on or off, then select
			 * black if on */
			value = bytestream_get_next(img);

			if (img->eof) {
				c = INKY_COLOR_WHITE;
			} else if (value < th) {
				c =  INKY_COLOR_BLACK;
			} else {
				c = INKY_COLOR_WHITE;
			}

			/* Load changes into framebuffer */
			rst = inky_fb_set_pixel(dev, w, h, c);
			error_handler(rst);
		}
	}

	return rst;
}

int main(int argc, char *const argv[])
{
	int rst;
	inky_config *dev = &intf.dev;
	bytestream img;

	parse_options(argc, argv);

	bytestream_init(&img, hello_world, ARRAY_LEN(hello_world));

	/* Initialize the interface */
	rst = inky_spidev_init(&intf, spidev, gpiochip, reset_pin,
			       busy_pin, dc_pin);

	if (rst < 0) {
		fprintf(stderr, "ERROR: Failed to initialize interface"
			" with error %d\n", rst);

		return(EXIT_FAILURE);
	}

	/* Run setup function to prepare allocate framebuffer */
	rst = inky_setup(dev);
	error_handler(rst);

	rst = inky_clear(dev);
	error_handler(rst);

	/* Write monochrome image to display */
	write_monochrome_img(dev, &img);

	/* Must call the update function or the image won't be
	 * displayed */
	rst = inky_update(dev);
	error_handler(rst);

	/* Reclaim memory from the framebuffer */
	rst = inky_free(dev);
	error_handler(rst);

	/* Release resources for SPI and GPIO */
	rst = inky_spidev_deinit(&intf);
	error_handler(rst);

	return EXIT_SUCCESS;
}
