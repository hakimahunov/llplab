#include <stdbool.h>

#include "efm32gg.h"
#include "notes.h"
#include "common.h"

void playSound(int *notes, int *noteLengths, int sizeOfArray,
	       bool isExplosion)
{
    for (int i = 0; i < sizeOfArray; i++) {	//Play each note from the sound
	int k = notes[i];
	if (k != 0)
	    playSingleNote(notes[i], noteLengths[i], isExplosion);
	else {			//If frequency of the note is zero (R), then play nothing, just delay
	    int j = 0;
	    k = noteLengths[i] * 100000;
	    while (j < k) {
		j++;
	    }
	}
    }

    *DAC0_CH0DATA = 0;		//Clear channels at the end of the sound
    *DAC0_CH1DATA = 0;
}

void playSingleNote(int frequency, int noteLengthCoef, bool isExplosion)
{
    int timerCounter = *TIMER1_CNT;	//Keep track of the timer
    int periodCounter = 1;
    int halfPeriodCounter = 0;
    //Adjust period amount for explosion sound
    int periodAmount =
	(isExplosion ? BASE_PERIODS_EXPL * noteLengthCoef : BASE_PERIODS *
	 noteLengthCoef);
    int samplesInPeriod = SAMPLE_RATE / frequency;
    //Make number of samples in period even. Is needed to exact define number of samples if a half period
    samplesInPeriod =
	(samplesInPeriod % 2 != 0) ? samplesInPeriod + 1 : samplesInPeriod;
    int samplesInHalfPeriod = samplesInPeriod / 2;
    int sample = 0;
    while (periodCounter < periodAmount) {
	if (*TIMER1_CNT < timerCounter) {	//If the new timercount is less than the previous, then timer has expired
	    halfPeriodCounter++;
	    *TIMER1_CMD = 2;	//Stop the timer while performing calculations
	    if ((halfPeriodCounter % samplesInHalfPeriod) == 0) {	//Generate square waves
		sample = (sample == AMPLITUDE) ? 0 : AMPLITUDE;
		if ((halfPeriodCounter % (samplesInPeriod)) == 0)
		    periodCounter++;
	    }
	    pushToDAC(sample);
	    *TIMER1_CMD = 1;	//Start the timer before starting the next loop iteration 
	}
	timerCounter = *TIMER1_CNT;	//Continuously read the timer's value
    }
}

//====== SOUNDS ======//

int pinkPanther[] = {		//Sequence of notes
    Gs4, A4, R, B4, C5, R, Gs4,
    A4, R, B4, C5, R, F5, E5,
    R, A4, C5, R, E5, Ds5, R,
    D5, C5, A4, G4, A4, R, A4
};

int pinkPantherCoefs[] = {	//Length coefficient for each note
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

int blasterStrong[] = {
    B5, C6, Cs6, D6, Ds6, E6, R,
    E6, Ds6, D6, Cs6, C6, B5, R,
    B5, C6, Cs6, D6, Ds6, E6, R,
    E6, Ds6, D6, Cs6, C6, B5, R
};
int blasterStrongCoefs[] = {[0 ... 27] = 1 };

int explosion[] = {
    C1, E1, C2, E2, F2, Fs2, B1, F1, C1, C1
};
int explosionCoefs[] = {[0 ... 9] = 1 };