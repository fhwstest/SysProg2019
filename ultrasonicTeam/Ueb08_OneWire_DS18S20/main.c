/*
* Main.c
*
* Created: 18.05.2019 11:11:36
* Author : Dietrich
* Temperatur (Temperatursensor DS18S20) �ber USART anzeigen.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "hterm_short.h"  //for use of printf(), alternatices may be used
#include "DS18S20.h"

int main(void)
{
    auto presence;				//Meldung an Master das Slave ready...Master zieht Bus Leitung auf Low..l�sst wieder los ..geht auto auf high...Slave zieht wieder auf Low
    unsigned char scratchpad[9]; //�bernahme-Puffer vom DS18S20 Scratchpad

    //USART Settings einstellen
    stdout = &mystdout; //setup stdio stream
    usart_init();
    printf("usart started\n");
    
    //unique id vom Sensor (8bit Family - 48Bit ID - 8bit CRC)
    ow_read_rom();
    
    while (1)
    {
        //Temperaturmessung ausl�sen (DS18S20)
        ow_send_measure_cmd();
        
        //DS18S20 Scratchpad (Speicher) auslesen
        ow_read_scratchpad(scratchpad);
        
        //scratchpad[1] == 0 (+ Temp., 0xFF - Temp), scratchpad[0] enth�lt die Temperatur geht bis -55 - + 125
        printf("Raumtemperatur %d,%d\n\r", scratchpad[0]>>1, scratchpad[0]&0x01 ? 5 : 0);

        _delay_ms(1000);
    }

}

