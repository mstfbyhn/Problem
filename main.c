/*
 * ArduinoMegaProgram.c
 *
 * Created: 27.03.2021 22:17:53
 * Author : Mustafa
 */ 
/*  TIMER0 DC MOTOR PWM
	TÝMER2 PWM SERVO MOTOR
	TÝMER1 EXTERNAL CLOCK ENCODER
	Servo kontrol edildiðinde dc motorlarýn açýk kalma süresi encoder sayýsý ile kontrol edilebilir.
*/
#define F_CPU 16000000UL

#define ENDSTOPL ((PIND&(1<<7))>>7)		// Pin 0 1 olarak kontrol etmek istersen. Pin 0 ise sonucu 0 verir.1 ise o pinin biti 1 olacak þekilde ÖRN:10000000 //
#define ENDSTOPF ((PINB&(1<<0))>>0)		//7.pin için. Bu yüzden 7 bit shift yapýyoruz.//
#define ENDSTOPR ((PINB&(1<<1))>>1)		//ENDSTOP PÝNLERÝ 1 OLMA SARTINI KONTROL EDÝYOR//
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "HCSR04.h"


void SensorKesmeDeaktif();			// TRIG TETÝKLEYEN TÝMER1 REGÝSTERININ KESME MODE AYARLARI DEAKTÝF EDÝYOR//
void SensorKesmeAktif();			// TRIG TETÝKLEYEN TÝMER1 REGÝSTERININ KESME MODE AYARLARI AKTÝF EDÝLÝYOR//
void pin_kesme();					// PÝN CHANGE KESMESÝ AKTÝF HALE GETÝRÝYOR//

ISR (TIMER1_COMPA_vect)
{
	switch(durum)
	{
		case 0:
		PORTD|=(1<<Trig);	//Tetiklendi Hc-sr04//
		OCR1A=trigdelay;	//20 PULSE SONRA TEKRAR KESME GÝRECEK. 10 microsaniye=20 pulse (16MHZ , 8 prescale, 2MHZ güncel clock ,0.5microsaniye period,2pulse 1 microsaniye)//
		
		durum++;
		break;
		
		case 1:
		PORTD&=(~(1<<Trig));	//Tetik tamamlandý. 20 mikrosaniyelik pulse oluþtu.//
		OCR1A=limitsayac;		//Maximum bir deðer belirlendi.Limit mesafe 172cm seçtik.172/seshýzý(0,0344) =5000microsaniye=10000MHZ PULSE  , Gidiþ dönüþ 20000.//
		durum++;
		break;
		
		case 2:
		durum=0;	//Ölçüm yapýlamadý
		/*TCCR1B|=(1<<WGM12)|(1<<CS11);*/
		break;
		
		case 3:
		mesafe=(sayac*seshizi);	//Mesafe ölçüldü.//
		durum=4;
		
		
		TCCR1B=0X00;	//Kesme iptal edildi.
		break;
		
		default:
		durum=0;
		break;
		
	}
}
ISR (INT0_vect){
	if (durum==2){
		if (!(PIND&(1<<echo)))
		{// echo low kesme oldu, ölçüm yapýldý
			sayac=TCNT1;// sayaç TCNT ile eþitlendi
			sayac>>=2;// sesin gidiþ ve dönüþü ve 2 darbe 1us hesabý için  4'e bölüyoruz, geçen us bulundu
			durum++;// zaman kesmesinde durumu deðiþtiriyoruz. durum=3
			OCR1A=olcmedelay;
			/*transmitString_F(PSTR("KESME"));*/
			}
			else
			{
			TCNT1=0;//echo ilk kesme, sesin dönüþü için TCNT sýfýrlandý
		}
	}
}

int main(void)
{
	int sayim2=0;
	int sayim3=0;
    /* Replace with your application code */
	Trig=4;
	durum=0;
	sensor=3;// bos olan gelince 1 olcak.sað sol sensor yerlerýnde duzeltme gerekli.
	DDRB&=~((1<<0)|(1<<1));
	DDRD&=(~(1<<7));
	DDRD|=(1<<4)|(1<<5)|(1<<6);
	PORTD&=~((1<<4)|(1<<5)|(1<<6));
	pin_kesme();
	uart0_init();
	SensorKesmeAktif();
    while (1) 
    {
		if (((ENDSTOPL)&(ENDSTOPF)&(ENDSTOPR)))
		{
			
		
		if (durum==4)
		{
			durum=0;
		
		
		switch(sensor)
		{
			case 1:
			
			if (mesafe<0x0A)
			{
				SEND=0x01;
 				transmitByte(SEND);
				/*transmitByte(mesafe);	*/
			}
			else if ((0x10<mesafe)&(mesafe<0x1A))
			{
				SEND=0x02;
				transmitByte(SEND);
				/*transmitByte(mesafe);*/
			}
			
// 			SEND=0x00;
//  			transmitByte(SEND);
// 			transmitByte(mesafe);
			
			/*transmitByte(mesafe);*/
			sensor=3;
			Trig=4;
 			TCCR1B|=(1<<WGM12)|(1<<CS11);  //Kesme aktif.//
			
			break;
			
			case 2:
			
			if (mesafe<0x0A)
			{
								
				SEND=0x03;
 				transmitByte(SEND);
				/*transmitByte(mesafe);*/
				
				
				
			}
			
			sensor=1;
			Trig=6;
			TCCR1B|=(1<<WGM12)|(1<<CS11);
			/*UCSR0A|=(1<<TXC0);*/
			break;
			
			case 3:

			if (mesafe<0x0A)
			{
			
				SEND=0x04;
				transmitByte(SEND);
				/*transmitByte(mesafe);*/
			}
			
			
			/*transmitByte(mesafe);*/
			
 			sensor=2;//
 			Trig=5;// 
			TCCR1B|=(1<<WGM12)|(1<<CS11);
			/*UCSR0A|=(1<<TXC0);*/
			break;
		
		}
		}
		}
		
		if ((!ENDSTOPL))
		{
			SensorKesmeDeaktif();
			SEND=0x99;
			transmitByte(SEND);
			SensorKesmeAktif();
// 			SEND=5;
// 			transmitByte(SEND);
		}
		if ((!ENDSTOPF))
		{
			SensorKesmeDeaktif();
			SEND=0x88;
			transmitByte(SEND);
			SensorKesmeAktif();

// 			SEND=6;
// 			transmitByte(SEND);
		}
		if((!ENDSTOPR))
		{
			SensorKesmeDeaktif();
			SEND=0x77;
			transmitByte(SEND);
			SensorKesmeAktif();
			//
// 			SEND=7;
// 			transmitByte(SEND);
		}
    
}
}

