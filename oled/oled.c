#include <avr/io.h>
#include "spi.h"
#include "oled.h"

#define COMMAND_MODE() PORTA.OUTCLR = (1 << 2)
/* PORTCbits.RC3 = 0; // command mode */
#define DATA_MODE() PORTA.OUTSET = (1 << 2)
/* PORTCbits.RC3 = 1; // data mode */


void oled_command(unsigned char c) {
    COMMAND_MODE();
    spi_write1(c);
}

void oled_data(unsigned char c) {
    DATA_MODE();
    spi_write1(c);
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

#define vccstate (!SSD1306_EXTERNALVCC)

void oled_init() {

  oled_command(SSD1306_DISPLAYOFF);                    // 0xAE
  oled_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
  oled_command(0x80);                                  // the suggested ratio 0x80

  oled_command(SSD1306_SETMULTIPLEX);                  // 0xA8
  oled_command(SSD1306_LCDHEIGHT - 1);

  oled_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
  oled_command(0x0);                                   // no offset
  oled_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
  oled_command(SSD1306_CHARGEPUMP);                    // 0x8D
  if (vccstate == SSD1306_EXTERNALVCC)
    { oled_command(0x10); }
  else
    { oled_command(0x14); }
  oled_command(SSD1306_MEMORYMODE);                    // 0x20
  oled_command(0x00);                                  // 0x0 act like ks0108
  oled_command(SSD1306_SEGREMAP | 0x1);
  oled_command(SSD1306_COMSCANDEC);

 #if defined SSD1306_128_32
  oled_command(SSD1306_SETCOMPINS);                    // 0xDA
  oled_command(0x02);
  oled_command(SSD1306_SETCONTRAST);                   // 0x81
  oled_command(0x8F);

#elif defined SSD1306_128_64
  oled_command(SSD1306_SETCOMPINS);                    // 0xDA
  oled_command(0x12);
  oled_command(SSD1306_SETCONTRAST);                   // 0x81
  if (vccstate == SSD1306_EXTERNALVCC)
    { oled_command(0x9F); }
  else
    { oled_command(0xCF); }

#elif defined SSD1306_96_16
  oled_command(SSD1306_SETCOMPINS);                    // 0xDA
  oled_command(0x2);   //ada x12
  oled_command(SSD1306_SETCONTRAST);                   // 0x81
  if (vccstate == SSD1306_EXTERNALVCC)
    { oled_command(0x10); }
  else
    { oled_command(0xAF); }

#endif

  oled_command(SSD1306_SETPRECHARGE);                  // 0xd9
  if (vccstate == SSD1306_EXTERNALVCC)
    { oled_command(0x22); }
  else
    { oled_command(0xF1); }
  oled_command(SSD1306_SETVCOMDETECT);                 // 0xDB
  oled_command(0x40);
  oled_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
  oled_command(SSD1306_NORMALDISPLAY);                 // 0xA6

  oled_command(SSD1306_DEACTIVATE_SCROLL);

  oled_command(SSD1306_DISPLAYON);//--turn on oled panel
}

void oled_spi_init() {

    SPI0.CTRLA = SPI_PRESC_DIV128_gc
        | SPI_MASTER_bm // master
        ;

    SPI0.CTRLB = SPI_MODE_0_gc
        | SPI_BUFEN_bm // BUFEN = buffer mode
        | SPI_SSD_bm // disable multi-master mode
        ;

    SPI0.CTRLA |= SPI_ENABLE_bm; // enable
    
    /*
      enable
      0000 unused x 4
      lsbf = 0
      1 = master / 0 = slave
      bmode
      ---
      bmode = 1 => SPIxTWIDTH bits for every transfer (0 = 8 bit transferred; non-zero N = N bits transferred);
      bmode = 1 => SPIxTCT[HL] specifies how many transfers are made

     */

    /* SPI1CON0 = 0b00000011; */

    /*
      SDI = sample phase
      1 = data out changes on active to idle / 0 = idle to active
      1 = SCK idle high / 0 = SCK idle low
      1 = delay to first SCK may be less than 1/2 baud period / 0 = may not
      0 unused
      1 = slave select active low / 0 = high
      1 = SDI active low / 0 = high
      1 = SDO active low / 0 = high
      ---
     */

    /* SPI1CON1 = 0b11000100; */

    /*
      0 busy
      1 fault status
      . 000 unused x 3
      5 slave select control
      6 txr
      7 rxr
      ---
      txr & !rxr => transmit-only mode
     */

    /* SPI1CON2 = 0b10000010; */

    /*
      0000 unused x 4
      CLKSEL3-0
      --
      SPI1CLK
     */
    // baud = (64000000) / (2 * (p + 1))
    // 64000000 / baud = 2 * (p + 1)
    // 32000000 / baud = p + 1
    // 32000000 / baud - 1 = p
    // (- (/ 32000000 10000) 1) = 3199
    // (- (/ 32000000 115200) 1) = 276   (* 115200 4)
    // (- (/ 32000000 460800) 1) = 68
    // (- (/ 500000 10000) 1) = 49
    // (- (/ 32000000 8000000) 1) = 3
    /* SPI1BAUD = 49; // 10000 */
    /* SPI1CLK = 0b0010; // MFINTOSC */

    /* SPI1BAUD = 3; // 10000 */
    /* SPI1CLK = 0b0001; // HFINTOSC */

    /* BMODE = 1; */

    /* SPI1TWIDTH = 0; */

    // Now enable.
    /* SPI1CON0bits.EN = 1; */
}

void oled_clear() {
    for (int j = 0; j < 128 * 8; ++j) {
        oled_data(0);
    }
}
