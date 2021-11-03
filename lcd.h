/*
 * lcd.h
 *
 *  Created on: Nov 2, 2021
 *      Author: mathe
 */

#ifndef LCD_H_
#define LCD_H_

/*
 * PCF8574AT -> 0x3F
 * PCF8574A -> 0x27
 */
#define LCD_ADDRESS 0x27

// Bits para controle do LCD
#define BIT_RS BIT0
#define BIT_RW BIT1
#define BIT_E BIT2
#define BIT_BL BIT3


void lcd_i2c_write(char dado);
void lcd_limpar();
void lcd_aux(char dado);
void lcd_inic(void);
void lcd_comando(char x);
void lcd_cursor(char x);
void lcd_escrever_char(char x);
void lcd_escrever_string(char *vt);
void lcd_decimal(int y);

#endif /* LCD_H_ */
