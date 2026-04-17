#include <msp430fr6989.h>
#include "Grlib/grlib/grlib.h"
#include "LcdDriver/lcd_driver.h"
#include <stdio.h>
#include <stdlib.h>

const unsigned int scale[7] = {1915, 1706, 1519, 1432, 1275, 1136, 1012};
unsigned int current_notes[3];
Graphics_Context g_sContext;
int score = 0;
int total_rounds = 5;

void Initialize_Clock_System();
void setup_hardware(void);
int read_joystick(void);
void roll_notes(void);

// Hardware and ADC setup
void setup_hardware(void) {
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    P2DIR |= BIT7; // Buzzer Pin

    // S1 Button setup for the Start Menu
    P1DIR &= ~BIT1;
    P1REN |= BIT1;
    P1OUT |= BIT1;

    // Joystick Y axis setup
    P8SEL0 |= BIT2;
    P8SEL1 |= BIT2;

    // ADC12 setup
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;
    ADC12CTL1 = ADC12SHP;
    ADC12CTL2 = ADC12RES_2;
    ADC12MCTL0 = ADC12INCH_10;

    // Timer A0 for Buzzer
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

// Main game logic
void main(void) {
    Initialize_Clock_System();
    setup_hardware();

    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(0);
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);

    // Menu loop
    while(1) {
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

        if (!(P1IN & BIT1)) break;
        __delay_cycles(1000000);
    }

    __enable_interrupt();

    int r;
    for (r = 1; r <= total_rounds; r++) {
        roll_notes();
        Graphics_clearDisplay(&g_sContext);

        char status[20];
        sprintf(status, "Test %d of %d", r, total_rounds);
        Graphics_drawStringCentered(&g_sContext, (int8_t *)status, AUTO_STRING_LENGTH, 64, 20, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, (int8_t *)"Listen...", AUTO_STRING_LENGTH, 64, 64, OPAQUE_TEXT);

        // Play notes
        int i;
        for (i = 0; i < 3; i++) {
            TA0CCR0 = current_notes[i];
            TA0CTL |= MC_1;
            __delay_cycles(16000000);
            TA0CTL &= ~MC_3;
            P2OUT &= ~BIT7;
            __delay_cycles(8000000);
        }

        // Decision phase
        Graphics_clearDisplay(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, (int8_t *)"Was Pitch 3 Higher?", AUTO_STRING_LENGTH, 64, 40, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, (int8_t *)"Up: YES | Down: NO", AUTO_STRING_LENGTH, 64, 70, OPAQUE_TEXT);

        // Safety lock waiting for center
        while (read_joystick() > 3200 || read_joystick() < 800);

        int user_answer = 0;

        // Wait for answer
        while(user_answer == 0) {
            int move = read_joystick();
            if (move > 3200) user_answer = 1;
            else if (move < 800) user_answer = 2;
        }

        int actually_higher = (current_notes[2] < current_notes[0]) ? 1 : 2;

        Graphics_clearDisplay(&g_sContext);
        if (user_answer == actually_higher) {
            score++;
            Graphics_drawStringCentered(&g_sContext, (int8_t *)"CORRECT!", AUTO_STRING_LENGTH, 64, 64, OPAQUE_TEXT);
        } else {
            Graphics_drawStringCentered(&g_sContext, (int8_t *)"WRONG", AUTO_STRING_LENGTH, 64, 64, OPAQUE_TEXT);
        }
        __delay_cycles(16000000);
    }

    // Final score
    Graphics_clearDisplay(&g_sContext);
    char final[20];
    sprintf(final, "Score: %d/%d", score, total_rounds);
    Graphics_drawStringCentered(&g_sContext, (int8_t *)"GAME OVER", AUTO_STRING_LENGTH, 64, 40, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, (int8_t *)final, AUTO_STRING_LENGTH, 64, 70, OPAQUE_TEXT);

    while(1);
}

// Support functions
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
