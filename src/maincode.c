#include <msp430fr6989.h>
#include "Grlib/grlib/grlib.h"
#include "LcdDriver/lcd_driver.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "displays.h"
#include "startup_tune.h"

// Global Variables
const unsigned int scale[7] = {1915, 1706, 1519, 1432, 1275, 1136, 1012};
unsigned int current_notes[3];
Graphics_Context g_sContext;
int score = 0;
int total_rounds = 5;

// Define System States
typedef enum {
    STATE_STARTUP,
    STATE_IDLE,
    STATE_MENU,
    STATE_TRACKING,
    STATE_GAMEOVER
} system_state_t;

system_state_t current_state = STATE_STARTUP;

// Forward Declarations
void Init_Master_Clock();
void setup_hardware(void);
int read_joystick(void);
void roll_notes(void);

// Hardware Setup
void setup_hardware(void) {
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    P2DIR |= BIT7; // Buzzer

    // S1 Button
    P1DIR &= ~BIT1;
    P1REN |= BIT1;
    P1OUT |= BIT1;

    // Joystick Y axis on P8.2 (Channel A10)
    P8SEL0 |= BIT2;
    P8SEL1 |= BIT2;

    // ADC12 Setup
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;
    ADC12CTL1 = ADC12SHP;
    ADC12CTL2 = ADC12RES_2;
    ADC12MCTL0 = ADC12INCH_10;

    // Timer A0
    TA0CTL = TASSEL_2 | MC_0 | TACLR;
    TA0CCTL0 = CCIE;
}

int read_joystick(void) {
    ADC12CTL0 |= ADC12ENC | ADC12SC;
    while (ADC12CTL1 & ADC12BUSY);
    return ADC12MEM0;
}

void roll_notes(void) {
    srand(TA0R);
    current_notes[0] = scale[(unsigned int)(rand() % 7)];
    current_notes[1] = scale[(unsigned int)(rand() % 7)];
    current_notes[2] = scale[(unsigned int)(rand() % 7)];
}

// Main System Loop
                              void main(void) {
    Init_Master_Clock();
    setup_hardware();

    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(0);
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);

    __enable_interrupt();

    while(1) {
        switch (current_state) {

            case STATE_STARTUP:
                play_startup_tune();
                current_state = STATE_IDLE;
                break;

            case STATE_IDLE:
                display_idle_state(&g_sContext);

                // Wait for user to press S1
                if (!(P1IN & BIT1)) {
                    current_state = STATE_MENU;
                    __delay_cycles(2000000); // Debounce delay
                }
                break;

            case STATE_MENU:
                {
                    int joy = read_joystick();
                    if (joy > 3200) total_rounds = 10;
                    else if (joy < 800) total_rounds = 5;

                    char round_msg[20];
                    sprintf(round_msg, "Rounds: %d", total_rounds);
                    Graphics_clearDisplay(&g_sContext);
                    Graphics_drawStringCentered(&g_sContext, (int8_t *)"TONE TRACKER", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);
                    Graphics_drawStringCentered(&g_sContext, (int8_t *)"Joy Up: 10 | Down: 5", AUTO_STRING_LENGTH, 64, 50, OPAQUE_TEXT);
                    Graphics_drawStringCentered(&g_sContext, (int8_t *)round_msg, AUTO_STRING_LENGTH, 64, 70, OPAQUE_TEXT);
                    Graphics_drawStringCentered(&g_sContext, (int8_t *)"Press S1 to Start", AUTO_STRING_LENGTH, 64, 100, OPAQUE_TEXT);

                    if (!(P1IN & BIT1)) {
                        score = 0;
                        current_state = STATE_TRACKING;
                        __delay_cycles(2000000);
                    }
                }
                break;

            case STATE_TRACKING:
                {
                    int r;
                    for (r = 1; r <= total_rounds; r++) {
                        roll_notes();

                        // 1. NATALIA'S SEQUENCE SCREEN
                        display_sequence_state(&g_sContext, r);

                        // 2. PLAY NOTES (Timer bug completely fixed)
                        int i;
                        for (i = 0; i < 3; i++) {
                            TA0CCR0 = (current_notes[i] * 16) - 1;

                            // Must restore TASSEL_2 clock source every time!
                            TA0CTL = TASSEL_2 | MC_1 | TACLR;
                            __delay_cycles(16000000);

                            TA0CTL = MC_0;  // Stop timer
                            P2OUT &= ~BIT7; // Pull buzzer low
                            __delay_cycles(8000000);
                        }

                        // 3. NATALIA'S COMPARE SCREEN (2 = DIR_NONE for empty slots)
                        display_compare_state(&g_sContext, 2, 2, 2, r);

                        // Joystick Safety lock
                        while (read_joystick() > 3200 || read_joystick() < 800);

                        int user_answer = 0;
                        int user_dir = 2;

                        // Your Joystick Logic
                        while(user_answer == 0) {
                            int move = read_joystick();
                            if (move > 3200) {
                                user_answer = 1;
                                user_dir = 1; // 1 = DIR_UP
                            }
                            else if (move < 800) {
                                user_answer = 2;
                                user_dir = 0; // 0 = DIR_DOWN
                            }
                        }

                        // Draw the arrow you just picked!
                        display_compare_state(&g_sContext, user_dir, 2, 2, r);
                        __delay_cycles(8000000); // 0.5s pause to see arrow

                        int actually_higher = (current_notes[2] < current_notes[0]) ? 1 : 2;

                        // 4. NATALIA'S FEEDBACK SCREEN
                        if (user_answer == actually_higher) {
                            score++;
                            display_round_feedback_state(&g_sContext, 1); // 1 = Correct Checkmark
                        } else {
                            display_round_feedback_state(&g_sContext, 0); // 0 = Wrong X
                        }
                        __delay_cycles(16000000);
                    }
                    current_state = STATE_GAMEOVER;
                }
                break;

            case STATE_GAMEOVER:
                // 5. NATALIA'S FINAL SCORE SCREEN
                display_final_score_state(&g_sContext, score, total_rounds);

                // Wait for S1 to Restart
                if (!(P1IN & BIT1)) {
                    current_state = STATE_STARTUP;
                    __delay_cycles(2000000);
                }
                break;
        }
    }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Buzzer_ISR (void) {
    P2OUT ^= BIT7;
}

void Init_Master_Clock() {
    FRCTL0 = FRCTLPW | NWAITS_1;
    CSCTL0 = CSKEY;
    CSCTL1 &= ~DCOFSEL_7;
    CSCTL1 |= DCOFSEL_4 | DCORSEL;
    CSCTL3 &= ~(DIVS2|DIVS1|DIVS0|DIVM2|DIVM1|DIVM0);
    CSCTL0_H = 0;
}
