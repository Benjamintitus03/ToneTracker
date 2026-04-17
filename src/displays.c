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

void display_menu_state(Graphics_Context *context)
{
  //PLACE CLAMPS WHERE WE UPDATE STATE (ISR?)BELOW ARE THE CLAMPS FOR MENU_ROUNDS TO NOT GO OUT OF RANGE (1=20)
  //if (menu_rounds < 1) menu_rounds = 1;
//if (menu_rounds > 20) menu_rounds = 20;
    char buf[3];
    Graphics_clearDisplay(context);
    Graphics_drawStringCentered(context, (int8_t *)"TONE TRACKER", AUTO_STRING_LENGTH, 64, 10, OPAQUE_TEXT);
    Graphics_drawStringCentered(context, (int8_t *)"TOTAL ROUNDS", AUTO_STRING_LENGTH, 64, 40, OPAQUE_TEXT);



    // Up arrow
    Graphics_drawLine(context, 64, 60, 60, 66);
    Graphics_drawLine(context, 64, 60, 68, 66);

    buf[0] = (menu_rounds / 10) + '0';
    buf[1] = (menu_rounds % 10) + '0';
    buf[2] = '\0';
//FIXES SINGLE DIGITS TO NOT DISPLAY LEADING ZEROS
    if (menu_rounds < 10) {
        buf[0] = buf[1];
        buf[1] = '\0';
    }
    // Number at y = 73
    Graphics_drawStringCentered(context, (int8_t *)buf,
                                AUTO_STRING_LENGTH, 65, 74, OPAQUE_TEXT);

    // Down arrow
    Graphics_drawLine(context, 60, 84, 64, 90);
    Graphics_drawLine(context, 68, 84, 64, 90);

    Graphics_drawStringCentered(context, (int8_t *)"PRESS S2 TO CONFIRM", AUTO_STRING_LENGTH, 64, 110, OPAQUE_TEXT);
    Graphics_flushBuffer(context);
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

