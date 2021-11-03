/*
 * gpio.c
 *
 *  Created on: Nov 2, 2021
 *      Author: mathe
 */
#include <msp430.h>

void configurar_led(void)
{

    P4DIR |= BIT7; // P4.7 = sa�da (verde)
    P4OUT &= ~BIT7;

    P1DIR |= BIT0; // P1.0 = sa�da (vermelho)
    P1OUT &= ~BIT0;
}



