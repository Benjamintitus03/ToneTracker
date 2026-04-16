//WELCOME SCREEN AKA IDLE STATE
void display_idle(Graphics_Context *context){
  Graphics_clearDisplay(context);
    // Set background and foreground colors
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_PINK);

    // Set the default font for strings
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    // Print message
    Graphics_drawStringCentered(&g_sContext, "Tone Tracker", AUTO_STRING_LENGTH, 64, 10, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Made By:", AUTO_STRING_LENGTH, 64, 105, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Natalia & Benjamin", AUTO_STRING_LENGTH, 64, 120, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, "PRESS S1 TO PLAY", AUTO_STRING_LENGTH, 64, 50, OPAQUE_TEXT);
}
// MUSIC PLAYS FOR 5 SECONDS IN THIS STATE
void display_startup(Graphics_Context *context){
  Graphics_clearDisplay(context);
    // Set background and foreground colors
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_PINK);

    // Set the default font for strings
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    // Print message
    Graphics_drawStringCentered(&g_sContext, "Tone Tracker", AUTO_STRING_LENGTH, 64, 10, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Made By:", AUTO_STRING_LENGTH, 64, 105, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Natalia & Benjamin", AUTO_STRING_LENGTH, 64, 120, OPAQUE_TEXT);
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

