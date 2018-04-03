#include <msp430g2553.h>
#include "CurrentSource.h"
#include "SPI.h"

void SetOutputCurrent(unsigned char AD5290_register) {

        P1OUT &= (~CS_AD5290); // when CS is low, data is loaded into the serial register

        while (!(IFG2 & UCA0TXIFG)); // USCI_A0 TX buffer ready?
        UCA0TXBUF = AD5290_register; // Send 0xxx over SPI to Slave (0x17 = 200uA)

        __delay_cycles(100000);
        P1OUT |= (CS_AD5290); // Transfer data to RDAC register when CS returns to high
}


