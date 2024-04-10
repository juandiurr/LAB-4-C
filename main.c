/*
 * PRUEBAS DE C.c
 *
 * Created: 7/04/2024 14:58:43
 * Author : diego
 */ 
#define F_CPU 2000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

const uint8_t mylist[] ={0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x0, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E};
uint8_t hola = 0;
uint8_t poten00 = 0;
uint8_t poten01 = 0;
uint8_t cont = 0;
void setup(void);
void display1(void);
void display2(void);
void leds(void);
void initadc(void);
void postlab(void);


int main(void)
{
	setup();
	initadc();
    while (1) 
    {
		leds();
		display1();
		display2();
		postlab();
    }
}
void postlab(void){
	if (cont < ADCH)
	{
		PORTC |= (1<<PORTC3);
	}
	else
	{
		PORTC &= ~(1<<PORTC3);
	}
}
void display1(void){
	hola = 0;
	do
	{
		//HABILITAR CONVERSION
		ADCSRA |= (1<<ADSC);
		PORTC |= (1<<PORTC1);
		PORTC &= ~(1<<PORTC2);
		PORTC &= ~(1<<PORTC0);
		PORTD = mylist[poten00];;
	}while(hola == 0);
}
void display2(void){
	hola = 0;
	do
	{
		//HABILITAR CONVERSION
		ADCSRA |= (1<<ADSC);
		PORTC |= (1<<PORTC2);
		PORTC &= ~(1<<PORTC1);
		PORTC &= ~(1<<PORTC0);
		PORTD = mylist[poten01];
	}while (hola == 0);
}
void leds(void){
	hola = 0;
	do
	{
		PORTC |= (1<<PORTC0);
		PORTC &= ~(1<<PORTC1);
		PORTC &= ~(1<<PORTC2);
		PORTD = cont;
	}while (hola == 0);
}
void setup(void){
	cli();
	UCSR0B = 0;
	DDRD = 0xFF;
	PORTD = 0; //ESTABLECE TODO EL PUERTO D COMO SALIDA Y LO PONE EN 0
	
	DDRB = 0b11111100;
	PORTB = 0b00000011;
	/*DDRB &= ~(1<<DDB0); //ESTABLECE PB0 COMO ENTRADA (PushBottom1)
	PORTB |= (1<<PORTB0); //HABILITA EL PULL-UP
	DDRB &= ~(1<<DDB1); //ESTABLECE PB1 COMO ENTRADA (PushBottom2)
	PORTB |= (1<<PORTB1); //HABILITA EL PULL-UP
	DDRB |= 0b11111100; // Setear demás pines como salida*/
	
	DDRC |= (1<<PORTC0);
	PORTC |= (1<<PORTC0); //PONE EN 1 PC0
	DDRC |= (1<<PORTC1);
	PORTC &= ~(1<<PORTC1); //PONE EN 0 PC1
	DDRC |= (1<<PORTC2);
	PORTC &= ~(1<<PORTC2); //PONE EN 0 PC2
	DDRC |= (1<<PORTC3);
	PORTC &= ~(1<<PORTC3); //PONE EN 0 PC2
	
	TCCR0B |= (1<<CS02) | (1<<CS00); //prescaler 1024
	TCNT0 = 158; //5ms
	TIMSK0 |= (1<<TOIE0); //habilitar int timer0
	
	PCICR |= (1<<PCIE0); //habilitar interrupcion en el pcint0:7
	PCMSK0 |= (1<<PCINT0) | (1<<PCINT1); //HABILITAR INTERRUPCIONES EN LOS BOTONES
	
	sei();
}

void initadc(void){
	ADMUX = 0;
	//referencia AVCC = 5v
	ADMUX |= (1<<REFS0);
	//JUSTIFICACION A LA IZQUIERDA
	ADMUX |= (1<<ADLAR);
	//ADC5
	ADMUX |= (1<<MUX2) | (1<<MUX0);
	//HABILITAR INTERRUPCION DEL ADC
	ADCSRA |= (1<<ADIE);
	//PRESCALER de 32
	ADCSRA |= (1<<ADPS0) | (1<<ADPS2);
	//HABILITAR ADC
	ADCSRA |= (1<<ADEN);
}

ISR(TIMER0_OVF_vect){
	TCNT0 = 158; //5ms
	TIFR0 |= (1<<TOV0); //apagar bandera
	hola = 1;
}

ISR(ADC_vect){
	poten00 = ADCH; 
	poten01 = ADCH;
	poten01 = (poten01>>4); //corre los ultimos 4 bits a la derecha
	poten00 &= ~((1<<4) | (1<<5) | (1<<6) | (1<<7));
	ADCSRA |= (1<<ADIF); //apagar bandera
}
ISR(PCINT0_vect){
	if (PINB == 0b01)
	{
		cont++;
	}
	if (PINB == 0b10)
	{
		cont--;
	}
	if (cont < 0)
	{
		cont = 255;
	}
	if (cont > 255)
	{
		cont = 0;
	}
}