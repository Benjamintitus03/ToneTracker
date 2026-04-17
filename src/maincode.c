#include <msp430fr6989.h>
#include "Grlib/grlib/grlib.h"          // Graphics library
#include "LcdDriver/lcd_driver.h"       // LCD driver
#include <stdio.h>

// Global Graphics Context
Graphics_Context g_sContext;

void main(void) {
    // 1. STANDARD SETUP
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    // Initialize Clock to 16MHz (Required for LCD speed)
    Initialize_Clock_System();

    // 2. LCD INITIALIZATION
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(0);
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);

    // 3. WELCOME SCREEN
    Graphics_drawStringCentered(&g_sContext, "TONE TRACKER", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Push Joystick Up", AUTO_STRING_LENGTH, 64, 60, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, "To Start", AUTO_STRING_LENGTH, 64, 75, OPAQUE_TEXT);

    // Keep your buzzer/joystick logic here...
}
void Initialize_Clock_System() {
  // DCO frequency = 16 MHz
  // MCLK = fDCO/1 = 16 MHz
  // SMCLK = fDCO/1 = 16 MHz

  // Activate memory wait state for 16MHz operation
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
