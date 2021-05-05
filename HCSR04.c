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
	DDRD&=~(1<<echo);//pd2 giri� yap�ld�

	EICRA|=(1<<ISC00);// kesme akif
	EIMSK|=(1<<INT0);//echo pin de�i�im aktif
	sei();	//t�m kesmeler a��ld�
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
