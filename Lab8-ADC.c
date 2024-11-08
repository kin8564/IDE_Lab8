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

double pulses = 0;

// Interrupt Service Routine for Timer32-1
void Timer32_1_ISR(void)
{
	char temp[64];
	unsigned int analogIn = 0;
	double VoutThreshhold = 6600.0; 				//measured lowest height of the lowest frequency input
	
	analogIn = ADC_In();
	
	//Print decimal
	uart0_put("\n\rDecimal Value: ");
	sprintf(temp, "%i", analogIn);
	uart0_put(temp);

	//voltage went past threshhold, indicating a pulse
	if ((analogIn>VoutThreshhold)){
		//pulse has occured
		pulses++;
	}
	
}
// Interrupt Service Routine for Timer32-1
void Timer32_2_ISR(void)
{
	char temp[64];
	double BPM = 0.0;
	
	//Should be even, so combine rising and falling edges
	pulses = pulses / 2;
	
	//TODO
	//divide pulses by time unit
	BPM = pulses / 60000.0 ;
	
	//TODO
	//print BPM to PuTTY
	//tempIn = Vout * 10;		//10mV to V this needed?
	uart0_put("\n\rBPM: ");
	sprintf(temp, "%f", BPM);
	uart0_put(temp);
	
}
int main(void)
{
	//initializations
	uart0_init();
	uart0_put("\r\nLab8 ADC demo\r\n");

	Timer32_1_Init(&Timer32_1_ISR, CalcPeriodFromFrequency(10), T32DIV1); // initialize Timer A32-1;
	Timer32_2_Init(&Timer32_2_ISR, CalcPeriodFromFrequency(2), T32DIV1);		// initialize Timer A32-2

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

