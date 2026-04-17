#include <msp430fr6989.h>
#include "Grlib/grlib/grlib.h"
#include "LcdDriver/lcd_driver.h"
#include <stdio.h>
#include <stdlib.h>

const unsigned int scale[7] = {1915, 1706, 1519, 1432, 1275, 1136, 1012};
unsigned int current_notes[3];
Graphics_Context g_sContext;
int score = 0;
int total_rounds = 5; // Default

// --- ADC/Joystick Functions ---
void setup_adc(void) {
    P6SEL0 |= BIT0; P6SEL1 |= BIT0; // Set P6.0 to Analog
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;
    ADC12CTL1 = ADC12SHP;
    ADC12CTL2 = ADC12RES_2;
    ADC12MCTL0 = ADC12INCH_0;
}

int read_joystick(void) {
    ADC12CTL0 |= ADC12ENC | ADC12SC;
    while (!(ADC12IFG0 & BIT0));
    return ADC12MEM0;
}

void roll_notes(void) {
    srand(TA0R);
    current_notes[0] = scale[rand() % 7];
    current_notes[1] = scale[rand() % 7];
    current_notes[2] = scale[rand() % 7];
}

void main(void) {
    Initialize_Clock_System();

    // Hardware Setup
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;
    P2DIR |= BIT7; // Buzzer
    P1DIR &= ~BIT1; P1REN |= BIT1; P1OUT |= BIT1; // S1
    setup_adc();

    // LCD Setup
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(0);
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);

    // --- 1. ROUND SELECTION MENU ---
    while(1) {
        int joy = read_joystick();
        if (joy > 3500) total_rounds = 10;
        if (joy < 500)  total_rounds = 5;

        char round_msg[20];
        sprintf(round_msg, "Rounds: %d", total_rounds);

        Graphics_clearDisplay(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "SET TEST AMOUNT", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "Joy Up: 10 | Down: 5", AUTO_STRING_LENGTH, 64, 50, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, round_msg, AUTO_STRING_LENGTH, 64, 70, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "Press S1 to Start", AUTO_STRING_LENGTH, 64, 100, OPAQUE_TEXT);

        if (!(P1IN & BIT1)) break; // Start game on S1 press
        __delay_cycles(1000000); // Small debounce/refresh delay
    }

    __enable_interrupt();
    TA0CTL = TASSEL_2 | MC_1 | TACLR;
    TA0CCTL0 = CCIE;

    // --- 2. MAIN GAME LOOP ---
    int r;
    for (r = 1; r <= total_rounds; r++) {
        roll_notes();
        Graphics_clearDisplay(&g_sContext);

        char status[20];
        sprintf(status, "Test %d of %d", r, total_rounds);
        Graphics_drawStringCentered(&g_sContext, status, AUTO_STRING_LENGTH, 64, 20, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "Listen...", AUTO_STRING_LENGTH, 64, 64, OPAQUE_TEXT);

        // Play Sequence
        int i;
        for (i = 0; i < 3; i++) {
            TA0CCR0 = current_notes[i];
            TA0CTL |= MC_1;
            __delay_cycles(16000000); // 1.0s
            TA0CTL &= ~MC_3; // Stop
            P2OUT &= ~BIT7;
            __delay_cycles(8000000); // 0.5s gap
        }

        // --- 3. WAIT FOR INPUT ---
        Graphics_clearDisplay(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "Was Pitch 3 Higher?", AUTO_STRING_LENGTH, 64, 40, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "Up: YES | Down: NO", AUTO_STRING_LENGTH, 64, 70, OPAQUE_TEXT);

        int user_answer = 0; // 1 = High, 2 = Low
        while(user_answer == 0) {
            int move = read_joystick();
            if (move > 3500) user_answer = 1;
            else if (move < 500) user_answer = 2;
        }

        // --- 4. SCORING ---
        // Note: Lower CCR0 value = Higher frequency
        int actually_higher = (current_notes[2] < current_notes[0]) ? 1 : 2;

        if (user_answer == actually_higher) {
            score++;
            Graphics_drawStringCentered(&g_sContext, "CORRECT!", AUTO_STRING_LENGTH, 64, 100, OPAQUE_TEXT);
        } else {
            Graphics_drawStringCentered(&g_sContext, "WRONG", AUTO_STRING_LENGTH, 64, 100, OPAQUE_TEXT);
        }
        __delay_cycles(24000000); // 1.5s to see result
    }

    // --- 5. FINAL SCORE ---
    Graphics_clearDisplay(&g_sContext);
    char final[20];
    sprintf(final, "Score: %d/%d", score, total_rounds);
    Graphics_drawStringCentered(&g_sContext, "GAME OVER", AUTO_STRING_LENGTH, 64, 40, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, final, AUTO_STRING_LENGTH, 64, 70, OPAQUE_TEXT);

    while(1);
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Buzzer_ISR (void) {
    P2OUT ^= BIT7;
}

void Initialize_Clock_System() {
    FRCTL0 = FRCTLPW | NWAITS_1;
    CSCTL0 = CSKEY;
    CSCTL1 &= ~DCOFSEL_7; CSCTL1 |= DCOFSEL_4 | DCORSEL;
    CSCTL3 &= ~(DIVS2|DIVS1|DIVS0|DIVM2|DIVM1|DIVM0);
    CSCTL0_H = 0;
}
