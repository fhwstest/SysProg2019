/*
 * DS18S20.h
 *
 * Created: 07.07.2019 14:50:30
 *  Author: Dietrich
 */ 


#ifndef DS18S20_H_
#define DS18S20_H_

//Function Prototypes
void ow_releasebus(void);
int ow_reset(void);

// Bit - Ebene
void ow_write_bit_1(void);
void ow_write_bit_0(void);
unsigned char ow_read_bit(void);

//Byte - Ebene
void ow_write_byte(unsigned char);
unsigned char ow_read_byte(void);

//ROM Commands
void ow_read_rom(void);

//Device Function Command. (Bei n-Slaves mit MATCH ROM Slave auswählen. Brauchen wir bei einem nicht.)
void ow_read_scratchpad(unsigned char*);
void ow_send_measure_cmd(void);

#endif /* DS18S20_H_ */