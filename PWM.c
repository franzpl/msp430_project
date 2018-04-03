#include <msp430g2553.h>
#include "PWM.h"

void PWM(int freq, int pulse_width)
{
    P2DIR |= BIT1;                           // P2.1
    P2SEL |= BIT1;                           // P2.1
    TA1CCR0 = 500000 / freq - 1;                // set CCR0 to the desired PWM frequency
    TA1CCTL1 = OUTMOD_7;                        // CCR1 toggle/set
    TA1CCR1 = pulse_width * 0.5;             // set CCR1 to the desired PWM pulse width ( 500 kHz / 1e6)
    TA1CTL = TASSEL_2 + MC_1 + ID_1;          // SMCLK, up-down mode, 500 kHz
}


