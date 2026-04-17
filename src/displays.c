//prototypes
void Initialize_Clock_System(void);
void display_idle(Graphics_Context *context);
void display_startup(Graphics_Context *context);
void display_menu_state(Graphics_Context *context);
void display_sequence_state(Graphics_Context *context, unsigned int current_round)
void draw_music_note(Graphics_Context *context, int x, int y);
void display_compare_state(Graphics_Context *context,
                           unsigned int slot1,
                           unsigned int slot2,
                           unsigned int preview_dir,
                           unsigned int round_num);
volatile unsigned int total_rounds;      // rounds chosen in menu state, 1-20
volatile unsigned int current_round = 1;     // current round

//global variables
Graphics_Context g_sContext;
//MENU STATE
volatile system_state_t current_state = STATE_WELCOME;
volatile unsigned int menu_rounds = 1;
volatile unsigned char menu_needs_redraw = 1;
volatile unsigned int total_rounds;      // rounds chosen in menu state, 1-20
volatile unsigned int current_round = 1;     // current round

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
void display_sequence_state(Graphics_Context *context, unsigned int round_num)
{
    char buf[3];

    Graphics_clearDisplay(context);

    Graphics_drawStringCentered(context, (int8_t *)"TONE TRACKER",
                                AUTO_STRING_LENGTH, 64, 10, OPAQUE_TEXT);

    Graphics_drawStringCentered(context, (int8_t *)"LISTEN CLOSELY!",
                                AUTO_STRING_LENGTH, 64, 40, OPAQUE_TEXT);

    //MUSIC NOTES:)
    draw_music_note(context, 34, 65);
    draw_music_note(context, 64, 65);
    draw_music_note(context, 94, 65);

    buf[0] = (round_num / 10) + '0';
    buf[1] = (round_num % 10) + '0';
    buf[2] = '\0';

    if (round_num < 10) {
        buf[0] = buf[1];
        buf[1] = '\0';
    }

    Graphics_drawStringCentered(context, (int8_t *)"Round",
                                AUTO_STRING_LENGTH, 50, 90, OPAQUE_TEXT);

    Graphics_drawStringCentered(context, (int8_t *)buf,
                                AUTO_STRING_LENGTH, 78, 90, OPAQUE_TEXT);

    Graphics_flushBuffer(context);
}

void draw_music_note(Graphics_Context *context, int x, int y)
{
    // note head
    Graphics_fillCircle(context, x, y, 2);

    // stem
    Graphics_drawLine(context, x + 2, y, x + 2, y - 12);

    // flag
    Graphics_drawLine(context, x + 3, y - 12, x + 8, y - 9);
    Graphics_drawLine(context, x + 3, y - 11, x + 8, y - 8);
}

void display_compare_state(Graphics_Context *context,
                           unsigned int slot1,
                           unsigned int slot2,
                           unsigned int preview_dir,
                           unsigned int round_num)
{
    char buf[3];

    // x positions for the two compare slots
    int slot1_x = 44;
    int slot2_x = 84;

    // y position for the arrows
    int arrow_y = 68;

    // underline heights
    int underline_y_high = 86;   // still picking first arrow
    int underline_y_low  = 92;   // first arrow locked in, now picking second

    Graphics_clearDisplay(context);

    // title
    Graphics_drawStringCentered(context, (int8_t *)"TONE TRACKER",
                                AUTO_STRING_LENGTH, 64, 10, OPAQUE_TEXT);

    // screen label
    Graphics_drawStringCentered(context, (int8_t *)"COMPARE NOTES!",
                                AUTO_STRING_LENGTH, 64, 35, OPAQUE_TEXT);

    // ---------------------------------------------------------
    // draw slot 1
    // if slot1 already has a confirmed direction, draw that
    // otherwise draw the current preview direction there
    // ---------------------------------------------------------
    if (slot1 != DIR_NONE) {
        draw_direction_arrow(context, slot1_x, arrow_y, slot1);
    } else {
        draw_direction_arrow(context, slot1_x, arrow_y, preview_dir);
    }

    // ---------------------------------------------------------
    // draw slot 2
    // only draw something in slot 2 once slot1 is already chosen
    // if slot2 is confirmed, draw that
    // if slot2 is not confirmed yet but slot1 is done, draw preview
    // ---------------------------------------------------------
    if (slot1 != DIR_NONE) {
        if (slot2 != DIR_NONE) {
            draw_direction_arrow(context, slot2_x, arrow_y, slot2);
        } else {
            draw_direction_arrow(context, slot2_x, arrow_y, preview_dir);
        }
    }

    // ---------------------------------------------------------
    // underline logic
    // if slot1 is still empty, underline slot1 a little higher
    // once slot1 is selected, move to slot2 and drop underline lower
    // ---------------------------------------------------------
    if (slot1 == DIR_NONE) {
        Graphics_drawLine(context, slot1_x - 8, underline_y_high,
                                   slot1_x + 8, underline_y_high);
    } else if (slot2 == DIR_NONE) {
        Graphics_drawLine(context, slot2_x - 8, underline_y_low,
                                   slot2_x + 8, underline_y_low);
    }

    // ---------------------------------------------------------
    // round number display
    // converts 1-20 into a string without snprintf
    // ---------------------------------------------------------
    buf[0] = (round_num / 10) + '0';
    buf[1] = (round_num % 10) + '0';
    buf[2] = '\0';

    // for 1-9, remove the leading zero
    if (round_num < 10) {
        buf[0] = buf[1];
        buf[1] = '\0';
    }

    Graphics_drawStringCentered(context, (int8_t *)"ROUND",
                                AUTO_STRING_LENGTH, 52, 105, OPAQUE_TEXT);

    Graphics_drawStringCentered(context, (int8_t *)buf,
                                AUTO_STRING_LENGTH, 82, 105, OPAQUE_TEXT);

    // bottom controls
    Graphics_drawStringCentered(context, (int8_t *)"S1:CLEAR",
                                AUTO_STRING_LENGTH, 30, 118, OPAQUE_TEXT);

    Graphics_drawStringCentered(context, (int8_t *)"S2:CONFIRM",
                                AUTO_STRING_LENGTH, 95, 118, OPAQUE_TEXT);

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

void draw_big_up_arrow(Graphics_Context *context, int x, int y)
{
    // shaft
    Graphics_drawLine(context, x, y, x, y + 18);

    // arrow head
    Graphics_drawLine(context, x, y, x - 6, y + 6);
    Graphics_drawLine(context, x, y, x + 6, y + 6);
}

void draw_big_down_arrow(Graphics_Context *context, int x, int y)
{
    // shaft
    Graphics_drawLine(context, x, y, x, y - 18);

    // arrow head
    Graphics_drawLine(context, x, y, x - 6, y - 6);
    Graphics_drawLine(context, x, y, x + 6, y - 6);
}
void draw_direction_arrow(Graphics_Context *context, int x, int y, unsigned int dir)
{
    if (dir == DIR_UP) {
        draw_big_up_arrow(context, x, y);
    }
    else if (dir == DIR_DOWN) {
        draw_big_down_arrow(context, x, y);
    }
}
