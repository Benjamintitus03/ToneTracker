#include <msp430fr6989.h>

void main(void) {
    // Stop the watchdog timer (critical for all MSP430 code)
    WDTCTL = WDTPW | WDTHOLD;

    PM5CTL0 &= ~LOCKLPM5;

    // Configure Pin 2.0 to its Timer A1 Output function
    P2DIR |= BIT0;
    P2SEL0 |= BIT0;
    P2SEL1 &= ~BIT0;

    // Set the Pitch: 440 Hz (Note 'A')
    TA1CCR0 = 2272;       

    TA1CCR1 = 1136;       
    

    TA1CCTL1 = OUTMOD_7;
    TA1CTL = TASSEL_2 | MC_1 | TACLR;

    // Infinite loop to keep the microcontroller running
    while (1) {
        // Just hanging out while the buzzer screams
    }
}