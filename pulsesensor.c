/*
 * pulsesensor.c
 *
 *  Created on: Nov 3, 2021
 *      Author: mathe
 */
#include <msp430.h>

/*
 * Configura��o do conversor AD
 */
void adc_config()
{

        ADC12CTL0 &= ~ADC12ENC;   // Desativa o trigger
        REFCTL0 = 0;              // Desativa o modulo de ref
        ADC12CTL0 = ADC12SHT0_2 | // 16 batidas de clk
                    ADC12MSC |    // Multiplas conversoes do trigger
                    ADC12ON;      // Liga o quantizador

        ADC12CTL1 = ADC12CSTARTADD_0 | // In�cio da mem�ria: Resultado em MEM0
                    ADC12SHS_0 |       // Trigger -> bit SC (por software)
                    ADC12SSEL_0 |      // Clock de 5MHZ
                    ADC12CONSEQ_0;

        ADC12CTL2 = ADC12TCOFF | // Desativa o sensor de temperatura
                    ADC12RES_2;  // Modo de 12 bits

        ADC12MCTL0 = 0; // Controle de mem�ria 0

        ADC12IE = BIT0;
        ADC12CTL0 |= ADC12ENC; // Habilitar o ADC
}

void timer_adc_config(void)
{
        TA0CTL = TASSEL__ACLK | MC__UP; // Timer para ADC12
        TA0CCR0 = 65;                   // f = 500 Hz
        TA0CCTL0 = CCIE;
}
