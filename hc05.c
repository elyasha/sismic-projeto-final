/*
 * hc05.c
 *
 *  Created on: Oct 31, 2021
 *      Author: mathe
 */
// HC-05: VCC = +3.3 V
// HC-05: GND = GND
// HC-05: RXD = P3.3 (USCI_A0: TXD)
// HC-05: TXD = P3.4 (USCI_A0: RXD)
// USCI_A0
// 9.600 bauds (ACLK), 8 bits, sem paridade, 1 stop

#include <msp430.h>

// Enviar um caracter pela serial
void bt_char(char c)
{
    while ((UCA0IFG & UCTXIFG) == 0)
        ; //Esperar TXIFG=1
    UCA0TXBUF = c;
}

// Receber um caractere serial. Elimina '\n' ou '\r'
// Prende esperando chegar algo
char bt_ler(void)
{
    char c = '\n';
    while (c == '\n' || c == '\r')
    { //Consumir '\n' e '\r'
        while ((UCA0IFG & UCRXIFG) == 0)
            ;          //Esperar RXIFG=1
        c = UCA0RXBUF; //Ler buffer
    }
    return c;
}

// Enviar um valor decimal para o bluetooth
void bt_decimal(int valor)
{
    int z, x = valor;

    z = x / 100; // centenas
    bt_char(0x30 + z);

    x = x - 100 * z;
    z = x / 10; // dezenas
    bt_char(0x30 + z);

    x = x - 10 * z;
    z = x / 1; // unidades
    bt_char(0x30 + z);
}

// Enviar uma string pela serial
void bt_str(char *vet)
{
    unsigned int i = 0;
    while (vet[i] != '\0')
        bt_char(vet[i++]);
}

// Configurar USCI_A0
void USCI_A0_config(void)
{
    UCA0CTL1 = UCSWRST;   // RST=1 para USCI_A0
    UCA0CTL0 = 0;         // sem paridade, 8 bits, 1 stop, modo UART
    UCA0BRW = 3;          // Divisor
    UCA0MCTL = UCBRS_3;   // Modulador = 3 e UCOS=0
    P3SEL |= BIT4 | BIT3; // Disponibilizar pinos
    UCA0CTL1 = UCSSEL_1;  // RST=0, ACLK
}
