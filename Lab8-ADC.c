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

unsigned long MillisecondCounter = 0;



// Interrupt Service Routine for Timer32-1
void Timer32_1_ISR(void)
{
	char BPMBuf[32];
	unsigned int analogIn = 0;
	double BPM = 0.0;
	double Vout = 0.0;	
	double VoutThreshhold = 1000.0; 				//Idk how this will need to be measured
	static BOOLEAN Timer1RunningFlag = FALSE;
	unsigned long MillisecondCounter = 0; 			//not float, need to change?
	
	analogIn = ADC_In();
	
//	//Print decimal
//	uart0_put("\n\rDecimal Value: ");
//	sprintf(temp, "%i", analogIn);
//	uart0_put(temp); 

//	//Print HEX
//	uart0_put("\n\rHex Value: 0x");
//	sprintf(temp, "%X", analogIn);
//	uart0_put(temp);
//	uart0_put("\r\n");	

	
	//Print BPM
	Vout = (2.5 * analogIn) / (pow(2.0, 14.0)-1.0);  //What this do?
	//Vout = analogIn;

	//voltage went past threshhold, indicating a pulse
	if ((Vout>VoutThreshhold) && !timerStart){
		//start timer
		Timer1RunningFlag = true;
		while(Timer1RunningFlag){

			MillisecondCounter++

			//end timer at next pulse
			if((Vout>VoutThreshhold) && timerStart){
			Timer1RunningFlag = false;

			//divide 1min by time measured (make sure units match)
			BPM = 60000.0 / MillisecondCounter ;

			//reset timer
			MillisecondCounter = 0;
			}

		}
	}

	//print BPM to PuTTY
	//tempIn = Vout * 10;		//10mV to V this needed?
	uart0_put("\n\rBPM: ");
	sprintf(BPMBuf, "%f", BPM);
	uart0_put(BPMBuf);
	for (i = 0; i < 32; i++) {
				BPMBuf[i] = NULL;
		}
}



int main(void)
{
	//char temp[64];
	//unsigned int analogIn = 0;
	//initializations
	uart0_init();
	uart0_put("\r\nLab8 ADC demo\r\n");

	Timer32_1_Init(&Timer32_1_ISR, CalcPeriodFromFrequency(2), T32DIV1); // initialize Timer A32-1;

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

