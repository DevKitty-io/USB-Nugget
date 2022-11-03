## Building / flashing from source

If want to flash the latest version of the USB-Nugget project, [the method
documented
here](https://github.com/HakCat-Tech/USB-Nugget#how-to-update-your-nugget) is
easiest.

Docker is used to produce a replicable build environment to ensure
library and tool dependencies are met. Linux is recommended when developing,
building, and flashing from source.

### Linux

Dependencies: docker, make.
```
# Clone the repo
git clone https://github.com/HakCat-Tech/USB-Nugget.git

# Navigate to src
cd USB-Nugget/src

# Build
sudo make build

# Build and flash the ESP32-S2 (may need sudo)
sudo make flash
#optionally specify port: sudo make flash PORT=<port>
#optionally install default scripts: sudo make flash RESET_SCRIPTS_DURING_FLASH=true

# Produce .img for release
sudo make generate_bin
```

### macOS

Docker on mac has issues connecting to usb devices and complicates the flashing
process. A workaround is in place -- USB-Nugget will be built within docker but
then flashed outside of the container.  Some additional steps are also
necessary to prepare your development environment:

1. Install docker and ensure the daemon is running. The daemon is running if the
   command `docker info` is successful.
2. Install python3 and pip3, then `pip3 install esptool`

```bash
# Clone the repo
git clone https://github.com/HakCat-Tech/USB-Nugget.git

# Navigate to src
cd USB-Nugget/src

# Verify everything is installed and configured correctly
make check-mac-deps

# To build only, run
make mac-build

# Plug in your USB-Nugget dev kit in flashing mode by holding down the '0'
# button while plugging it in.

# Find the port of your arduino. It's likely something like
# "/dev/cu.usbmodem01".
ls /dev/cu*

# To build and flash, run
make flash-on-mac PORT=<port>

# If you want to overwrite the FAT partition to include default payloads, set
# RESET_SCRIPTS_DURING_FLASH=true:
make flash PORT=<port> RESET_SCRIPTS_DURING_FLASH=true
```

### Windows

Not currently supported
