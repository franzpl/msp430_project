#include <msp430g2553.h>
#include "SPI.h"


void SPISetup() {

        // CS is inactive high
        PORT_AD5290_DIR |= CS_AD5290;
        PORT_AD5290_OUT |= CS_AD5290;


        PORT_SPI_SEL0 = MISO | MOSI | CLK; // 1.2 UCA0SIMO, 1.4 UCA0CLK
        PORT_SPI_SEL1 = MISO | MOSI | CLK;

        UCA0CTL1 = UCSWRST;
        UCA0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC; // 3-pin, 8-bit SPI master
        UCA0CTL1 |= UCSSEL_2; // SMCLK
        UCA0BR0 |= 0x02; // Clock prescaler, /2
        UCA0BR1 = 0; // High Byte for Clock /2
        UCA0MCTL = 0; // No modulation
        UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**

        __delay_cycles(100000);

}
