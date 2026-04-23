#include <stdio.h>
#include <msp430fr6989.h>

/*
  Passes the  timer ticdirectly to the hardware to play a testtone.

  Timer Tick Values, default 1 MHz SMCLK:
  Note 'A' (440 Hz) = 2272
  Note 'B' (494 Hz) = 2023
  Note 'C' (261 Hz) = 3830
 */
void play_buzzer(int period_ticks) {
    // Configure Pin 2.0 for Timer A1 Output, not sure which pin well be using here~
    P2DIR |= BIT0;
    P2SEL0 |= BIT0;
    P2SEL1 &= ~BIT0;

    // Set the Pitch freq lim
    TA1CCR0 = period_ticks - 1;       

    // 3. Set the Volume 50% Duty Cycle as we mentioned prior
    TA1CCR1 = period_ticks / 2;       
    
    // 4. Start the timer to generate the PWM square wave
    TA1CCTL1 = OUTMOD_7;
    TA1CTL = TASSEL_2 | MC_1 | TACLR;
}

// Stops the timer and silences the buzzer
 
void stop_buzzer(void) {
    TA1CCTL1 = 0;
    TA1CTL = MC_0;
}