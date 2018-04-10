//
//
//                                            (Host/Tester)
//                   RF430                     MSP430G2553
//                  (Slave)                      Master
//             _________________            _________________
//            |             MOSI|<-------->|P1.7/            |
//            |             CLK |<---SPI-->|P1.5             |
//            |             MISO|<-+------>|P1.6/            |
//            |             ~CS |<---------|P2.5             |
//      GND<--|E0,E1,E2     /RST|<---------|P2.0             |
//            |             INTO|--------->|P2.4             |
//            |                 |          |             P1.0|---->LED1
//            |                 |          |                 |
//            |_________________|          |_________________|
//
// Built with  CCS 7.4.0.00015
//******************************************************************************

#include "msp430.h"
#include "RF430_SPI_example.h"

unsigned char NDEF_Application_Data[] = RF430_DEFAULT_DATA;

unsigned char test_data[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};

unsigned char CRC_Data[] = {1,2,3,4,5,6,7,8,9};

unsigned char Cmd = 0;  //Command byte for SPI mode
unsigned char read_complete = 0;
unsigned char rx_byte_count = 0;
unsigned char tx_byte_count = 0;
unsigned int Results[11] = {0,0,0,0,0,0,0,0,0,0,0};

/****************************************************************************/
/* Code-binary that opens on ETW and re-trims LF oscillator to below 280kHz */
/****************************************************************************/
unsigned char func_retrim_osc[] = {
        0xB2, 0x40, 0x11, 0x96, 0x10, 0x01,
        0xB2, 0x40, 0x60, 0x03, 0x18, 0x01,
        0x30, 0x41
};

unsigned char into_fired = 0;

unsigned char read_data[200];

void SPI_init();

void main (void)
{
    volatile unsigned int test = 0;
    unsigned int flags = 0;
    unsigned int temp = 0;
    //unsigned char read_data[200];

    WDTCTL = WDTPW + WDTHOLD;               // Turn off Watch Dog


    BCSCTL1 = CALBC1_1MHZ;                  // Set range
    DCOCTL = CALDCO_1MHZ;                   // SMCLK = MCLK = DCO = 8 MHz   ACLK = LF oscillator

    SPI_init();
    __delay_cycles(100000);


    // RST RF430 (in case board remained powered but the MSP430 reset for some reason - MSP430 RST button pushed for example)
    PORT_RST_SEL0 &= ~RST;                  // Setting as GPIO pin functionality
    PORT_RST_SEL1 &= ~RST;
    PORT_RST_OUT &= ~RST;
    PORT_RST_DIR |= RST;                    // RF430CL330H device in Reset
    __delay_cycles(1000);
    PORT_RST_OUT |= RST;                    // Release the RF430CL330H from Reset

    //configure LED1
    PORT_LED_SEL0 &= ~LED1;                 // Confirming GPIO pin functionality
    PORT_LED_SEL1 &= ~LED1;
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

    PORT_LED_OUT |= LED1;
    __delay_cycles(1000000);
    while(!(Read_Register(STATUS_REG) & READY)); // Wait until READY bit has been set
    PORT_LED_OUT &= ~LED1;



    /****************************************************************************/
    /* This code is used to change data rate capability                         */
    /****************************************************************************/

//  Write_Register(TEST_MODE_REG, TEST_MODE_KEY);   //unlock test mode
//  Write_Register(CONTROL_REG, TEST430_ENABLE);    //enable test mode, now have to use actual addresses
//  Write_Register(0x2a7C, 0xF7);                   //the bit rate capability setting, 0xF7=848kbits
//  Write_Register(0x2814, 0);                      //exit test mode (CONTROL_REG is at real address 0x2814)
//  Write_Register(TEST_MODE_REG, 0);               //exiting this mode, RF is disabled as Control register is set to 0


        /****************************************************************************/
        /* Configure RF430CL330H for Typical Usage Scenario                         */
        /****************************************************************************/

    //write NDEF memory with Capability Container + NDEF message
    Write_Continuous(0, NDEF_Application_Data, 48);

    //Enable interrupts for End of Read and End of Write
    Write_Register(INT_ENABLE_REG, EOW_INT_ENABLE + EOR_INT_ENABLE);

    //Configure INTO pin for active low and enable RF
    Write_Register(CONTROL_REG, INT_ENABLE + INTO_DRIVE + RF_ENABLE);


    PORT_INTO_IFG &= ~INTO;         //clear any pending flags
    PORT_INTO_IE |= INTO;           //enable interrupt

    while (1)
    {

        __bis_SR_register(LPM3_bits + GIE); //go to low power mode and enable interrupts. We are waiting for an NFC read or write of/to the RF430
                __no_operation();

                //device has woken up, check status
                if(into_fired)
                {
                    //before we read/write to RF430CL330H we should disable RF
                    Write_Register(CONTROL_REG, INT_ENABLE + INTO_DRIVE); //clear control reg to disable RF
                    __delay_cycles(1000);
                    flags = Read_Register(INT_FLAG_REG); //read the flag register to check if a read or write occurred
                    Write_Register(INT_FLAG_REG, EOW_INT_FLAG + EOR_INT_FLAG); //ACK the flags to clear

                    Write_Register(INT_ENABLE_REG, 0);


                   PORT_LED_OUT |= LED1;          //clear LEDs


                    if(flags & EOW_INT_FLAG) //check if the tag was written
                    {
                        //tag was updated, so we should read out the new data
                        //read out the data
                        //Read_Continuous(0, read_data, 200);
                        __no_operation(); //breakpoint here to examine the data

                        //show that tag was written by blinking LED 3 times
                        for(temp = 0; temp < 3; temp++){
                            PORT_LED_OUT |= LED1;
                            __delay_cycles(300000);
                            PORT_LED_OUT &= ~LED1;//clear LED
                            __delay_cycles(300000);
                        }
                    }
                    else if(flags & EOR_INT_FLAG) //check if the tag was read
                    {
                        __no_operation();

                        //show that tag was read with LED by holding it on for 1 second
                        PORT_LED_OUT |= LED1;
                        __delay_cycles(1000000);
                        PORT_LED_OUT &= ~LED1;//clear LED
                    }

                    flags = 0;
                    into_fired = 0; //we have serviced INTO

                    //Enable interrupts for End of Read and End of Write
                     Write_Register(INT_ENABLE_REG, EOW_INT_ENABLE + EOR_INT_ENABLE);
                     test = Read_Register(INT_ENABLE_REG);
                    //Configure INTO pin for active low and re-enable RF
                    Write_Register(CONTROL_REG, INT_ENABLE + INTO_DRIVE + RF_ENABLE);

                    //re-enable INTO
                    PORT_INTO_IFG &= ~INTO;
                    PORT_INTO_IE |= INTO;

                    PORT_LED_OUT &= ~LED1;

                    __no_operation();
                }
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

#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
    //INTO interrupt fired
    if(PORT_INTO_IFG & INTO)
    {
        into_fired = 1;

        PORT_INTO_IE &= ~INTO; //disable INTO
        PORT_INTO_IFG &= ~INTO; //clear interrupt flag

        __bic_SR_register_on_exit(LPM3_bits); //wake up to handle INTO
    }
}

#pragma vector = PORT1_VECTOR,ADC10_VECTOR,WDT_VECTOR,NMI_VECTOR,TIMER0_A1_VECTOR, \
           TIMER0_A0_VECTOR, COMPARATORA_VECTOR, TIMER1_A1_VECTOR, TIMER1_A0_VECTOR, \
           USCIAB0TX_VECTOR, USCIAB0RX_VECTOR
 __interrupt void ISR_trap(void)
 {
   while(1);  // IT'S A TRAP!!!
 }
