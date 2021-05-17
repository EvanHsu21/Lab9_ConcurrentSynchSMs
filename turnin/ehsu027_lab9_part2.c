/*	Author: Evan Hsu
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 9  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	Video link:
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;



	OCR1A = 125;


	TIMSK1 = 0x02;

	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR (TIMER1_COMPA_vect) {

	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

unsigned char TL[3] = { 0x01, 0x02, 0x04 };
unsigned char BL[2] = { 0x00, 0x08 };
unsigned char threeLEDs = 0x01;
unsigned char blinkingLED = 0x00;
unsigned char i = 0;
unsigned char j = 0;

enum ThreeLED { TL_Start, TL_Next, TL_Wait } TL_state;
void ThreeLEDsSM() {
	switch(TL_state) {
		case TL_Start:
			TL_state = TL_Next;
		break;
		case TL_Next:
			TL_state = TL_Wait;
		break;
		case TL_Wait:
			if (i == 2) {
				i = 0;
			}
			else {
				i++;
			}
			threeLEDs = TL[i];
			TL_state = TL_Next;
		break;
		default:
		break;
	}
}

enum BlinkLED { BL_Start, BL_Next, BL_Wait } BL_State;
void BlinkingLEDSM() {
	switch(BL_State) {
		case BL_Start:
			BL_State = BL_Next;
		break;
		case BL_Next:
			BL_State = BL_Wait;
		break;
		case BL_Wait:
			if (j == 1) {
				j = 0;
			}
			else {
				j++;
			}
			blinkingLED = BL[j];
			BL_State = BL_Next;
		break;
		default:
		break;
	}
}

enum CombineLED { CL_Start, CL_Out } CL_State;
void CombineLEDsSM() {
	switch(CL_State) {
		case CL_Start:
			CL_State = CL_Out;
		break;
		case CL_Out:
			CL_State = CL_Out;
		break;
		default:
		break;
	}
	switch(CL_State) {
		case CL_Start:
		break;
		case CL_Out:
			PORTB = threeLEDs | blinkingLED;
		break;
	}
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	TL_state = TL_Start;
	BL_State = BL_Start;
	CL_State = CL_Start;
	unsigned long BL_elapsedTime = 1000;
	unsigned long TL_elapsedTime = 300;
	TimerSet(100);
	TimerOn();
    /* Insert your solution below */
    while (1) {
	if (TL_elapsedTime >= 300) {
		ThreeLEDsSM();
		TL_elapsedTime = 0;
	}
	if (BL_elapsedTime >= 1000) {
	       	BlinkingLEDSM();
		BL_elapsedTime = 0;
	}
	while (!TimerFlag) {};
	TimerFlag = 0;
	BL_elapsedTime += 100;
	TL_elapsedTime += 100;
	CombineLEDsSM();
    }
    return 1;
}
