/*
 * USART.h
 *
 * Created: 29.04.2019 12:17:08
 *  Author: Paul
 */ 


#ifndef USART_H_
#define USART_H_

void USART_Init(unsigned int baud);
unsigned char USART_Receive(void);
void USART_Transmit(unsigned char msg);

#endif /* USART_H_ */