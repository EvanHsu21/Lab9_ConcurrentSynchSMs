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

unsigned char SpeakerOnB = 0;
unsigned long period = 0;

enum Speaker { S_Start, S_Off, S_On, S_Wait } S_State;
void SpeakerSM() {
	switch(S_State) {
		case S_Start:
			S_State = S_Wait;
		break;
		case S_Wait:
			if((PINA & 0x04) == 0x00) {
                                S_State = S_On;
                        }
		case S_Off:
			if((PINA & 0x04) == 0x00) {
				S_State = S_On;
			}
			else if ((PINA & 0x04) == 0x04) {
				S_State = S_Wait;
			}
		break;
		case S_On:
                       	if ((PINA & 0x04) == 0x00) {
				S_State = S_Off;
			}
			else if ((PINA & 0x04) == 0x04) {
				S_State = S_Wait;
			}
		break;
		default:
		break;
	}
	switch(S_State) {
		case S_Start:
		break;
		case S_Wait:
		case S_Off:
			SpeakerOnB = 0x00;
		break;
		case S_On:
			SpeakerOnB = 0x10;
		break;
		default:
		break;
	}
}

enum ChangeFreq { CF_Start, CF_Wait, CF_Up, CF_Down, CF_WaitUp, CF_WaitDown } CF_State;
void ChangeFreqSM() {
	switch(CF_State) {
		case CF_Start:
			CF_State = CF_Wait;
		break;
		case CF_Wait:
			if ((PINA & 0x03) == 0x01) {
				if (period < 10) {
					period++;
				}
				CF_State = CF_Down;
			}
			else if ((PINA & 0x03) == 0x02) {
				if (period > 2) {
					period--;
				}
				CF_State = CF_Up;
			}
			else if (((PINA & 0x03) == 0x03) || (PINA & 0x03) == 0x00) {
				CF_State = CF_Wait;
			}
		break;
		case CF_Up:
			CF_State = CF_WaitUp;
		break;
		case CF_WaitUp:
			if ((PINA & 0x03) == 0x02) {
				CF_State = CF_WaitUp;
			}
			else {
				CF_State = CF_Wait;
			}
		break;
		case CF_Down:
			CF_State = CF_WaitDown;
		break;
		case CF_WaitDown:
			if ((PINA & 0x03) == 0x01) {
				CF_State = CF_WaitDown;
			}
			else {
				CF_State = CF_Wait;
			}
		break;
		default:
			CF_State = CF_Wait;
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
			PORTB = threeLEDs | (blinkingLED | SpeakerOnB);
		break;
	}
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TL_state = TL_Start;
	BL_State = BL_Start;
	CL_State = CL_Start;
	S_State = S_Start;
	CF_State = CF_Start;
	unsigned long BL_elapsedTime = 1000;
	unsigned long TL_elapsedTime = 300;
	unsigned long S_elapsedTime = 2;
	TimerSet(1);
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
	if (S_elapsedTime >= period) {
		SpeakerSM();
		S_elapsedTime = 0;
	}
	while (!TimerFlag) {};
	TimerFlag = 0;
	BL_elapsedTime += 1;
	TL_elapsedTime += 1;
	S_elapsedTime += 1;
	ChangeFreqSM();
	CombineLEDsSM();
    }
    return 1;
}
