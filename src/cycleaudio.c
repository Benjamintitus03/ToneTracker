#include <msp430fr6989.h>
#include "Grlib/grlib/grlib.h"
#include "LcdDriver/lcd_driver.h"
#include <stdio.h>
#include <stdlib.h>

const unsigned int scale[7] = {1915, 1706, 1519, 1432, 1275, 1136, 1012};
unsigned int current_notes[3];
Graphics_Context g_sContext;

void setup_hardware(void) {
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    P2DIR |= BIT7; // Buzzer

    // Configure S1 (P1.1) as Input with Pull-up Resistor
    P1DIR &= ~BIT1;
    P1REN |= BIT1;
    P1OUT |= BIT1;

    // Timer A0 for Buzzer
    TA0CTL = TASSEL_2 | MC_1 | TACLR;
    TA0CCTL0 = CCIE;
}

void roll_notes(void) {
    srand(TA0R);
    current_notes[0] = scale[(unsigned int)(rand() % 7)];
    current_notes[1] = scale[(unsigned int)(rand() % 7)];
    current_notes[2] = scale[(unsigned int)(rand() % 7)];
}

void main(void) {
    Initialize_Clock_System();
    setup_hardware();

    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(0);
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);

    Graphics_drawStringCentered(&g_sContext, "TONE TRACKER", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Press S1 to Start", AUTO_STRING_LENGTH, 64, 70, OPAQUE_TEXT);

    // Stall until S1 is pressed (P1IN & BIT1 will be 0 when pressed)
    while (P1IN & BIT1);

    __enable_interrupt();

    while (1) {
        roll_notes();
        Graphics_clearDisplay(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "Listening...", AUTO_STRING_LENGTH, 64, 64, OPAQUE_TEXT);

        int i;
        for (i = 0; i < 3; i++) {
            TA0CCR0 = current_notes[i];
            TA0CTL = TASSEL_2 | MC_1 | TACLR;
            __delay_cycles(8000000); // 0.5s play
            TA0CTL = MC_0;
            P2OUT &= ~BIT7;
            __delay_cycles(1600000); // Gap
        }

        Graphics_clearDisplay(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "Sequence Done!", AUTO_STRING_LENGTH, 64, 64, OPAQUE_TEXT);

        // Brief pause before next round
        __delay_cycles(32000000);
    }
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
