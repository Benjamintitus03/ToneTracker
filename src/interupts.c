//TIMER INTERRUPT

//JOYSTICK INTERRUPT FOR MENU STATE
#pragma vector=PORTx_VECTOR
__interrupt void joystick_ISR(void)
{
    if (current_state == STATE_MENU)
    {
        if (JOYSTICK_UP_FLAG)
        {
            if (menu_rounds < 20)
            {
                menu_rounds++;
            }
            menu_needs_redraw = 1;
            CLEAR_JOYSTICK_UP_FLAG();
        }

        if (JOYSTICK_DOWN_FLAG)
        {
            if (menu_rounds > 1)
            {
                menu_rounds--;
            }
            menu_needs_redraw = 1;
            CLEAR_JOYSTICK_DOWN_FLAG();
        }
    }
}
