/*
 * gpio.c
 *
 *  Created on: Nov 2, 2021
 *      Author: mathe
 */
#include <msp430.h>

void configurar_led(void)
{

    P4DIR |= BIT7; // P4.7 = saida (verde)
    P4OUT &= ~BIT7;

    P1DIR |= BIT0; // P1.0 = saida (vermelho)
    P1OUT &= ~BIT0;
}

void configurar_pinos(void)
{
    P6SEL |= BIT0; // Pino utilizado para receber o sinal de frequencia cardiaca
}
