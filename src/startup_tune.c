//IN PROGRESS
#include <msp430.h>

/* =========================================================
 * TONE TRACKER STARTUP SONG
 * Passive buzzer on P2.7 using TimerA0 interrupt toggle
 * SMCLK assumed ~1 MHz
 * ========================================================= */

/* =========================================================
 * 1 MHz SMCLK NOTE PERIOD TICKS
 * higher octave set + accidentals
 * smaller number = higher pitch
 * ========================================================= */
/* ===================== 4th OCTAVE ===================== */
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

/* ----- 5th octave ----- */
#define NOTE_C5    1911
#define NOTE_Db5   1802   // C#5 / Db5
#define NOTE_D5    1703
#define NOTE_Eb5   1606   // D#5 / Eb5
#define NOTE_E5    1517
#define NOTE_F5    1432
#define NOTE_Fs5   1351   // F#5 / Gb5
#define NOTE_G5    1276
#define NOTE_A5    1136
#define NOTE_Bb5   1073   // A#5 / Bb5
#define NOTE_B5    1012

//enharmonic aliases
#define NOTE_Cs5   NOTE_Db5
#define NOTE_Ds5   NOTE_Eb5
#define NOTE_Gb5   NOTE_Fs5
#define NOTE_As5   NOTE_Bb5

#define NOTE_Cs6   NOTE_Db6
#define NOTE_Ds6   NOTE_Eb6
#define NOTE_Gb6   NOTE_Fs6
#define NOTE_As6   NOTE_Bb6

#define NOTE_REST  0


// FUNCTION PROTOTYPES

void setup_buzzer(void);
void play_note(unsigned int period_ticks);
void stop_buzzer(void);
void delay_cycles_variable(unsigned long cycles);
void play_startup_song(void);

// MAIN
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    setup_buzzer();
    __enable_interrupt();

 /*
     * PLAY SONG ON STARTUP
*/
    play_startup_song();

    while (1)
    {
        /* idle here for now */
    }
}

/* 
 * BUZZER SETUP
 * Passive buzzer on P2.7
*/
void setup_buzzer(void)
{
    /* buzzer pin as output */
    P2DIR |= BIT7;
    P2OUT &= ~BIT7;

    /* TimerA0 setup
     * SMCLK source
     * stopped initially
     * CCR0 interrupt enabled
     */
    TA0CTL = TASSEL_2 | MC_0 | TACLR;
    TA0CCTL0 = CCIE;
}

/* 
 * PLAY ONE NOTE
 * period_ticks controls pitch
 */
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

// STOP BUZZER
void stop_buzzer(void)
{
    TA0CTL = MC_0;       /* stop timer */
    P2OUT &= ~BIT7;      /* force output low */
}

/*
 * VARIABLE DELAY
 * __delay_cycles() needs constants, so this helper lets us
 * use variable timing values safely
*/
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

/*
 * STARTUP SONG
 * 64 notes total
 * grouped in chunks of 4
 * tiny gap between notes
 * slightly longer break after every 4 notes
 *
 * EDITS FOR
 * 1. the actual melody notes
 * 2. note duration
 * 3. small gap
 * 4. group break
 */
void play_startup_song(void)
{
    unsigned int i;

    /* 
     * EDIT HERE: SONG NOTES
     * This is a buzzer rolling melody
     */
    const unsigned int melody[64] = {
        NOTE_A5, NOTE_D5, NOTE_Bb4, NOTE_G4,
        NOTE_G5, NOTE_D5, NOTE_Bb4, NOTE_G4,
        NOTE_Fs5, NOTE_D5, NOTE_Bb4, NOTE_G4,
        NOTE_G5, NOTE_D5, NOTE_Bb4, NOTE_G4,

        NOTE_G5, NOTE_C5, NOTE_A4, NOTE_F4,
        NOTE_F5, NOTE_C5, NOTE_A4, NOTE_F4,
        NOTE_E5, NOTE_C5, NOTE_A4, NOTE_F4,
        NOTE_F5, NOTE_C5, NOTE_A4, NOTE_F4,

        NOTE_F5, NOTE_Bb4, NOTE_G4, NOTE_E4,
        NOTE_E5, NOTE_Bb4, NOTE_G4, NOTE_E4,
        NOTE_Eb5, NOTE_Bb4, NOTE_G4, NOTE_E4,
        NOTE_E5, NOTE_Bb4, NOTE_G4, NOTE_E4,

        NOTE_E5, NOTE_A4, NOTE_F4, NOTE_D4,
        NOTE_D5, NOTE_A4, NOTE_F4, NOTE_D4,
        NOTE_Db5, NOTE_A4, NOTE_F4, NOTE_D4,
        NOTE_D5, NOTE_A4, NOTE_F4, NOTE_D4,
    };

    /* 
     *NOTE TIMING
     *
     * These values are chosen to land around ~10 seconds total
     * for startup guidelines
     * note_duration = how long each note sounds
     * note_gap      = tiny gap between notes
     * group_break   = slightly longer gap after every 4th note
     */
    const unsigned long note_duration = 120000UL;  /* ~0.12 s */
    const unsigned long note_gap      = 10000UL;   /* ~0.01 s */
    const unsigned long group_break   = 40000UL;   /* ~0.04 s */

    for (i = 0; i < 64; i++)
    {
        play_note(melody[i]);
        delay_cycles_variable(note_duration);

        stop_buzzer();

        /* longer break after every group of 4 notes */
        if (((i + 1) % 4) == 0)
        {
            delay_cycles_variable(group_break);
        }
        else
        {
            delay_cycles_variable(note_gap);
        }
    }
}

/* 
 * TIMER0_A0 ISR
 * toggles buzzer pin to generate square wave
 */
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Buzzer_ISR(void)
{
    P2OUT ^= BIT7;
}
