/*
 * Usart_main.c
 *
 * Created: 11/10/2018 06:52:31 p.m.
 * Author : Laptop
 */ 
#define F_CPU 12000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"

#define NULL 0
#define  PRESIONAR !(PINC &(1<<PINC5)) == 0

char dato[] = "hola ";
volatile char data[16];
volatile uint8_t valor = 0;
volatile uint8_t flag = 0;

usart_config(){
	UCSR0A |= (0<<U2X0)|(0<<MPCM0); 
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0)|(1<<TXCIE0)|(1<<RXCIE0);//RX & TX ENABLED, TRANSMISION COMPLETE INTERRUPTION ENABLED, RECEPCION COMPLETE INTERRUPT ENABLED
	UCSR0C |= (0<<UMSEL00)|(0<<UMSEL01)|(0<<UPM00)|(0<<UPM01)|(0<<USBS0)|(0<<UCSZ02)|(1<<UCSZ01)|(1<<UCSZ00); //8BITS , 1 STOP BITS, NO PARITY, ASINCRONO
	UBRR0 |= 624;//BAUD RATE = 1200
    
	sei();
}

pin_config(){
	DDRD |= (0<<PORTD0)|(1<<PORTD1);
	DDRC |= (0<<PORTC5);
}

ISR(USART_TX_vect){
}

ISR(USART_RX_vect){
	data[valor] = UDR0;
	valor++;

	flag = 1;
}

char * shift_left(char * arr){
	char aux;
	for(int i = 0; i<valor; i++){
		aux = arr[i+1];
		arr[i] = aux;
	}	
	return arr;
}

int main(void)
{
	pin_config();
	usart_config(); 
	lcd_init(LCD_DISP_ON_CURSOR_BLINK);
	
    while (1) 
    {
		if( PRESIONAR && (UCSR0A &(1<<UDRE0)) ){ // si se presiona el boton (pull down) y UDR0 est� vacio, entonces...
			for(int i = 0; i<16; i++){
				while( !(UCSR0A &(1<<UDRE0)) ){}
				if(dato[i] == NULL) break;
				UDR0 = dato[i]; //enviar por tx 'dato'
			}
			_delay_ms(150);
		}
		else if (flag == 1)
		{
			_delay_ms(150);
			flag = 0;
			lcd_clrscr();
			
			if(data[0] == '#'){
				lcd_gotoxy(0,1);
				lcd_puts(shift_left(data));
			}
			else{
				lcd_puts(data);
			}
			for(int i = 0; i<valor; i++) data[i] = ' ';
			valor = 0;
		}
		
	}
	return 0;
}

