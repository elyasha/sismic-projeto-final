/*
 * lcd.c
 *
 *  Created on: Nov 2, 2021
 *      Author: mathe
 */
#include <msp430.h>
#include <lcd.h>

void lcd_atraso(long x)
{
    volatile int y;
    for (y = 0; y < x; y++)
        ;
}

// Escrever dado na porta
void lcd_i2c_write(char dado)
{
    UCB0I2CSA = LCD_ADDRESS;
    UCB0CTL1 |= UCTR | UCTXSTT; // TX+START
    while ((UCB0IFG & UCTXIFG) == 0)
        ;             // TXIFG?
    UCB0TXBUF = dado; //dado
    while ((UCB0CTL1 & UCTXSTT) == UCTXSTT)
        ;
    if ((UCB0IFG & UCNACKIFG) == UCNACKIFG)
    {
        P1OUT |= BIT0; // NACK=Sinalizar problema
        while (1)
            ; // Travar execu��o
    }
    UCB0CTL1 |= UCTXSTP; // Gerar STOP
    while ((UCB0CTL1 & UCTXSTP) == UCTXSTP)
        ;
}

// Funcao para escrever uma letra (RW=0 e RS=1)
void lcd_escrever_char(char x)
{
    char most_sig_nibble, least_sig_nibble;
    most_sig_nibble = x & 0xF0; // ABCD EFGH & 1111 0000
    least_sig_nibble = x << 4;  // ABCD EFGH

    lcd_i2c_write(most_sig_nibble | 0x9);
    lcd_i2c_write(most_sig_nibble | 0xD);
    lcd_i2c_write(most_sig_nibble | 0x9);
    lcd_i2c_write(least_sig_nibble | 0x9);
    lcd_i2c_write(least_sig_nibble | 0xD);
    lcd_i2c_write(least_sig_nibble | 0x9);
}

// Enviar comando para o LCD (RW=0 e RS=0)
void lcd_comando(char x)
{
    char most_sig_nibble, least_sig_nibble;
    most_sig_nibble = x & 0xF0;
    least_sig_nibble = x << 4;

    lcd_i2c_write(most_sig_nibble | 0x8);
    lcd_i2c_write(most_sig_nibble | 0xC);
    lcd_i2c_write(most_sig_nibble | 0x8);
    lcd_i2c_write(least_sig_nibble | 0x8);
    lcd_i2c_write(least_sig_nibble | 0xC);
    lcd_i2c_write(least_sig_nibble | 0x8);
}

// Mover o cursor
void lcd_cursor(char x)
{
    lcd_comando(0x80 | (x & 0x7F));
}

// Funcao para escrever uma string
void lcd_escrever_string(char *vt)
{
    unsigned int i = 0;
    while (vt[i] != '\0')
    {
        lcd_escrever_char(vt[i++]);
    }
}

// Apagar todas informa��es do LCD
void lcd_limpar()
{
    lcd_cursor(0);
    lcd_escrever_string("                ");
    lcd_cursor(0x40);
    lcd_escrever_string("                ");
}

/*
 * Auxiliar inicializa��o do LCD (RS=RW=0)
 * S� serve para a inicializa��o
 */
void lcd_aux(char dado)
{
    while ((UCB0IFG & UCTXIFG) == 0)
        ;                                      // Esperar TXIFG=1
    UCB0TXBUF = ((dado << 4) & 0XF0) | BIT_BL; // PCF7:4 = dado;
    lcd_atraso(50);
    while ((UCB0IFG & UCTXIFG) == 0)
        ;                                              // Esperar TXIFG=1
    UCB0TXBUF = ((dado << 4) & 0XF0) | BIT_BL | BIT_E; // E=1
    lcd_atraso(50);
    while ((UCB0IFG & UCTXIFG) == 0)
        ;                                      // Esperar TXIFG=1
    UCB0TXBUF = ((dado << 4) & 0XF0) | BIT_BL; // E=0
}

/*
 * Inicializar LCD em modo 4 bits
 */
void lcd_inic(void)
{

    UCB0I2CSA = LCD_ADDRESS; //Endere�o do Escravo

    // Preparar I2C para operar
    UCB0CTL1 |= UCTR |   // Mestre TX
                UCTXSTT; // Gerar START

    while ((UCB0IFG & UCTXIFG) == 0)
        ;          // Esperar TXIFG = 1
    UCB0TXBUF = 0; // Sa�da PCF = 0;
    while ((UCB0CTL1 & UCTXSTT) == UCTXSTT)
        ; // Esperar STT = 0

    // Comecar inicializa��o
    lcd_aux(0); // RS=RW=0, BL=1
    lcd_atraso(20000);
    lcd_aux(3); // 3
    lcd_atraso(10000);
    lcd_aux(3); // 3
    lcd_atraso(10000);
    lcd_aux(3); // 3
    lcd_atraso(10000);
    lcd_aux(2); // 2
    lcd_atraso(10000);

    // Entrou em modo 4 bits
    lcd_aux(2);
    lcd_aux(8); // 0x28
    lcd_aux(0);
    lcd_aux(8); // 0x08
    lcd_aux(0);
    lcd_aux(1); // 0x01
    lcd_aux(0);
    lcd_aux(6); // 0x06
    lcd_aux(0);
    lcd_aux(0xF); // 0x0F

    while ((UCB0IFG & UCTXIFG) == 0)
        ;                // Esperar TXIFG=1
    UCB0CTL1 |= UCTXSTP; // Gerar STOP
    while ((UCB0CTL1 & UCTXSTP) == UCTXSTP)
        ; // Esperar STOP
    lcd_atraso(50);
}

void lcd_decimal(int y)
{
    int z, x = y;

    z = x / 100; // centenas
    lcd_escrever_char(0x30 + z);

    x = x - 100 * z;
    z = x / 10; // dezenas
    lcd_escrever_char(0x30 + z);

    x = x - 10 * z;
    z = x / 1; // unidades
    lcd_escrever_char(0x30 + z);
}
