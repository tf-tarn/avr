
    make all

will build the firmware, a utility program for converting raw images
(one byte per pixel; zero is assumed to be black, all other values are
mapped to white) to a format suitable for the OLED, and it will also
use the utility to convert an included sample image. You can simply
cat the resulting image to the device, e.g.:

    cat build/bowman.oled > /dev/ttyUSB1
