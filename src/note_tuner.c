#include <msp430.h>

/* 
 * BUZZER NOTE TESTER
 * Passive buzzer on P2.7 using TimerA0 interrupt toggle
 * SMCLK assumed ~1 MHz
 *
 * Change TEST_NOTE to whatever note you want to hear.
 * Total repeat period = 5 seconds
 */

/*  4th OCTAVE */
#define NOTE_C4    3822
#define NOTE_Db4   3608

#define NOTE_Eb4   3214
#define NOTE_E4    3034
#define NOTE_F4    2863
#define NOTE_Fs4   2703
#define NOTE_G4    2551
#define NOTE_A4    2273
#define NOTE_Bb4   2145
#define NOTE_B4    2025

/* 5th OCTAVE */
#define NOTE_C5    1911
#define NOTE_Db5   1802

#define NOTE_Eb5   1606
#define NOTE_E5    1517
#define NOTE_F5    1432
#define NOTE_Fs5   1351
#define NOTE_G5    1276
#define NOTE_A5    1136
#define NOTE_Bb5   1073
#define NOTE_B5    1012
#define TUNE(x) ((unsigned int)((x) * 1.1))

#define NOTE_D4 TUNE(3405)
#define NOTE_D5 TUNE(1703)

/* Enharmonic aliases */
#define NOTE_Cs4   NOTE_Db4
#define NOTE_Ds4   NOTE_Eb4
#define NOTE_Gb4   NOTE_Fs4
#define NOTE_As4   NOTE_Bb4

#define NOTE_Cs5   NOTE_Db5
#define NOTE_Ds5   NOTE_Eb5
#define NOTE_Gb5   NOTE_Fs5
#define NOTE_As5   NOTE_Bb5

#define NOTE_REST  0

/* 
 * EDIT THIS NOTE
 * */
#define TEST_NOTE NOTE_D4

/* 
 * TIMING
 */
#define NOTE_ON_TIME_CYCLES   1000000UL   /* ~1.0 s */
#define TOTAL_PERIOD_CYCLES  5000000UL    /* 5.0 s total */
#define NOTE_OFF_TIME_CYCLES (TOTAL_PERIOD_CYCLES - NOTE_ON_TIME_CYCLES)

/*
 * FUNCTION PROTOTYPES
 */
void setup_buzzer(void);
void play_note(unsigned int period_ticks);
void stop_buzzer(void);
void delay_cycles_variable(unsigned long cycles);

// MAIN

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    setup_buzzer();
    __enable_interrupt();

    while (1)
    {
        play_note(TEST_NOTE);
        delay_cycles_variable(NOTE_ON_TIME_CYCLES);

        stop_buzzer();
        delay_cycles_variable(NOTE_OFF_TIME_CYCLES);
    }
}

// BUZZER SETUP

void setup_buzzer(void)
{
    P2DIR |= BIT7;
    P2OUT &= ~BIT7;

    TA0CTL = TASSEL_2 | MC_0 | TACLR;   /* SMCLK, stopped */
    TA0CCTL0 = CCIE;                    /* enable CCR0 interrupt */
}

//PLAY NOTE
void play_note(unsigned int period_ticks)
{
    if (period_ticks == NOTE_REST)
    {
        stop_buzzer();
        return;
    }

    TA0CCR0 = period_ticks - 1;
    TA0CTL = TASSEL_2 | MC_1 | TACLR;   /* SMCLK, up mode */
}

//STOP BUZZER
void stop_buzzer(void)
{
    TA0CTL = MC_0;
    P2OUT &= ~BIT7;
}

//VARIABLE DELAY

void delay_cycles_variable(unsigned long cycles)
{
    while (cycles >= 10000UL)
    {
        __delay_cycles(10000);
        cycles -= 10000UL;
    }

    while (cycles > 0UL)
    {
        __delay_cycles(1);
        cycles--;
    }
}

// TIMER ISR
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Buzzer_ISR(void)
{
    P2OUT ^= BIT7;
}
