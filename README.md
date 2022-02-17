# Inky Linux Userspace Driver

by Tyler Anderson

## Introduction

The Inky Linux Userspace Driver provides a wrapper around
the general-purpose driver for the Pimoroni Inky-branded e-ink
display from [Pimoroni Ltd.](https://shop.pimoroni.com/).

As the driver is a userspace driver, it uses spidev for SPI
communication and libgpiod to control and read the gpio pins.

## License

The inky linux userspace driver is copyright Tyler Anderson,
2022. Use, modification, and distribution is allowed and
welcome under the terms of the BSD 3-Clause license. See
[LICENSE](LICENSE) for more details.

## Building and Installing

This project uses CMake as a build system.

Unpack the source tarball `inky-linux-spidev.tbz`, then `cd` to
the source root. Then enter the following commands:

``` bash
# Build with docs (Requires Doxygen)
cmake -DINKY_BUILD_DOCS=true -S . -B build

# OR Build without docs
#cmake -S . -B build

# Install library
cmake --install build

# OR Install locally
#cmake --install build --prefix ~/.local
```

## Usage

### As submodule

Unpack the library to your project's source directory tree in the
desired location (Example: project_root/lib/inky-linux-spidev).
Then add the following to the project's `CMakeLists.txt` (if using CMake):
`add_subdirectory(path/to/library)` and add `inkyuserspace-static` to your
link libraries list. Then to use in C source:

``` c
#include "inky-spidev.h"
```

If using Unix Makefiles, you will need to manually point to the include
directories and C source files in the library in your Makefile. Angular
brackets may be required depending on your project setup.

### As a system library

Once installed on the system, the only thing required is to point to the
proper header in your C source files. This should be as follows:

``` c
#include <inkyuserspace/inky-spidev.h>
```

You will also need to link to the library as well with the
`-linkyuserspace` option (for clang or gcc). In CMake this can be
accomplished by adding `inkyuserspace` to your `target_link_libraries()`
list.

### Using the library in C

Before the INKY API functions can by called, the linux userspace driver
must be initialized. Then when done with the Inky, the userspace driver
should be de-initialized to return resources back to the system. This
is done as follows:

``` c
#include <inkyuserspace/inky-spidev.h>

int main()
{
    int ret;

    inky_spidev_intf intf; /* struct for passing driver data */

    /* Info to setup spi and gpio devices -- may want to get from
       arguments */
    const char* spidev = "/dev/spidev1.1";
    const char* gpiochip = "/dev/gpiochip0";
    uint8_t reset_pin = 1;
    uint8_t busy_pin = 2;
    uint8_t dc_pin = 3;

    ret = inky_spidev_init(&intf, spidev, gpiochip, reset_bin,
                           busy_pin, dc_pin);
    
    if (ret < 0) {
        return ret;
    }

    /* Add your Inky API functions here (See docs for functions) */

    ret = inky_spidev_deinit(&intf);

    return ret;
}
```

API functions for the inky can be be found in the
[`inky-api.h`](lib/pimoroni-inky-driver/include/inky-api.h)
header file in the Pimoroni Inky Driver submodule. This is installed
by default to `/usr/local/include/inkyuserspace/inky-api.h`. Alternatively
if you build the documentation the API documentation can be found by default in
`/usr/local/share/doc/inkyuserspace/html`.
