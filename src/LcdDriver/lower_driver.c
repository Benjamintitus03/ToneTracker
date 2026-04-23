// This code was ported from TI's sample code. See Copyright notice at the bottom of this file.

#include <LcdDriver/lower_driver.h>
#include "msp430fr6989.h"
#include "Grlib/grlib/grlib.h"
#include <stdint.h>

void HAL_LCD_PortInit(void)
{
    // Configure SPI Pins: P1.4 (Clock) and P1.6 (SIMO)
    P1SEL0 |= BIT4 | BIT6;
    P1SEL1 &= ~(BIT4 | BIT6);

    // LCD Reset Pin: P9.4
    P9DIR |= BIT4;
    
    // LCD Data/Command (D/C): P2.3
    P2DIR |= BIT3;
    
    // LCD Chip Select (CS): P8.0
    P8DIR |= BIT0;
    P8OUT &= ~BIT0; // Keep CS low to stay enabled
}

void HAL_LCD_SpiInit(void)
{
    // Put eUSCI in reset state
    UCB0CTLW0 = UCSWRST;

    // 3-pin SPI Master, MSB first, 8-bit, Synchronous, SMCLK
    UCB0CTLW0 |= UCCKPH | UCMSB | UCMST | UCSYNC | UCSSEL__SMCLK;

    // Clock Divider: 16MHz / 2 = 8MHz (LCD max is 10MHz)
    UCB0BRW = 0x0002;

    // Exit reset state
    UCB0CTLW0 &= ~UCSWRST;
}


//*****************************************************************************
// Writes a command to the CFAF128128B-0145T.  This function implements the basic SPI
// interface to the LCD display.
//*****************************************************************************
void HAL_LCD_writeCommand(uint8_t command)
{
    // Wait as long as the module is busy
    while (UCB0STATW & UCBUSY);

    // For command, set the DC' bit to low before transmission
    P2OUT &= ~BIT3;

    // Transmit data
    UCB0TXBUF = command;

    return;
}


//*****************************************************************************
// Writes a data to the CFAF128128B-0145T.  This function implements the basic SPI
// interface to the LCD display.
//*****************************************************************************
void HAL_LCD_writeData(uint8_t data)
{
    // Wait as long as the module is busy
    while (UCB0STATW & UCBUSY);

    // Set DC' bit back to high
    P2OUT |= BIT3;

    // Transmit data
    UCB0TXBUF = data;

    return;
}






/* --COPYRIGHT--,BSD
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
