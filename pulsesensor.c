/*
 * pulsesensor.c
 *
 *  Created on: Nov 3, 2021
 *      Author: mathe
 */
#include <msp430.h>

/*
 * Configuração do conversor AD
 */
void adc_config()
{

       ADC12CTL0 &= ~ADC12ENC; //desativa o trigger
       REFCTL0 = 0;            //desativa o modulo de ref
       ADC12CTL0 = ADC12SHT0_2 | //16 batidas de clk
               ADC12MSC    | //multiplas conversoes do trigger
               ADC12ON;       //liga o quantizador

       ADC12CTL1 = ADC12CSTARTADD_0 | //resultado MEM0
               ADC12SHS_0       |   //trigger -> bit SC
               ADC12SSEL_0      |   //clock de 5MHZ
               ADC12CONSEQ_0;        // single convers reptt

       ADC12CTL2 = ADC12TCOFF   |   //desativa o sensor de temperatura
               ADC12RES_2;         //resolucao de 12 bits

       ADC12MCTL0 = 0;  //ch0

       ADC12IE = BIT0;
       ADC12CTL0 |= ADC12ENC;  //habilita o conv ad
}

void timer_adc_config(void)
{
    TA0CTL = TASSEL__ACLK | MC__UP; //timer utiiizado pelo ADC12
    TA0CCR0 = 65; //500 hz
    TA0CCTL0 = CCIE;
}

