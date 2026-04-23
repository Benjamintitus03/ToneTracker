void HAL_LCD_PortInit(void)
{
    // SPI pins: P1.4 = UCB0CLK, P1.6 = UCB0SIMO
    P1SEL0 |= (BIT4 | BIT6);
    P1SEL1 &= ~(BIT4 | BIT6);

    // GPIO pins: P2.3 = DC, P2.5 = CS, P2.6 = backlight, P9.4 = RESET
    P2SEL0 &= ~(BIT3 | BIT5 | BIT6);
    P2SEL1 &= ~(BIT3 | BIT5 | BIT6);
    P9SEL0 &= ~BIT4;
    P9SEL1 &= ~BIT4;

    // Set directions
    P2DIR |= (BIT3 | BIT5 | BIT6);
    P9DIR |= BIT4;

    // Start from known outputs
    P2OUT &= ~(BIT3 | BIT5 | BIT6);
    P9OUT &= ~BIT4;

    P2OUT &= ~BIT5;  // CS permanently low (enabled)
    P9OUT |= BIT4;   // RESET high
    P2OUT |= BIT6;   // backlight on
}
void HAL_LCD_SpiInit(void)
{
    UCB0CTLW0 = UCSWRST;                              // hold in reset

    UCB0CTLW0 |= UCSSEL__SMCLK;                       // SMCLK
    UCB0CTLW0 |= UCSYNC;                              // SPI
    UCB0CTLW0 |= UCMST;                               // master
    UCB0CTLW0 |= UCMSB;                               // MSB first
    UCB0CTLW0 |= UCCKPH;
    UCB0CTLW0 &= ~UCCKPL;
    // UCMODE defaults to 00 = 3-pin SPI, which is what we want

    UCB0BRW = 2;                                      // 16 MHz / 2 = 8 MHz

    UCB0CTLW0 &= ~UCSWRST;                            // enable module
}
