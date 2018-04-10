/*
 * {RF430_example.c}
 *
#include "msp430.h"
#include "RF430_SPI_example.h"

unsigned char dummy = 0;

unsigned char RxData[2] = {0,0};
unsigned char TxData[2] = {0,0};
unsigned char TxAddr[2] = {0,0};

void Write_Register(unsigned int address, unsigned int value)
{
    TxAddr[0] = address >> 8;
    TxAddr[1] = address & 0x00FF;
    TxData[0] = value >> 8;
    TxData[1] = value & 0x00FF;

    P2OUT &= ~BIT5; //CS

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

    return (RxData[1] << 8 | RxData[0]);
}

unsigned int Read_Register_BIP8(unsigned int address)
{
    unsigned char BIP8 = 0;
    TxAddr[0] = address >> 8;
    TxAddr[1] = address & 0x00FF;

    P2OUT &= ~BIT5; //CS

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

