/*
 * hc05.h
 *
 *  Created on: Oct 31, 2021
 *      Author: mathe
 */

#ifndef HC05_H_
#define HC05_H_

void USCI_A0_config(void);
char bt_ler(void);
void bt_str(char *vet);
void bt_char(char c);
void bt_decimal(int valor);

#endif /* HC05_H_ */
