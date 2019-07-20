/*
 * usart.h
 *
 * Created: 12.04.2019 23:37:51
 *  Author: Dietrich
 */ 


#ifndef USART_H_
#define USART_H_

void USART_Init(unsigned int baud) ;
void USART_Transmit(char* message);
unsigned char USART_Receive( void );

#endif /* USART_H_ */