/*
 * Projeto Final
 * Matheus Elyasha Lopes
 * 4 Novembro 2021
 *
 * Uma aplicação embarcada que transforma a LaunchPad num medidor de frequência cardiaca
 * e mostra o resultado dos batimentos cardiacos num display LCD e além disso monitora
 * o nível de batimentos e informa num terminal que a pessoa está num nível acima de um limiar definido
 *
 * I2C: LCD (com módulo i2c)
 * UART: HC-05
 * A0: Pulse sensor
 */
#include <msp430.h>
#include <stdint.h>
#include "gpio.h"
#include "hc05.h"
#include "i2c.h"
#include "lcd.h"
#include "pulsesensor.h"

#define LED1_ON (P1OUT |= BIT0)
#define LED1_OFF (P1OUT &= ~BIT0)
#define LED1_TOGGLE (P1OUT ^= BIT0)

#define LED2_ON (P4OUT |= BIT7)
#define LED2_OFF (P4OUT &= ~BIT7)
#define LED2_TOGGLE (P4OUT ^= BIT7)

#define SMCLK 1048576L // frequencia do SMCLK
#define ACLK 32768     // frequencia do ACLK

#define TRUE 1
#define FALSE 0
#define READ 1  //Enderecar Escravo para ler
#define WRITE 0 //Enderecar Escravo para escrever

volatile uint16_t adc_output, i, pico_base, numero_batimentos, numero_pulsos_amostragrem = 0, adc_vetor[128], heart_rate = 0;

/**
 * main.c
 */
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    // Inicializar os LEDs
    configurar_led();

    // Inicializar os pinos (P6.0)
    configurar_pinos();

    // Configurar USCI A0 para interface UART
    USCI_A0_config();

    // Configurar USCI B1 para ser mestre do barramento I2C
    USCI_B0_config();

    // Inicializar o ADC12 do MSP430 para fazer conversão
    adc_config();

    // Inicializar o timer para uso conjunto ao ADC12
    timer_adc_config();

    // Inicializar o LCD
    lcd_inic();
    lcd_comando(0x0C); // Desabilitar o cursor
    lcd_i2c_write(8);  // Acender o backlight

    __enable_interrupt();

    // Inicializar enviando mensagem para o usuário
    bt_str("Inicializando...\n");
    lcd_cursor(0);
    lcd_escrever_string("Inicializando...");
    LED2_ON; // Inicialização OK
    __delay_cycles(1280000);

    // Testar as interfaces
    LED2_OFF;

    // Não vamos testar Bluetooth pois é a fonte principal de comunicação

    // Testar LCD
    if (i2c_test(LCD_ADDRESS, READ) == FALSE || i2c_test(LCD_ADDRESS, WRITE) == FALSE)
    {
        LED1_ON; // Ligar LED vermelho pois há um erro (LCD perdeu a conexão depois da inicialização)
        bt_str("LCD não conectado. Por favor reveja a ligação e tente novamente.\n");
        while (1)
            ; // Travar execução
    }
    bt_str("LCD conectado\n");
    lcd_limpar();
    lcd_cursor(0);
    lcd_escrever_string("LCD conectado");
    __delay_cycles(1280000);

    // Iniciar medição
    bt_str("Iniciando medicao\n");
    lcd_limpar();
    lcd_cursor(0);
    lcd_escrever_string("Inicio medicao");
    __delay_cycles(1280000);

    // Loop infinito para acompanhamento
    while (TRUE)
    {
        pico_base = 0;

        // Filtrar um pico base do sinal a cada loop

        // Aqui pode ser melhorado esse algoritmo para usar um limiar (por exemplo 2800 ou 3000)
        // ou outra estratégia para calcular os batimentos e a freq. cardíaca
        for (i = 0; i < 128; i++)
        {
            adc_vetor[i] = adc_output;
            if (adc_vetor[i] > pico_base)
                pico_base = adc_vetor[i];
        }

        if ((adc_output > pico_base) && (numero_batimentos < 5))
        {
            numero_batimentos++;
            P1OUT &= ~BIT0;
            P1OUT |= BIT0;
            __delay_cycles(128000);
            P1OUT ^= BIT0;
        }

        // Calcular o BMP novo devido aos cinco batimentos
        if (numero_batimentos >= 5)
        {
            // heart_rate = (500[Hz]*5*60)/numero_pulsos_amostragrem
            heart_rate = 150000 / numero_pulsos_amostragrem;

            // Filtro de frequência cardíaca em software para exibir valores coerentes com a realidade
            if ((heart_rate > 20) && (heart_rate < 180))
            {
                // Imprimir no LCD e no modulo Bluetooth
                // Informar a nova média da frequência cardíaca
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

                if (heart_rate < 50)
                {
                    lcd_cursor(0x40);
                    lcd_escrever_string("! BMP MT baixa !");
                    bt_str("! BMP MT baixa !\n");
                }
                else if (heart_rate >= 50 && heart_rate < 60)
                {
                    lcd_cursor(0x40);
                    lcd_escrever_string("BMP baixa");
                    bt_str("BMP baixa\n");
                }
                else if (heart_rate >= 60 && heart_rate < 80)
                {
                    lcd_cursor(0x40);
                    lcd_escrever_string("BMP normal");
                    bt_str("BMP normal\n");
                }
                else if (heart_rate >= 80 && heart_rate < 120)
                {
                    lcd_cursor(0x40);
                    lcd_escrever_string("BMP alta");
                    bt_str("BMP alta\n");
                }
                else
                {
                    lcd_cursor(0x40);
                    lcd_escrever_string("! BMP MT alta !");
                    bt_str("! BMP MT alta !\n");
                }
            }

            LED2_OFF;
            LED2_ON;
            __delay_cycles(128000);
            LED2_TOGGLE;

            // Resetar os contadores para recalcular a frequencia cardiaca
            numero_batimentos = 0;
            numero_pulsos_amostragrem = 0;
        }
    }

}

// Delay para evitar rebotes
void delay(int x)
{
    volatile int i;
    for (i = 0; i < x; i++)
        ;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void ta0_isr()
{
    ADC12CTL0 |= ADC12ENC; // Habilita o conversor
    ADC12CTL0 &= ~ADC12SC; // Gera uma flanco de subida em SC
    ADC12CTL0 |= ADC12SC;
    numero_pulsos_amostragrem++;
}

#pragma vector = ADC12_VECTOR
__interrupt void adc12_isr()
{
    ADC12IFG = 0;
    adc_output = ADC12MEM0;
}
