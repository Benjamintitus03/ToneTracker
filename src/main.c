#include <msp430fr6989.h>
#include "Grlib/grlib/grlib.h"          // Graphics library (grlib)
#include "LcdDriver/lcd_driver.h"       // LCD driver
#include <stdio.h>

#define redLED BIT0
#define greenLED BIT7
#define S1 BIT1
#define S2 BIT2
//prototypes
void Initialize_Clock_System(void);
void display_idle(Graphics_Context *context);
void display_startup(Graphics_Context *context);
void display_menu_state(Graphics_Context *context);
typedef enum {
    STATE_IDLE,
    STATE_STARTUP
    STATE_MENU,
    STATE_TRACKING
} system_state_t;
//global variables
Graphics_Context g_sContext;
//MENU STATE
volatile system_state_t current_state = STATE_WELCOME;
volatile unsigned int menu_rounds = 1;
volatile unsigned char menu_needs_redraw = 1;

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
    
//WE SHOULD CONTROL THE BUZZER IN STATE BEHAVIOR, we dont want tone to start right away and stay on
    TA1CCTL1 = OUTMOD_7;
    TA1CTL = TASSEL_2 | MC_1 | TACLR;

    
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
       switch (current_state)
        {
        case STATE_WELCOME:
            // Wait for button or timeout to move to menu
            break;

        case STATE_MENU:
            if (menu_needs_redraw)
            {
                menu_needs_redraw = 0;
                DRAW_MENU(&g_sContext);
            }
            break;

        case STATE_TRACKING:
            break;

        default:
            break;
        // Just hanging out while the buzzer screams
    }
}


