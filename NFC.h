#ifndef NFC_H_
#define NFC_H_

#include <msp430g2553.h>

void SPI_Write_Register(unsigned int address, unsigned int value);
void SPI_Write_Register8(unsigned int address, unsigned int value);
void SPI_Write_Register_BIP8(unsigned int address, unsigned int value);
void SPI_Write_Continuous(unsigned int address, unsigned char* write_data, unsigned int data_length);
unsigned int SPI_Read_Register(unsigned int address);
unsigned int SPI_Read_Register_BIP8(unsigned int address);
void SPI_Read_Continuous(unsigned int address, unsigned char* read_data, unsigned int data_length);


#endif
