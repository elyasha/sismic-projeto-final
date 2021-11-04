/*
 * i2c.c
 *
 *  Created on: Nov 2, 2021
 *      Author: mathe
 */
#include <msp430.h>
#include "i2c.h"

// Configurar USCI_B0
void USCI_B0_config(void)
{
    // Configurar MSP com mestre no I2C (Usando UCB0)
    UCB0CTL1 |= UCSWRST;  // UCSI B0 em ressete
    UCB0CTL0 = UCSYNC |   //S�ncrono
               UCMODE_3 | //Modo I2C
               UCMST;     //Mestre
    UCB0BRW = BR100K;     //100 kbps
    P3SEL |= BIT1 | BIT0; // Use dedicated module
    UCB0CTL1 = UCSSEL_2;  //SMCLK e remove ressete

    P3SEL |= BIT1 | BIT0; // Funcoes alternativas
    P3REN |= BIT1 | BIT0; // Habilitar resistor
    P3OUT |= BIT1 | BIT0; // Pullup
}

// Testar o endereco adr para escrita ou leitura
// modo = READ --> Enderecar escravo para leitura
// modo = WRITE --> Enderecar escravo para escrita
char i2c_test(char adr, char modo)
{
    UCB0I2CSA = adr; //Endereco a ser testado
    if (modo == WRITE)
    {                        //WR = Escravo receptor
        UCB0CTL1 |= UCTR;    //Mestre TX --> escravo RX
        UCB0CTL1 |= UCTXSTT; //Gerar STASRT
        while ((UCB0IFG & UCTXIFG) == 0)
            ; //TXIFG=1, START iniciado
    }
    else
    {                         //RD = Escravo transmissor
        UCB0CTL1 &= ~UCTR;    //Mestre RX <-- escravo TX
        UCB0CTL1 |= UCTXNACK; //NACK ao receber um dado
        UCB0CTL1 |= UCTXSTT;  //Gerar START
        while ((UCB0CTL1 & UCTXSTT) == UCTXSTT)
            ; //Esperar START
    }
    UCB0CTL1 |= UCTXSTP; //Gerar STOP
    while ((UCB0CTL1 & UCTXSTP) == UCTXSTP)
        ; //Esperar STOP
    //Teste do ACK
    if ((UCB0IFG & UCNACKIFG) == 0)
        return TRUE; //Chegou ACK
    else
        return FALSE; //Chegou NACK
}
