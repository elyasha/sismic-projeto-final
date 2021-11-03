/*
 * Projeto Final
 * Matheus Elyasha Lopes
 * 4 Novembro 2021
 *
 * Uma aplica��o embarcada que transforma a LaunchPad num medidor de frequ�ncia cardiaca
 * e mostra o resultado dos batimentos cardiacos num display LCD e al�m disso monitora
 * o n�vel de batimentos e informa num terminal que a pessoa est� num n�vel acima de um limiar definido
 *
 * I2C: MAX30102 + LCD (com m�dulo i2c)
 * UART: HC-05
 */
#include <msp430.h> 
#include <stdint.h>
#include "gpio.h"
#include "hc05.h"
#include "i2c.h"
#include "lcd.h"
#include "max30102.h"

#define LED1_ON (P1OUT |= BIT0)
#define LED1_OFF (P1OUT &= ~BIT0)
#define LED1_TOGGLE (P1OUT ^= BIT0)

#define LED2_ON (P4OUT |= BIT7)
#define LED2_OFF (P4OUT &= ~BIT7)
#define LED2_TOGGLE (P4OUT ^= BIT7

#define SMCLK 1048576L // frequencia do SMCLK
#define ACLK 32768     // frequencia do ACLK



#define TRUE 1
#define FALSE 0
#define READ 1 //Enderecar Escravo para ler
#define WRITE 0 //Enderecar Escravo para escrever


uint32_t amostras_infrared[100]; // Dados do sensor usando LED infravermelho
uint32_t amostras_red[100]; // Dados do sensor usando LED vermelho
int32_t numero_amostras = 100; // Numero total de amostras
int32_t spo2; // Valor de saturacao SpO2
int8_t spo2_valid; // Indicador para saber se o valor de SPO2 e valido
int32_t heart_rate = 87; // Valor da frequencia cardiaca
int8_t heart_rate_valid; // Indicador para saber se a frequencia cardiaca e valida

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Inicializar os LEDs
	configurar_led();

	// Configurar USCI A0 para interface UART
	USCI_A0_config();

	// Configurar USCI B1 para ser mestre do barramento I2C
	USCI_B0_config();

	// Inicializar o LCD
	lcd_inic();
	lcd_comando(0x0C); // Desabilitar o cursor
	lcd_i2c_write(8); // Acender o backlight

	__enable_interrupt();

    // Inicializar enviando mensagem para o usu�rio
    bt_str("Inicializando...\n");
    lcd_cursor(0);
    lcd_escrever_string("Inicializando...");
    LED2_ON; // Inicializa��o OK

    // Testar as interfaces
    LED2_OFF;

    // N�o vamos testar Bluetooth

    // Testar LCD
      if(i2c_test(LCD_ADDRESS, READ) == FALSE || i2c_test(LCD_ADDRESS, WRITE) == FALSE) {
          LED1_ON; // Ligar LED vermelho pois h� um erro (LCD perdeu a conex�o depois da inicializa��o)
          bt_str("LCD n�o conectado. Por favor reveja a liga��o e tente novamente.\n");
          while(1); // Travar execu��o
      }
      bt_str("LCD conectado\n");
      lcd_limpar();
      lcd_cursor(0);
      lcd_escrever_string("LCD conectado");
      // Testar Modulo MAX30102
      if(i2c_test(MAX30102_ADDRESS, READ) == FALSE || i2c_test(MAX30102_ADDRESS, WRITE) == FALSE) {
          LED1_ON;
          bt_str("MAX30102 n�o conectado\n");
          lcd_limpar();
          lcd_cursor(0);
          lcd_escrever_string("MAXIM desligado");
          while(1);
      }

      bt_str("MAX30102 conectado\n");
      lcd_limpar();
      lcd_cursor(0);
      lcd_escrever_string("MAXIM ligado");


    // Iniciar medi��o
      bt_str("Iniciando medi��o\n");
      lcd_limpar();
      lcd_cursor(0);
      lcd_escrever_string("Inicio medicao");



    // Informar a nova m�dia da frequ�ncia card�aca
    bt_str("BMP: ");
    bt_decimal(heart_rate);
    bt_str("  S2\n");
    lcd_limpar();
    lcd_cursor(0);
    lcd_escrever_string("BMP:");
    lcd_cursor(5);
    lcd_decimal(heart_rate);

    lcd_cursor(10);
    lcd_escrever_string("S2");

	return 0;
}

// Delay para evitar rebotes
void delay(int x)
{
    volatile int i;
    for (i = 0; i < x; i++);
}
