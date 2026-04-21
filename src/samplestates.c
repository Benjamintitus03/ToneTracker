
typedef enum {
    STATE_IDLE,
    STATE_STARTUP,
    STATE_MENU,
    STATE_TRACKING
} system_state_t;

// Start in the IDLE state
volatile system_state_t current_state = STATE_IDLE; 
volatile unsigned char menu_needs_redraw = 1;

void main(void) {
    

    // Draw the initial screen once before the loop starts
    display_idle(&g_sContext); 

    while (1) {
       switch (current_state) {
        
        case STATE_IDLE:
            // Check if Button S1  is pressed
            if (!(P1IN & S1)) { 
                current_state = STATE_MENU; // Change state!
                menu_needs_redraw = 1;      // Flag to draw the new screen
                __delay_cycles(2000000);    // Debounce delay
            }
            break;

        case STATE_MENU:
            if (menu_needs_redraw) {
               
                // display_menu(&g_sContext); 
                menu_needs_redraw = 0;
            }
            
            // Wait for joystick input to pick rounds, then press S1 to start
            if (!(P1IN & S1)) { 
                current_state = STATE_TRACKING; 
                __delay_cycles(2000000);
            }
            break;

        case STATE_TRACKING:

            
            // Once the game is over, go back to IDLE
            current_state = STATE_IDLE;
            display_idle(&g_sContext); // Redraw the welcome screen
            break;

        default:
            current_state = STATE_IDLE;
            break;
        }
    }
}
