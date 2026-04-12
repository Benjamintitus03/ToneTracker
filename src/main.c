#include <msp430fr6989.h>
#include "Grlib/grlib/grlib.h"          // Graphics library (grlib)
#include "LcdDriver/lcd_driver.h"       // LCD driver
#include <stdio.h>

#define redLED BIT0
#define greenLED BIT7
#define S1 BIT1
#define S2 BIT2

void Initialize_Clock_System();
void DRAW_WELCOME(Graphics_Context *context);

void main(void) {
    // Stop the watchdog timer (critical for all MSP430 code)
    WDTCTL = WDTPW | WDTHOLD;

    PM5CTL0 &= ~LOCKLPM5;

    // Configure Pin 2.0 to its Timer A1 Output function
    P2DIR |= BIT0;
    P2SEL0 |= BIT0;
    P2SEL1 &= ~BIT0;

    // Set the Pitch: 440 Hz (Note 'A')
    TA1CCR0 = 2272;       

    TA1CCR1 = 1136;       
    

    TA1CCTL1 = OUTMOD_7;
    TA1CTL = TASSEL_2 | MC_1 | TACLR;

    Graphics_Context g_sContext;
    //INIT PIXEL DISPLAY
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(0);

    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
    //DRAW WELCOME AT STARTUP
    DRAW_WELCOME(&g_sContext);

    // Infinite loop to keep the microcontroller running
    while (1) {
        // Just hanging out while the buzzer screams
    }
}

void Initialize_Clock_System() {
  // DCO frequency = 16 MHz
  // MCLK = fDCO/1 = 16 MHz
  // SMCLK = fDCO/1 = 16 MHz

  // Activate memory wait state
  FRCTL0 = FRCTLPW | NWAITS_1;    // Wait state=1
  CSCTL0 = CSKEY;
  // Set DCOFSEL to 4 (3-bit field)
  CSCTL1 &= ~DCOFSEL_7;
  CSCTL1 |= DCOFSEL_4;
  // Set DCORSEL to 1 (1-bit field)
  CSCTL1 |= DCORSEL;
  // Change the dividers to 0 (div by 1)
  CSCTL3 &= ~(DIVS2|DIVS1|DIVS0);    // DIVS=0 (3-bit)
  CSCTL3 &= ~(DIVM2|DIVM1|DIVM0);    // DIVM=0 (3-bit)
  CSCTL0_H = 0;

  return;
}
