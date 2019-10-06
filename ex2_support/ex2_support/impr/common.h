#include <stdbool.h>

#ifndef common
#define common

#define   SAMPLE_PERIOD   	158	// 14MHz/88200 samples per second
#define   BASE_PERIODS    	25	//Default amount of periods to play for one note
#define   BASE_PERIODS_EXPL 3	//Default amount of periods to play for one note in explosion sound
#define   AMPLITUDE			100
#define   SAMPLE_RATE		88200

#define BTN_CHECKER(shift_amount, state) ((1<<(shift_amount)) & ~(state))	//Macros to check which button is pressed
/*Examples:
* state = 11101111
* shift_amount = 4
* ~state = 00010000
* shifted_value = 00010000
* ~state & shifted_value = 00010000 & 00010000 = 00010000
*
* state = 10111111
* shift_amount = 4
* ~state = 01000000
* shifted_value = 00010000
* ~state & shifted_value = 01000000 & 00010000 = 00000000
*/

//====== DECLARATION OF FUNCTIONS ======//

//From sounds.c
void playSound(int *notes, int *noteLengths, int sizeOfArray,
	       bool isExoplosion);
void playSingleNote(int frequency, int noteLengthCoef, bool isExplosion);
//From gpio.c
void setupGPIO();
void setupNVIC();
//From timer.c
void setupTimer(uint32_t period);
void releaseTimer ();
void suspendTimer();
//From dac.c
void setupDAC();
void pushToDAC(int sample);
//From interrupt_handler.c
void btnInterruptHandler();
void timerInterruptHandler();
void initializeLocalVariables(int notes[], int noteLengths[], int sizeOfArray, bool isExplosion);
void delay(int time);


//====== SOUNDS ======//

int pinkPanther[28];
int pinkPantherCoefs[28];
int blaster[21];
int blasterCoefs[21];
int blasterStrong[28];
int blasterStrongCoefs[28];
int explosion[10];
int explosionCoefs[10];


#endif
