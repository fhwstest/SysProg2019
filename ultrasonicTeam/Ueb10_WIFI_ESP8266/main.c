/*
* Main.c
*
* Created: 01.07.2019 11:52:23
* Author : Torsten Dietrich
* DS18S20 Temperatur über ESP8266 Wifi-Modul bereitstellen
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "hterm_short.h"
#include "DS18S20.h"

//Function Prototypes
void prg_init(void);
void switch_led_on(uint16_t);
void switch_led_off(uint16_t);
void send_at_command(char*);
int  recv_at_command(char*, uint8_t);
void send_to_client(int, const char*);
void to_infinity_and_beyond(void);
void reset_receive_buffer(void);
unsigned int get_connection_id(void);

//Keine Connection ID, keiner will etwas gesendet haben
#define INVALIDCONNECTIONID 999

//Temp-Buffer to get connectionIDs.
#define RECVBUFFERSIZE  500
static char recv_buffer[RECVBUFFERSIZE] = ""; //A static global variable or a function is "seen" only in the file it's declared in
static unsigned int  recv_cursor = 0;

//Website-Frame: Is filled with the current temperature before sending. (Über sprintf in extra Puffer -> somit geht hier const)
static const char website[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\n\n\n<html><meta charset=\'utf-8\'/><head><h1>Current Temperature: %d,%d</h1></head></html>\r\n";

//At the same time, we only support one connection
static int connection_id  = INVALIDCONNECTIONID;

//******************************************************************************************************************
int main(void)
{
    sei(); //enable global interrupts
    
    //DEBUG-LEDs
    DDRB = 0xFF;         //LED Leiste...
    PORTB = 0xFF;        //... ausschalten
    
    switch_led_on(PB0);
    
    prg_init();

    //start program loop
    to_infinity_and_beyond();

    switch_led_on(PB5);
}

//****************** USART und ESP8266 initalisieren *********************************
void prg_init(void)
{
    switch_led_on(PB1);
    
    stdout = &mystdout; //setup stdio stream
    usart_init();     
        
    //ESP8266 Modul Zeit zum Starten geben
    _delay_ms(1500);
    
    switch_led_on(PB2);

    //Modus eigens WLAN (1=an bestehendes anmelden, 2=eigenes, 3=Kombi)
    send_at_command("CWMODE=2");

    //Software reset. (stefanfrings: nötig nach CWMODE Einstellung)
    send_at_command("RST");

    //Reset brauch noch ein wenig
    _delay_ms(500);

    //0 = Single Connection...1 = Multiple Connection (up to 4/5..je nach Firmware)
    send_at_command("CIPMUX=1");

    //Create TCP Server (reachable via PORT 80)...Cipserver=<mode>,<port>...mode=0 -> delete...mode=1 -> create
    send_at_command("CIPSERVER=1,80");
        
    //IP Adresse abfragen (jetzt kommt im HTERM die IP vom ESP8266-Modul an. Diese im Browser eingeben. )
    send_at_command("CIFSR");
    
}
    
//******************************** Progammschleife ***************************************************************
void to_infinity_and_beyond(void)
{

    int toggle = 1;	//LED Blinker
    char buffer[400];
    unsigned char scratchpad[9]; //Daten vom DS18S20 Temperatursensor
    
    switch_led_on(PB4);

    //Ab jetzt erst gilts.
    reset_receive_buffer();
    
    //Hier bleiben wir bis zum bitteren Ende
    while (true)
    {
        
        //Endlosschleife ein bisschen verzögern (Schonzeit)
        _delay_ms(100);
        
        //LED7 blinken lassen, damit wir wissen wo wir sind
        toggle ? switch_led_on(PB7) : switch_led_off(PB7);
        toggle = !toggle;
        
        //Wartet ein Client auf die WebSite?
        if (!(connection_id == INVALIDCONNECTIONID))
        {
            
            //bitte ohne Unterbrechung (Anders ist der Komplettversand von AT commands nicht garantiert.)
            cli();
            
            switch_led_on(PB6);

            //Temperaturmessung auslösen (DS18S20)
            ow_send_measure_cmd();
            
            //DS18S20 Scratchpad (Speicher) auslesen
            ow_read_scratchpad(scratchpad);

            //scratchpad[1] == 0 (+ Temp., 0xFF - Temp), scratchpad[0] enthält die Temperatur geht bis -55 - + 125
            //aktuelle Temperatur in den Buffer schreiben
            snprintf ( buffer, 400, website, scratchpad[0]>>1, (scratchpad[0]&0x01 ? 5 : 0));
            
            //Über ESP8266 an Client senden.
            send_to_client(connection_id, buffer);
            
            //Sendedauer abwarten (aus HTERM Erfahrung)
            _delay_ms(200);

            //Für nächsten freigeben
            connection_id  = INVALIDCONNECTIONID;

            sei();

        }
        else
        {
            switch_led_off(PB6);
        }
    }
}

//************************ Fürs Debuggen **********************************************************
void switch_led_on(uint16_t ledpos)
{
    PORTB &= ~(1<<ledpos);  //LED anschalten
}

void switch_led_off(uint16_t ledpos)
{
    PORTB |= (1<<ledpos);  //LED ausschalten
}


//**************************** send command to ESP8266 *******************************************
void send_at_command(char* command) {
    
    printf("AT+%s\r\n",command);

    //ESP8266 braucht ein bisschen Zeit (ansonsten quitiert er den nächsten mit "busy...")
    _delay_ms(100);

}

//**************************** send string to wifi client (ESP8266) ******************************
void send_to_client(int connId, const char* content)
{
    
    uint16_t bytes2send = 0;
    char buffer[400];
    
    //Anzahl zu sendender Bytes berechnen. (ESP8266 erwartet vor jeder Übertragung die Anzahl zu sendender Bytes.) strlen sucht auto nach \0
    bytes2send = strlen(content);

    //AT Commando basteln mit dem wir Ziel-connectionid und Anzahl zu sendender Bytes dem ESP8266 mitteilen...
    snprintf ( buffer, 400, "CIPSEND=%u,%u", connId, bytes2send);
    
    //..und abschicken
    send_at_command (buffer);
    
    //send content (eigentliche Daten senden.)
    printf(content);

    _delay_ms(500);

    //Verbindungsanfrage ist nun bearbeitet.
    snprintf ( buffer, 400, "CIPCLOSE=%u", connId);

    send_at_command(buffer);
    
}

//*************************** receiver **********************************************************
ISR(USART_RXC_vect) {
    
    switch_led_on(PB3);
    unsigned char c = UDR; //Daten aus Datenregister holen
    
    //Kein Platz mehr im Buffer?
    if (!(recv_cursor < RECVBUFFERSIZE))
    {
        //Overflow (sollte nicht oder selten kommen, da wir nach dem Erkennen eines Zeilenendes immer den Buffer leeren)
        reset_receive_buffer();
    }
    
    //Zeichen aufnehmen
    recv_buffer[recv_cursor] = c;
    recv_cursor++;

    // Nach jedem Endezeichen nach einer Connection-Id suchen
    if (c == '\n')
    {

        //Nur setzen, wenn nicht bereits ein Client wartet.(Im wahren Leben müssten wir hier n-Connections bedienen.)        
        if (connection_id == INVALIDCONNECTIONID)
        {
            //Zeile enthält eine connection id?
            connection_id  = get_connection_id();
        }

        //den durchsuchten Eingangspuffer verwerfen.
        reset_receive_buffer();
    }        
}

//**************************************************************************************************************
unsigned int get_connection_id() {
    
    char* ipdpos = strstr(recv_buffer, "+IPD,");
    
    //Keyword der ConnectionId nicht enthalten?
    if (!(ipdpos))
    {
        return INVALIDCONNECTIONID; 
    }
    
    //"+IPD,3": ipdpos zeigt aufs "+" -> +5, um den Zeiger auf die ConnectioId "3" zu verschieben
    return  (*(ipdpos + 5)) - '0';
}


//******************** Reset receive buffer ***************************************************
void reset_receive_buffer() {
    
    recv_cursor = 0;

    for (int i = 0; i < RECVBUFFERSIZE; i++ )
    {
        recv_buffer[i] = '\0';
    }
}







