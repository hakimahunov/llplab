#ifndef common
#define common

#define   SAMPLE_PERIOD   217
#define   BASE_PERIODS    25

//====== Declaration of peripheral setup functions ======//

void playSound(int *notes, int *noteLengths, int sizeOfArray);
void playSingleNote(int frequency, int noteLengthCoef);

void setupGPIO();
void setupTimer(uint32_t period);
void setupDAC();
void pushToDAC(int sample);


//====== SOUNDS ======//

int pinkPanther[28];
int pinkPantherCoefs[28];
int blaster[21];
int blasterCoefs[21];


#endif
