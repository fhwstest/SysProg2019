/*
* DS18S20.c
*
* Created: 07.07.2019 14:47:37
* Author: Dietrich
* Kommunikation mit Temperatursensor DS18S20 über OneWire.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "DS18S20.h"

#define OW_PIN PC0        //OneWire Datenleitung frei zu wählen.
#define OW_INTERFACE DDRC //Helfer um OW_PIN abwechseln als INPUT/OUTPUT zu setzen
#define OW_PORT PORTC     //Helfer um OW_PIN im Outputmode auf HIGH/LOW zu ziehen

//Delayslots ergeben sich aus dem Handbuch. (ein timeslot 60us) (z.B. DELAY_H = RESET (8 * 60us = 480 us))
#define DELAY_A 6
#define DELAY_B 64
#define DELAY_C 60
#define DELAY_D 10
#define DELAY_E 9
#define DELAY_F 55
#define DELAY_H 480
#define DELAY_I 70
#define DELAY_J 410

//OneWire ROM Commands (OneWire spezifisch und für alle OneWire-Devices identisch (Recherche))
#define READ_ROM 0x33        //64Bit Id eines Devices abfragen. (Bei n-Devices->SEARCH_ROM (sonst kommt es zu Kollisionen))
#define SKIP_ROM 0xCC        //Alle Slaves sollen antworten (geht weil wir nur einen haben) -> n-Devices MATCH_ROM

//DS18S20 Devicespezifische Funktionen
#define CONVERT_T  0x44      //Temperatur messen und im Scratchpad speichern
#define READ_SCRATCHPAD 0xBE //Scratchpad (Devicespeicher) auslesen

//*** OneWire RESET Initalisierungssequenz. Slaves angeschlossen und startklar? ****
int ow_reset(void) {
    
    cli();                           //Bitte Unterbrechungsfrei, damit die timeslots stimmen
    
    OW_INTERFACE |= (1 << OW_PIN);   //Output enabled
    OW_PORT      &= ~(1 << OW_PIN);  //Pin on low
    _delay_us(DELAY_H);              //wait > 480 us
    ow_releasebus();
    _delay_us(DELAY_I);              //slave pull the bus low within 60us after
    //it was released by the master and hold it low for at least 60us.
    int presence = !(PINC & (1 << OW_PIN)); //LOW==Slave present-> ! um in TRUE zu drehen
    _delay_us(DELAY_J);
    sei();
    
    return presence;
}

//****************************************************************************
void ow_releasebus()
{
    OW_INTERFACE &= ~(1 << OW_PIN);  //Input enabled
}


// *********** ROM Command READ_ROM ******************************
void ow_read_rom()
{
    if(ow_reset())
    {
        printf("reset succeeded");
        ow_write_byte(READ_ROM);

        //64bit - identifier lesen
        for (int i = 0; i < 8; ++i) {
            unsigned char data = ow_read_byte();
            printf("data: 0x%02X \n", data);
        }
    }    
}

// ****** DS18S20 Function CONVERT_T (MEASURE) ******************
void ow_send_measure_cmd(void)
{
    if (ow_reset())
    {
        ow_write_byte(SKIP_ROM);   //Alles Slaves addressieren (haben ja nur einen)
        ow_write_byte(CONVERT_T);  //Temperaturmessung auslösen
        printf("start measurement\n");
        do
        {
        } while (!(PINC & (1 << OW_PIN)));  //Slave hält OW_PIN auf low bis Messung abgeschlossen ist.

        _delay_ms(750);
    }
}

// ****** DS18S20 Function READ_SCRATCHPAD (read SRAM) ********
void ow_read_scratchpad(unsigned char* scratchpad) {

    if (ow_reset())
    {
        ow_write_byte(SKIP_ROM);        //Alles Slaves addressieren (haben ja nur einen)
        ow_write_byte(READ_SCRATCHPAD);

        //printf("scratchpad:\n");

        //DS18S20 Scatchpad enhält 9 Bytes
        for (int i = 0; i < 9; ++i) {
            unsigned char data = ow_read_byte();
            //printf("scratch data: 0x%02X \n", data);
            scratchpad[i] = data;
        }
    }
}

//************* Byte - Ebene ******************************************************
void ow_write_byte(unsigned char byte) {
    
    for (int i = 0; i < 8; i++)
    {
        int lsb = byte & 0x01;
        //printf("Write bit #%d: %d\n", i, lsb);
        if (lsb == 1) {
            ow_write_bit_1();
            } else {
            ow_write_bit_0();
        }
        byte >>= 1; //nächstes Bit auf LSB shiften
    }
}

unsigned char ow_read_byte(void) {
    
    unsigned char data = 0;
    
    //LSBit first. little endian

    for (int i = 0; i < 8; i++)
    {
        data >>= 1;         // datainhalt um 1 nach rechts shiften und data wieder zuweisen
        unsigned char msb = ow_read_bit();        
        data |= (msb << 7); //neues MSB setzen. Wird in der Schleife data >>= 1. An seine Zielpos. geschoben
    }
    return data;
}


//************* Bit - Ebene (Struktugramme AVR318 Page 10)*****************************
void ow_write_bit_1(void) {
    cli();                         //Disable interrupts
    OW_INTERFACE |= (1 << OW_PIN); //Output enabled
    //OW_PORT |= (1 << OW_PIN);    //Pullup setzen. automatisch durch PullUp Widerstand
    OW_PORT &= ~(1 << OW_PIN);     //Pin on low
    _delay_us(DELAY_A);            //6 us. Range lt. Doku AVR318 Page 2. 1-15us
    ow_releasebus();
    _delay_us(DELAY_B);            //restlicher timeslot 54us (60us - 6us)
    sei();    
}

void ow_write_bit_0(void) {
    cli();                         //Disable interrupts
    OW_INTERFACE |= (1 << OW_PIN); //Output enabled
    //OW_PORT |= (1 << OW_PIN);    //Pullup setzen. automatisch durch PullUp Widerstand
    OW_PORT &= ~(1 << OW_PIN);     //Pin on low
    _delay_us(DELAY_C);            //wait 60us - 120us. hier spitz auf knopf 60us (ein ganzer timeslot)
    ow_releasebus();
    _delay_us(DELAY_D);            //Doku AVR318 Page 9.
    sei();
}

unsigned char ow_read_bit(void) {
    cli();                         //Disable interrupts
    OW_INTERFACE |= (1 << OW_PIN); //Output enabled
    OW_PORT &= ~(1 << OW_PIN);     //Pin on low
    _delay_us(DELAY_A);            //6 us. Range lt. Doku AVR318 Page 2. 1 -15us
    ow_releasebus();
    _delay_us(DELAY_E);
    unsigned char read = (PINC & (1 << OW_PIN)); //0 oder 1 vom Slave.
    _delay_us(DELAY_F);
    sei();
    return read;
}
