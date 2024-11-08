/*
* Rochester Institute of Technology
* Department of Computer Engineering
* CMPE 460  Interfacing Digital Electronics
* LJBeato
* 1/14/2021
*
* Filename: main_timer_template.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "msp.h"
#include "uart.h"
#include "leds.h"
#include "switches.h"
#include "Timer32.h"
#include "CortexM.h"
#include "Common.h"
#include "ADC14.h"
// The sprintf function seemed to cause a hange in the interrupt service routine.
// I think if we increase the HEAP size, it will work
// change to Heap_Size       EQU     0x00000200 in startup_msp432p401r_uvision.s


BOOLEAN Timer1RunningFlag = FALSE;
BOOLEAN Timer2RunningFlag = FALSE;

int pulses = 0;
int BPM = 0;
BOOLEAN high = FALSE;
double VoutThreshhold = 6600.0; 				//measured lowest height of the lowest frequency input
int beats0 = 0;
int beats1 = 0;
int beats2 = 0;
int beats3 = 0;
BOOLEAN firstBPM = TRUE;

// Interrupt Service Routine for Timer32-1
void Timer32_1_ISR(void)
{
	char temp[64];
	unsigned int analogIn = 0;
	
	analogIn = ADC_In();

	//voltage went past threshhold, indicating a pulse
	if ((analogIn>VoutThreshhold) && high == FALSE){
		//pulse is occuring
		high = TRUE;
	}
	if ((analogIn<VoutThreshhold) && high == TRUE){
		//pulse has ended
		pulses++;
		high = FALSE;
	}
	
//	uart0_put("\n\r     Pulses: ");
//	sprintf(temp, "%i", pulses);
//	uart0_put(temp);
//	uart0_put("     ");
//	sprintf(temp, "%i", analogIn);
//	uart0_put(temp);
}
// Interrupt Service Routine for Timer32-1
void Timer32_2_ISR(void)
{
	char temp[64];
	//divide pulses by time unit
	//BPM = (BPM + (pulses * 60.0 / 5.0) ;
	//BPM = (pulses * 12.0);
	
	if (firstBPM) {
		beats0 = pulses * 12.0;
		beats1 = pulses * 12.0;
//		beats2 = pulses * 24.0;
//		beats3 = pulses * 24.0;
		firstBPM = FALSE;
	}
	
	BPM = (beats0 + beats1 + (pulses * 12.0)) / 3.0;	
//	beats3 = beats2;
//	beats2 = beats1;
	beats1 = beats0;
	beats0 = BPM;
		
	//print BPM to PuTTY
	uart0_put("\n\rBPM: ");
	sprintf(temp, "%i", BPM);
	uart0_put(temp);
	
	pulses = 0;
	
}
int main(void)
{
	//initializations
	uart0_init();
	uart0_put("\r\nLab8 ADC demo\r\n");

	Timer32_1_Init(&Timer32_1_ISR, CalcPeriodFromFrequency(10), T32DIV1); // initialize Timer A32-1;
	Timer32_2_Init(&Timer32_2_ISR, CalcPeriodFromFrequency(0.2), T32DIV1);		// initialize Timer A32-2

	LED1_Init();
	LED2_Init();
	Switch2_Init();
	ADC0_InitSWTriggerCh6();
	EnableInterrupts();
	while(1)
	{
   WaitForInterrupt();	
  }
}

