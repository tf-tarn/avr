
    make all

will build the firmware, a utility program for converting raw images
to a formwat suitable for the OLED, and it will also use the utility
to convert an included sample image. Images in this format must be
written to the device slowly, via e.g.:

    pv -q -L 64 < build/bowman.oled > /dev/ttyUSB1
