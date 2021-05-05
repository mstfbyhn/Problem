/*
 * HCSR04.c
 *
 * Created: 27.03.2021 22:19:35
 *  Author: Mustafa
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "HCSR04.h"



void pin_kesme(){
	DDRD&=~(1<<echo);//pd2 giriþ yapýldý

	EICRA|=(1<<ISC00);// kesme akif
	EIMSK|=(1<<INT0);//echo pin deðiþim aktif
	sei();	//tüm kesmeler açýldý
}
void SensorKesmeAktif()
{
	TCCR1B|=(1<<WGM12)|(1<<CS11);
	TIMSK1|=(1<<OCIE1A);
	OCR1A=olcmedelay;
	sei();
}

void SensorKesmeDeaktif()
{
	TCCR1B=0X00;
	TIMSK1=0X00;
	
}
