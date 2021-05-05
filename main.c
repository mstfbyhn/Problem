/*
 * ArduinoMegaProgram.c
 *
 * Created: 27.03.2021 22:17:53
 * Author : Mustafa
 */ 
/*  TIMER0 DC MOTOR PWM
	T�MER2 PWM SERVO MOTOR
	T�MER1 EXTERNAL CLOCK ENCODER
	Servo kontrol edildi�inde dc motorlar�n a��k kalma s�resi encoder say�s� ile kontrol edilebilir.
*/
#define F_CPU 16000000UL

#define ENDSTOPL ((PIND&(1<<7))>>7)		// Pin 0 1 olarak kontrol etmek istersen. Pin 0 ise sonucu 0 verir.1 ise o pinin biti 1 olacak �ekilde �RN:10000000 //
#define ENDSTOPF ((PINB&(1<<0))>>0)		//7.pin i�in. Bu y�zden 7 bit shift yap�yoruz.//
#define ENDSTOPR ((PINB&(1<<1))>>1)		//ENDSTOP P�NLER� 1 OLMA SARTINI KONTROL ED�YOR//
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "HCSR04.h"


void SensorKesmeDeaktif();			// TRIG TET�KLEYEN T�MER1 REG�STERININ KESME MODE AYARLARI DEAKT�F ED�YOR//
void SensorKesmeAktif();			// TRIG TET�KLEYEN T�MER1 REG�STERININ KESME MODE AYARLARI AKT�F ED�L�YOR//
void pin_kesme();					// P�N CHANGE KESMES� AKT�F HALE GET�R�YOR//

ISR (TIMER1_COMPA_vect)
{
	switch(durum)
	{
		case 0:
		PORTD|=(1<<Trig);	//Tetiklendi Hc-sr04//
		OCR1A=trigdelay;	//20 PULSE SONRA TEKRAR KESME G�RECEK. 10 microsaniye=20 pulse (16MHZ , 8 prescale, 2MHZ g�ncel clock ,0.5microsaniye period,2pulse 1 microsaniye)//
		
		durum++;
		break;
		
		case 1:
		PORTD&=(~(1<<Trig));	//Tetik tamamland�. 20 mikrosaniyelik pulse olu�tu.//
		OCR1A=limitsayac;		//Maximum bir de�er belirlendi.Limit mesafe 172cm se�tik.172/sesh�z�(0,0344) =5000microsaniye=10000MHZ PULSE  , Gidi� d�n�� 20000.//
		durum++;
		break;
		
		case 2:
		durum=0;	//�l��m yap�lamad�
		/*TCCR1B|=(1<<WGM12)|(1<<CS11);*/
		break;
		
		case 3:
		mesafe=(sayac*seshizi);	//Mesafe �l��ld�.//
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
		{// echo low kesme oldu, �l��m yap�ld�
			sayac=TCNT1;// saya� TCNT ile e�itlendi
			sayac>>=2;// sesin gidi� ve d�n��� ve 2 darbe 1us hesab� i�in  4'e b�l�yoruz, ge�en us bulundu
			durum++;// zaman kesmesinde durumu de�i�tiriyoruz. durum=3
			OCR1A=olcmedelay;
			/*transmitString_F(PSTR("KESME"));*/
			}
			else
			{
			TCNT1=0;//echo ilk kesme, sesin d�n��� i�in TCNT s�f�rland�
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
	sensor=3;// bos olan gelince 1 olcak.sa� sol sensor yerler�nde duzeltme gerekli.
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

