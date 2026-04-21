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

    // S1 (P1.1) and S2 (P1.2) Buttons
    P1DIR &= ~(BIT1 | BIT2);
    P1REN |= (BIT1 | BIT2);
    P1OUT |= (BIT1 | BIT2);

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

                // Wait for S1 to start
                if (!(P1IN & BIT1)) {
                    current_state = STATE_MENU;
                    while(!(P1IN & BIT1)); // Wait for release
                    __delay_cycles(100000);
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
                    Graphics_drawStringCentered(&g_sContext, (int8_t *)"Press S2 to Confirm", AUTO_STRING_LENGTH, 64, 100, OPAQUE_TEXT);

                    // Wait for S2 to Confirm Menu
                    if (!(P1IN & BIT2)) {
                        score = 0;
                        current_state = STATE_TRACKING;
                        while(!(P1IN & BIT2)); // Wait for release
                        __delay_cycles(100000);
                    }
                }
                break;

            case STATE_TRACKING:
                {
                    int r;
                    for (r = 1; r <= total_rounds; r++) {
                        roll_notes();

                        // 1. PLAY SEQUENCE
                        display_sequence_state(&g_sContext, r);

                        int i;
                        for (i = 0; i < 3; i++) {
                            TA0CCR0 = (current_notes[i] * 16) - 1;
                            TA0CTL = TASSEL_2 | MC_1 | TACLR;
                            __delay_cycles(16000000);
                            TA0CTL = MC_0;
                            P2OUT &= ~BIT7;
                            __delay_cycles(8000000);
                        }

                        // 2. INPUT COLLECTION PHASE (2 = DIR_NONE)
                        int slot1 = 2;
                        int slot2 = 2;
                        int preview = 2;

                        display_compare_state(&g_sContext, slot1, slot2, preview, r);

                        while(slot1 == 2 || slot2 == 2) {
                            int joy = read_joystick();
                            int current_preview = preview;

                            // Read Joystick for preview direction
                            if (joy > 3200) current_preview = 1;      // UP
                            else if (joy < 800) current_preview = 0;  // DOWN

                            int redraw = 0;
                            if (current_preview != preview) {
                                preview = current_preview;
                                redraw = 1;
                            }

                            // CHECK S2: CONFIRM SELECTION
                            if (!(P1IN & BIT2)) {
                                if (preview != 2) { // Only confirm if they actually picked a direction
                                    if (slot1 == 2) {
                                        slot1 = preview;
                                        preview = 2; // Reset preview for the next slot
                                    } else if (slot2 == 2) {
                                        slot2 = preview;
                                    }
                                    redraw = 1;
                                    while(!(P1IN & BIT2)); // Wait for release so it doesn't double-click
                                    __delay_cycles(100000);
                                }
                            }

                            // CHECK S1: CLEAR SELECTION
                            if (!(P1IN & BIT1)) {
                                if (slot2 != 2) {
                                    slot2 = 2; // Clear slot 2 first
                                } else if (slot1 != 2) {
                                    slot1 = 2; // Clear slot 1
                                }
                                preview = 2;
                                redraw = 1;
                                while(!(P1IN & BIT1)); // Wait for release
                                __delay_cycles(100000);
                            }

                            // Update screen only if something changed
                            if (redraw) {
                                display_compare_state(&g_sContext, slot1, slot2, preview, r);
                            }
                        }

                        __delay_cycles(4000000); // Tiny pause to see the final arrow

                        // 3. CHECK ANSWERS (Smaller array number = higher frequency pitch)
                        int actual_slot1 = (current_notes[1] < current_notes[0]) ? 1 : 0;
                        int actual_slot2 = (current_notes[2] < current_notes[1]) ? 1 : 0;

                        if (slot1 == actual_slot1 && slot2 == actual_slot2) {
                            score++;
                            display_round_feedback_state(&g_sContext, 1); // Correct Checkmark
                        } else {
                            display_round_feedback_state(&g_sContext, 0); // Wrong X
                        }
                        __delay_cycles(16000000);
                    }
                    current_state = STATE_GAMEOVER;
                }
                break;

            case STATE_GAMEOVER:
                display_final_score_state(&g_sContext, score, total_rounds);

                // Wait for S1 to Restart
                if (!(P1IN & BIT1)) {
                    current_state = STATE_STARTUP;
                    while(!(P1IN & BIT1));
                    __delay_cycles(100000);
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
