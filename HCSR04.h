/*
 * HCSR04.h
 *
 * Created: 27.03.2021 22:19:53
 *  Author: Mustafa
 */ 


#ifndef HCSR04_H_
#define HCSR04_H_
#define olcmedelay 10000
#define trigdelay 20
#define limitsayac 20000
#define limitmesafe 172
#define seshizi 0.0344
#define echo 2

volatile static uint16_t sayac=0;
uint16_t mesafe;
int Trig;
int sensor;
int durum;
unsigned char z;
uint8_t SEND;


#endif /* HCSR04_H_ */