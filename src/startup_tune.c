#include <msp430.h>
#include "startup_tune.h"

/* =========================================================
 * 1 MHz SMCLK NOTE PERIOD TICKS
 * ========================================================= */
#define NOTE_C4    3822
#define NOTE_Db4   3608
#define NOTE_D4    3405
#define NOTE_Eb4   3214
#define NOTE_E4    3034
#define NOTE_F4    2863
#define NOTE_Fs4   2703
#define NOTE_G4    2551
#define NOTE_A4    2273
#define NOTE_Bb4   2145
#define NOTE_B4    2025

#define NOTE_C5    1911
#define NOTE_Db5   1802
#define NOTE_D5    1703
#define NOTE_Eb5   1606
#define NOTE_E5    1517
#define NOTE_F5    1432
#define NOTE_Fs5   1351
#define NOTE_G5    1276
#define NOTE_A5    1136
#define NOTE_Bb5   1073
#define NOTE_B5    1012

#define NOTE_REST  0

/* =========================================================
 * PLAY ONE NOTE
 * ========================================================= */
void play_note(unsigned int period_ticks)
{
    if (period_ticks == NOTE_REST)
    {
        TA0CTL = MC_0;       // Stop timer
        P2OUT &= ~BIT7;      // Force output low
        return;
    }

    // MULTIPLY PITCH BY 16 TO MATCH YOUR 16MHz CLOCK
    TA0CCR0 = (period_ticks * 16) - 1;
    TA0CTL = TASSEL_2 | MC_1 | TACLR;
}

/* =========================================================
 * VARIABLE DELAY
 * ========================================================= */
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

/* =========================================================
 * STARTUP SONG
 * ========================================================= */
void play_startup_tune(void)
{
    unsigned int i;

    const unsigned int melody[64] = {
        NOTE_A5, NOTE_D5, NOTE_Bb4, NOTE_G4,
        NOTE_G5, NOTE_D5, NOTE_B4, NOTE_G4,
        NOTE_Fs5, NOTE_Db5, NOTE_B4, NOTE_G4,
        NOTE_G5, NOTE_D5, NOTE_B4, NOTE_G4,
        NOTE_G5, NOTE_C5, NOTE_A4, NOTE_F4,
        NOTE_F5, NOTE_C5, NOTE_A4, NOTE_F4,
        NOTE_E5, NOTE_C5, NOTE_A4, NOTE_F4,
        NOTE_F5, NOTE_C5, NOTE_A4, NOTE_F4,
        NOTE_F5, NOTE_Bb4, NOTE_G4, NOTE_E4,
        NOTE_E5, NOTE_B4, NOTE_G4, NOTE_E4,
        NOTE_Eb5, NOTE_Bb4, NOTE_G4, NOTE_E4,
        NOTE_E5, NOTE_B4, NOTE_G4, NOTE_E4,
        NOTE_E5, NOTE_A4, NOTE_F4, NOTE_D4,
        NOTE_D5, NOTE_A4, NOTE_F4, NOTE_D4,
        NOTE_Db5, NOTE_A4, NOTE_F4, NOTE_D4,
        NOTE_D5, NOTE_A4, NOTE_F4, NOTE_D4,
    };

    // MULTIPLY DELAYS BY 16 TO PREVENT THE SONG PLAYING IN 0.5 SECONDS
    const unsigned long note_duration = 1920000UL;  // 120000 * 16
    const unsigned long note_gap      = 160000UL;   // 10000 * 16
    const unsigned long group_break   = 640000UL;   // 40000 * 16

    for (i = 0; i < 64; i++)
    {
        play_note(melody[i]);
        delay_cycles_variable(note_duration);

        TA0CTL = MC_0;       // Stop timer between notes
        P2OUT &= ~BIT7;

        if (((i + 1) % 4) == 0) delay_cycles_variable(group_break);
        else delay_cycles_variable(note_gap);
    }
}
