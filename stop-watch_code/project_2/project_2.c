/*
 * project_2.c
 *
 *  Created on: Sep 17, 2022
 *      Author: Narden Sobhy
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* global variables to count the time */
unsigned char sec1 = 0;
unsigned char sec2 = 0;
unsigned char min1 = 0;
unsigned char min2 = 0;
unsigned char hours1 = 0;
unsigned char hours2 = 0;


/* Interrupt Service Routine for timer1 compare mode */
ISR(TIMER1_COMPA_vect){

	sec1++;

}

/* For System clock=1Mhz and prescale F_CPU/1024.
 * So we just need 1000 counts to get 1s period.
 */
void Timer1_CTC_Init(void){

	TCNT1 = 0;                                   /* Set timer1 initial count to zero */
	OCR1A = 1000;                                /* Set the Compare value to 1000*/
	TIMSK |= (1<<OCIE1A);                        /* Enable Timer1 Compare A Interrupt */
	/* Configure timer control register TCCR1A
		 * 1. Disconnect OC1A and OC1B  COM1A1=0 COM1A0=0 COM1B0=0 COM1B1=0
		 * 2. FOC1A=1 FOC1B=1
		 * 3. CTC Mode WGM10=0 WGM11=0 (Mode Number 4)
	 */
	TCCR1A = (1<<FOC1A)|(1<<FOC1B);
	/* Configure timer control register TCCR1B
		 * 1. CTC Mode WGM12=1 WGM13=0 (Mode Number 4)
		 * 2. Prescaler = F_CPU/1024 CS10=1 CS11=1 CS12=1
	*/
	TCCR1B = (1<<WGM12)|(1<<CS12)|(1<<CS10);

}

ISR(INT0_vect){
	//reset
	TCNT1 = 0;
	sec1 = 0;
    sec2 = 0;
	min1 = 0;
	min2 = 0;
	hours1 = 0;
	hours2 = 0;

}

void INT0_Init(void){

	MCUCR |= (1<<ISC01);                // Trigger INT0 with the falling edge
	PORTD|=(1<<PD2);
	GICR |= (1<<INT0);                 // Enable external interrupt pin INT0

}

ISR(INT1_vect){
	//pause
	//Stop the counter in Timer1
	TCCR1B &=~(1<<CS12)&~(1<<CS10);

}

void INT1_Init(void){

	MCUCR |= (1<<ISC11)|(1<<ISC10);      // Trigger INT1 with the rising edge
	GICR  |= (1<<INT1);                 // Enable external interrupt pin INT1

}

ISR(INT2_vect){
	//resume
	//Start or resume the counter in Timer1
	TCCR1B |=(1<<CS12)|(1<<CS10);

}

void INT2_Init(void){

	MCUCSR &= ~(1<<ISC2);                // Trigger INT2 with the falling edge
	PORTB|=(1<<PB2);
	GICR  |= (1<<INT2);                 // Enable external interrupt pin INT2

}

/*Function that display the time on the 7 segments
 */
void display(){
	PORTA &=0xC0;
	PORTA |=0x01;
	PORTC=(PORTC & 0xF0) | (sec1 & 0x0F);
	_delay_ms(2);
	PORTA &=0xC0;
	PORTA |=(0x02);
	PORTC=(PORTC & 0xF0)| (sec2 & 0x0F);
	_delay_ms(2);
	PORTA &=0xC0;
	PORTA |=0x04;
	PORTC=(PORTC & 0xF0)| (min1 & 0x0F);
	_delay_ms(2);
	PORTA &=0xC0;
	PORTA |=0x08;
	PORTC=(PORTC & 0xF0)| (min2 & 0x0F);
	_delay_ms(2);
	PORTA &=0xC0;
	PORTA |=0x10;
	PORTC=(PORTC & 0xF0)| (hours1 & 0x0F);
	_delay_ms(2);
	PORTA &=0xC0;
	PORTA |=0x20;
	PORTC=(PORTC & 0xF0)| (hours2 & 0x0F);
	_delay_ms(2);
}


int main(void){

	DDRA |= 0x3F; // configure first six pins of PORTA as output pins
	DDRC |= 0x0F; // configure first four pins of PORTC as output pins
	DDRD &= (~(1<<2));   // configure pin 2 in PORTD as input pins
	DDRD &= (~(1<<3));   // configure pin 3 in PORTD as input pins
	DDRB &= (~(1<<2));    // configure pin 2 in PORTB as input pins

	PORTA |=0x3F;       /* Set the first 6 pins in PORTA*/
	PORTC &=0xF0;       /* clear the first 4 pins in PORTC*/

	Timer1_CTC_Init();
	INT0_Init();
	INT1_Init();
	INT2_Init();
	SREG |= (1<<7);/* Enable global interrupts */

	while(1){

		if(sec1<=9){
			display();
		}
		else if((sec1>9)){
			sec1=0;
			sec2++;
			display();
		}
	   if((sec2==6)&&(min1<=9)){
			sec1=0;
			sec2=0;
			min1++;
			display();
		}
		else if((min1>9)&&(min2!=6)){
			sec1=0;
			sec2=0;
			min1=0;
			min2++;
			display();
		}
	   if(min2==6){
			sec1=0;
			sec2=0;
			min1=0;
			min2=0;
			hours1++;
			display();
		}
	 if(hours1==9){
			sec1=0;
			sec2=0;
			min1=0;
			min2=0;
			hours1=0;
			hours2++;
			display();
		}

	}
}
