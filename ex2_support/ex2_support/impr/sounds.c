#include <stdbool.h>

#include "efm32gg.h"
#include "notes.h"
#include "common.h"


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
