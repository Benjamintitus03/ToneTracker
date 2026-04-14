#include <msp430fr6989.h>

// Array of Half-Period Timer Ticks for C4 through B4
// Calculated as: (1,000,000 Hz / Frequency) / 2
const unsigned int notes[7] = {
    1915, // C (261 Hz)
    1706, // D (293 Hz)
    1519, // E (329 Hz)
    1432, // F (349 Hz)
    1275, // G (392 Hz)
    1136, // A (440 Hz)
    1012  // B (494 Hz)
};

void main(void) {
    // 1. Stop Watchdog & Unlock Pins
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    // 2. Setup P2.7 (Buzzer) as GPIO Output
    P2DIR |= BIT7;
    P2OUT &= ~BIT7;

    // 3. Setup Timer A0 for Background Interrupts
    // TASSEL_2 = SMCLK (1MHz), MC_1 = Up mode
    TA0CTL = TASSEL_2 | MC_1 | TACLR;
    TA0CCTL0 = CCIE; // Enable Timer A0 Interrupt

    // 4. Enable Global Interrupts (turns on the background system)
    __enable_interrupt();

    int current_note = 0;

    // 5. The Main Loop
    while (1) {
        // Set the background timer to the current pitch
        TA0CCR0 = notes[current_note];

        // Let the note play for 0.5 seconds
        // (The CPU just waits here while the interrupt makes the sound)
        __delay_cycles(500000);

        // Add a tiny  silence 
        TA0CTL = MC_0;         // Stop the timer
        P2OUT &= ~BIT7;        // Ensure buzzer is forced off
        __delay_cycles(50000); // 50 milliseconds of silence
        TA0CTL = TASSEL_2 | MC_1 | TACLR; // Restart the timer

        // Move to the next note, loop back to 0 if we hit the end
        current_note++;
        if (current_note > 6) {
            current_note = 0;
        }
    }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {
    P2OUT ^= BIT7; // Toggle the buzzer pin to create the sound wave
}
