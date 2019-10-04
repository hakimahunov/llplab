#include <stdbool.h>

#ifndef common
#define common

#define   SAMPLE_PERIOD   	158	// 14MHz/88200 samples per second
#define   BASE_PERIODS    	25	//Default amount of periods to play for one note
#define   BASE_PERIODS_EXPL 3	//Default amount of periods to play for one note in explosion sound
#define   AMPLITUDE			100
#define   SAMPLE_RATE		88200

#define BTN_CHECK(state, btn_mask) (~(~(state) & btn_mask))	//Macros to check which button is pressed
/*Example:
* state = 11101111
* bnt_mask = 11101111
* ~state = 00010000
* ~state & btn_mask = 11101111 & 00010000 = 00000000
* ~(~state & btn_mask) = 11111111
*/

//====== DECLARATION OF FUNCTIONS ======//

//From sounds.c
void playSound(int *notes, int *noteLengths, int sizeOfArray,
	       bool isExoplosion);
void playSingleNote(int frequency, int noteLengthCoef, bool isExplosion);
//From gpio.c
void setupGPIO();
//From timer.c
void setupTimer(uint32_t period);
//From dac.c
void setupDAC();
void pushToDAC(int sample);
//From ex2.c
void btnHandler(int btnState);


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
