#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "common.h"

//====== LOCAL VARIABLES ======//

int *notesLocal;
int *noteLengthsLocal;
int sizeOfArrayLocal;
int periodsAmountLocal;
int halfPeriodCounter;
int interruptCounter;
int sample;
int noteIndex;



/*
 * TIMER1 interrupt handler 
 */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
    timerInterruptHandler();
}

/*
 * GPIO even pin interrupt handler 
 */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
    btnInterruptHandler();
}

/*
 * GPIO odd pin interrupt handler 
 */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
    btnInterruptHandler();
}

void btnInterruptHandler()
{
    *GPIO_IFC = *GPIO_IF;	//Clear interrupt
    sleepMode(2);

    int btnState = *GPIO_PC_DIN;	//Read the buttons state
    *GPIO_PA_DOUT = btnState << 8;	//Write buttons state to LEDs


    bool isExplosion = false;	//Is needed for adjusting period duration for the notes
    if (BTN_CHECKER(4, btnState)) {	// SW5
	initializeLocalVariables(pinkPanther, pinkPantherCoefs,
				 sizeof(pinkPanther) / sizeof(int),
				 isExplosion);
    } else if (BTN_CHECKER(5, btnState)) {	// SW6
	initializeLocalVariables(blaster, blasterCoefs,
				 sizeof(blaster) / sizeof(int),
				 isExplosion);
    } else if (BTN_CHECKER(6, btnState)) {	// SW7
	initializeLocalVariables(blasterStrong, blasterStrongCoefs,
				 sizeof(blasterStrong) / sizeof(int),
				 isExplosion);
    } else if (BTN_CHECKER(7, btnState)) {	// SW8
	isExplosion = true;
	initializeLocalVariables(explosion, explosionCoefs,
				 sizeof(explosion) / sizeof(int),
				 isExplosion);
    } else {
	*GPIO_PA_DOUT = 0xff00;	//Turn off the LEDs
	sleepMode(6);
    }
}

void timerInterruptHandler()
{
    *TIMER1_IFC = 1;		//Clear interrupt
    interruptCounter++;

    if (notesLocal[noteIndex] == 0
	|| halfPeriodCounter / 2 >
	periodsAmountLocal * noteLengthsLocal[noteIndex]) {
	if (notesLocal[noteIndex] == 0)
	    delay(noteLengthsLocal[noteIndex] * 100000);
	noteIndex++;		//Switch to the next note
	interruptCounter = 0;
	halfPeriodCounter = 0;
    }

    if (noteIndex >= sizeOfArrayLocal) {	//Done with all notes
	suspendTimer();
	disableDAC();
	*GPIO_PA_DOUT = 0xff00;	//Turn off the LEDs
	sleepMode(6);
	return;
    }

    if (interruptCounter % (SAMPLE_RATE / notesLocal[noteIndex] / 2) == 0) {
	halfPeriodCounter++;
	sample = (sample == AMPLITUDE) ? 0 : AMPLITUDE;	//Generate square waves
	pushToDAC(sample);
    }
}

void initializeLocalVariables(int notes[], int noteLengths[],
			      int sizeOfArray, bool isExplosion)
{
    notesLocal = notes;
    noteLengthsLocal = noteLengths;
    sizeOfArrayLocal = sizeOfArray;
    halfPeriodCounter = 0;
    periodsAmountLocal = isExplosion ? BASE_PERIODS_EXPL : BASE_PERIODS;
    interruptCounter = 0;
    sample = 0;
    noteIndex = 0;

    setupDAC();
    releaseTimer();
}

void delay(int time)
{
    int j = 0;
    while (j < time) {
	j++;
    }
}
