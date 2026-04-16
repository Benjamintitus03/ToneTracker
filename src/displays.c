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
    Graphics_drawStringCentered(&g_sContext, "Natalia & Benjamin", AUTO_STRING_LENGTH, 64, 120, OPAQUE_TEXT);=
}

