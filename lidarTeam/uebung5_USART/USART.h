/*
 * USART.h
 *
 * Created: 15.07.2019 12:35:47
 *  Author: PC
 */ 

#ifndef USART_H_
#define USART_H_

void USART_Init(unsigned int baud);
unsigned char USART_Receive(void);
void USART_Transmit(unsigned char msg);
void USART_TransmitStr(const char* string);

#endif /* USART_H_ */