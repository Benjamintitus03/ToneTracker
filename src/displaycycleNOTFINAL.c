// cycles through all display states not needed in final revision

#include "msp430fr6989.h"
#include "Grlib/grlib/grlib.h"          // Graphics library (grlib)
#include "LcdDriver/lcd_driver.h"       // LCD driver
#include <stdio.h>
/*
EEL 4742C - UCF

Code that prints a welcome message to the pixel display.
*/
typedef enum {
    DIR_NONE = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} direction_t;

#define DIR_UP   1
#define DIR_DOWN 0
#define DIR_NONE 2
#define RESULT_CORRECT 1
#define RESULT_WRONG   0
#define RESTART_YES 0
#define RESTART_NO  1

Graphics_Context g_sContext;        // Declare a graphic library context GLOBALLY
void draw_big_checkmark(Graphics_Context *context, int x, int y);
void draw_big_x(Graphics_Context *context, int x, int y);
void draw_music_note(Graphics_Context *context, int x, int y);
void draw_direction_arrow(Graphics_Context *context, int x, int y, unsigned int dir);
void draw_big_down_arrow(Graphics_Context *context, int x, int y);
void draw_big_up_arrow(Graphics_Context *context, int x, int y);
volatile unsigned int total_rounds;      // rounds chosen in menu state, 1-20
volatile unsigned int current_round = 1;     // current round
//prototypes
void Initialize_Clock_System(void);
void display_idle_state(Graphics_Context *context);
void display_startup_state(Graphics_Context *context);
void display_menu_state(Graphics_Context *context);
void display_sequence_state(Graphics_Context *context, unsigned int current_round);
void display_compare_state(Graphics_Context *context,
                           unsigned int slot1,
                           unsigned int slot2,
                           unsigned int preview_dir,
                           unsigned int round_num);
void display_round_feedback_state(Graphics_Context *context, unsigned int result);
void display_final_score_state(Graphics_Context *context,
                               unsigned int score,
                               unsigned int total_rounds);
void display_confirm_restart_state(Graphics_Context *context, unsigned int selection);
//DESIGNS

typedef enum {
    STATE_WELCOME,
    STATE_STARTUP,
    STATE_MENU,
    STATE_SEQUENCE,
    STATE_COMPARE,
    STATE_ROUND_FEEDBACK,
    STATE_FINAL_SCORE,
    STATE_CONFIRM_RESTART
} system_state_t;
//MENU STATE
volatile system_state_t current_state = STATE_WELCOME;
volatile unsigned int menu_rounds = 1;
volatile unsigned char menu_needs_redraw = 1;


#define redLED BIT0
#define greenLED BIT7
#define S1 BIT1
#define S2 BIT2

void Initialize_Clock_System();

// ****************************************************************************
void main(void) {
    char mystring[20];

    // Configure WDT & GPIO
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    // Configure LEDs
    P1DIR |= redLED;                P9DIR |= greenLED;
    P1OUT &= ~redLED;               P9OUT &= ~greenLED;

    // Configure buttons
    P1DIR &= ~(S1|S2);
    P1REN |= (S1|S2);
    P1OUT |= (S1|S2);
    P1IFG &= ~(S1|S2);          // Flags are used for latched polling

    // Set the LCD backlight to highest level
    //P2DIR |= BIT6;
    //P2OUT |= BIT6;

    // Configure clock system
    Initialize_Clock_System();

    ////////////////////////////////////////////////////////////////////////////////////////////
    // Graphics functions
    Crystalfontz128x128_Init();         // Initialize the display

    // Set the screen orientation
    Crystalfontz128x128_SetOrientation(0);

    // Initialize the context
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);

    // Set background and foreground colors
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);

    // Set the default font for strings
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);

    //Clear the screen
    Graphics_clearDisplay(&g_sContext);
    ////////////////////////////////////////////////////////////////////////////////////////////

    // Print message

    Graphics_drawStringCentered(&g_sContext, mystring, AUTO_STRING_LENGTH, 64, 55, OPAQUE_TEXT);

    while(1){ // 1. IDLE / WELCOME
        display_idle_state(&g_sContext);
        delay_ms(2500);

        // 2. STARTUP
        display_startup_state(&g_sContext);
        delay_ms(2500);

        // 3. MENU (simulate different values)
        menu_rounds = 5;
        display_menu_state(&g_sContext);
        delay_ms(2500);

        menu_rounds = 12;
        display_menu_state(&g_sContext);
        delay_ms(2500);

        // 4. SEQUENCE
        display_sequence_state(&g_sContext, 1);
        delay_ms(2500);

        display_sequence_state(&g_sContext, 10);
        delay_ms(2500);

        // 5. COMPARE (simulate progression)
        display_compare_state(&g_sContext, DIR_NONE, DIR_NONE, DIR_UP, 1);
        delay_ms(2500);

        display_compare_state(&g_sContext, DIR_UP, DIR_NONE, DIR_DOWN, 1);
        delay_ms(2500);

        display_compare_state(&g_sContext, DIR_UP, DIR_DOWN, DIR_NONE, 1);
        delay_ms(2500);

        // 6. ROUND FEEDBACK
        display_round_feedback_state(&g_sContext, RESULT_CORRECT);
        delay_ms(2000);

        display_round_feedback_state(&g_sContext, RESULT_WRONG);
        delay_ms(2000);

        // 7. FINAL SCORE
        display_final_score_state(&g_sContext, 7, 10);
        delay_ms(3000);

        // 8. CONFIRM RESTART
        display_confirm_restart_state(&g_sContext, RESTART_YES);
        delay_ms(2000);

        display_confirm_restart_state(&g_sContext, RESTART_NO);
        delay_ms(2000);}

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

void delay_ms(unsigned int ms)
{
    while (ms--)
    {
        __delay_cycles(16000); // 16 MHz → 1 ms
    }
}
//WELCOME SCREEN AKA IDLE STATE
void display_idle_state(Graphics_Context *context){
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
void display_startup_state(Graphics_Context *context){
  Graphics_clearDisplay(context);
    // Set background and foreground colors
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_PINK);
    //MUSIC NOTES:)
    draw_music_note(context, 34, 65);
    draw_music_note(context, 64, 65);
    draw_music_note(context, 94, 65);

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
//buf stores space for x characters like
    buf[0] = (menu_rounds / 10) + '0'; // left digit
    buf[1] = (menu_rounds % 10) + '0'; //right digit
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

    int slot1_x = 44;
    int slot2_x = 84;
    int arrow_y = 70;

    Graphics_clearDisplay(context);

    // title
    Graphics_drawStringCentered(context, (int8_t *)"TONE TRACKER",
                                AUTO_STRING_LENGTH, 64, 10, OPAQUE_TEXT);

    // screen label
    Graphics_drawStringCentered(context, (int8_t *)"COMPARE NOTES!",
                                AUTO_STRING_LENGTH, 64, 35, OPAQUE_TEXT);

    // slot 1
    if (slot1 != DIR_NONE) {
        draw_direction_arrow(context, slot1_x, arrow_y, slot1);
    } else {
        draw_direction_arrow(context, slot1_x, arrow_y, preview_dir);
    }

    // slot 2
    if (slot1 != DIR_NONE) {
        if (slot2 != DIR_NONE) {
            draw_direction_arrow(context, slot2_x, arrow_y, slot2);
        } else {
            draw_direction_arrow(context, slot2_x, arrow_y, preview_dir);
        }
    }

    // round number display
    buf[0] = (round_num / 10) + '0';
    buf[1] = (round_num % 10) + '0';
    buf[2] = '\0';

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
void display_round_feedback_state(Graphics_Context *context, unsigned int result){
    Graphics_clearDisplay(context);

        // title
        Graphics_drawStringCentered(context, (int8_t *)"TONE TRACKER",
                                    AUTO_STRING_LENGTH, 64, 10, OPAQUE_TEXT);

        // 3 centered music notes under title
        draw_music_note(context, 42, 32);
        draw_music_note(context, 64, 32);
        draw_music_note(context, 86, 32);

        // big result symbol in middle
        if (result == RESULT_CORRECT) {
            draw_big_checkmark(context, 64, 78);
        } else {
            draw_big_x(context, 64, 78);
        }

        Graphics_flushBuffer(context);
}
void display_final_score_state(Graphics_Context *context,
                               unsigned int score,
                               unsigned int total_rounds)
{
    char buf[6]; // enough for "20/20"

    Graphics_clearDisplay(context);
    // TITLE
    Graphics_drawStringCentered(context, (int8_t *)"TONE TRACKER",
                                AUTO_STRING_LENGTH, 64, 10, OPAQUE_TEXT);
    // MUSIC NOTES (centered cluster)
    draw_music_note(context, 44, 28);
    draw_music_note(context, 64, 28);
    draw_music_note(context, 84, 28);
    Graphics_drawStringCentered(context, (int8_t *)"YOU SCORED",
                                AUTO_STRING_LENGTH, 64, 50, OPAQUE_TEXT);
    // SCORE FORMAT: x/ROUNDS
    // build string manually (snprintf wasnt allowed:/)

    // tens digit of score
    if (score >= 10) {
        buf[0] = (score / 10) + '0';
        buf[1] = (score % 10) + '0';
        buf[2] = '/';
        buf[3] = (total_rounds / 10) + '0';
        buf[4] = (total_rounds % 10) + '0';
        buf[5] = '\0';
    } else {
        buf[0] = score + '0';
        buf[1] = '/';
        buf[2] = (total_rounds / 10) + '0';
        buf[3] = (total_rounds % 10) + '0';
        buf[4] = '\0';
    }

    Graphics_drawStringCentered(context, (int8_t *)buf,
                                AUTO_STRING_LENGTH, 64, 70, OPAQUE_TEXT);
    // RESTART INSTRUCTION
  //I dont want S1 TO GO TO CONFIRM_RESTART_sTATE it should auto go to idle or start up

    Graphics_drawStringCentered(context, (int8_t *)"Press S1 To Restart",
                                AUTO_STRING_LENGTH, 64, 105, OPAQUE_TEXT);

    Graphics_flushBuffer(context);
}
void display_confirm_restart_state(Graphics_Context *context, unsigned int selection)
{
    int yes_x = 24;
    int no_x  = 96;
    int choice_y = 78;
    int underline_y = 92;

    Graphics_clearDisplay(context);
    // title
    Graphics_drawStringCentered(context, (int8_t *)"TONE TRACKER",
                                AUTO_STRING_LENGTH, 64, 10, OPAQUE_TEXT);

    // 3 music notes
    draw_music_note(context, 44, 28);
    draw_music_note(context, 64, 28);
    draw_music_note(context, 84, 28);
    // restart prompt
    Graphics_drawStringCentered(context, (int8_t *)"RESTART GAME?",
                                AUTO_STRING_LENGTH, 64, 50, OPAQUE_TEXT);

    Graphics_drawStringCentered(context, (int8_t *)"Progress will be LOST",
                                AUTO_STRING_LENGTH, 64, 62, OPAQUE_TEXT);
    // choices

    Graphics_drawStringCentered(context, (int8_t *)"YES",
                                AUTO_STRING_LENGTH, yes_x, choice_y, OPAQUE_TEXT);

    Graphics_drawStringCentered(context, (int8_t *)"NO",
                                AUTO_STRING_LENGTH, no_x, choice_y, OPAQUE_TEXT);


    // underline active selection
    // joystick will toggle this between yes and no
    if (selection == RESTART_YES) {
        Graphics_drawLine(context, yes_x - 10, underline_y,
                                   yes_x + 10, underline_y);
    } else {
        Graphics_drawLine(context, no_x - 8, underline_y,
                                   no_x + 8, underline_y);
    }

    Graphics_drawStringCentered(context, (int8_t *)"S2:CONFIRM SELECTION",
                                AUTO_STRING_LENGTH, 64, 112, OPAQUE_TEXT);

    Graphics_flushBuffer(context);
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
    switch (dir) {
    case DIR_UP:
        Graphics_drawLine(context, x, y - 6, x, y + 6);
        Graphics_drawLine(context, x, y - 6, x - 4, y - 2);
        Graphics_drawLine(context, x, y - 6, x + 4, y - 2);
        break;

    case DIR_DOWN:
        Graphics_drawLine(context, x, y - 6, x, y + 6);
        Graphics_drawLine(context, x, y + 6, x - 4, y + 2);
        Graphics_drawLine(context, x, y + 6, x + 4, y + 2);
        break;

    case DIR_LEFT:
        Graphics_drawLine(context, x - 6, y, x + 6, y);
        Graphics_drawLine(context, x - 6, y, x - 2, y - 4);
        Graphics_drawLine(context, x - 6, y, x - 2, y + 4);
        break;

    case DIR_RIGHT:
        Graphics_drawLine(context, x - 6, y, x + 6, y);
        Graphics_drawLine(context, x + 6, y, x + 2, y - 4);
        Graphics_drawLine(context, x + 6, y, x + 2, y + 4);
        break;

    default:
        break;
    }
}
void draw_big_checkmark(Graphics_Context *context, int x, int y)
{
    // left rising part
    Graphics_drawLine(context, x - 10, y,     x - 2, y + 10);
    Graphics_drawLine(context, x - 9,  y,     x - 1, y + 10);

    // right longer part
    Graphics_drawLine(context, x - 2, y + 10, x + 14, y - 12);
    Graphics_drawLine(context, x - 1, y + 10, x + 15, y - 12);
}
void draw_big_x(Graphics_Context *context, int x, int y)
{
    Graphics_drawLine(context, x - 12, y - 12, x + 12, y + 12);
    Graphics_drawLine(context, x - 11, y - 12, x + 13, y + 12);

    Graphics_drawLine(context, x - 12, y + 12, x + 12, y - 12);
    Graphics_drawLine(context, x - 11, y + 12, x + 13, y - 12);
}






