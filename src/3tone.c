#include <msp430.h>
#include <stdlib.h>

// Note frequencies: C4, D4, E4, F4, G4, A4, B4
const unsigned int scale[7] = {1915, 1706, 1519, 1432, 1275, 1136, 1012};
int total_rounds = 5;
unsigned int current_notes[3];

void setup_hardware(void) {
    // Stop Watchdog & Unlock Pins
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    // Buzzer (Pin 2.7)
    P2DIR |= BIT7;
    P2OUT &= ~BIT7;

    // Joystick Y-Axis (Pin 6.0)
    P6SEL0 |= BIT0;
    P6SEL1 |= BIT0;

    // ADC12_B Configuration
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;
    ADC12CTL1 = ADC12SHP;
    ADC12CTL2 = ADC12RES_2;
    ADC12MCTL0 = ADC12INCH_0;

    // Timer A0 for Background Buzzer
    TA0CTL = TASSEL_2 | MC_1 | TACLR;
    TA0CCTL0 = CCIE;
}

int read_joystick(void) {
    ADC12CTL0 |= ADC12ENC | ADC12SC;
    while (!(ADC12IFG0 & BIT0));
    return ADC12MEM0;
}

void roll_new_notes(void) {
    int joy_val = read_joystick();

    // Select 5 or 10 rounds based on joystick position
    total_rounds = (joy_val > 2048) ? 10 : 5;

    srand(TA0R); // Seed with timer count
    current_notes[0] = scale[(unsigned int)(rand() % 7)];
    current_notes[1] = scale[(unsigned int)(rand() % 7)];
    current_notes[2] = scale[(unsigned int)(rand() % 7)];
}

void main(void) {
    int i; 

    setup_hardware();
    __enable_interrupt();

    roll_new_notes();

    while (1) {
        // Play the 3-note sequence
        for (i = 0; i < 3; i++) { 
            TA0CCR0 = current_notes[i];
            TA0CTL = TASSEL_2 | MC_1 | TACLR;
            __delay_cycles(500000);           // Play for 0.5s

            TA0CTL = MC_0;                    // Silence
            P2OUT &= ~BIT7;
            __delay_cycles(100000);           // Gap
        }

        __delay_cycles(2000000); // Wait 2 seconds before next sequence
        roll_new_notes();
    }
}


#pragma vector = TIMER0_A0_VECTOR
__interrupt void Buzzer_ISR (void) {
    P2OUT ^= BIT7;
}
