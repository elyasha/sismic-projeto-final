/*
 * i2c.h
 *
 *  Created on: Nov 2, 2021
 *      Author: mathe
 */

#ifndef I2C_H_
#define I2C_H_

#define BR100K 11 //(SMCLK) 1.048.576/11 ~= 100kHz
#define BR10K 105 //(SMCLK) 1.048.576/105 ~= 10kHz
#define TRUE 1
#define FALSE 0
#define READ 1 //Enderecar Escravo para ler
#define WRITE 0 //Enderecar Escravo para escrever

void USCI_B0_config(void);
char i2c_test(char adr, char modo);

#endif /* I2C_H_ */
