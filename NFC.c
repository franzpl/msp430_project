/*Eddie LaCost has adapted the TI example code http://www.ti.com/tool/RF430CL330H-EXAMPLE-CODE (sloc303) to handle
 *SPI communication with RF430CL330H:
 *https://e2e.ti.com/support/wireless_connectivity/nfc_rfid/f/667/t/315391?Initialization-RF430CL330H-NFC-SPI-interface-after-power-up
 *Thanks! */

#include "msp430.h"
#include "NFC.h"
#include "SPI.h"

unsigned char dummy = 0;

unsigned char RxData[2] = {0,0};
unsigned char TxData[2] = {0,0};
unsigned char TxAddr[2] = {0,0};

void NFCSetup(void)

{
        // RST RF430 (in case board remained powered but the MSP430 reset for some reason - MSP430 RST button pushed for example)
        PORT_RST_OUT &= ~RST;
        PORT_RST_DIR |= RST;                    // RF430CL330H device in Reset
        __delay_cycles(1000);
        PORT_RST_OUT |= RST;                    // Release the RF430CL330H from Reset

        __delay_cycles(100000);

        //configure LED1
        PORT_LED_DIR |= LED1;                   // Set as Output
        PORT_LED_OUT &= ~LED1;                  // Start LED off

        //configure pin for INTO interrupt
        PORT_INTO_SEL0 &= ~INTO;                // Confirming GPIO pin functionality
        PORT_INTO_SEL1 &= ~INTO;
        PORT_INTO_DIR &= ~INTO;                 // Set as Input
        PORT_INTO_OUT |= INTO;                  // Set output register high
        PORT_INTO_REN |= INTO;                  // Internal pull-up resistor


        PORT_INTO_IFG &= ~INTO;                 // Clear interrupt flag
        PORT_INTO_IES |= INTO;                  // Set interrupt trigger as high-to-low transition, since INTO will be setup active low below

        __delay_cycles(30000);                // Leave time for the RF430CL33H to get itself initialized; should be 20ms or greater

        //CS Select
        SelectRF430();

       // PORT_LED_OUT |= LED1;
        //__delay_cycles(1000000);
       // while(!(Read_Register(STATUS_REG) & READY)); // Wait until READY bit has been set
       // PORT_LED_OUT &= ~LED1;

}

void Write_Register(unsigned int address, unsigned int value)
{
    TxAddr[0] = address >> 8;
    TxAddr[1] = address & 0x00FF;
    TxData[0] = value >> 8;
    TxData[1] = value & 0x00FF;

    P2OUT &= ~BIT5; //CS

    __delay_cycles(1000);

    while(!(IFG2 & UCB0TXIFG));
    UCB0TXBUF = 0x02; //Write command byte
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send address
    UCB0TXBUF = TxAddr[0];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send address
    UCB0TXBUF = TxAddr[1];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send data
    UCB0TXBUF = TxData[1];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send data
    UCB0TXBUF = TxData[0];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    P2OUT |= BIT5; //CS
}

void Write_Register8(unsigned int address, unsigned int value)
{
    TxAddr[0] = address >> 8;
    TxAddr[1] = address & 0x00FF;
    TxData[0] = value >> 8;
    TxData[1] = value & 0x00FF;

    P2OUT &= ~BIT5; //CS

    __delay_cycles(1000);

    while(!(IFG2 & UCB0TXIFG));
    UCB0TXBUF = 0x02; //Write command byte
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send address
    UCB0TXBUF = TxAddr[0];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send address
    UCB0TXBUF = TxAddr[1];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send data
    UCB0TXBUF = TxData[1];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

//  while(!(UCA0IFG & UCB0TXIFG)); //send data
//  UCA0TXBUF = TxData[0];
//  while(!(UCA0IFG & UCB0RXIFG)); //dummy read
//  dummy = UCA0RXBUF;

    P2OUT |= BIT5; //CS
}

void Write_Register_BIP8(unsigned int address, unsigned int value)
{
    unsigned char BIP8 = 0;

    TxAddr[0] = address >> 8;
    TxAddr[1] = address & 0x00FF;
    TxData[0] = value >> 8;
    TxData[1] = value & 0x00FF;

    P2OUT &= ~BIT5; //CS

    __delay_cycles(1000);

    while(!(IFG2 & UCB0TXIFG));
    UCB0TXBUF = 0x02; //Write command byte
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send address
    UCB0TXBUF = TxAddr[0];
    BIP8 ^= TxAddr[0];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send address
    UCB0TXBUF = TxAddr[1];
    BIP8 ^= TxAddr[1];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send data
    UCB0TXBUF = TxData[0];
    BIP8 ^= TxData[0];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send data
    UCB0TXBUF = TxData[1];
    BIP8 ^= TxData[1];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send BIP8 byte
    UCB0TXBUF = BIP8;
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;


    P2OUT |= BIT5; //CS
}


void Write_Continuous(unsigned int address, unsigned char* write_data, unsigned int data_length)
{
    unsigned int i;
    TxAddr[0] = address >> 8;
    TxAddr[1] = address & 0x00FF;

    //TxData[0] = value >> 8;
    //TxData[1] = value & 0x00FF;

    P2OUT &= ~BIT5; //CS

    __delay_cycles(1000);

    __delay_cycles(15);

    while(!(IFG2 & UCB0TXIFG));
    UCB0TXBUF = 0x02; //Write command byte
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send address
    UCB0TXBUF = TxAddr[0];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send address
    UCB0TXBUF = TxAddr[1];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    for(i = 0; i < data_length; i++)
    {
        while(!(IFG2 & UCB0TXIFG)); //send data
        UCB0TXBUF = write_data[i];
        while(!(IFG2 & UCB0RXIFG)); //dummy read
        dummy = UCB0RXBUF;
    }


    P2OUT |= BIT5; //CS
}

unsigned int Read_Register(unsigned int address)
{
    TxAddr[0] = address >> 8;
    TxAddr[1] = address & 0x00FF;

    P2OUT &= ~BIT5; //CS
    __delay_cycles(15);

    while(!(IFG2 & UCB0TXIFG));
    //UCA0TXBUF = 0x03; //Read command byte
    UCB0TXBUF = 0x0B; //Read command byte
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send address
    UCB0TXBUF = TxAddr[0];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send address
    UCB0TXBUF = TxAddr[1];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //dummy write
    UCB0TXBUF = 0;
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //dummy write
    UCB0TXBUF = 0;
    while(!(IFG2 & UCB0RXIFG)); //read data
    RxData[0] = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //dummy write
    UCB0TXBUF = 0;
    while(!(IFG2 & UCB0RXIFG)); //read data
    RxData[1] = UCB0RXBUF;

    P2OUT |= BIT5; //CS
    __delay_cycles(1000);

    return (RxData[1] << 8 | RxData[0]);
}

unsigned int Read_Register_BIP8(unsigned int address)
{
    unsigned char BIP8 = 0;
    TxAddr[0] = address >> 8;
    TxAddr[1] = address & 0x00FF;

    P2OUT &= ~BIT5; //CS

    __delay_cycles(1000);

    while(!(IFG2 & UCB0TXIFG));
    //UCA0TXBUF = 0x03; //Read command byte
    UCB0TXBUF = 0x0B; //Read command byte
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send address
    UCB0TXBUF = TxAddr[0];
    BIP8 ^= TxAddr[0];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send address
    UCB0TXBUF = TxAddr[1];
    BIP8 ^= TxAddr[1];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //dummy write
    UCB0TXBUF = 0;
    BIP8 ^= 0;
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //dummy write
    UCB0TXBUF = 0;
    while(!(IFG2 & UCB0RXIFG)); //read data
    RxData[0] = UCB0RXBUF;
    BIP8 ^= RxData[0];

    while(!(IFG2 & UCB0TXIFG)); //dummy write
    UCB0TXBUF = 0;
    while(!(IFG2 & UCB0RXIFG)); //read data
    RxData[1] = UCB0RXBUF;
    BIP8 ^= RxData[1];

    while(!(IFG2 & UCB0TXIFG)); //dummy write
    UCB0TXBUF = 0;
    while(!(IFG2 & UCB0RXIFG)); //read BIP8 byte
    if(BIP8 != UCB0RXBUF)
        __no_operation();

    P2OUT |= BIT5; //CS

    return (RxData[0] << 8 | RxData[1]);
}

void Read_Continuous(unsigned int address, unsigned char* read_data, unsigned int data_length)
{
    unsigned int i;
    TxAddr[0] = address >> 8;
    TxAddr[1] = address & 0x00FF;

    P2OUT &= ~BIT5; //CS

    __delay_cycles(1000);

    while(!(IFG2 & UCB0TXIFG));
    //UCA0TXBUF = 0x03; //Read command byte
    UCB0TXBUF = 0x0B; //Read command byte
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send address
    UCB0TXBUF = TxAddr[0];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //send address
    UCB0TXBUF = TxAddr[1];
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    while(!(IFG2 & UCB0TXIFG)); //dummy write
    UCB0TXBUF = 0;
    while(!(IFG2 & UCB0RXIFG)); //dummy read
    dummy = UCB0RXBUF;

    for(i = 0; i < data_length; i++)
    {
        while(!(IFG2 & UCB0TXIFG)); //dummy write
        UCB0TXBUF = 0;
        while(!(IFG2 & UCB0RXIFG)); //read data
        read_data[i] = UCB0RXBUF;
    }

    P2OUT |= BIT5; //CS
}

