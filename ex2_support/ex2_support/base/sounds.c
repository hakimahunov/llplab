#include "efm32gg.h"
#include "notes.h"
#include "common.h"

void playSound(int *notes, int *noteLengths, int sizeOfArray)
{
	for (int i = 0; i < sizeOfArray; i++) {
		int k = notes[i];
		if (k != 0)
			playSingleNote(notes[i], noteLengths[i]);
		else {
			int j = 0;
			k = noteLengths[i] * 100000;
			while (j < k) {
				j++;
			}
		}
	}

	*DAC0_CH0DATA = 0;
	*DAC0_CH1DATA = 0;
}

void playSingleNote(int frequency, int noteLengthCoef)
{
	int timerCounter = *TIMER1_CNT;	//Keep track of the timer
	int periodCounter = 1;
	int halfPeriodCounter = 0;
	int periodAmount = BASE_PERIODS * noteLengthCoef;
	int samplesInPeriod = 44100 / frequency;
	samplesInPeriod =
	    (samplesInPeriod % 2 !=
	     0) ? samplesInPeriod + 1 : samplesInPeriod;
	int samplesInHalfPeriod = samplesInPeriod / 2;
	int sample = 0;
	while (periodCounter < periodAmount) {
		if (*TIMER1_CNT < timerCounter) {	//If the new timercount is less than the previous, then timer has expired
			halfPeriodCounter++;
			*TIMER1_CMD = 2;	//Stop the timer while performing calculations
			if ((halfPeriodCounter % samplesInHalfPeriod) == 0) {
				sample = (sample == 100) ? 0 : 100;
				if ((halfPeriodCounter %
				     (samplesInPeriod)) == 0)
					periodCounter++;
			}
			pushToDAC(sample);
			*TIMER1_CMD = 1;	//Start the timer before starting the next loop iteration 
		}
		timerCounter = *TIMER1_CNT;	//Continuously read the timer's value
	}
}

//====== SOUNDS ======//

int pinkPanther[] = {
	Gs4, A4, R, B4, C5, R, Gs4,
	A4, R, B4, C5, R, F5, E5,
	R, A4, C5, R, E5, Ds5, R,
	D5, C5, A4, G4, A4, R, A4
};

int pinkPantherCoefs[] = {
	2, 2, 5, 2, 2, 4, 2,
	2, 1, 2, 2, 1, 2, 2,
	1, 2, 2, 1, 2, 4, 3,
	2, 2, 2, 2, 2, 1, 4
};

int blaster[] = {
	G7, G7, A7, G7, G7, A7, R,
	G7, G7, A7, G7, G7, A7, R,
	G7, G7, A7, G7, G7, A7, R
};

int blasterCoefs[] = {[0 ... 20] = 1 };
