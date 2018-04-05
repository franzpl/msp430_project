#include <msp430g2553.h>
//
//              RF430CL330H                   MSP430G2553
//                  (Slave)                    Master
//             _________________     _________________
//            |             MOSI|<-|----+->|P1.7/UCB0SIMO    |
//            |                 |  | SPI   |                 |
//            |             MISO|<-+------>|P1.6/UCB0SOMI    |
//            |               CS|<---------|P2.5             |
//      GND<--|E(2-0)           |          |                 |
//            |                 |          |                 |
//            |                 |          |                 |
//            |                 |          |                 |
//            |_________________|          |_________________|
//

// SPI is chosen if SCMS/CS-Pin (9) isn't tied low externally
// SPI communication is initiated by the master by pulling the CS pin low

#define CONTROL_REG         0xFFFE // Control Register
#define STATUS_REG          0xFFFC // Status Register
#define INT_ENABLE_REG      0xFFFA // Interrupt Enable
#define INT_FLAG_REG        0xFFF8 // Interrupt Flags
#define CRC_RESULT_REG      0xFFF6/ // CRC Result
#define CRC_LENGTH_REG      0xFFF4 // CRC Length
#define CRC_START_ADDR_REG  0xFFF2 // CRC Start Address
#define COMM_WD_CTRL_REG    0xFFF0 // Communication Watchdog Control Register
#define VERSION_REG         0xFFEE //contains the software version of the ROM

unsigned char dummy = 0;

unsigned char RxData[2] = {0,0};
unsigned char TxAddr[2] = {0,0};
unsigned char TxData[2] = {0,0};

unsigned char results[11];

unsigned int Result = 0;

void SPI_Write_Register(unsigned int address, unsigned int value);
void SPI_Write_Register_BIP8(unsigned int address, unsigned int value);
void SPI_Write_Continuous(unsigned int address, unsigned char* write_data, unsigned int data_length);
unsigned int SPI_Read_Register(unsigned int address);
unsigned int SPI_Read_Register_BIP8(unsigned int address);
void SPI_Read_Continuous(unsigned int address, unsigned char* read_data, unsigned int data_length);
void SPI_Write_Register8(unsigned int address, unsigned int value);
void SPI_init(void);

unsigned char NDEF_Application_Data[] =
{

        /*NDEF Tag Application */
        0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01,

        0xE1, 0x03,     /*Capability Container ID*/

        /* CC file start */
        0x00, 0x0F,     /* CCLEN 15bytes fix*/
        0x20,     /* Mapping version 2.0 */
        0x00, 0xF0,     /* MLe (49 bytes); Maximum R-APDU data size */
        0x00, 0xF0,     /* MLc (52 bytes); Maximum C-APDU data size */
        0x04,     /* Tag, File Control TLV (4 = NDEF file) */
        0x06,     /* Length, File Control TLV (6 = 6 bytes of data for this tag) */
        0xE1, 0x04,     /* Type4 Tag File Identifier */
        0x0B, 0xDF,     /* Max NDEF size (3037 bytes of RF430CL330 useable memory) */
        0x00,     /* NDEF file read access condition, read access without any security */
        0x00,     /* NDEF file write access condition; write access without any security */
        /* CC file end */

        0xE1, 0x04,     /* NDEF File ID */

           //NDEF File for Hello World  (48 bytes total length)
        0x00, 0x14, //NLEN; NDEF length (3 byte long message)
        0xD1, 0x01, 0x10,
        0x54, //T
        0x02,
        0x65, 0x6E, //'e', 'n',
        0x48, 0x65, 0x6C, 0x6C, 0x6f, 0x2c, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21//'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!' //NDEF data

    } ;/* End of data */

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL  = CALDCO_1MHZ;

	SPI_init();
	__delay_cycles(100000);

	SPI_Write_Continuous(0, NDEF_Application_Data, 48);     //Write NDEF message

	    __delay_cycles(15);                           // required time for CS to stay high in between read and write

	    SPI_Write_Register(INT_ENABLE_REG, 0x0002);   // enables IRQ

	    __delay_cycles(15);                           // required time for CS to stay high in between read and write

	    SPI_Write_Register(CONTROL_REG, 0x1E);        // enables RF

	    __delay_cycles(15);                           // required time for CS to stay high in between read and write

	    Result = SPI_Read_Register(CONTROL_REG);      // read control register

	//  __delay_cycles(15);                           // required time for CS to stay high in between read and write

	//  Result = SPI_Read_Register(CONTROL_REG);      // read control register

	    __no_operation();

	  while(1)
	  {

	  }

}

void SPI_init(){

    P2OUT |= BIT5; // P2.5 = CS;
    P2DIR |= BIT5;
    P1SEL = BIT5 | BIT6 | BIT7;  // P1.5 = CLK, P1.6 = MISO, P1.7 = MOSI
    P1SEL2 = BIT5 | BIT6 | BIT7;  // P1.5 = CLK, P1.6 = MISO, P1.7 = MOSI

    UCB0CTL1 = UCSWRST;
    UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC; // 3-pin, 8-bit SPI master
    UCB0CTL1 |= UCSSEL_2; // SMCLK
    UCB0BR0 |= 0x02; // /2
    UCB0BR1 = 0; //
    UCB0CTL1 &= ~UCSWRST; // Reset UART-Module

}

void SPI_Write_Register(unsigned int address, unsigned int value)
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

void SPI_Write_Register8(unsigned int address, unsigned int value)
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

void SPI_Write_Register_BIP8(unsigned int address, unsigned int value)
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


void SPI_Write_Continuous(unsigned int address, unsigned char* write_data, unsigned int data_length)
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

unsigned int SPI_Read_Register(unsigned int address)
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

unsigned int SPI_Read_Register_BIP8(unsigned int address)
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

void SPI_Read_Continuous(unsigned int address, unsigned char* read_data, unsigned int data_length)
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

