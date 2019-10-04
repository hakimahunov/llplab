#include <stdbool.h>

#ifndef common
#define common

#define   SAMPLE_PERIOD   	158	// 14MHz/88200 samples per second
#define   BASE_PERIODS    	25	//Default amount of periods to play for one note
#define   BASE_PERIODS_EXPL 3	//Default amount of periods to play for one note in explosion sound
#define   AMPLITUDE			100
#define   SAMPLE_RATE		88200

//====== DECLARATION OF FUNCTIONS ======//

void playSound(int *notes, int *noteLengths, int sizeOfArray,
	       bool isExoplosion);
void playSingleNote(int frequency, int noteLengthCoef, bool isExplosion);

void setupGPIO();
void setupTimer(uint32_t period);
void setupDAC();
void pushToDAC(int sample);
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
